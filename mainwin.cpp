#include "mainwin.h"
#include "ui_mainwin.h"

#include "stdio.h"
#include "string.h"
#include <string>
#include <fstream>

#include <QFile>
#include <QFileDialog>
#include <QPushButton>
#include <QAction>
#include <QMouseEvent>
#include <QDesktopServices>

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
    project_menu->win = this;
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
    //Calculate root project path
    conf.projectRootPath = conf.projFilePath;
    conf.projectRootPath.resize(conf.projectRootPath.size() - step);

    //Copy required data
    conf.editwin_proj.label = conf.projLabel;
    conf.editwin_proj.root_path = conf.projectRootPath;

    //push to vector
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
            std::string project_inf_path;
            proj_inf_file >> project_inf_path; //read path to path
            QString str = QString::fromStdString(project_inf_path);

            if(QFile::exists(str)){ //If we reached project root path

                conf.projFilePath = str; //Read path to project configuration file and store it

                std::ifstream project_conf_stream;
                project_conf_stream.open(conf.projFilePath.toStdString(), std::ifstream::in); //Opening file stream for reading
                //read project's configuration file
                Project mProject;
                while(!project_conf_stream.eof()){ //If reaading finished
                    std::string prefix;
                    project_conf_stream >> prefix; //Reading prefix
                    if(prefix.compare("ver") == 0){ //If reched to ver
                        int ver = 0;
                        project_conf_stream >> ver; //Reading version
                        mProject.version = ver; //Storing version in project struct
                    }
                    if(prefix.compare("persp") == 0){ //If reched to persp
                        project_conf_stream >> mProject.perspective; //Reading perspective
                    }
                }
                conf.editwin_proj = mProject; //Copy project data

                addProjectToVector(conf); //Add project vector

                updateListWidgetContent();
            }
        }
    }
    proj_inf_file.close();
}


void MainWin::setupProjectsConfFile(){
    //Write basic information
    std::ofstream conf_stream;
    //open projs.inf file
    conf_stream.open("projs.inf", std::ofstream::out);
    conf_stream << "proj_num 0";
    conf_stream.close();
}

void MainWin::saveProjectsConfiguration(){
    std::ofstream conf_stream;
    //open projs.inf file
    conf_stream.open("projs.inf", std::ofstream::out);
    //Write projects amount
    conf_stream << "proj_num " << static_cast<int>(this->projects.size()) << "\n";
    //Iterate over all projects and write them
    for(unsigned int i = 0; i < this->projects.size(); i ++){ //Iterate over all projects in vector
        std::string pth_str_stl = this->projects[i].projFilePath.toStdString();
        conf_stream << "project " << pth_str_stl << "\n";
    }

    conf_stream.close();
}

void MainWin::onAddProjButtonClicked(){

    QString path = QFileDialog::getOpenFileName(this, tr("Project Configuration File"), "/");
    if ( path.isNull() == false && path.endsWith(".inf")) //If user specified file path
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

            this->edit_win_ptr = ZSEditor::openProject(conf_ptr->editwin_proj); //Call project opening
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
    QListWidgetItem* selected_proj_item = ui->projList->currentItem();
    QString proj_label = selected_proj_item->text();

    ProjectConf* conf = nullptr;

    for(unsigned int entry_i = 0; entry_i < this->projects.size(); entry_i++){ //Iterate over all projects
        ProjectConf* conf_ptr = &projects[entry_i];
        if(proj_label.compare(conf_ptr->projLabel) == 0){ //If strings have same content
            conf = conf_ptr;
        }
    }
    project_menu->project_conf_ptr = conf;
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
    this->action_open_folder = new QAction("Open Root directory", win);
#ifdef USE_ZSPIRE
    this->action_run_engine = new QAction("Run in engine instance", win);
    this->action_run_engine_vk = new QAction("Run in engine instance (Vulkan)", win);
#endif

    menu->addAction(action_delete);
    menu->addAction(action_open_folder);
#ifdef USE_ZSPIRE
    menu->addAction(action_run_engine);
    menu->addAction(action_run_engine_vk);
#endif

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_open_folder, SIGNAL(triggered(bool)), this, SLOT(onOpenDirClicked()));
#ifdef USE_ZSPIRE
    QObject::connect(this->action_run_engine, SIGNAL(triggered(bool)), this, SLOT(runEngineClickedGL()));
    QObject::connect(this->action_run_engine_vk, SIGNAL(triggered(bool)), this, SLOT(runEngineClickedVK()));
#endif
}

void ProjectCtxMenu::show(QPoint point){
    this->menu->popup(QCursor::pos());
}
void ProjectCtxMenu::onDeleteClicked(){
    this->project_conf_ptr->removed = true; //mark as removed

    for (unsigned int i = 0; i < win->projects.size(); i ++) { //Iterating over all objects

        if(&win->projects[i] == this->project_conf_ptr){ //we found removed object

            for (unsigned int obj_i = i + 1; obj_i < win->projects.size(); obj_i ++) { //Iterate over all next chidren
                win->projects[obj_i - 1] = win->projects[obj_i]; //Move it to previous place
            }

        }
    }
    win->projects.resize(win->projects.size() - 1); //remove last element

    win->updateListWidgetContent();
    win->saveProjectsConfiguration();
}

void ProjectCtxMenu::onOpenDirClicked(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(this->project_conf_ptr->projectRootPath));
}

#ifdef USE_ZSPIRE
void ProjectCtxMenu::runEngineClickedGL(){
    runEngineClicked(OGL32);
}
void ProjectCtxMenu::runEngineClickedVK(){
    runEngineClicked(VULKAN);
}

void ProjectCtxMenu::runEngineClicked(ZSGAPI gapi){

    QListWidgetItem* selected_proj_item = win->ui->projList->currentItem();
    QString proj_label = selected_proj_item->text();

    ProjectConf* conf = nullptr;

    for(unsigned int entry_i = 0; entry_i < this->win->projects.size(); entry_i++){ //Iterate over all projects
        ProjectConf* conf_ptr = &win->projects[entry_i];
        if(proj_label.compare(conf_ptr->projLabel) == 0){ //If strings have same content
            conf = conf_ptr;
        }
    }

    ZSENGINE_CREATE_INFO engine_create_info;
    engine_create_info.appName = "GameEditorRun";
    engine_create_info.createWindow = true; //window already created, we don't need one
    engine_create_info.graphicsApi = gapi; //use opengl

    ZSWINDOW_CREATE_INFO window_create_info;
    window_create_info.title = "Preview";

    ZSGAME_DESC game_info;
    game_info.app_label = proj_label.toStdString(); //Setting app label
    game_info.app_version = 0;
    game_info.resource_type = TYPE_FILES;
    game_info.game_dir = conf->projectRootPath.toStdString();

    win->engine = new ZSpireEngine(&engine_create_info, &window_create_info, &game_info);
    win->engine->loadGame();

}
#endif
