#include "headers/ProjCreateWin.h"
#include "ui_proj_create_win.h"

#include <QFileDialog>

CreateProjectWindow::CreateProjectWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CreateProjectWindow)
{
    ui->setupUi(this);

   QObject::connect(ui->prCreateBtn, SIGNAL(clicked()),this, SLOT(onCreateButtonPressed()));
   QObject::connect(ui->dirBrouseBtn, SIGNAL(clicked()),this, SLOT(onBrowsePathButtonPressed()));
}

CreateProjectWindow::~CreateProjectWindow()
{
    delete ui;
}

void CreateProjectWindow::onCreateButtonPressed(){
    QString label = ui->prNameEdit->text();
}

void CreateProjectWindow::onBrowsePathButtonPressed(){
    QString filename = QFileDialog::getExistingDirectory(this, tr("Select project folder"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->rootPathField->setText(filename); //Let user see picked path
    root_path = filename; //Store picked directory
}
