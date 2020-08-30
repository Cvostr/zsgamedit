#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include "headers/ConsoleLog.h"
#include "headers/InspEditAreas.h"
#include "../World/headers/World.h"
#include <Scripting/AngelScript.hpp>
#include <render/zs-materials.h>
#include "../World/headers/terrain.h"
#include "world/go_properties.h"
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
#include "../mainwin.h"
#include <misc/zs3m-master.h>
#include "../World/headers/Misc.h"
#include "headers/LocStringEditWin.h"
#include "headers/DialogsMaster.h"
//Hack to support meshes
extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

EditWindow* _editor_win;
InspectorWin* _inspector_win;
ConsoleLog* _console_log_win;
Project* project_ptr;
EdActions* _ed_actions_container;
RenderPipeline* renderer;

extern Material* default3dmat;
extern Material* defaultTerrainMat;

EditWindow::EditWindow(QApplication* app, QWidget *parent) :
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
    QObject::connect(ui->actionLocStringFile, SIGNAL(triggered()), this, SLOT(onNewLocalizedStringFile()));

    QObject::connect(ui->actionImport_Resource, SIGNAL(triggered()), this, SLOT(onImportResource()));
    QObject::connect(ui->actionClose_project, SIGNAL(triggered()), this, SLOT(onCloseProject()));
    QObject::connect(ui->actionBuild, SIGNAL(triggered(bool)), this, SLOT(onBuildProject()));
    QObject::connect(ui->actionRun, SIGNAL(triggered(bool)), this, SLOT(onRunProject()));

    QObject::connect(ui->actionUndo, SIGNAL(triggered()), this, SLOT(onUndoPressed()));
    QObject::connect(ui->actionRedo, SIGNAL(triggered()), this, SLOT(onRedoPressed()));

    QObject::connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(onObjectCopy()));
    QObject::connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(onObjectPaste()));

    QObject::connect(ui->objsList, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
                this, SLOT(onObjectListItemClicked())); //Signal comes, when user clicks on GameObject

    QObject::connect(ui->objsList, SIGNAL(onRightClick(QPoint)), this, SLOT(onObjectCtxMenuShow(QPoint)));
    QObject::connect(ui->objsList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onCameraToObjTeleport()));

    //Create new something (actions)
    QObject::connect(ui->actionNew_Cube, SIGNAL(triggered()), this, SLOT(addNewCube()));
    QObject::connect(ui->actionNew_Light, SIGNAL(triggered()), this, SLOT(addNewLight()));
    QObject::connect(ui->actionNew_Tile, SIGNAL(triggered()), this, SLOT(addNewTile()));
    QObject::connect(ui->actionNew_Terrain, SIGNAL(triggered()), this, SLOT(addNewTerrain()));

    QObject::connect(ui->actionRender_settings, SIGNAL(triggered()), this, SLOT(openRenderSettings()));
    QObject::connect(ui->actionPhysics_Settings, SIGNAL(triggered()), this, SLOT(openPhysicsSettings()));

    QObject::connect(ui->actionConsole_log, SIGNAL(triggered()), this, SLOT(onOpenConsoleLog()));
    QObject::connect(ui->actionInspectorWin, SIGNAL(triggered()), this, SLOT(onOpenInspectorWin()));

    ready = false; //Firstly set it to 0
    hasSceneFile = false; //No scene loaded by default
    isSceneRun = false; //Not running by default
    isWorldCamera = false;
    hasSheduledWorld = false;
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

    world.obj_widget_ptr = ui->objsList;

    ui->fileList->setViewMode(QListView::IconMode);

    this->obj_ctx_menu = new ObjectCtxMenu(this); //Allocating object Context menu
    //Allocate file ctx menu
    this->file_ctx_menu = new FileCtxMenu(this);

    this->app_ptr = app;

    ui->actionCopy->setShortcut(Qt::Key_C | Qt::CTRL);
    ui->actionPaste->setShortcut(Qt::Key_V | Qt::CTRL);

    ui->actionOpen->setShortcut(Qt::Key_O | Qt::CTRL);
    ui->actionSave->setShortcut(Qt::Key_S | Qt::CTRL);
    ui->actionUndo->setShortcut(Qt::Key_Z | Qt::CTRL);
    ui->actionRedo->setShortcut(Qt::Key_Y | Qt::CTRL);

    ui->actionBuild->setShortcut(Qt::Key_B | Qt::CTRL);
    ui->actionRun->setShortcut(Qt::Key_R | Qt::CTRL);
    ui->actionConsole_log->setShortcut(Qt::Key_L | Qt::CTRL);

    ui->actionNew_Object->setShortcut(Qt::Key_N | Qt::CTRL);
    ui->actionToggle_Cameras->setShortcut(Qt::Key_H | Qt::CTRL);

    this->glcontext = nullptr;

    ZSENGINE_CREATE_INFO* engine_create_info = new ZSENGINE_CREATE_INFO;
    engine_create_info->createWindow = false; //window already created, we don't need one
    engine_create_info->graphicsApi = OGL32; //use opengl


    engine_ptr = new ZSpireEngine();
    engine_ptr->engine_info = engine_create_info;
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
    //If no window settings made
    if(this->settings.isFirstSetup){
        //Set base windows values
        this->settings.gameView_win_pos_x = this->width();
        this->settings.gameView_win_pos_y = 0;
        //For windows, fix bug with invisible window title
#ifdef _WIN32
        this->settings.gameView_win_pos_y = 25;
#endif

        this->settings.editor_win_width = this->width();
        this->settings.editor_win_height = this->height();

        this->settings.inspector_win_width = _inspector_win->width();
        this->settings.inspector_win_height = _inspector_win->height();
    }
    //If dark theme is set
    if(this->settings.isDarkTheme){
        QPalette palette = QPalette();
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, Qt::black);
        app_ptr->setPalette(palette);
    }

    this->window = SDL_CreateWindow("Game View", this->settings.gameView_win_pos_x, this->settings.gameView_win_pos_y, settings.gameViewWin_Width, settings.gameViewWin_Height, SDL_WINDOW_OPENGL); //Create window
    this->glcontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowResizable(window, SDL_TRUE);
    std::cout << "SDL - GL context creation requested!" << std::endl;
    glewExperimental = GL_TRUE;
    std::cout << "Calling GLEW creation" << std::endl;

    if (glewInit() != GLEW_OK){
        std::cout << "OPENGL GLEW: Creation failed ";
        return;
    }

    std::cout << "GLEW creation successful" << std::endl;

    ZSGAME_DESC* game_desc = new ZSGAME_DESC;
    game_desc->game_perspective = project.perspective;
    engine_ptr->desc = game_desc;
    game_desc->game_dir = project.root_path;

    //init render
    render = new RenderPipeline;
    this->startManager(render);
    renderer = render;
    renderer->allowOnUpdate = false;
    //init glyph manager
    this->glyph_manager = new GlyphManager;
    this->startManager(glyph_manager);
    //Init thumbnails manager
    this->thumb_master = new ThumbnailsMaster;
    this->startManager(thumb_master);


    game_data = new ZSGAME_DATA;
    game_data->resources = new Engine::ResourceManager;
    startManager(game_data->resources);

    game_data->script_manager = new Engine::AGScriptMgr;
    game_data->glyph_manager = this->glyph_manager;
    game_data->pipeline = this->render;
    game_data->out_manager = new Engine::OutputManager;
    game_data->out_manager->consoleLogWorking = true;
    game_data->isEditor = true;
    game_data->world = &this->world;

    std::string absolute = project.root_path + "/";
    Engine::Loader::setBlobRootDirectory(absolute);
    Engine::Loader::start();
    startTerrainThread();

    ready = true;//Everything is ready
    //Init OpenAL sound system
    Engine::SFX::initAL();

    switch(project.perspective){
        case PERSP_2D:{ //2D project

            this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
            edit_camera.setPosition(ZSVECTOR3(0,0,0));
            edit_camera.setFront(ZSVECTOR3(0,0,1));
            break;
        }
        case PERSP_3D:{ //3D project
            this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
            edit_camera.setPosition(ZSVECTOR3(-20.f, 10.f, 0.f));
            edit_camera.setFront(ZSVECTOR3(1.f, -0.33f, 0.f));
            edit_camera.setZplanes(0.1f, 5000.f);
            break;
        }
    }
    Engine::ZSVIEWPORT viewport = Engine::ZSVIEWPORT(0,0,static_cast<unsigned int>(this->settings.gameViewWin_Width),static_cast<unsigned int>( this->settings.gameViewWin_Height));
    edit_camera.setViewport(viewport);

    world.world_camera = edit_camera;
    //World gameplay camera will work with openal listener
    world.world_camera.isAlListenerCamera = true;
    //Store pointer to Project structure
    project_ptr = &this->project;
}

//Slots
void EditWindow::openFile(QString file_path){

    if(file_path.endsWith(".scn")){ //If it is scene
        //Unpick object first
        world.unpickObject();
        QString gl_win_title = "Game View - " + file_path;
        SDL_SetWindowTitle(window, gl_win_title.toUtf8());
        //if scene is running, then call onRunProject() to stop scene
        if(isSceneRun == true)
            emit onRunProject();

        obj_trstate.isTransforming = false;
        ppaint_state.enabled = false;
        object_buffer = nullptr;
        //Back render settings to defaults
        this->render->getRenderSettings()->defaults();

        _ed_actions_container->clear();
        //setupObjectsHieList(); //Clear everything, at first
        //world.clear();
        world.openFromFile(file_path.toStdString(), ui->objsList); //Open this scene

        scene_path = file_path; //Assign scene path
        hasSceneFile = true; //Scene is saved
        this->edit_camera.setPosition(ZSVECTOR3(0.0f, 0.0f, 0.0f)); //Set camera to 0
        _inspector_win->clearContentLayout(); //Clear content, if not empty
    }else if(file_path.endsWith(".lcstr")){
        LocStringEditWindow* lsew = new LocStringEditWindow;
        lsew->showWindowWithFile(file_path.toStdString());
    }
    else{
#ifdef _WIN32
        QDesktopServices::openUrl(QUrl::fromLocalFile("file://" + file_path));
#endif
#ifdef __linux
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_path));
#endif
    }
}

void EditWindow::addFileToObjectList(QString file_path){
    if(checkExtension(file_path, ".prefab")){
        this->world.addObjectsFromPrefab(file_path.toStdString());
    }
    if(checkExtension(file_path, ".fbx") || checkExtension(file_path, ".dae") || checkExtension(file_path, ".zs3m")){
        this->world.addMeshGroup(file_path.toStdString());
    }
}

QString EditWindow::createNewTextFile(QString directory, QString name, QString ext, const char* content, uint32_t size){
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
    newfile_stream.write(content, size);
    newfile_stream.close();

    return newfile_name;
}

void EditWindow::onSceneSaveAs(){
    _ed_actions_container->hasChangesUnsaved = false;

    QString filename = QFileDialog::getSaveFileName(this, tr("Save scene file"), QString::fromStdString(project.root_path), "*.scn");
    if(!filename.endsWith(".scn")) //If filename doesn't end with ".scn"
        filename.append(".scn"); //Add this extension
    world.saveToFile(filename.toStdString()); //Save to picked file
    scene_path = filename; //Assign scene path
    hasSceneFile = true; //Scene is saved

    updateFileList(); //Make new scene visible in file list
}

void EditWindow::onOpenScene(){
    QString filter = tr("ZSpire Scene (*.scn *.SCN);;");
    QString path = QFileDialog::getOpenFileName(this, tr("Scene File"), QString::fromStdString(project.root_path), filter);
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
        //unset unsavedchanges flag to avoid showing dialog
        _ed_actions_container->hasChangesUnsaved = false;
        //perform scene save
        world.saveToFile(this->scene_path.toStdString());
    }
}

void EditWindow::onNewScene(){
    setupObjectsHieList();
    world.clear();
    //if Scene is Running
    if(isSceneRun == true)
        //Then call onRunProject() to stop it, first
        emit onRunProject();

    ppaint_state.enabled = false;
    obj_trstate.isTransforming = false;
    //clear actions history
    _ed_actions_container->clear();

    //Back render settings to defaults
    this->render->getRenderSettings()->defaults();

    hasSceneFile = false; //We have new scene
}


void EditWindow::openRenderSettings(){
    _inspector_win->clearContentLayout(); //clear everything, that was before

    Engine::RenderSettings* ptr = this->render->getRenderSettings();

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Ambient light color");
    lcolor->color = &ptr->ambient_light_color;
    _inspector_win->addPropertyArea(lcolor);

}

void EditWindow::openPhysicsSettings(){
    _inspector_win->clearContentLayout(); //clear everything, that was before

    PhysicalWorldSettings* ptr = &this->world.phys_settngs;

    Float3PropertyArea* float3_area = new Float3PropertyArea;
    float3_area->setLabel("Gravity"); //Its label
    float3_area->vector = &ptr->gravity;
    float3_area->go_property = reinterpret_cast<Engine::GameObjectProperty*>(this);
    _inspector_win->addPropertyArea(float3_area);
}

Engine::GameObject* EditWindow::onAddNewGameObject(){
    //get free index in array
    int free_ind = world.getFreeObjectSpaceIndex();
    //if we have no free space inside array
    if(free_ind == static_cast<int>(world.objects.size())){
        Engine::GameObject* obj = new Engine::GameObject;
        obj->alive = false;
        obj->world_ptr = &world;
        obj->array_index = free_ind;
        world.objects.push_back(obj);
    }
    //Create action
    _ed_actions_container->newGameObjectAction((world.objects[static_cast<unsigned int>(free_ind)])->getLinkToThisObject());
    //Add new object to world
    Engine::GameObject* obj_ptr = this->world.newObject();
    //New object will not have parents, so will be spawned at top
    ui->objsList->addTopLevelItem(GO_W_I::getItem(obj_ptr->array_index));

    return obj_ptr;
}

void EditWindow::addNewCube(){
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH);
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Cube_", &add_num);
    *obj->label_ptr = "Cube_" + std::to_string(add_num);
    GO_W_I::getItem(obj->array_index)->setText(0, QString::fromStdString(*obj->label_ptr));
    //Set MESH properties
    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@cube";
    mesh->updateMeshPtr();
    //Set MATERIAL properties
    Engine::MaterialProperty* mat = obj->getPropertyPtr<Engine::MaterialProperty>();
    mat->setMaterial("@default");

}
void EditWindow::addNewLight(){
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Light_", &add_num);
    *obj->label_ptr = "Light_" + std::to_string(add_num);
    GO_W_I::getItem(obj->array_index)->setText(0, QString::fromStdString(*obj->label_ptr));
}

void EditWindow::addNewTile(){
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE); //Creates tile inside
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH); //Creates mesh inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Tile_", &add_num);
    *obj->label_ptr = "Tile_" + std::to_string(add_num);
    GO_W_I::getItem(obj->array_index)->setText(0, QString::fromStdString(*obj->label_ptr));
    //Assign @mesh
    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@plane";
    mesh->updateMeshPtr();
    //Assign new scale
    Engine::TransformProperty* transform = obj->getPropertyPtr<Engine::TransformProperty>();
    transform->scale = ZSVECTOR3(100, 100, 1);
    transform->updateMatrix();
}
void EditWindow::addNewTerrain(){
    Engine::GameObject* obj = onAddNewGameObject();
    //Creates material inside
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Terrain_", &add_num);
    *obj->label_ptr = "Terrain_" + std::to_string(add_num);
    //Update object label on widget
    GO_W_I::getItem(obj->array_index)->setText(0, QString::fromStdString(*obj->label_ptr));
    //Add terrain property
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN); //Creates terrain inside
    obj->getPropertyPtr<Engine::TerrainProperty>()->onAddToObject();
    //Get pointer to MaterialProperty and set to id Default Terrain Material 
    Engine::MaterialProperty* mat = obj->getPropertyPtr<Engine::MaterialProperty>();
    mat->setMaterial("@defaultHeightmap");

    updateFileList();

}
void EditWindow::setupObjectsHieList(){
    QTreeWidget* w_ptr = ui->objsList; //Getting pointer to objects list widget
    w_ptr->clear(); //Clears widget
}

void EditWindow::onImportResource(){
    QString dir = "/home";
#ifdef _WIN32
    dir = "C:\\\n";
#endif
    QString filter = tr("GPU compressed texture (.DDS) (*.dds *.DDS);; 3D model (*.fbx *.FBX *.dae *.DAE);; Sound (*.wav *.WAV);; All files (*.*);;");

    QFileDialog dialog;
    QString path = dialog.getOpenFileName(this, tr("Select Resource"), dir, filter);

    if ( path.isNull() == false ) //If user specified file path
    {
        ImportResource(path);
    }
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

        //Clear shader groups
        MtShProps::clearMtShaderGroups();
        //Show main menu window to avoid crash
        MainWin* win = static_cast<MainWin*>(this->mainwin_ptr);
        win->show();

        //Close Qt windows
        _editor_win->close();
        _inspector_win->close();

        delete game_data;
        //Terminate Loader Thread
        Engine::Loader::stop();
        //Terminate Terrain Thread
        stopTerrainThread();

        destroyAllManagers();

        _ed_actions_container->clear();
        Engine::SFX::destroyAL();
        //won't render anymore
        this->ready = false; 

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
    renderer->allowOnUpdate = true;
    //call onStart() on all objects
    world.physical_world = new PhysicalWorld(&world.phys_settngs);
    world.call_onStart();
    
}
void EditWindow::stopWorld(){
    //Avoi crash on skybox rendering
    this->render->getRenderSettings()->resetPointers();
    //Prepare world for stopping
    
    //Set storing actions to undo changes
    _ed_actions_container->setStoreActions(true);
    //Clear Inspector Win
    _inspector_win->clearContentLayout();
    isWorldCamera = false;
    isSceneRun = false; //set toggle to true
    renderer->allowOnUpdate = false;
    //call onStop() on all objects
    world.call_onStop();
}

void EditWindow::sheduleWorldLoad(QString file_path){
    this->sheduled_world = file_path;
    hasSheduledWorld = true;
}

void EditWindow::onRunProject(){
    if(isSceneRun == false){ //if we are Not running scene
        game_data->out_manager->clearMessagesWithType(LogEntryType::LE_TYPE_SCRIPT_ERROR);
        this->world.putToShapshot(&run_world_snapshot); //create snapshot of current state to recover it later
        //Reload all scripts
        game_data->resources->reloadScripts();
        //perform world activity startup
        runWorld();
        if (game_data->out_manager->RE_TYPE == RuntimeErrorType::RE_TYPE_SCRIPT_ERROR) {
            stopWorld();
            return;
        }
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
        //Destroy physical world object
        delete world.physical_world;
    }
}

void EditWindow::onObjectListItemClicked(){
    this->obj_trstate.isTransforming = false; //disabling object transform
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item

    QString obj_name = selected_item->text(0); //Get label of clicked obj

    Engine::GameObject* obj_ptr = world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label
    //if no object with this name found, then go out
    if(obj_ptr == nullptr) return;

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = obj_ptr->getTransformProperty();
    _inspector_win->ShowObjectProperties(obj_ptr);
}
//Objects list right pressed
void EditWindow::onObjectCtxMenuShow(QPoint point){
    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    //We selected empty space
    if(selected_item == nullptr) return;
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    Engine::GameObject* obj_ptr = world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->show(point);
}


void EditWindow::onCameraToObjTeleport(){
    if(isWorldCamera) return;

    QTreeWidgetItem* selected_item = ui->objsList->currentItem(); //Obtain pointer to clicked obj item
    QString obj_name = selected_item->text(0); //Get label of clicked obj
    Engine::GameObject* obj_ptr = world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label

    Engine::TransformProperty* transform = obj_ptr->getPropertyPtr<Engine::TransformProperty>(); //Obtain pointer to object transform
    //Define to store absolute transform
    ZSVECTOR3 _t = ZSVECTOR3(0.0f);
    ZSVECTOR3 _s = ZSVECTOR3(1.0f);
    ZSVECTOR3 _r = ZSVECTOR3(0.0f);
    transform->getAbsoluteParentTransform(_t, _s, _r); //Calculate absolute transform

    edit_camera._dest_pos = _t; //Sending position
    if(project.perspective == PERSP_3D){ //if we're in 3D
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
    Engine::GameObject* obj_ptr = world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label

    this->object_buffer = obj_ptr;
}
void EditWindow::onObjectPaste(){
    if(object_buffer != nullptr)
        this->world.Instantiate(object_buffer);
}

void EditWindow::toggleCameras(){
    //This slot toggles cameras
    this->isWorldCamera = !this->isWorldCamera;
}

void EditWindow::glRender(){

    GO_W_I::updateObjsNames(&world);

    if (game_data->out_manager->RE_TYPE == RuntimeErrorType::RE_TYPE_SCRIPT_ERROR) {
        showErrorMessageBox("Error(s) in scripts", "In one or more scripts there are one or more errors! \nPlease fix them, to get your scene running!\nPress CTRL+L to get look at them");
        game_data->out_manager->unsetRuntimeError();
    }
    //iterate over all objects in world
    for(unsigned int obj_i = 0; obj_i < world.objects.size(); obj_i ++){
        Engine::GameObject* obj_ptr = world.objects[obj_i];
        //if object is alive
        if(obj_ptr->alive){
            //if object's name changed
            if (GO_W_I::getItem(obj_ptr->array_index)->text(0).toStdString().compare(*obj_ptr->label_ptr) == 1) {
                GO_W_I::getItem(obj_ptr->array_index)->setText(0, QString::fromStdString(*obj_ptr->label_ptr));
            }
            //if object active, and dark theme is on
            if(obj_ptr->active){
                if(settings.isDarkTheme)
                    GO_W_I::getItem(obj_ptr->array_index)->setTextColor(0, QColor(Qt::white));
                else {
                    GO_W_I::getItem(obj_ptr->array_index)->setTextColor(0, QColor(Qt::black));
                }
            }else
                GO_W_I::getItem(obj_ptr->array_index)->setTextColor(0, QColor(Qt::gray));
        }
    }
    //Update inspector position variables
    this->settings.inspector_win_pos_X = _inspector_win->pos().x();
    this->settings.inspector_win_pos_Y = _inspector_win->pos().y();
    //if scene is running
    if(isSceneRun){
        //Update physics
        world.physical_world->stepSimulation(deltaTime);
    }
    //if user opened another scene and it had not been opened
    if(hasSheduledWorld){
        stopWorld(); //firstly, stop world
        //Clear object list
        ui->objsList->clear();
        //load world
        world.openFromFile(this->sheduled_world.toStdString(), world.obj_widget_ptr);
        //run loaded world
        runWorld();

        hasSheduledWorld = false;
    }
    //if opengl ready, then render scene
    if(ready == true && !hasSheduledWorld)
        render->render(this->window, this);
}

RenderPipeline* EditWindow::getRenderPipeline(){
    return render;
}

EditWindow* ZSEditor::openProject(QApplication* app, Project& project){
    _editor_win = new EditWindow(app); //Creating class object
    _inspector_win = new InspectorWin();
    _console_log_win = new ConsoleLog;
    //Send project datas to editor window class
    _editor_win->project = project;
    //Update widget content

    return openEditor(); //Return pointer to edit window
}
EditWindow* ZSEditor::openEditor(){
    EditorSettingsManager* settings_manager = new EditorSettingsManager(&_editor_win->settings);
    _editor_win->startManager(settings_manager);
    //Initialize EditWindow class
    _editor_win->init();

    _editor_win->close_reason = EW_CLOSE_REASON_UNCLOSED;
    //Make a vector of all resource files
    _editor_win->lookForResources(QString::fromStdString(_editor_win->project.root_path)); 
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
    //Allocating EdActions
    _ed_actions_container = new EdActions;
    _ed_actions_container->world_ptr = &_editor_win->world; //Put world pointer
    _ed_actions_container->setStoreActions(true);
    //Create thumbnails
    _editor_win->thumb_master->createTexturesThumbnails();
    _editor_win->thumb_master->createMaterialThumbnails();
    _editor_win->thumb_master->createMeshesThumbnails();
    //Set directory to be shown in file manager
    _editor_win->setViewDirectory(QString::fromStdString(_editor_win->project.root_path));

    return _editor_win;
}

InspectorWin* EditWindow::getInspector(){
    return _inspector_win;
}

void EditWindow::onOpenConsoleLog() {
    _console_log_win->show();
    _console_log_win->updateLogsList();
}
void EditWindow::onOpenInspectorWin() {
    _inspector_win->show();
}

void EditWindow::callObjectDeletion(Engine::GameObjectLink link){
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
    //if object pointer is null, then go out!
    if (obj_ptr == nullptr) return;
    //Calculate pointer to transform property
    this->tprop_ptr = obj_ptr->getTransformProperty();
    //We haven't found transform property
    if(tprop_ptr == nullptr) return;

    this->transformMode = transformMode;
    this->isTransforming = true;
    //Add property action
    Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(obj_ptr->getPropertyPtr<Engine::TransformProperty>());
    getActionManager()->newPropertyAction(prop_ptr->go_link, PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
}

void EditWindow::startManager(EngineComponentManager* manager){
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
    Engine::ZSVIEWPORT viewport = Engine::ZSVIEWPORT(0,0,static_cast<unsigned int>(W),static_cast<unsigned int>(H));
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
