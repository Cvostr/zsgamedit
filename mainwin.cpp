#include "mainwin.h"
#include "ui_mainwin.h"

#include "stdio.h"
#include "string.h"
#include <string>
#include <fstream>

#include <QFileDialog>
#include <QPushButton>
#include <QAction>
#include <QMouseEvent>

static CreateProjectWindow* cr_w;

MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);

    this->edit_win_ptr = nullptr; //To avoid crashes

    QObject::connect(ui->actionAdd_project, SIGNAL(triggered()),
                this, SLOT(onAddProjButtonClicked()));

    QObject::connect(ui->actionNew_project, SIGNAL(triggered()),
                this, SLOT(onShowCreateNewProjectWindow()));

    QObject::connect(ui->projList, SIGNAL(itemClicked(QListWidgetItem *)),
                this, SLOT(onSelectProjectToOpen()));

    QObject::connect(ui->projList, SIGNAL(onRightClick(QPoint)), this, SLOT(showCtxMenu(QPoint)));

    loadProjectsConfigurations();
    cr_w = new CreateProjectWindow;

    project_menu = new ProjectCtxMenu(this); //Allocate projectCtxMenu
}

MainWin::~MainWin()
{
    delete ui;
}

void MainWin::addProjectToVector(ProjectConf conf){
    int step = 5;
    int len = conf.projFilePath.size();
    while(conf.projFilePath[len - step] != '/'){
          conf.projLabel.insert(0, conf.projFilePath[len - step]);
          step += 1;
    }
    this->projects.push_back(conf);
    saveProjectsConfiguration();
}

void MainWin::loadProjectsConfigurations(){
    std::ifstream proj_inf_file;
    proj_inf_file.open("projs.inf", std::ifstream::in); //Opening file stream for reading

    if(!proj_inf_file.is_open()){ //No projects file, maybe first start
        setupProjectsConfFile(); //Setup this file then
        return;
    }

    int projects_num = 0;

    while(!proj_inf_file.eof()){ //If reaading finished
        std::string prefix;
        proj_inf_file >> prefix; //Reading prefix
        if(prefix.compare("proj_num") == 0){ //If reched to proj_num
            proj_inf_file >> projects_num; //Reading version
        }

        if(prefix.compare("project") == 0){ //If reched to proj_num
            ProjectConf conf;
            char path[128];
            proj_inf_file >> path;
            FILE* projFileD = fopen(path, "r"); //Trying to open project root directory
            if(projFileD){ //If we reached project root path
                fclose(projFileD);
                conf.projFilePath = QString(path);
                addProjectToVector(conf);

                updateListWidgetContent();
            }
            }
        }
    proj_inf_file.close();
}


void MainWin::setupProjectsConfFile(){
     FILE* proj_inf_file = fopen("projs.inf", "w");

     fprintf(proj_inf_file, "proj_num 0"); //Write proj amount

     fclose(proj_inf_file);
}

void MainWin::saveProjectsConfiguration(){
    FILE* proj_inf_file = fopen("projs.inf", "w"); //Open projects lst file stream

    fprintf(proj_inf_file, "proj_num %d\n", static_cast<int>(this->projects.size())); //Write proj amount

    for(unsigned int i = 0; i < this->projects.size(); i ++){ //Iterate over all projects in vector
        char path_tw[128];
        std::string pth_str_stl = this->projects[i].projFilePath.toStdString();
        strcpy(path_tw, pth_str_stl.c_str());
        fprintf(proj_inf_file, "project %s\n", path_tw); //Write project path
    }

    fclose(proj_inf_file);
}

void MainWin::onAddProjButtonClicked(){

    QString path = QFileDialog::getOpenFileName(this, tr("Project Configuration File"), "/");
    if ( path.isNull() == false ) //If user specified file path
    {
        ProjectConf conf; //Preparing struct
        conf.projFilePath = path; //Assigning project file path

        addProjectToVector(conf);
        //Updating list widget
        updateListWidgetContent();

    }
}

void MainWin::onShowCreateNewProjectWindow(){
    cr_w->mainwin_ptr = static_cast<void*>(this); //Send pointer to this window
    cr_w->show();
}

void MainWin::onSelectProjectToOpen(){
    QListWidgetItem* selected_proj_item = ui->projList->currentItem();
    QString proj_label = selected_proj_item->text(); //Getting text of pressed entry

    for(unsigned int entry_i = 0; entry_i < projects.size(); entry_i++){ //Iterate over all projects
        ProjectConf* conf_ptr = &this->projects[entry_i];
        if(proj_label.compare(conf_ptr->projLabel) == 0){ //If strings have same content
            //We found index, keep going
            this->hide(); //Close project selection window
            this->edit_win_ptr = ZSEditor::openProject(conf_ptr->projFilePath); //Call project opening
        }
    }

}

void MainWin::updateListWidgetContent(){
    ui->projList->clear();
    for(unsigned int i = 0; i < this->projects.size(); i ++){

        ProjectConf* conf_ptr = &this->projects[i];
        //Add project entry to list
        new QListWidgetItem(conf_ptr->projLabel, ui->projList);

    }
}

void MainWin::showCtxMenu(QPoint point){
    project_menu->show(point);
}

ProjectListWgt::ProjectListWgt(QWidget* parent) : QListWidget (parent){
}

void ProjectListWgt::mousePressEvent(QMouseEvent *event){
    QListWidget::mousePressEvent(event);
    if(event->button() == Qt::RightButton)
    {
        emit onRightClick(event->pos());
    }
}

ProjectCtxMenu::ProjectCtxMenu(MainWin* win, QWidget* parent) : QObject(parent){
    this->win = win;
    //Allocate Qt stuff
    this->menu = new QMenu(win);

    this->action_delete = new QAction("Delete", win);
    this->action_run_engine = new QAction("Run in engine instance", win);
    this->action_run_engine_vk = new QAction("Run in engine instance (Vulkan)", win);

    menu->addAction(action_delete);
    menu->addAction(action_run_engine);
    menu->addAction(action_run_engine_vk);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_run_engine, SIGNAL(triggered(bool)), this, SLOT(runEngineClicked()));
    QObject::connect(this->action_run_engine_vk, SIGNAL(triggered(bool)), this, SLOT(runEngineClicked()));

}

void ProjectCtxMenu::show(QPoint point){
    this->menu->popup(point);
}
void ProjectCtxMenu::onDeleteClicked(){
    for (unsigned int i = 0; i < win->projects.size(); i ++) { //Iterating over all objects

            for (unsigned int obj_i = i + 1; obj_i < win->projects.size(); obj_i ++) { //Iterate over all next chidren
                win->projects[obj_i - 1] = win->projects[obj_i]; //Move it to previous place

            }
            win->projects.resize(win->projects.size() - 1);

    }
    win->updateListWidgetContent();
    win->saveProjectsConfiguration();
}

void ProjectCtxMenu::runEngineClicked(){
    ZSENGINE_CREATE_INFO engine_create_info;
    engine_create_info.appName = "GameEditorRun";
    engine_create_info.createWindow = true; //window already created, we don't need one
    engine_create_info.graphicsApi = OGL32; //use opengl

    ZSWINDOW_CREATE_INFO window_create_info;
    window_create_info.title = "Preview";

    ZSGAME_DESC game_info;
    //game_info.

    win->engine = new ZSpireEngine(&engine_create_info, &window_create_info);

}
