#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "../World/headers/obj_properties.h"
#include "../World/headers/2dtileproperties.h"
#include "../Misc/headers/AssimpMeshLoader.h"
#include "ui_editor.h"
#include <iostream>
#include <fstream>
#include <QDir>
#include <QDropEvent>
#include <QFileDialog>
#include <QShortcut>
#include <QDesktopServices>
#include <QMessageBox>
#include <mainwin.h>

EditWindow* _editor_win;
InspectorWin* _inspector_win;
EdActions* _ed_actions_container;

RenderPipeline* renderer;

EditWindow::EditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->fileList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onFileListItemClicked())); //Signal comes, when user clicks on file
    QObject::connect(ui->fileList, SIGNAL(onRightClick(QPoint)), this, SLOT(onFileCtxMenuShow(QPoint)));

    QObject::connect(ui->actionNew_Object, SIGNAL(triggered()),
                this, SLOT(onAddNewGameObject())); //Signal comes, when user clicks on Scene->Create
    QObject::connect(ui->actionToggle_Cameras, SIGNAL(triggered()), this, SLOT(toggleCameras()));

    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSceneSave())); //Signal comes, when user clicks on File->Save
    QObject::connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSceneSaveAs())); //Signal comes, when user clicks on File->Save As
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenScene()));

    QObject::connect(ui->actionCreateScene, SIGNAL(triggered()), this, SLOT(onNewScene()));
    QObject::connect(ui->actionCreateMaterial, SIGNAL(triggered()), this, SLOT(onNewMaterial()));
    QObject::connect(ui->actionCreateScript, SIGNAL(triggered()), this, SLOT(onNewScript()));

    QObject::connect(ui->actionClose_project, SIGNAL(triggered()), this, SLOT(onCloseProject()));
    QObject::connect(ui->actionBuild, SIGNAL(triggered(bool)), this, SLOT(onBuildProject()));
    QObject::connect(ui->actionRun, SIGNAL(triggered(bool)), this, SLOT(onRunProject()));

    QObject::connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(onUndoPressed()));
    QObject::connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(onRedoPressed()));

    QObject::connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(onObjectCopy()));
    QObject::connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(onObjectPaste()));

    QObject::connect(ui->objsList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(onObjectListItemClicked())); //Signal comes, when user clicks on File->Save As

    QObject::connect(ui->objsList, SIGNAL(onRightClick(QPoint)), this, SLOT(onObjectCtxMenuShow(QPoint)));
    QObject::connect(ui->objsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onCameraToObjTeleport()));

    //Create new something (actions)
    QObject::connect(ui->actionNew_Cube, SIGNAL(triggered()), this, SLOT(addNewCube()));
    QObject::connect(ui->actionNew_Light, SIGNAL(triggered()), this, SLOT(addNewLight()));
    QObject::connect(ui->actionNew_Tile, SIGNAL(triggered()), this, SLOT(addNewTile()));
    QObject::connect(ui->actionNew_Terrain, SIGNAL(triggered()), this, SLOT(addNewTerrain()));

    QObject::connect(ui->actionRender_settings, SIGNAL(triggered()), this, SLOT(openRenderSettings()));
    QObject::connect(ui->actionPhysics_Settings, SIGNAL(triggered()), this, SLOT(openPhysicsSettings()));

    ready = false; //Firstly set it to 0
    hasSceneFile = false; //No scene loaded by default
    isSceneRun = false; //Not running by default
    isWorldCamera = false;
    object_buffer = nullptr;

    setupObjectsHieList();
    //Drag & drop in objects tree
    ui->objsList->setAcceptDrops(true);
    ui->objsList->setDragEnabled(true);

    //Drag & drop in file list
    ui->fileList->setAcceptDrops(true);
    ui->fileList->setDragEnabled(true);
    ui->fileList->setDragDropMode(QAbstractItemView::InternalMove);

    ui->objsList->world_ptr = &world;

    world.proj_ptr = &project; //Assigning project pointer into world's variable
    world.obj_widget_ptr = ui->objsList;

    ui->fileList->setViewMode(QListView::IconMode);

    this->obj_ctx_menu = new ObjectCtxMenu(this); //Allocating object Context menu
    //Allocate file ctx menu
    this->file_ctx_menu = new FileCtxMenu(this);

    this->ui->objsList->win_ptr = this; //putting pointer to window to custom tree view
    this->ui->fileList->win_ptr = this;

    ui->actionCopy->setShortcut(Qt::Key_C | Qt::CTRL);
    ui->actionPaste->setShortcut(Qt::Key_V | Qt::CTRL);

    ui->actionOpen->setShortcut(Qt::Key_O | Qt::CTRL);
    ui->actionSave->setShortcut(Qt::Key_S | Qt::CTRL);
    ui->actionUndo->setShortcut(Qt::Key_Z | Qt::CTRL);
    ui->actionRedo->setShortcut(Qt::Key_Y | Qt::CTRL);

    ui->actionBuild->setShortcut(Qt::Key_B | Qt::CTRL);
    ui->actionRun->setShortcut(Qt::Key_R | Qt::CTRL);

    ui->actionNew_Object->setShortcut(Qt::Key_N | Qt::CTRL);
    ui->actionToggle_Cameras->setShortcut(Qt::Key_Apostrophe | Qt::CTRL);

    this->glcontext = nullptr;
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
    input_state.isLAltHold = false;
    input_state.isMidBtnHold = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    std::cout << "SDL window creation requested" << std::endl;
    if(this->settings.isFirstSetup){
        this->settings.gameView_win_pos_x = this->width();
        this->settings.gameView_win_pos_y = 0;

        this->settings.editor_win_width = this->width();
        this->settings.editor_win_height = this->height();

        this->settings.inspector_win_width = _inspector_win->width();
        this->settings.inspector_win_height = _inspector_win->height();
    }

    this->window = SDL_CreateWindow("Game View", this->settings.gameView_win_pos_x, this->settings.gameView_win_pos_y, settings.gameViewWin_Width, settings.gameViewWin_Height, SDL_WINDOW_OPENGL); //Create window
    this->glcontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowResizable(window, SDL_TRUE);
    std::cout << "SDL - GL context creation requested!" << std::endl;

    //init render
    render = new RenderPipeline;
    this->startManager(render);
    renderer = render;
    //init glyph manager
    this->glyph_manager = new GlyphManager;
    glyph_manager->pipeline_ptr = render;
    this->startManager(glyph_manager);

    this->thumb_master = new ThumbnailsMaster;
    this->startManager(thumb_master);

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
            edit_camera.setZplanes(0.1f, 2000.f);
            break;
        }
    }
    ZSVIEWPORT viewport = ZSVIEWPORT(0,0,static_cast<unsigned int>(this->settings.gameViewWin_Width),static_cast<unsigned int>( this->settings.gameViewWin_Height));
    edit_camera.setViewport(viewport);

    world.world_camera = edit_camera;
    world.world_camera.isAlListenerCamera = true;
}

void EditWindow::assignIconFile(QListWidgetItem* item){
    item->setIcon(QIcon::fromTheme("application-x-executable"));
    if(item->text().endsWith(".txt") || item->text().endsWith(".inf") || item->text().endsWith(".scn")){
        item->setIcon(QIcon::fromTheme("text-x-generic"));
    }
    if(item->text().endsWith(".dds") || item->text().endsWith(".DDS")){
        QString path = this->current_dir + "/" + item->text();
        QImage* img = thumb_master->texture_thumbnails.at(path.toStdString());
        item->setIcon(QIcon(QPixmap::fromImage(*img)));
    }
    if(item->text().endsWith(".fbx") || item->text().endsWith(".FBX")){
        item->setIcon(QIcon::fromTheme("applications-graphics"));
    }
    if(item->text().endsWith(".wav") || item->text().endsWith(".WAV")){
        item->setIcon(QIcon::fromTheme("audio-x-generic"));
    }
    if(item->text().endsWith(".lua")){
        item->setIcon(QIcon::fromTheme("text-x-script"));
    }
    if(item->text().endsWith(".zsmat") || item->text().endsWith(".ZSMAT")){
        //QString path = this->current_dir + "/" + item->text();
        //QImage* img = thumb_master->texture_thumbnails.at(path.toStdString());
        //item->setIcon(QIcon(QPixmap::fromImage(*img)));
    }
}

void EditWindow::setViewDirectory(QString dir_path){

    this->current_dir = dir_path;
    this->updateFileList();
}
//Slots
void EditWindow::openFile(QString file_path){

    if(file_path.endsWith(".scn")){ //If it is scene
        //Unpick object first
        world.unpickObject();
        QString gl_win_title = "Game View - " + file_path;
        SDL_SetWindowTitle(window, gl_win_title.toUtf8());

        if(isSceneRun == true)
            emit onRunProject();

        obj_trstate.isTransforming = false;
        ppaint_state.enabled = false;
        object_buffer = nullptr;
        //Back render settings to defaults
        this->render->getRenderSettings()->defaults();

        _ed_actions_container->clear();
        setupObjectsHieList(); //Clear everything, at first
        world.openFromFile(file_path, ui->objsList); //Open this scene

        scene_path = file_path; //Assign scene path
        hasSceneFile = true; //Scene is saved
        this->edit_camera.setPosition(ZSVECTOR3(0.0f, 0.0f, 0.0f)); //Set camera to 0
        _inspector_win->clearContentLayout(); //Clear content, if not empty
    }else{
#ifdef _WIN32
        QDesktopServices::openUrl(QUrl::fromLocalFile("file://" + file_path));
#endif
#ifdef __linux
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
#endif
    }
}

void EditWindow::addFileToObjectList(QString file_path){
    if(file_path.endsWith(".prefab")){
        this->world.addObjectsFromPrefab(file_path);
    }
    if(file_path.endsWith(".fbx") || file_path.endsWith(".FBX")){
        this->world.addMeshGroup(file_path.toStdString());
    }
}

QString EditWindow::getCurrentDirectory(){
    return this->current_dir;
}

QString EditWindow::createNewTextFile(QString directory, QString name, QString ext, std::string content){
    int addition_number = 0;
    QString newfile_name = directory + "/" + name + "_" + QString::number(addition_number) + ext;
    //Iterate until we find free name
    while(QFile::exists(newfile_name)){
        addition_number += 1;
        newfile_name = directory + "/" + name + "_" + QString::number(addition_number) + ext;
    }
    //create new file and write content to it.
    std::ofstream newfile_stream;
    newfile_stream.open(newfile_name.toStdString(), std::ofstream::out);
    newfile_stream << content;
    newfile_stream.close();

    return newfile_name;
}

void EditWindow::onSceneSaveAs(){
    _ed_actions_container->hasChangesUnsaved = false;

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
    if(isSceneRun) return; //No save during scene run

    if(hasSceneFile == false){ //If new created scene without file
        onSceneSaveAs(); //Show dialog and save
    }else{
        _ed_actions_container->hasChangesUnsaved = false;
        world.saveToFile(this->scene_path);
    }
}

void EditWindow::onNewScene(){
    setupObjectsHieList();
    world.clear();

    if(isSceneRun == true)
        emit onRunProject();

    ppaint_state.enabled = false;
    obj_trstate.isTransforming = false;
    //clear actions history
    _ed_actions_container->clear();

    //Back render settings to defaults
    this->render->getRenderSettings()->defaults();

    hasSceneFile = false; //We have new scene
}

void EditWindow::onNewScript(){
    std::string scriptContent = "onStart = function(g_object, world)\n  return 0\nend\n\n";
            scriptContent +=  "onFrame = function(frameTime)\n  return 0\nend";
    this->createNewTextFile(current_dir, "Script", ".lua",scriptContent);

    updateFileList(); //Make new file visible
}
void EditWindow::onNewMaterial(){
    std::string matContent = "ZSP_MATERIAL\nGROUP @default\n";
    QString picked_name = this->createNewTextFile(current_dir, "Material", ".zsmat", matContent);
    //Rgister new material in list
    Resource resource;
    resource.file_path = picked_name;
    resource.rel_path = picked_name; //Preparing to get relative path
    resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
    resource.resource_label = resource.rel_path.toStdString();
    resource.type = RESOURCE_TYPE_MATERIAL; //Type of resource is mesh
    loadResource(&resource); //Perform mesh processing & loading to OpenGL
    this->project.resources.push_back(resource);

    updateFileList(); //Make new file visible
}

void EditWindow::openRenderSettings(){
    _inspector_win->clearContentLayout(); //clear everything, that was before

    RenderSettings* ptr = this->render->getRenderSettings();

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Ambient light color");
    lcolor->color = &ptr->ambient_light_color;
    _inspector_win->addPropertyArea(lcolor);

    PropertyPickArea* sky_ppickarea = new PropertyPickArea(GO_PROPERTY_TYPE_SKYBOX);
    sky_ppickarea->property_ptr_ptr = reinterpret_cast<GameObjectProperty**>(&ptr->skybox_ptr);
    sky_ppickarea->setLabel("Skybox");
    _inspector_win->addPropertyArea(sky_ppickarea);

    PropertyPickArea* shadow_ppickarea = new PropertyPickArea(GO_PROPERTY_TYPE_SHADOWCASTER);
    shadow_ppickarea->property_ptr_ptr = reinterpret_cast<GameObjectProperty**>(&ptr->shadowcaster_ptr);
    shadow_ppickarea->setLabel("Shadowcaster");
    _inspector_win->addPropertyArea(shadow_ppickarea);
}

void EditWindow::openPhysicsSettings(){
    _inspector_win->clearContentLayout(); //clear everything, that was before

    PhysicalWorldSettings* ptr = &this->world.phys_settngs;

    Float3PropertyArea* float3_area = new Float3PropertyArea;
    float3_area->setLabel("Gravity"); //Its label
    float3_area->vector = &ptr->gravity;
    float3_area->go_property = static_cast<void*>(this);
    _inspector_win->addPropertyArea(float3_area);

}

GameObject* EditWindow::onAddNewGameObject(){
    //get free index in array
    int free_ind = world.getFreeObjectSpaceIndex();
    //if we have no free space inside array
    if(free_ind == static_cast<int>(world.objects.size())){
        GameObject obj;
        obj.alive = false;
        obj.world_ptr = &world;
        obj.array_index = free_ind;
        world.objects.push_back(obj);
    }
    //Create action
    _ed_actions_container->newGameObjectAction(world.objects[static_cast<unsigned int>(free_ind)].getLinkToThisObject());

    GameObject* obj_ptr = this->world.newObject(); //Add new object to world
    ui->objsList->addTopLevelItem(obj_ptr->item_ptr); //New object will not have parents, so will be spawned at top

    return obj_ptr;
}

void EditWindow::addNewCube(){
    GameObject* obj = onAddNewGameObject();
    obj->addProperty(GO_PROPERTY_TYPE_MESH);
    obj->addProperty(GO_PROPERTY_TYPE_MATERIAL);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Cube_", &add_num);
    *obj->label = "Cube_" + QString::number(add_num);
    obj->item_ptr->setText(0, *obj->label);

    MeshProperty* mesh = static_cast<MeshProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    mesh->resource_relpath = "@cube";
    mesh->updateMeshPtr();
}
void EditWindow::addNewLight(){
    GameObject* obj = onAddNewGameObject();
    obj->addProperty(GO_PROPERTY_TYPE_LIGHTSOURCE);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Light_", &add_num);
    *obj->label = "Light_" + QString::number(add_num);
    obj->item_ptr->setText(0, *obj->label);
}

void EditWindow::addNewTile(){
    GameObject* obj = onAddNewGameObject();
    obj->addProperty(GO_PROPERTY_TYPE_TILE); //Creates tile inside
    obj->addProperty(GO_PROPERTY_TYPE_MESH); //Creates mesh inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Tile_", &add_num);
    *obj->label = "Tile_" + QString::number(add_num);
    obj->item_ptr->setText(0, *obj->label);
    //Assign @mesh
    MeshProperty* mesh = static_cast<MeshProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));
    mesh->resource_relpath = "@plane";
    mesh->updateMeshPtr();
    //Assign new scale
    TransformProperty* transform =
            static_cast<TransformProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    transform->scale = ZSVECTOR3(100, 100, 1);
    transform->updateMat();
}
void EditWindow::addNewTerrain(){
    GameObject* obj = onAddNewGameObject();

    obj->addProperty(GO_PROPERTY_TYPE_MATERIAL); //Creates material inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Terrain_", &add_num);
    *obj->label = "Terrain_" + QString::number(add_num);
    obj->item_ptr->setText(0, *obj->label);
    //Add terrain property
    obj->addProperty(GO_PROPERTY_TYPE_TERRAIN); //Creates terrain inside
    obj->getPropertyPtr<TerrainProperty>()->onAddToObject();

    updateFileList();

}
void EditWindow::setupObjectsHieList(){
    QTreeWidget* w_ptr = ui->objsList; //Getting pointer to objects list widget
    w_ptr->clear(); //Clears widget
}

bool EditWindow::onCloseProject(){
    QMessageBox::StandardButton reply = QMessageBox::Yes;
    if(_ed_actions_container->hasChangesUnsaved)
        reply = QMessageBox::question(this, "Are you sure to quit?", "You have unsaved changes. Do you really want to quit?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        world.clear(); //clear world
        SDL_DestroyWindow(window); //Destroy SDL and opengl
        SDL_GL_DeleteContext(glcontext);

        MtShProps::clearMtShaderGroups();
        //clear all resources
        this->project.resources.clear(); //Clear resources list
        //Show main menu window to avoid crash
        MainWin* win = static_cast<MainWin*>(this->mainwin_ptr);
        win->show();

        //Close Qt windows
        _editor_win->close();
        _inspector_win->close();

        destroyAllManagers();

        _ed_actions_container->clear();
        ZSPIRE::SFX::destroyAL();

        this->ready = false; //won't render anymore

        return true;
    }
    return false;
}

void EditWindow::onBuildProject(){
    ProjBuilder builder(&this->project);

    builder.showWindow();
    builder.start();
}

void EditWindow::runWorld(){
    _ed_actions_container->setStoreActions(false);
    isSceneRun = true; //set toggle to true
    isWorldCamera = true;

    world.physical_world = new PhysicalWorld(&world.phys_settngs);
}
void EditWindow::stopWorld(){
    //Prepare world for stopping
    for(unsigned int object_i = 0; object_i < world.objects.size(); object_i ++){
        GameObject* object_ptr = &world.objects[object_i];
        //Obtain script
        ScriptGroupProperty* script_ptr = static_cast<ScriptGroupProperty*>(object_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_SCRIPTGROUP));
        if(script_ptr != nullptr)
            script_ptr->shutdown(); //stop all scripts
    }

    _ed_actions_container->setStoreActions(true);
    //Clear Inspector Win
    _inspector_win->clearContentLayout();
    isWorldCamera = false;
    isSceneRun = false; //set toggle to true
}

void EditWindow::sheduleWorldLoad(QString file_path){
    this->sheduled_world = file_path;
    hasSheduledWorld = true;
}

void EditWindow::onRunProject(){
    if(isSceneRun == false){ //if we are Not running scene
        this->world.putToShapshot(&run_world_snapshot); //create snapshot of current state to recover it later
        //perform world activity startup
        runWorld();
        //Change button text
        this->ui->actionRun->setText("Stop");

    }else{ //lets stop scene run
        //return base window size
        SDL_SetWindowSize(this->window, this->settings.gameViewWin_Width, this->settings.gameViewWin_Height);
        SDL_SetWindowFullscreen(this->window, 0);
        //Stop world
        stopWorld();
        //Recover world snapshot
        this->world.recoverFromSnapshot(&run_world_snapshot); //create snapshot of current state to recover it later
        run_world_snapshot.clear(); //Clear snapshot to free up memory
        //Change button text
        this->ui->actionRun->setText("Run");
    }
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
    //if user pressed back button
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
    if(selected_item == nullptr) return;
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->show(point);
}

void EditWindow::onFileCtxMenuShow(QPoint point){
    QListWidgetItem* selected_item = ui->fileList->currentItem(); //get selected item
    if(selected_item != nullptr){ //if selected file
        QString file_name = selected_item->text();

        this->file_ctx_menu->file_path = current_dir + "/" + file_name; //set file path
        this->file_ctx_menu->file_name = file_name; //set file name
        this->file_ctx_menu->directory = this->current_dir; //set dirctory
        this->file_ctx_menu->show(point); //show menu
    }
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

void EditWindow::onObjectCopy(){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    GameObject* obj_ptr = world.getObjectByLabel(obj_name); //Obtain pointer to selected object by label

    this->object_buffer = obj_ptr;
}
void EditWindow::onObjectPaste(){
    if(object_buffer != nullptr)
        this->world.Instantiate(object_buffer);
}

void EditWindow::toggleCameras(){
    //This slot toggles cameras
    if(this->isWorldCamera){
        this->isWorldCamera = false;
    }else{
        this->isWorldCamera = true;
    }
}

void EditWindow::glRender(){
    this->settings.inspector_win_pos_X = _inspector_win->pos().x();
    this->settings.inspector_win_pos_Y = _inspector_win->pos().y();

    if(isSceneRun){
        world.physical_world->stepSimulation(deltaTime);
    }

    if(hasSheduledWorld){
        stopWorld(); //firstly, stop world
        //load world
        ui->objsList->clear();
        world.openFromFile(this->sheduled_world, world.obj_widget_ptr);
        //run loaded world
        runWorld();

        hasSheduledWorld = false;
    }

    if(ready == true && !hasSheduledWorld) //if opengl ready, then render scene
        render->render(this->window, static_cast<void*>(this));
}

RenderPipeline* EditWindow::getRenderPipeline(){
    return render;
}

GlyphFontContainer* EditWindow::getFontContainer(std::string label){
    return this->glyph_manager->getFontContainer(label);
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
            if(name.endsWith(".ttf") || name.endsWith(".TTF")){
                GlyphFontContainer* gf_container = new GlyphFontContainer(fileInfo.absoluteFilePath().toStdString(), 48, this->glyph_manager);
                //load font
                gf_container->loadGlyphs();
                //register font
                glyph_manager->addFontContainer(gf_container);
            }
            if(name.endsWith(".DDS") || name.endsWith(".dds")){ //If its an texture
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath(); //Writing full path
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.resource_label = resource.rel_path.toStdString();
                resource.type = RESOURCE_TYPE_TEXTURE; //Type is texture
                loadResource(&resource); //Perform texture loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".FBX") || name.endsWith(".fbx")){ //If its an mesh
                //getting meshes amount
                unsigned int num_meshes = Engine::getMeshesAmount(fileInfo.absoluteFilePath().toStdString());
                //iterate to read all the meshes
                for(unsigned int mesh_i = 0; mesh_i < num_meshes; mesh_i ++){
                    Resource resource;
                    resource.file_path = fileInfo.absoluteFilePath();
                    resource.rel_path = resource.file_path; //Preparing to get relative path
                    resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                    resource.type = RESOURCE_TYPE_MESH; //Type of resource is mesh

                    resource.class_ptr = static_cast<void*>(new ZSPIRE::Mesh);

                    this->project.resources.push_back(resource);
                    Engine::loadMesh(fileInfo.absoluteFilePath().toStdString(), static_cast<ZSPIRE::Mesh*>(this->project.resources.back().class_ptr), static_cast<int>(mesh_i));
                    this->project.resources.back().resource_label = static_cast<ZSPIRE::Mesh*>(this->project.resources.back().class_ptr)->mesh_label;
                }
            }
            if(name.endsWith(".WAV") || name.endsWith(".wav")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.resource_label = resource.rel_path.toStdString();
                resource.type = RESOURCE_TYPE_AUDIO; //Type of resource is mesh
                loadResource(&resource); //Perform mesh processing & loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".ZSMAT") || name.endsWith(".zsmat")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.resource_label = resource.rel_path.toStdString();
                resource.type = RESOURCE_TYPE_MATERIAL; //Type of resource is mesh
                loadResource(&resource); //Perform mesh processing & loading to OpenGL
                this->project.resources.push_back(resource);
            }
            if(name.endsWith(".lua") || name.endsWith(".LUA") || name.endsWith(".ZSCR") || name.endsWith(".zscr")){ //If its an mesh
                Resource resource;
                resource.file_path = fileInfo.absoluteFilePath();
                resource.rel_path = resource.file_path; //Preparing to get relative path
                resource.rel_path.remove(0, project.root_path.size() + 1); //Get relative path by removing length of project root from start
                resource.resource_label = resource.rel_path.toStdString();
                resource.type = RESOURCE_TYPE_SCRIPT; //Type of resource is mesh
                loadResource(&resource); //Perform mesh processing & loading to OpenGL
                this->project.resources.push_back(resource);
            }
        }

        if(fileInfo.isDir() == true){ //If it is directory
            QString newdir_str = path + "/" + fileInfo.fileName();
            lookForResources(newdir_str); //Call this function inside next dir
        }
    }
}

void EditWindow::loadResource(Resource* resource){
    switch(resource->type){
        case RESOURCE_TYPE_SCRIPT : {
            break;
        }
        case RESOURCE_TYPE_NONE:{
            break;
        }
        case RESOURCE_TYPE_FILE:{
            break;
        }
        case RESOURCE_TYPE_TEXTURE:{ //If resource type is texture
            resource->class_ptr = static_cast<void*>(new ZSPIRE::Texture); //Initialize pointer to texture
            ZSPIRE::Texture* texture_ptr = static_cast<ZSPIRE::Texture*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            texture_ptr->LoadDDSTextureFromFile(str.c_str()); //Perform texture resource loading
            break;
        }
        case RESOURCE_TYPE_MESH:{

            break;
        }
        case RESOURCE_TYPE_AUDIO:{
            resource->class_ptr = static_cast<void*>(new SoundBuffer); //Initialize pointer to sound buffer
            SoundBuffer* sound_ptr = static_cast<SoundBuffer*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            sound_ptr->loadFileWAV(str.c_str()); //Load music file
            break;
        }
        case RESOURCE_TYPE_MATERIAL:{
            resource->class_ptr = static_cast<void*>(new Material); //Initialize pointer to sound buffer
            Material* mat_ptr = static_cast<Material*>(resource->class_ptr); //Aquire casted pointer
            std::string str = resource->file_path.toStdString();
            mat_ptr->loadFromFile(str); //Load music file
            break;
        }
    }
}

EditWindow* ZSEditor::openProject(Project& project){
    _editor_win = new EditWindow(); //Creating class object
    _inspector_win = new InspectorWin();
    //Send project datas to editor window class
    _editor_win->project = project;
    //Update widget content

    return openEditor(); //Return pointer to edit window


}
EditWindow* ZSEditor::openEditor(){
    EditorSettingsManager* settings_manager = new EditorSettingsManager(&_editor_win->settings);
    _editor_win->startManager(settings_manager);

    _editor_win->init();

    _editor_win->close_reason = EW_CLOSE_REASON_UNCLOSED;
    _editor_win->lookForResources(_editor_win->project.root_path); //Make a vector of all resource files
    _editor_win->move(0,0); //Editor base win would be in the left part of screen
    //resize editor window
    _editor_win->resize(_editor_win->settings.editor_win_width, _editor_win->settings.editor_win_height);
    _editor_win->show(); //Show editor window
    //If no settings, then set to defaults
    if(_editor_win->settings.isFirstSetup){
        _editor_win->settings.inspector_win_pos_X = _editor_win->width() + _editor_win->settings.gameViewWin_Width;
        _editor_win->settings.inspector_win_pos_Y = 0;
    }

    //Move window
    _inspector_win->move(_editor_win->settings.inspector_win_pos_X, _editor_win->settings.inspector_win_pos_Y);
    _inspector_win->resize(_editor_win->settings.inspector_win_width, _editor_win->settings.inspector_win_height);
    _inspector_win->show();

    _inspector_win->editwindow_ptr = static_cast<void*>(_editor_win);

    _ed_actions_container = new EdActions; //Allocating EdActions
    _ed_actions_container->world_ptr = &_editor_win->world; //Put world pointer
    _ed_actions_container->insp_win = _inspector_win; //Put inspector win pointer
    _ed_actions_container->setStoreActions(true);
    //Create thumbnails
    _editor_win->thumb_master->createTexturesThumbnails();
    //_editor_win->thumb_master->createMaterialThumbnails();

    _editor_win->setViewDirectory(_editor_win->project.root_path);

    return _editor_win;
}

InspectorWin* EditWindow::getInspector(){
    return _inspector_win;
}


void EditWindow::onLeftBtnClicked(int X, int Y){
    //Terrain painting
    if(_inspector_win->gameobject_ptr != nullptr){
        GameObject* obj = static_cast<GameObject*>(_inspector_win->gameobject_ptr);
        TerrainProperty* terrain = obj->getPropertyPtr<TerrainProperty>();

        if(terrain != nullptr)
            terrain->onMouseClick(this->input_state.mouseX, input_state.mouseY,  settings.gameViewWin_Height, this->input_state.isLeftBtnHold,
                                 this->input_state.isLCtrlHold);
    }

    //Stop camera moving
    this->edit_camera.stopMoving();
    this->obj_ctx_menu->close(); //Close ctx menu

    if(obj_trstate.isTransforming || isWorldCamera) return;
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    if(clicked > world.objects.size() || clicked >= 256 * 256 * 256){
        world.unpickObject();
        return;
    }
    GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
    this->ui->objsList->setCurrentItem(obj_ptr->item_ptr); //item selected in tree
}
void EditWindow::onRightBtnClicked(int X, int Y){
    if(isWorldCamera) return;

    //Stop camera moving
    this->edit_camera.stopMoving();

    this->obj_trstate.isTransforming = false; //disabling object transform
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    if(clicked > world.objects.size() || clicked >= 256 * 256 * 256)
        return;
    GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label

    world.unpickObject(); //Clear isPicked property from all objects
    obj_ptr->pick(); //mark object picked
    this->obj_trstate.obj_ptr = obj_ptr;

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->displayTransforms = true;
    this->obj_ctx_menu->show(QPoint(settings.gameView_win_pos_x + X, settings.gameView_win_pos_y + Y));
    this->obj_ctx_menu->displayTransforms = false;

}
void EditWindow::onMouseWheel(int x, int y){
    if(isWorldCamera) return;
    //Stop camera moving
    this->edit_camera.stopMoving();
    //If we are in 3D project
    if(project.perspective == 3){
        ZSVECTOR3 front = edit_camera.getCameraFrontVec(); //obtain front vector
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position

        edit_camera.setPosition(pos + front * y);
    }
    //2D project
    if(project.perspective == 2 &&
            edit_camera.orthogonal_factor + static_cast<float>(y) / 50.F >= 0.2f &&
            edit_camera.orthogonal_factor + static_cast<float>(y) / 50.F <= 1.7f){
        edit_camera.orthogonal_factor += static_cast<float>(y) / 50.F;
        edit_camera.updateProjectionMat();
    }
}
void EditWindow::onMouseMotion(int relX, int relY){
    //Terrain painting
    if(_inspector_win->gameobject_ptr != nullptr){
        GameObject* obj = static_cast<GameObject*>(_inspector_win->gameobject_ptr);
        TerrainProperty* terrain = obj->getPropertyPtr<TerrainProperty>();

        if(terrain != nullptr)
            terrain->onMouseMotion(this->input_state.mouseX, input_state.mouseY, relX, relY, settings.gameViewWin_Height, this->input_state.isLeftBtnHold,
                                 this->input_state.isLCtrlHold);
    }

    //Property painting
    if(this->ppaint_state.enabled && input_state.isLeftBtnHold == true){ //we just move on map
        if(ppaint_state.time == 0.0f && !isSceneRun){
            ppaint_state.time += deltaTime;

            unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), input_state.mouseX, input_state.mouseY);
            //if we pressed on empty space
            if(clicked > world.objects.size() || clicked >= 256 * 256 * 256 || ppaint_state.last_obj == static_cast<int>(clicked))
                return;

            GameObject* obj_ptr = &world.objects[clicked]; //Obtain pointer to selected object by label

            ppaint_state.last_obj = static_cast<int>(clicked); //Set clicked as last object ID
            //Obtain pointer to object's property
            GameObjectProperty* prop_ptr = obj_ptr->getPropertyPtrByType(this->ppaint_state.prop_ptr->type);

            if (prop_ptr == nullptr) { //if no property with that type in object
                //add new property
                obj_ptr->addProperty(this->ppaint_state.prop_ptr->type);
                //update pointer
                prop_ptr = obj_ptr->getPropertyPtrByType(this->ppaint_state.prop_ptr->type);
            }

            getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
            //Copy property data
            ppaint_state.prop_ptr->copyTo(prop_ptr);
        }else{
            ppaint_state.time += deltaTime;
            if(ppaint_state.time >= 100) ppaint_state.time = 0;
        }

    }
    //We are in 2D project, move camera by the mouse
    if(project.perspective == 2 && !isWorldCamera){ //Only affective in 2D
        //If middle button of mouse pressed
        if(input_state.isMidBtnHold == true){ //we just move on map
            //Stop camera moving
            this->edit_camera.stopMoving();

            ZSVECTOR3 cam_pos = edit_camera.getCameraPosition();
            cam_pos.X += relX;
            cam_pos.Y += relY;
            edit_camera.setPosition(cam_pos);
        }
    }

    //We are in 2D project, move camera by the mouse and rotate it
    if(project.perspective == 3 && !isWorldCamera){//Only affective in 3D

        if(input_state.isMidBtnHold == true){
            this->cam_yaw += relX * 0.16f;
            cam_pitch += relY * 0.16f;

                if (cam_pitch > 89.0f)
                    cam_pitch = 89.0f;
                if (cam_pitch < -89.0f)
                    cam_pitch = -89.0f;

            ZSVECTOR3 front;
            front.X = static_cast<float>((cos(DegToRad(cam_yaw)) * cos(DegToRad(cam_pitch))));
            front.Y = -sin(DegToRad(cam_pitch));
            front.Z = sin(DegToRad(cam_yaw)) * cos(DegToRad(cam_pitch));
            vNormalize(&front);
            edit_camera.setFront(front);
        }
    }
    if(obj_trstate.isModifying && input_state.isLeftBtnHold == false){
        obj_trstate.isModifying = false;


    }
    //Visual transform control
    if(obj_trstate.isTransforming == true && input_state.isLeftBtnHold == true){ //Only affective if object is transforming
        ZSRGBCOLOR color = render->getColorOfPickedTransformControl(obj_trstate.tprop_ptr->_last_translation, this->input_state.mouseX, this->input_state.mouseY);
        //If transformation method isn't set
        if(obj_trstate.isModifying == false){
            //Set all coordinates to 0
            obj_trstate.Xcf = 0;
            obj_trstate.Ycf = 0;
            obj_trstate.Zcf = 0;

            if(color.r == 255) obj_trstate.Xcf = 1; else obj_trstate.Xcf = 0;
            if(color.g == 255) obj_trstate.Ycf = 1; else obj_trstate.Ycf = 0;
            if(color.b == 255) obj_trstate.Zcf = 1; else obj_trstate.Zcf = 0;
        }

        obj_trstate.isModifying = true;

        ZSVECTOR3* vec_ptr = nullptr; //pointer to modifying vector
        if(obj_trstate.transformMode == GO_TRANSFORM_MODE_TRANSLATE){
            vec_ptr = &obj_trstate.tprop_ptr->translation;
        }
        //if we scaling
        if(obj_trstate.transformMode == GO_TRANSFORM_MODE_SCALE){
            vec_ptr = &obj_trstate.tprop_ptr->scale;
        }
        //if we rotating
        if(obj_trstate.transformMode == GO_TRANSFORM_MODE_ROTATE){
            vec_ptr = &obj_trstate.tprop_ptr->rotation;
        }

        *vec_ptr = *vec_ptr + ZSVECTOR3(-relX, -relY,relX) * ZSVECTOR3(obj_trstate.Xcf, obj_trstate.Ycf, obj_trstate.Zcf);
    }
}

void EditWindow::keyPressEvent(QKeyEvent* ke){
    if(ke->key() == Qt::Key_Delete){ //User pressed delete button
        QTreeWidgetItem* object_toRemove = this->ui->objsList->currentItem();
        QListWidgetItem* file_toRemove = this->ui->fileList->currentItem();
        if(object_toRemove != nullptr && ui->objsList->hasFocus()){ //if user wish to delete object
            GameObject* obj = this->world.getObjectByLabel(object_toRemove->text(0));
            _inspector_win->clearContentLayout(); //Prevent variable conflicts
            GameObjectLink link = obj->getLinkToThisObject();
            obj_trstate.isTransforming = false; //disabling object transform
            callObjectDeletion(link); //removing object
        }
        if(file_toRemove != nullptr && ui->fileList->hasFocus()){ //if user wish to remove file
            FileDeleteDialog* dialog = new FileDeleteDialog(this->current_dir + "/" + file_toRemove->text());
            dialog->exec();
            delete dialog;
            updateFileList();
        }
    }
    if(ke->key() == Qt::Key_F2){ //User pressed f2 key
        QListWidgetItem* item_toRename = this->ui->fileList->currentItem();
        if(item_toRename != nullptr && ui->fileList->hasFocus()){
            FileRenameDialog* dialog = new FileRenameDialog(this->current_dir + "/" + item_toRename->text(), item_toRename->text(), this);
            dialog->exec();
            delete dialog;
            updateFileList();
        }
    }
    if(ke->key() == Qt::Key_F5){ //User pressed f5 key
        if(ui->fileList->hasFocus()){
            updateFileList();
        }
    }

    QMainWindow::keyPressEvent(ke); // base class implementation
}

void EditWindow::onKeyDown(SDL_Keysym sym){

    if(sym.sym == SDLK_a && !isSceneRun && !input_state.isLCtrlHold){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * -2.2f * deltaTime;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_d && !isSceneRun && !input_state.isLCtrlHold){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * 2.2f * deltaTime;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_w && !isSceneRun && !input_state.isLCtrlHold){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos.Y += 2.2f * deltaTime;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_s && !isSceneRun && !input_state.isLCtrlHold){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        pos.Y -= 2.2f * deltaTime;
        edit_camera.setPosition(pos);
    }

    if(input_state.isLAltHold && sym.sym == SDLK_t){
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_TRANSLATE);
    }
    if(input_state.isLAltHold && sym.sym == SDLK_e){
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_SCALE);
    }
    if(input_state.isLAltHold && sym.sym == SDLK_r){
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_ROTATE);
    }

    if(sym.sym == SDLK_DELETE){
        GameObjectLink link = this->obj_trstate.obj_ptr->getLinkToThisObject();
        callObjectDeletion(link);
    }
    //If we pressed CTRL + O
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
    if(input_state.isLCtrlHold && sym.sym == SDLK_r){
        emit onRunProject();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_n){
        emit onAddNewGameObject();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_c){
        emit onObjectCopy();
    }
    if(input_state.isLCtrlHold && sym.sym == SDLK_v){
        emit onObjectPaste();
    }
}

void EditWindow::callObjectDeletion(GameObjectLink link){
    //Create new Object action
    _ed_actions_container->newGameObjectAction(link);

    world.removeObj(link); //delete object
    this->obj_trstate.isTransforming = false; //disabling object transform
    getInspector()->clearContentLayout(); //Detach object from inspector
}

EdActions* getActionManager(){
    return _ed_actions_container;
}

void ObjectTransformState::setTransformOnObject(GO_TRANSFORM_MODE transformMode){
    //Calculate pointer to transform property
    this->tprop_ptr = static_cast<TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //We haven't found transform property
    if(tprop_ptr == nullptr) return;

    this->transformMode = transformMode;
    this->isTransforming = true;
    //Add property action
    GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(obj_ptr->getTransformProperty());
    getActionManager()->newPropertyAction(prop_ptr->go_link, GO_PROPERTY_TYPE_TRANSFORM);
}

void EditWindow::startManager(EditorComponentManager* manager){
    manager->setDpMetrics(this->settings.gameViewWin_Width, this->settings.gameViewWin_Height);
    manager->setProjectStructPtr(&this->project);
    manager->init();
    this->managers.push_back(manager);
}

void EditWindow::updateDeltaTime(float deltaTime){
    this->deltaTime = deltaTime;

    for(unsigned int i = 0; i < managers.size(); i ++){
        managers[i]->deltaTime = deltaTime;
    }
}

void EditWindow::destroyAllManagers(){
    //we must do that in reverse order
    for(int i = static_cast<int>(managers.size()) - 1; i >= 0; i --){
        delete managers[static_cast<unsigned int>(i)];
    }
}

void EditWindow::setGameViewWindowSize(int W, int H){
    if(W < 1 && H < 1) return;
    //Set SDL window size
    SDL_SetWindowSize(this->window, W, H);
    //Apply new viewport to cameras
    ZSVIEWPORT viewport = ZSVIEWPORT(0,0,static_cast<unsigned int>(W),static_cast<unsigned int>(H));
    edit_camera.setViewport(viewport);
    world.world_camera.setViewport(viewport);

    for(unsigned int i = 0; i < managers.size(); i ++){
        managers[i]->WIDTH = W;
        managers[i]->HEIGHT = H;
        managers[i]->updateWindowSize(W, H);
    }
}

void EditWindow::setGameViewWindowMode(unsigned int mode){
    SDL_SetWindowFullscreen(this->window, mode);
}

QTreeWidget* EditWindow::getObjectListWidget(){
    return this->ui->objsList;
}
QListWidget* EditWindow::getFilesListWidget(){
    return this->ui->fileList;
}
