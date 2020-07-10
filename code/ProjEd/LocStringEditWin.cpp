#include "headers/LocStringEditWin.h"
#include "ui_LocStringFileEdit.h"
#include <fstream>
#include <QTreeWidget>
#include <QResizeEvent>

using namespace ZSPIRE;
using namespace std;

QString captions[LANGS_COUNT] = { "English (en)", "Deutsche", "Fr", "Es", "Italian", "Russian" };

LocStringEditWindow::LocStringEditWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::LocStringEditWindow)
{
	ui->setupUi(this);
	lsf = nullptr;
	ui->stringsList->clear();

	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
	connect(ui->actionNew_String, SIGNAL(triggered()), this, SLOT(onNewString()));

	QObject::connect(ui->stringsList, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
		this, SLOT(onStringsListItemClicked()));
	QObject::connect(ui->stringsList, SIGNAL(doubleClicked(QModelIndex)),
		this, SLOT(onStringsListItemDoubleClicked()));

	ui->actionSave->setShortcut(Qt::Key_S | Qt::CTRL);
	ui->actionNew_String->setShortcut(Qt::Key_N | Qt::CTRL);
}
LocStringEditWindow::~LocStringEditWindow() {

}

void LocStringEditWindow::resizeEvent(QResizeEvent* event) {
	QMainWindow::resizeEvent(event);

	int new_width = event->size().width();
	int new_height = event->size().height();

	ui->stringsList->resize(new_width, new_height);
}

void LocStringEditWindow::showWindowWithFile(std::string file) {
	this->file_path = file;

	lsf = new LocaleStringsFile(file);
	//Show dialog
	this->show();
	//if no languages specified in file
	if (lsf->getLanguagesCount() == 0) {
		//Show language select dialog
		LanguagesDialog* lgd = new LanguagesDialog(lsf, this);
		lgd->exec();
	}
	//Add all items for strings
	for (unsigned int str_i = 0; str_i < lsf->getStringsCount(); str_i++) {
		LocString* str = lsf->getString(str_i);
		addStringToUiList(str);
	}
	
}
void LocStringEditWindow::onNewString() {
	LocString string;
	string.ID = lsf->getStringsCount();
	string.str_ID = "STR_" + to_string(string.ID);
	//Add string to UI list
	addStringToUiList(&string);
	lsf->pushString(string);
}

void LocStringEditWindow::addStringToUiList(LocString* str) {
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, QString::number(str->ID) + " " + QString::fromStdString(str->str_ID));
	ui->stringsList->addTopLevelItem(item);
}

void LocStringEditWindow::onStringsListItemClicked() {
	QTreeWidgetItem* item = ui->stringsList->currentItem();
	item->setExpanded(true);
}

void LocStringEditWindow::onStringsListItemDoubleClicked() {
	QTreeWidgetItem* item = ui->stringsList->currentItem();
	//This code below gets digits from Item string and transform that to int
	QString text = item->text(0);
	QString __t;
	int iter = 0;
	while (text[iter] != " ") {
		__t += text[iter];
		iter++;
	}
	int ID = __t.toInt();
	//Call String edit dialog
	LocString* string = lsf->getStringById(ID);
	StringEditDialog* sed = new StringEditDialog(string, lsf, this);
	sed->exec();
	//Update list item text
	item->setText(0, QString::number(string->ID) + " " + QString::fromStdString(string->str_ID));

	delete sed;
}

void LocStringEditWindow::onSave() {
	ofstream stream;
	stream.open(file_path, ofstream::binary);

	char header[] = "ZSLOCALIZEDSTRINGSBINARY";
	//Write HEADER
	stream.write(header, 25);

	unsigned int stringsCount = lsf->getStringsCount();
	unsigned int langsCount = lsf->getLanguagesCount();
	//Write amount of strings
	stream.write((const char*)(&stringsCount), sizeof(unsigned int));
	stream.write((const char*)(&langsCount), sizeof(unsigned int));
	//Write languages
	for (unsigned int lang_i = 0; lang_i < langsCount; lang_i++) {
		unsigned int lang = lsf->getLang(lang_i);
		stream.write((const char*)(&lang), sizeof(unsigned int));
	}
	//Write all strings
	for (unsigned int str_i = 0; str_i < stringsCount; str_i ++) {
		LocString* str = lsf->getString(str_i);
		//Write String ID
		stream << str->str_ID << '\0';
		//Write Integer ID
		stream.write((const char*)&str->ID, sizeof(unsigned int));
		//Write strings for each language
		for (unsigned int lstr_i = 0; lstr_i < lsf->getLanguagesCount(); lstr_i++) {
			std::u32string* _str = &str->STR[lstr_i];
			//Write LANG code of localized string
			unsigned int lang = lsf->getLang(lstr_i);
			stream.write((const char*)&lang, sizeof(unsigned int));
			//Get length of string
			unsigned int str_size = _str->size();
			//Write length of string
			stream.write((const char*)&str_size, sizeof(unsigned int));
			//Write localized string
			for (unsigned int c_i = 0; c_i < str_size; c_i++) {
				char32_t _char = _str->at(c_i);
				stream.write((const char*)&_char, sizeof(char32_t));
			}
		}
	}
	//Close stream
	stream.close();
}

LanguagesDialog::LanguagesDialog(ZSPIRE::LocaleStringsFile* lsf, QWidget* parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
	//Store LocaleStringsFile pointer
	this->lsf = lsf;

	//Allocation of main layout
    contentLayout = new QGridLayout;
	setLayout(contentLayout);
	//Allocation of apply button
	apply_btn = new QPushButton;
	apply_btn->setText("Apply");
	connect(apply_btn, SIGNAL(clicked()), this, SLOT(onApply()));
	contentLayout->addWidget(apply_btn, LANGS_COUNT, 0);
    this->setWindowTitle("Set languages");
	//Allocate array for QCheckBox pointers
	lang_checkboxes = new QCheckBox*[LANGS_COUNT];
	//Spawn checkboxes for each language
	for (unsigned int i = 0; i < LANGS_COUNT; i++) {
		lang_checkboxes[i] = new QCheckBox;
		lang_checkboxes[i]->setText(captions[i]);
		contentLayout->addWidget(lang_checkboxes[i], i, 0);
	}
}
LanguagesDialog::~LanguagesDialog() {
	delete contentLayout;
	delete apply_btn;
	for (unsigned int i = 0; i < LANGS_COUNT; i++) {
		delete lang_checkboxes[i];
	}
	delete lang_checkboxes;
}

void LanguagesDialog::onApply() {
	for (unsigned int i = 0; i < LANGS_COUNT; i++) {
		//if checkbox with language checked
		if (lang_checkboxes[i]->isChecked()) {
			//Push language code
			lsf->pushLanguageCode(i);
		}
	}
	//if no languages selected
	if (lsf->getLanguagesCount() == 0)
		return;

	accept();
}

StringEditDialog::StringEditDialog(LocString* str, ZSPIRE::LocaleStringsFile* lsf, QWidget* parent) :
	QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint) {
	//Store pointers
	this->str = str;
	this->lsf = lsf;

	setWindowTitle("Editing " + QString::fromStdString(str->str_ID));

	//Allocation of main layout
	contentLayout = new QGridLayout;
	setLayout(contentLayout);
	

	line = new QFrame;
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	contentLayout->addWidget(line, 1, 0);

	//Allocate StringEditDialogLine for String ID
	str_id_line = new StringEditDialogLine;

	str_id_line->label->setText("String ID");
	contentLayout->addWidget(str_id_line->label, 0, 0);
	str_id_line->edit->setText(QString::fromStdString(str->str_ID));
	contentLayout->addWidget(str_id_line->edit, 0, 1);

	//Allocate array of lines in size of languages amount
	lines = new StringEditDialogLine[lsf->getLanguagesCount()];
	for (unsigned int lstr_i = 0; lstr_i < lsf->getLanguagesCount(); lstr_i++) {
		lines[lstr_i].label->setText(captions[lstr_i]);

		contentLayout->addWidget(lines[lstr_i].label, lstr_i + 2, 0);
		contentLayout->addWidget(lines[lstr_i].edit, lstr_i + 2, 1);
		//Set String to text field
		lines[lstr_i].edit->setText(QString::fromStdU32String(str->STR[lstr_i]));
	}

	//Allocation of apply button
	apply_btn = new QPushButton;
	apply_btn->setText("Apply");
	connect(apply_btn, SIGNAL(clicked()), this, SLOT(onApply()));
	contentLayout->addWidget(apply_btn, LANGS_COUNT + 2, 0);
}
StringEditDialog::~StringEditDialog() {
	delete contentLayout;
	delete apply_btn;
	delete str_id_line;
	delete[] lines;
}

void StringEditDialog::onApply() {
	str->str_ID = str_id_line->edit->text().toStdString();
	//Send all modified strings
	for (unsigned int lstr_i = 0; lstr_i < lsf->getLanguagesCount(); lstr_i++) {
		str->STR[lstr_i] = lines[lstr_i].edit->text().toStdU32String();
	}
	//Close dialog
	accept();
}