#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include "ui_editor.h"
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <QDir>
#include <QDropEvent>
#include <QFileDialog>

static EditWindow* _editor_win;
static InspectorWin* _inspector_win;

EditWindow::EditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->fileList, SIGNAL(itemClicked(QListWidgetItem *)),
                this, SLOT(onFileListItemClicked())); //Signal comes, when user clicks on file
    QObject::connect(ui->actionNew_Object, SIGNAL(triggered()),
                this, SLOT(onAddNewGameObject())); //Signal comes, when user clicks on Object->Create
    QObject::connect(ui->actionSave, SIGNAL(triggered()),
                this, SLOT(onSceneSave())); //Signal comes, when user clicks on File->Save
    QObject::connect(ui->actionSave_As, SIGNAL(triggered()),
                this, SLOT(onSceneSaveAs())); //Signal comes, when user clicks on File->Save As
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenScene()));

    QObject::connect(ui->actionCreateScene, SIGNAL(triggered()), this, SLOT(onNewScene()));

    QObject::connect(ui->objsList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(onObjectListItemClicked())); //Signal comes, when user clicks on File->Save As

    QObject::connect(ui->objsList, SIGNAL(onRightClick(QPoint)), this, SLOT(onObjectCtxMenuShow(QPoint)));
    QObject::connect(ui->objsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onCameraToObjTeleport()));

    ready = false; //Firstly set it to 0
    hasSceneFile = false; //No scene loaded by default

    setupObjectsHieList();

    ui->objsList->setAcceptDrops(true);
    ui->objsList->setDragEnabled(true);
    ui->objsList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->objsList->world_ptr = &world;

    world.proj_ptr = static_cast<void*>(&project); //Assigning project pointer into world's variable

    ui->fileList->setViewMode(QListView::IconMode);

    this->obj_ctx_menu = new ObjectCtxMenu(this); //Allocating object Context menu

}

EditWindow::~EditWindow()
{
    delete ui;
}

void ObjTreeWgt::mousePressEvent(QMouseEvent *event){
    QTreeWidget::mousePressEvent(event);
    if(event->button() == Qt::RightButton)
    {
        emit onRightClick(event->pos());
    }
}

void EditWindow::init(){

    input_state.isLeftBtnHold = false;
    input_state.isRightBtnHold = false;
    input_state.isCtrlHold = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        //return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    this->window = SDL_CreateWindow("Game View", this->width(), 0, 640, 480, SDL_WINDOW_OPENGL); //Create window
    this->glcontext = SDL_GL_CreateContext(window);

    glViewport(0, 0, 640, 480);

    render = new RenderPipeline;
    render->InitGLEW();

    render->setup();
    ready = true;//Everything is ready

    switch(project.perspective){
    case 2:{ //2D project

        this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
        edit_camera.setPosition(ZSVECTOR3(0,0,0));
        edit_camera.setFront(ZSVECTOR3(0,0,1));
        break;
    }
    case 3:{ //3D project
        this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
        edit_camera.setPosition(ZSVECTOR3(0,0,0));
        edit_camera.setFront(ZSVECTOR3(0,0,1));
        break;
    }
    }

}

void EditWindow::assignIconFile(QListWidgetItem* item){
    if(item->text().endsWith(".txt") || item->text().endsWith(".inf") || item->text().endsWith(".scn")){
        item->setIcon(QIcon::fromTheme("text-x-generic"));
    }
    if(item->text().endsWith(".dds") || item->text().endsWith(".DDS")){
        item->setIcon(QIcon::fromTheme("image-x-generic"));
    }
}

void EditWindow::setViewDirectory(QString dir_path){

    this->current_dir = dir_path;
    this->updateFileList();
}
//Slots
void EditWindow::openFile(QString file_path){

    if(file_path.endsWith(".scn")){ //If it is scene
        setupObjectsHieList(); //Clear everything, at first
        world.openFromFile(file_path, ui->objsList); //Open this scene

        scene_path = file_path; //Assign scene path
        hasSceneFile = true; //Scene is saved
        this->edit_camera.setPosition(ZSVECTOR3(0.0f, 0.0f, 0.0f)); //Set camera to 0
        _inspector_win->clearContentLayout(); //Clear content, if not empty
    }
}

void EditWindow::onSceneSaveAs(){
    QString filename = QFileDialog::getSaveFileName(this, tr("Save scene file"), project.root_path, "*.scn");
    if(!filename.endsWith(".scn")) //If filename doesn't end with ".scn"
        filename.append(".scn"); //Add this extension
    world.saveToFile(filename); //Save to picked file
    scene_path = filename; //Assign scene path
    hasSceneFile = true; //Scene is saved

    updateFileList(); //Make new scene visible in file list
}

void EditWindow::onOpenScene(){
    QString path = QFileDialog::getOpenFileName(this, tr("Scene File"), project.root_path);
    if ( path.isNull() == false ) //If user specified file path
    {
        openFile(path);
    }
}

void EditWindow::onSceneSave(){
    if(hasSceneFile == false){ //If new created scene without file
        onSceneSaveAs(); //Show dialog and save
    }else{
        world.saveToFile(this->scene_path);
    }
}

void EditWindow::onNewScene(){
    setupObjectsHieList();
    world.clear();
    hasSceneFile = false; //We have new scene
}

void EditWindow::onAddNewGameObject(){
    GameObject* obj_ptr = this->world.newObject(); //Add new object to world
    ui->objsList->addTopLevelItem(obj_ptr->item_ptr); //New object will not have parents, so will be spawned at top
}

void EditWindow::setupObjectsHieList(){
    QTreeWidget* w_ptr = ui->objsList; //Getting pointer to objects list widget
    w_ptr->clear(); //Clears widget

    column_item_go = new QTreeWidgetItem; //Defining Objects list
    column_item_go->setText(0, "Objects"); //Setting text to Objects
}

void EditWindow::updateFileList(){
    ui->fileList->clear(); //Clear widget content

    QDir directory (this->current_dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::Name | QDir::Reversed);

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator
    if(this->current_dir.compare(project.root_path)){
        QListWidgetItem* backBtn_item = new QListWidgetItem("(back)", ui->fileList);
        backBtn_item->setIcon(QIcon::fromTheme("go-up"));
    }

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName(), ui->fileList);
        if(fileInfo.isDir()){
            item->setIcon(QIcon::fromTheme("folder"));
        }else{
            assignIconFile(item);
        }
    }
}
//Signal
void EditWindow::onFileListItemClicked(){
    QListWidgetItem* selected_file_item = ui->fileList->currentItem();

    if(selected_file_item->text().compare("(back)") == 0){
        QDir cur_folder = QDir(this->current_dir);
        cur_folder.cdUp();
        setViewDirectory(cur_folder.absolutePath());
        return;
    }

    QDir directory (this->current_dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::Name | QDir::Reversed);

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        if(fileInfo.fileName().compare(selected_file_item->text()) == 0){ //Find pressed item
            if(fileInfo.isDir()){ //If we pressed on directory
                QString new_path = this->current_dir + "/" + fileInfo.fileName(); //Get directory path
                setViewDirectory(new_path); //go to this
                return; // Exit function to prevent crash
            }else{
                QString new_path = this->current_dir + "/" + fileInfo.fileName(); //Get file path
                openFile(new_path); //Do something to open this file
                return;
            } //If it isn't directory, it is a file
        }
    }
}

void EditWindow::onObjectListItemClicked(){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item

    QString obj_name = selected_item->text(0); //Get label of clicked obj

    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
}

void EditWindow::onObjectCtxMenuShow(QPoint point){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->show(point);
}

void EditWindow::onCameraToObjTeleport(){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    TransformProperty* transform = obj_ptr->getTransformProperty(); //Obtain pointer to object transform

    edit_camera._dest_pos = transform->translation;
    edit_camera.startMoving();
}

void EditWindow::glRender(){
    if(ready == true)
        render->render(this->window, static_cast<void*>(this));
}

void EditWindow::lookForResources(QString path){
    QDir directory (path); //Creating QDir object
    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsLast); //I want to recursive call this function after all files

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info

        if(fileInfo.isFile() == true){
            QString name = fileInfo.fileName();
            if(name.endsWith(".DDS") || name.endsWith(".dds")){ //If its an texture
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath(); //Writing full path
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size()); //Get relative path by removing length of project root from start
                resource.type = RESOURCE_TYPE_TEXTURE; //Type is texture
                loadResource(&resource); //Perform texture loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".FBX") || name.endsWith(".fbx")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size()); //Get relative path by removing length of project root from start
                resource.type = RESOURCE_TYPE_MESH; //Type of resource is mesh
                loadResource(&resource); //Perform mesh processing & loading to OpenGL
                this->project.resources.push_back(resource);
            }
        }

        if(fileInfo.isDir() == true){ //If it is directory
            QString newdir_str = path + "/"+ fileInfo.fileName();
            lookForResources(newdir_str); //Call this function inside next dir
        }
    }
}

void EditWindow::loadResource(Resource* resource){
    switch(resource->type){
        case RESOURCE_TYPE_TEXTURE:{ //If resource type is texture
            resource->class_ptr = static_cast<void*>(new ZSPIRE::Texture); //Initialize pointer to texture
            ZSPIRE::Texture* texture_ptr = static_cast<ZSPIRE::Texture*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            texture_ptr->LoadDDSTextureFromFile(str.c_str()); //Perform texture resource loading
            break;
        }
        case RESOURCE_TYPE_MESH:{
            resource->class_ptr = static_cast<void*>(new ZSPIRE::Mesh); //Initialize pointer to mesh
            ZSPIRE::Mesh* mesh_ptr = static_cast<ZSPIRE::Mesh*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            mesh_ptr->LoadMeshesFromFileASSIMP(str.c_str());
            break;
        }

    }
}

EditWindow* ZSEditor::openProject(QString conf_file_path){
    _editor_win = new EditWindow(); //Creating class object
    _inspector_win = new InspectorWin();

    //Now reading config file
    std::ifstream project_conf_stream;
    project_conf_stream.open(conf_file_path.toStdString(), std::ifstream::in); //Opening file stream for reading

    while(!project_conf_stream.eof()){ //If reaading finished
        std::string prefix;
        project_conf_stream >> prefix; //Reading prefix
        if(prefix.compare("ver") == 0){ //If reched to ver
            int ver = 0;
            project_conf_stream >> ver; //Reading version
            _editor_win->project.version = ver; //Storing version in project struct
        }
        if(prefix.compare("persp") == 0){ //If reched to persp
            project_conf_stream >> _editor_win->project.perspective; //Reading perspective
        }
    }

    //These stupid funcs calculate project root directory
    int step = 5;
    int len = conf_file_path.size();
    while(conf_file_path[len - step] != '/'){
          _editor_win->project.label.insert(0, conf_file_path[len - step]);
          step += 1;
    }
    QString proj_root_dir = conf_file_path;
    proj_root_dir.resize(proj_root_dir.length() - step);
    _editor_win->project.root_path = proj_root_dir; //Setting root path
    _editor_win->setViewDirectory(proj_root_dir);

    project_conf_stream.close(); //Close file stream

    return openEditor(); //Return pointer to edit window
}
EditWindow* ZSEditor::openEditor(){
    _editor_win->init();

    _editor_win->lookForResources(_editor_win->project.root_path); //Make a vector of all resource files
    _editor_win->show(); //Show editor window
    _editor_win->move(0,0); //Editor base win would be in the left part of screen

    _inspector_win->show();
    _inspector_win->move(_editor_win->width() + 640, 0);

    return _editor_win;
}

InspectorWin* EditWindow::getInspector(){
    return _inspector_win;
}

ObjTreeWgt::ObjTreeWgt(QWidget* parent) : QTreeWidget (parent){
    this->world_ptr = nullptr; //Not assigned by default
}

ObjectCtxMenu::ObjectCtxMenu(EditWindow* win, QWidget* parent ) : QObject(parent){
    this->win_ptr = win;
    //Allocting menu container
    this->menu = new QMenu(win);
    //Allocating actions
    this->action_dub = new QAction("Dublicate", win);
    this->action_delete = new QAction("Delete", win);
    //Adding actions to menu container
    this->menu->addAction(action_dub);
    this->menu->addAction(action_delete);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
}

void ObjectCtxMenu::show(QPoint point){
    menu->popup(point);
}

void ObjectCtxMenu::setObjectPtr(GameObject* obj_ptr){
    this->obj_ptr = obj_ptr;
}

void ObjectCtxMenu::close(){
    menu->close();
}
//Object Ctx menu slots
void ObjectCtxMenu::onDeleteClicked(){
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    GameObjectLink link = obj_ptr->getLinkToThisObject();
    win_ptr->world.removeObj(link);
}
void ObjectCtxMenu::onDublicateClicked(){

}

void ObjTreeWgt::dropEvent(QDropEvent* event){
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    //User dropped object item
    QList<QTreeWidgetItem*> kids = this->selectedItems(); //Get list of selected object(it is moving object)

    GameObject* obj_ptr = world_ptr->getObjectByLabel(kids.at(0)->text(0)); //Receiving pointer to moving object

    QTreeWidgetItem* pparent = kids.at(0)->parent(); //parent of moved object
    if(pparent == nullptr){ //If object hadn't any parent

    }else{ //If object already parented
        GameObjectLink link = obj_ptr->getLinkToThisObject();
        GameObject* pparent_go = world_ptr->getObjectByLabel(pparent->text(0));
        pparent_go->removeChildObject(link); //Remove object from previous parent
    }

    QTreeWidget::dropEvent(event);


    QTreeWidgetItem* nparent = obj_ptr->item_ptr->parent(); //new parent
    if(nparent != nullptr){ //If we moved obj to another parent
        GameObject* nparent_go = world_ptr->getObjectByLabel(nparent->text(0));
        nparent_go->addChildObject(obj_ptr->getLinkToThisObject());
    }else{ //We unparented object
        obj_ptr->hasParent = false;
        this->addTopLevelItem(obj_ptr->item_ptr);
    }
}

void EditWindow::onLeftBtnClicked(int X, int Y){
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label
    if(clicked > world.objects.size() || obj_ptr == 0x0 || clicked >= 256 * 256 * 256)
        return;

    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
}
void EditWindow::onRightBtnClicked(int X, int Y){}
void EditWindow::onMouseMotion(int relX, int relY){
    if(project.perspective == 2) //Only affective in 2D

    if(input_state.isRightBtnHold == true){
        ZSVECTOR3 cam_pos = edit_camera.getCameraPosition();
        cam_pos.X += relX;
        cam_pos.Y += relY;
        edit_camera.setPosition(cam_pos);
    }
}
