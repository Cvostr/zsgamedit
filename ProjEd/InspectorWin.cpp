#include "headers/InspectorWin.h"
#include "ui_inspector_win.h"


InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin)
{
    ui->setupUi(this);
    x_win_start = 411 + 480;
}

InspectorWin::~InspectorWin()
{
    delete ui;
}

QVBoxLayout* InspectorWin::getContentLayout(){
    return ui->propertySpace;
}

void InspectorWin::clearContentLayout(){
   // ui->propertySpace->removeWidget();
}
