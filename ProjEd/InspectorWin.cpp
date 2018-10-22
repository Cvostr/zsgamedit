#include "headers/InspectorWin.h"
#include "ui_inspector_win.h"


InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin)
{
    ui->setupUi(this);


}

InspectorWin::~InspectorWin()
{
    delete ui;
}
