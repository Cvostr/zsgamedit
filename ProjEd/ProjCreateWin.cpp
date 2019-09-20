#include "headers/ProjCreateWin.h"
#include "../mainwin.h"
#include "ui_proj_create_win.h"

#include <QFileDialog>
#include <fstream>

CreateProjectWindow::CreateProjectWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CreateProjectWindow)
{
    ui->setupUi(this);

   QObject::connect(ui->prCreateBtn, SIGNAL(clicked()),this, SLOT(onCreateButtonPressed()));
   QObject::connect(ui->dirBrouseBtn, SIGNAL(clicked()),this, SLOT(onBrowsePathButtonPressed()));

   ui->projPersp2D->setChecked(true); //2D picked by default
}

CreateProjectWindow::~CreateProjectWindow()
{
    delete ui;
}

void CreateProjectWindow::onCreateButtonPressed(){
    QString label = ui->prNameEdit->text(); //Get new project label from text field

    if(this->root_path.isEmpty() || label.isEmpty()) //If path or name hasn't been set
        return;

    QString folder_path = root_path + "/" + label; //Getting absolute project root dir path

    QDir project_dir = QDir(root_path);
    project_dir.mkdir(label); //Make project directory

    QString proj_file = folder_path + "/" + label + ".inf";
    std::ofstream proj_conf_stream (proj_file.toStdString(), std::ofstream::out); //Opening file stream

    proj_conf_stream << "ver 1\n"; //Writing version
    if(ui->projPersp2D->isChecked()){ //if user picked 2D project
        proj_conf_stream << "persp 2";
    }
    if(ui->projPersp3D->isChecked()){ //if user chosed 3D
        proj_conf_stream << "persp 3";
    }

    ProjectConf project;
    project.projLabel = label;
    project.projFilePath = proj_file;

    MainWin* mainwin = static_cast<MainWin*>(this->mainwin_ptr);
    mainwin->addProjectToVector(project);
    mainwin->updateListWidgetContent();
    mainwin->saveProjectsConfiguration();

    this->hide();
}

void CreateProjectWindow::onBrowsePathButtonPressed(){
    QString filename = QFileDialog::getExistingDirectory(this, tr("Select project folder"), "/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->rootPathField->setText(filename); //Let user see picked path
    root_path = filename; //Store picked directory

}

void CreateProjectWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    int new_width = event->size().width();
    int new_height = event->size().height();

    ui->prCreateBtn->move(new_width - ui->prCreateBtn->width() - 50 , new_height - ui->prCreateBtn->height() * 2);
}
