#include "headers/LocStringEditWin.h"
#include "ui_LocStringFileEdit.h"
#include <fstream>

using namespace ZSPIRE;
using namespace std;

QString captions[LANGS_COUNT] = { "English (en)", "De", "Fr", "Es", "Italian", "Russian" };

LocStringEditWindow::LocStringEditWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::LocStringEditWindow)
{
	ui->setupUi(this);
	lsf = nullptr;

	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
}
LocStringEditWindow::~LocStringEditWindow() {

}

void LocStringEditWindow::showWindowWithFile(std::string file) {
	this->file_path = file;

	lsf = new LocaleStringsFile(file);
	//Show dialog
	this->show();

	if (lsf->getLanguagesCount() == 0) {
		LanguagesDialog* lgd = new LanguagesDialog(lsf, this);
		lgd->exec();
	}
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
	for (unsigned int str_i = 0; str_i < stringsCount; str_i ++) {

	}

	stream.close();
}

LanguagesDialog::LanguagesDialog(ZSPIRE::LocaleStringsFile* lsf, QWidget* parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint) {
	//Store LocaleStringsFile pointer
	this->lsf = lsf;

	//Allocation of main layout
    contentLayout = new QGridLayout;
	setLayout(contentLayout);
	apply_btn = new QPushButton;
	apply_btn->setText("Apply");
	connect(apply_btn, SIGNAL(clicked()), this, SLOT(onApply()));
	contentLayout->addWidget(apply_btn, LANGS_COUNT, 0);
    this->setWindowTitle("Set languages");
	//Allocate array for QCheckBox pointers
	lang_checkboxes = new QCheckBox*[LANGS_COUNT];

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
	std::string captions[LANGS_COUNT] = { "EN", "DE", "FR", "ES", "IT", "RU" };
	for (unsigned int i = 0; i < LANGS_COUNT; i++) {
		//if checkbox with language checked
		if (lang_checkboxes[i]->isChecked()) {
			//Push language code
			lsf->pushLanguageCodeStr(captions[i]);
		}
	}
	//if no languages selected
	if (lsf->getLanguagesCount() == 0)
		return;

	accept();
}