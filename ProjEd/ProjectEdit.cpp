#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include "ui_editor.h"
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <QDir>
#include <QDropEvent>
#include <QFileDialog>
#include <QShortcut>

#include "../include_engine.h" //include engine headers

static EditWindow* _editor_win;
static InspectorWin* _inspector_win;
static EdActions* _ed_actions_container;
static ZSpireEngine* engine;

EditWindow::EditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->fileList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onFileListItemClicked())); //Signal comes, when user clicks on file
    QObject::connect(ui->fileList, SIGNAL(onRightClick(QPoint)), this, SLOT(onFileCtxMenuShow(QPoint)));

    QObject::connect(ui->actionNew_Object, SIGNAL(triggered()),
                this, SLOT(onAddNewGameObject())); //Signal comes, when user clicks on Object->Create
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSceneSave())); //Signal comes, when user clicks on File->Save
    QObject::connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSceneSaveAs())); //Signal comes, when user clicks on File->Save As
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenScene()));

    QObject::connect(ui->actionCreateScene, SIGNAL(triggered()), this, SLOT(onNewScene()));

    QObject::connect(ui->actionClose_project, SIGNAL(triggered()), this, SLOT(onCloseProject()));
    QObject::connect(ui->actionBuild, SIGNAL(triggered(bool)), this, SLOT(onBuildProject()));
    QObject::connect(ui->actionRun, SIGNAL(triggered(bool)), this, SLOT(onRunProject()));

    QObject::connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(onUndoPressed()));
    QObject::connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(onRedoPressed()));

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
    world.obj_widget_ptr = ui->objsList;

    ui->fileList->setViewMode(QListView::IconMode);

    this->obj_ctx_menu = new ObjectCtxMenu(this); //Allocating object Context menu
    //Allocate file ctx menu
    this->file_ctx_menu = new FileCtxMenu(this);
    this->ui->objsList->win_ptr = this; //putting pointer to window to custom tree view

    ui->actionOpen->setShortcut(Qt::Key_O | Qt::CTRL);
    ui->actionSave->setShortcut(Qt::Key_S | Qt::CTRL);
    ui->actionUndo->setShortcut(Qt::Key_Z | Qt::CTRL);
    ui->actionRedo->setShortcut(Qt::Key_Y | Qt::CTRL);

    ui->actionBuild->setShortcut(Qt::Key_B | Qt::CTRL);
    ui->actionRun->setShortcut(Qt::Key_R | Qt::CTRL);

}

EditWindow::~EditWindow()
{
    delete ui;
}

void EditWindow::init(){

    input_state.isLeftBtnHold = false;
    input_state.isRightBtnHold = false;
    input_state.isLCtrlHold = false;
    input_state.isRCtrlHold = false;

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

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(16.0f);
    //If our proj is 3D, then enable depth test by default
    if(project.perspective == 3){
        glEnable(GL_DEPTH_TEST);
        render->depthTest = true;
    }

    render->setup();
    ready = true;//Everything is ready

    ZSPIRE::SFX::initAL();

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
    if(item->text().endsWith(".fbx") || item->text().endsWith(".FBX")){
        item->setIcon(QIcon::fromTheme("applications-graphics"));
    }
    if(item->text().endsWith(".wav") || item->text().endsWith(".WAV")){
        item->setIcon(QIcon::fromTheme("audio-x-generic"));
    }
}

void EditWindow::setViewDirectory(QString dir_path){

    this->current_dir = dir_path;
    this->updateFileList();
}
//Slots
void EditWindow::openFile(QString file_path){

    if(file_path.endsWith(".scn")){ //If it is scene
        obj_trstate.isTransforming = false;
        _ed_actions_container->clear();
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
    obj_trstate.isTransforming = false;
    hasSceneFile = false; //We have new scene
}

void EditWindow::onAddNewGameObject(){
    GameObject* obj_ptr = this->world.newObject(); //Add new object to world
    ui->objsList->addTopLevelItem(obj_ptr->item_ptr); //New object will not have parents, so will be spawned at top
}

void EditWindow::setupObjectsHieList(){
    QTreeWidget* w_ptr = ui->objsList; //Getting pointer to objects list widget
    w_ptr->clear(); //Clears widget
}

void EditWindow::onCloseProject(){
    world.clear(); //clear world
    SDL_DestroyWindow(window); //Destroy SDL and opengl
    SDL_GL_DeleteContext(glcontext);

    //Close Qt windows
    _editor_win->close();
    _inspector_win->close();

    delete render;
    _ed_actions_container->clear();
    ZSPIRE::SFX::destroyAL();

    this->ready = false; //won't render anymore
    this->close_reason = EW_CLOSE_REASON_PROJLIST;
}

void EditWindow::onBuildProject(){
    ProjBuilder builder(&this->project);

    builder.showWindow();
    builder.start();
}

void EditWindow::onRunProject(){
    ZSENGINE_CREATE_INFO engine_create_info;
    engine_create_info.appName = "GameEditorRun";
    engine_create_info.createWindow = false; //window already created, we don't need one
    engine_create_info.graphicsApi = OGL32; //use opengl

    engine = new ZSpireEngine(&engine_create_info, nullptr);
}

void EditWindow::updateFileList(){
    ui->fileList->clear(); //Clear widget content

    QDir directory (this->current_dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsFirst | QDir::Name | QDir::Reversed);

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
    this->obj_trstate.isTransforming = false; //disabling object transform
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item

    QString obj_name = selected_item->text(0); //Get label of clicked obj

    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
}
//Objects list right pressed
void EditWindow::onObjectCtxMenuShow(QPoint point){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    //We selected empty space
    if(selected_item == 0x0) return;
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->show(point);
}

void EditWindow::onFileCtxMenuShow(QPoint point){
    QListWidgetItem* selected_item = ui->fileList->currentItem(); //get selected item
    QString file_name = selected_item->text();

    this->file_ctx_menu->file_path = current_dir + "/" + file_name; //set file path
    this->file_ctx_menu->show(point);
}

void EditWindow::onCameraToObjTeleport(){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    TransformProperty* transform = obj_ptr->getTransformProperty(); //Obtain pointer to object transform
    //Define to store absolute transform
    ZSVECTOR3 _t = ZSVECTOR3(0.0f);
    ZSVECTOR3 _s = ZSVECTOR3(1.0f);
    ZSVECTOR3 _r = ZSVECTOR3(0.0f);
    transform->getAbsoluteParentTransform(_t, _s, _r); //Calculate absolute transform

    edit_camera._dest_pos = _t; //Sending position
    if(project.perspective == 3){ //if we're in 3D
        ZSVECTOR3 camFront = edit_camera.getCameraFrontVec();
        edit_camera._dest_pos = edit_camera._dest_pos - camFront * 6; //move back a little
    }
    edit_camera.startMoving();
}

void EditWindow::onUndoPressed(){
    _ed_actions_container->undo();
}
void EditWindow::onRedoPressed(){
    _ed_actions_container->redo();
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
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.type = RESOURCE_TYPE_TEXTURE; //Type is texture
                loadResource(&resource); //Perform texture loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".FBX") || name.endsWith(".fbx")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.type = RESOURCE_TYPE_MESH; //Type of resource is mesh
                loadResource(&resource); //Perform mesh processing & loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".WAV") || name.endsWith(".wav")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.type = RESOURCE_TYPE_AUDIO; //Type of resource is mesh
                //loadResource(&resource); //Perform mesh processing & loading to OpenGL
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
        case RESOURCE_TYPE_AUDIO:{
            resource->class_ptr = static_cast<void*>(new SoundBuffer); //Initialize pointer to sound buffer
            SoundBuffer* sound_ptr = static_cast<SoundBuffer*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            sound_ptr->loadFileWAV(str.c_str()); //Load music file
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

    _editor_win->close_reason = EW_CLOSE_REASON_UNCLOSED;
    _editor_win->lookForResources(_editor_win->project.root_path); //Make a vector of all resource files
    _editor_win->move(0,0); //Editor base win would be in the left part of screen
    _editor_win->show(); //Show editor window

    _inspector_win->show();
    _inspector_win->move(_editor_win->width() + 640, 0);

    _ed_actions_container = new EdActions; //Allocating EdActions
    _ed_actions_container->world_ptr = &_editor_win->world; //Put world pointer
    _ed_actions_container->insp_win = _inspector_win; //Put inspector win pointer

    return _editor_win;
}

InspectorWin* EditWindow::getInspector(){
    return _inspector_win;
}

void ObjectCtxMenu::setObjectPtr(GameObject* obj_ptr){
    this->obj_ptr = obj_ptr;
}

void ObjectCtxMenu::close(){
    menu->removeAction(action_move);
    menu->removeAction(action_scale);
    menu->removeAction(action_rotate);
}
//Object Ctx menu slots
void ObjectCtxMenu::onDeleteClicked(){
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    GameObjectLink link = obj_ptr->getLinkToThisObject();
    win_ptr->obj_trstate.isTransforming = false; //disabling object transform
    win_ptr->callObjectDeletion(link);
}
void ObjectCtxMenu::onDublicateClicked(){
    //Make snapshot actions
    _ed_actions_container->newSnapshotAction(&win_ptr->world);
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    GameObjectLink link = obj_ptr->getLinkToThisObject();
    GameObject* result = win_ptr->world.dublicateObject(link.ptr);

    if(result->hasParent){ //if object parented
        result->parent.ptr->item_ptr->addChild(result->item_ptr);
    }else{
        win_ptr->ui->objsList->addTopLevelItem(result->item_ptr);
    }
}

void ObjectCtxMenu::onMoveClicked(){
    //win_ptr->getInspector()->clearContentLayout(); //Detach object from inspector

    win_ptr->obj_trstate.isTransforming = true; //Transform operation
    win_ptr->obj_trstate.obj_ptr = obj_ptr; //Sending object ptr
    //Setting pointer to transform property
    win_ptr->obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    win_ptr->obj_trstate.transformMode = GO_TRANSFORM_MODE_TRANSLATE; //Setting transform type
}
void ObjectCtxMenu::onScaleClicked(){
    //win_ptr->getInspector()->clearContentLayout(); //Detach object from inspector

    win_ptr->obj_trstate.isTransforming = true; //Transform operation
    win_ptr->obj_trstate.obj_ptr = obj_ptr; //Sending object ptr
    //Setting pointer to transform property
    win_ptr->obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    win_ptr->obj_trstate.transformMode = GO_TRANSFORM_MODE_SCALE;//Setting transform type
}
void ObjectCtxMenu::onRotateClicked(){
    //win_ptr->getInspector()->clearContentLayout(); //Detach object from inspector

    win_ptr->obj_trstate.isTransforming = true; //Transform operation
    win_ptr->obj_trstate.obj_ptr = obj_ptr; //Sending object ptr
    //Setting pointer to transform property
    win_ptr->obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    win_ptr->obj_trstate.transformMode = GO_TRANSFORM_MODE_ROTATE;//Setting transform type
}

void ObjTreeWgt::dropEvent(QDropEvent* event){
    _ed_actions_container->newSnapshotAction(&win_ptr->world); //Add new snapshot action
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
    if(obj_trstate.isTransforming) return;
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label
    if(clicked > world.objects.size() || obj_ptr == 0x0 || clicked >= 256 * 256 * 256)
        return;

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
    this->ui->objsList->setCurrentItem(obj_ptr->item_ptr); //item selected in tree
}
void EditWindow::onRightBtnClicked(int X, int Y){
    this->obj_trstate.isTransforming = false; //disabling object transform
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label
    if(clicked > world.objects.size() || obj_ptr == 0x0 || clicked >= 256 * 256 * 256)
        return;
    world.unpickObject(); //Clear isPicked property from all objects
    obj_ptr->pick(); //mark object picked
    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->displayTransforms = true;
    this->obj_ctx_menu->show(QPoint(this->width() + X, Y));
    this->obj_ctx_menu->displayTransforms = false;
}
void EditWindow::onMouseWheel(int x, int y){
    if(project.perspective == 3){
        ZSVECTOR3 front = edit_camera.getCameraFrontVec(); //obtain front vector
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position

        edit_camera.setPosition(pos + front * y);
    }
    if(project.perspective == 2){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos.Y += y * 5;
        pos.X += x * 5;

        edit_camera.setPosition(pos);
    }
}
void EditWindow::onMouseMotion(int relX, int relY){
    if(project.perspective == 2){ //Only affective in 2D

        if(input_state.isRightBtnHold == true){ //we just move on map
            ZSVECTOR3 cam_pos = edit_camera.getCameraPosition();
            cam_pos.X += relX;
            cam_pos.Y += relY;
            edit_camera.setPosition(cam_pos);
        }

        if(obj_trstate.isTransforming == true && input_state.isLeftBtnHold == true){ //Only affective if object is transforming

            ZSVECTOR3 dir = ZSVECTOR3(0.0f);

            dir = ZSVECTOR3(-relX, -relY, 0);

            if ((abs(relX) > abs(relY)) && abs(relX - relY) > 3)
                dir = ZSVECTOR3(-relX, 0, 0);
            if ((abs(relX) < abs(relY)) && abs(relX - relY) > 3)
                dir = ZSVECTOR3(0, -relY, 0);
            //if we translating
            if(obj_trstate.transformMode == GO_TRANSFORM_MODE_TRANSLATE){
                obj_trstate.tprop_ptr->translation = obj_trstate.tprop_ptr->translation + dir;
            }
            //if we scaling
            if(obj_trstate.transformMode == GO_TRANSFORM_MODE_SCALE){
                obj_trstate.tprop_ptr->scale = obj_trstate.tprop_ptr->scale + dir / 3;
            }
            if(obj_trstate.transformMode == GO_TRANSFORM_MODE_ROTATE){
                obj_trstate.tprop_ptr->rotation = obj_trstate.tprop_ptr->rotation + dir / 3;
            }

            obj_trstate.tprop_ptr->updateMat();
            getInspector()->updateObjectProperties();
        }
    }
    if(project.perspective == 3){//Only affective in 3D

        if(input_state.isRightBtnHold == true){
            this->cam_yaw += relX * 0.16f;
            cam_pitch += relY * 0.16f;

                if (cam_pitch > 89.0f)
                    cam_pitch = 89.0f;
                if (cam_pitch < -89.0f)
                    cam_pitch = -89.0f;

            ZSVECTOR3 front;
            front.X = (float)(cos(DegToRad(cam_yaw)) * cos(DegToRad(cam_pitch)));
            front.Y = -sin(DegToRad(cam_pitch));
            front.Z = sin(DegToRad(cam_yaw)) * cos(DegToRad(cam_pitch));
            vNormalize(&front);
            edit_camera.setFront(front);
        }
    }
}

void EditWindow::onKeyDown(SDL_Keysym sym){
    if(sym.sym == SDLK_a){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * -0.2f;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_d){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * 0.2f;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_w){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos.Y += 0.2f;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_s){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos.Y -= 0.2f;
        edit_camera.setPosition(pos);
    }

    if(sym.sym == SDLK_t){
        getInspector()->clearContentLayout(); //Detach object from
        this->obj_trstate.transformMode = GO_TRANSFORM_MODE_TRANSLATE;
        this->obj_trstate.isTransforming = true;
    }
    if(sym.sym == SDLK_e){
        getInspector()->clearContentLayout(); //Detach object from
        this->obj_trstate.transformMode = GO_TRANSFORM_MODE_SCALE;
        this->obj_trstate.isTransforming = true;
    }
    if(sym.sym == SDLK_r){
        getInspector()->clearContentLayout(); //Detach object from
        this->obj_trstate.transformMode = GO_TRANSFORM_MODE_ROTATE;
        this->obj_trstate.isTransforming = true;
    }

    if(sym.sym == SDLK_DELETE){
        GameObjectLink link = this->obj_trstate.obj_ptr->getLinkToThisObject();
        callObjectDeletion(link);
    }

    if(input_state.isLCtrlHold && sym.sym == SDLK_o){
        emit onOpenScene();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_s){
        emit onSceneSave();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_z){
        emit onUndoPressed();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_y){
        emit onRedoPressed();
    }
}

void EditWindow::callObjectDeletion(GameObjectLink link){
    _ed_actions_container->newSnapshotAction(&this->world);
    world.removeObj(link); //delete object
    this->obj_trstate.isTransforming = false; //disabling object transform
    getInspector()->clearContentLayout(); //Detach object from inspector
}

EdActions* getActionManager(){
    return _ed_actions_container;
}
