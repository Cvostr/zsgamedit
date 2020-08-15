#include "headers/DialogsMaster.h"
#include <QMessageBox>

void showErrorMessageBox(std::string title, std::string text) {
	QMessageBox messageBox;
	messageBox.critical(0, QString::fromStdString(title), QString::fromStdString(text));
	messageBox.show();
}
