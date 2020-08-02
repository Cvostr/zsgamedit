#include "headers/ConsoleLog.h"
#include "ui_ConsoleLogWindow.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

using namespace Engine;

ConsoleLog::ConsoleLog(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleLog)
{
    ui->setupUi(this);

    tabs = ui->logTypeTabs;
    logs_list = ui->listWidget;

    tabs->setTabText(0, "Engine log");
    tabs->setTabText(1, "Script output");
    tabs->setTabText(2, "Script Errors");

    QObject::connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

    leType = LogEntryType::LE_TYPE_SCRIPT_ERROR;
}

void ConsoleLog::resizeEvent(QResizeEvent* event) {

}

void ConsoleLog::keyPressEvent(QKeyEvent* ke) {
	if (ke->key() == Qt::Key_Escape) { //User pressed delete button
        this->close();
	}

	QMainWindow::keyPressEvent(ke); // base class implementation
}

void ConsoleLog::currentTabChanged(int index) {
    leType = (LogEntryType)index;
    updateLogsList();
}

void ConsoleLog::updateLogsList() {
    logs_list->clear();
    OutputManager* manager = game_data->out_manager;

    for (unsigned int i = 0; i < manager->entries.size(); i++) {
        LogEntry* lEntry = &manager->entries[i];
        if (lEntry->log_type != leType)
            continue;
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(lEntry->message), logs_list);
        item->setSizeHint(QSize(item->sizeHint().width(), 100));
    }
}

ConsoleLog::~ConsoleLog()
{
    delete ui;
}