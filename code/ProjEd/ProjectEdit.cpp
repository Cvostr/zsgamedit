#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include "headers/ConsoleLog.h"
#include "headers/InspEditAreas.h"
#include "../World/headers/World.h"
#include <Scripting/AngelScript.hpp>
#include <render/Material.hpp>
#include "../World/headers/terrain.h"
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
#include "headers/LocStringEditWin.h"
#include "headers/DialogsMaster.h"
#include <input/Input.hpp>
#include <ui/UI.hpp>
#include <engine/Logger.hpp>

//Hack to support meshes
extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

EditWindow* _editor_win;
InspectorWin* _inspector_win;
ConsoleLog* _console_log_win;
Project* project_ptr;
EdActions* _ed_actions_container;
RenderPipelineEditor* renderer;

extern Material* default3dmat;
extern Material* defaultTerrainMat;

EditWindow::EditWindow(QApplication* app, QWidget *parent) :
    QMainWindow(parent),
    mFsWatcher(new QFileSystemWatcher(nullptr)),
    ui(new Ui::EditWindow),
    mObjectCopied(false),
    app_ptr(app)
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
    QObject::connect(ui->actionCreateRenderTarget, SIGNAL(triggered()), this, SLOT(onNewRenderTarget()));

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
    QObject::connect(ui->actionNew_Audio_source, SIGNAL(triggered()), this, SLOT(addNewAudsource()));
    QObject::connect(ui->actionNew_Camera, SIGNAL(triggered()), this, SLOT(addNewCamera()));
    QObject::connect(ui->actionNew_Particle_Emitter, SIGNAL(triggered()), this, SLOT(addNewParticleEmitter()));

    QObject::connect(ui->actionRender_settings, SIGNAL(triggered()), this, SLOT(openRenderSettings()));
    QObject::connect(ui->actionPhysics_Settings, SIGNAL(triggered()), this, SLOT(openPhysicsSettings()));

    QObject::connect(ui->actionConsole_log, SIGNAL(triggered()), this, SLOT(onOpenConsoleLog()));
    QObject::connect(ui->actionInspectorWin, SIGNAL(triggered()), this, SLOT(onOpenInspectorWin()));

    QObject::connect(mFsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onResourceFileChanged(QString)));
    QObject::connect(mFsWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));

    ready = false; //Firstly set it to 0
    hasSceneFile = false; //No scene loaded by default
    isSceneRun = false; //Not running by default
    isWorldCamera = false;
    hasSheduledWorld = false;

    setupObjectsHieList();
    //Drag & drop in objects tree
    ui->objsList->setAcceptDrops(true);
    ui->objsList->setDragEnabled(true);

    //Drag & drop in file list
    ui->fileList->setAcceptDrops(true);
    ui->fileList->setDragEnabled(true);
    ui->fileList->setDragDropMode(QAbstractItemView::InternalMove);
    ui->fileList->setResizeMode(QListView::Adjust);

    ui->objsList->world_ptr = &world;

    world.obj_widget_ptr = ui->objsList;

    ui->fileList->setViewMode(QListView::IconMode);

    this->obj_ctx_menu = new ObjectCtxMenu(this); //Allocating object Context menu
    //Allocate file ctx menu
    this->file_ctx_menu = new FileCtxMenu(this);

    

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

    ZSENGINE_CREATE_INFO* engine_create_info = new ZSENGINE_CREATE_INFO;
    engine_create_info->createWindow = false; //window already created, we don't need one
    engine_create_info->graphicsApi = OGL; //use opengl


    engine_ptr = new ZSpireEngine();
    engine_ptr->engine_info = engine_create_info;

    mComponentManager = new Engine::EngineComponentManager;
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

    ZSENGINE_CREATE_INFO engine_create_info;
    engine_create_info.appName = (char*)("GameEditorRun");
    engine_create_info.createWindow = true; //window already created, we don't need one
    engine_create_info.graphicsApi = OGL; //use selected graphics API

    Engine::ZSWINDOW_CREATE_INFO* window_create_info = new Engine::ZSWINDOW_CREATE_INFO;
    window_create_info->title = (char*)"Game View";
    window_create_info->Width = this->settings.gameViewWin_Width;
    window_create_info->Height = settings.gameViewWin_Width;
    window_create_info->PosX = this->settings.gameView_win_pos_x;
    window_create_info->PosY = this->settings.gameView_win_pos_y;

    mWindow = new Engine::Window;
    mWindow->CreateWindow(window_create_info, &engine_create_info);
    mWindow->SetComponentManager(this->mComponentManager);
    //Set window pointer
    engine_ptr->SetWindow(mWindow);
    
    mWindow->SetGLSwapInterval(1);
    mWindow->SetResizeable(true);
    

    ZSGAME_DESC* game_desc = new ZSGAME_DESC;
    game_desc->game_perspective = project.perspective;
    engine_ptr->desc = game_desc;
    game_desc->game_dir = project.root_path;
    //Allocate game data
    game_data = new ZSGAME_DATA;

    //init render
    render = new RenderPipelineEditor;
    mComponentManager->startManager(render);
    renderer = render;
    renderer->allowOnUpdate = false;
    //init glyph manager
    this->glyph_manager = new GlyphManager;
    mComponentManager->startManager(glyph_manager);
    //Init thumbnails manager
    this->thumb_master = new ThumbnailsMaster;
    mComponentManager->startManager(thumb_master);

   
    game_data->resources = new Engine::ResourceManager;
    mComponentManager->startManager(game_data->resources);

    game_data->script_manager = new Engine::AGScriptMgr;
    
    game_data->glyph_manager = this->glyph_manager;
    game_data->pipeline = this->render;
    game_data->out_manager = new Engine::OutputManager;
    game_data->ui_manager = new Engine::UiManager;
    game_data->oal_manager = new Engine::OALManager;
    game_data->time = new Engine::Time;
    mComponentManager->startManager(game_data->oal_manager);
    game_data->out_manager->consoleLogWorking = true;
    game_data->isEditor = true;
    game_data->world = &this->world;
    game_data->window = this->mWindow;

    std::string absolute = project.root_path + "/";
    Engine::Loader::setBlobRootDirectory(absolute);
    Engine::Loader::start();
    startTerrainThread();

    ready = true;//Everything is ready

    switch(project.perspective){
        case PERSP_2D:{ //2D project

            this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_ORTHOGONAL);
            edit_camera.setPosition(Vec3(0,0,0));
            edit_camera.setFront(Vec3(0,0,1));
            break;
        }
        case PERSP_3D:{ //3D project
            this->edit_camera.setProjectionType(ZSCAMERA_PROJECTION_PERSPECTIVE);
            edit_camera.setPosition(Vec3(-20.f, 10.f, 0.f));
            edit_camera.setFront(Vec3(1.f, -0.33f, 0.f));
            edit_camera.setZplanes(0.1f, 5000.f);
            break;
        }
    }
    Engine::ZSVIEWPORT viewport = Engine::ZSVIEWPORT(0,0,static_cast<unsigned int>(this->settings.gameViewWin_Width),static_cast<unsigned int>( this->settings.gameViewWin_Height));
    edit_camera.setViewport(viewport);
    //Store pointer to Project structure
    project_ptr = &this->project;
}

//Slots
void EditWindow::openFile(QString file_path){

    if(file_path.endsWith(".scn")){ //If it is scene
        //Unpick object first
        world.unpickObject();
        QString gl_win_title = "Game View - " + file_path;
        mWindow->SetTitle(gl_win_title.toUtf8());
        //if scene is running, then call onRunProject() to stop scene
        if(isSceneRun == true)
            emit onRunProject();

        obj_trstate.isTransforming = false;
        ppaint_state.enabled = false;
        //Back render settings to defaults
        this->render->getRenderSettings()->defaults();

        _ed_actions_container->clear();
        //setupObjectsHieList(); //Clear everything, at first
        world.openFromFile(file_path.toStdString(), ui->objsList); //Open this scene

        scene_path = file_path; //Assign scene path
        hasSceneFile = true; //Scene is saved
        this->edit_camera.setPosition(Vec3(0.0f, 0.0f, 0.0f)); //Set camera to 0
        _inspector_win->clearContentLayout(); //Clear content, if not empty
    }else if(checkExtension(file_path, ".lcstr")){
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

void EditWindow::setupObjectsHieList(){
    GO_W_I::recreateAll(MAX_OBJS);
}

bool EditWindow::onCloseProject(){
    QMessageBox::StandardButton reply = QMessageBox::Yes;
    if(_ed_actions_container->hasChangesUnsaved)
        reply = QMessageBox::question(this, "Are you sure to quit?", "You have unsaved changes. Do you really want to quit?",
                                    QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        world.clear(); //clear world
        mWindow->DestroyWindow();

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

        mComponentManager->destroyAllManagers();

        _ed_actions_container->clear();
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
            this->world.recoverFromSnapshot(&run_world_snapshot); //create snapshot of current state to recover it later
            run_world_snapshot.clear(); //Clear snapshot to free up memory
            return;
        }
        //Change button text
        this->ui->actionRun->setText("Stop");

    }else{ //lets stop scene run
        //return base window size
        mWindow->SetSize(this->settings.gameViewWin_Width, this->settings.gameViewWin_Height);
        mWindow->SetPosition(this->settings.gameView_win_pos_x, this->settings.gameView_win_pos_y);
        mWindow->SetWindowMode(0);
        Input::SetMouseRelativeMode(false);
        //Stop world
        stopWorld();
        //Recover world snapshot
        this->world.recoverFromSnapshot(&run_world_snapshot); //create snapshot of current state to recover it later
        run_world_snapshot.clear(); //Clear snapshot to free up memory
        //Change button text
        this->ui->actionRun->setText("Run");
        //Destroy physical world object
        delete world.physical_world;
        game_data->ui_manager->RemoveAllViews();
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
    Vec3 _t = Vec3(0.0f);
    Vec3 _s = Vec3(1.0f);
    Vec3 _r = Vec3(0.0f);
    transform->getAbsoluteParentTransform(_t, _s, _r); //Calculate absolute transform

    edit_camera._dest_pos = _t; //Sending position
    if(project.perspective == PERSP_3D){ //if we're in 3D
        Vec3 camFront = edit_camera.getCameraFrontVec();
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

    QString prefab_filepath = QString::fromStdString(engine_ptr->desc->game_dir) + "/.cache/copying_obj.prefab";
    //Call prefab storing
    _editor_win->world.storeObjectToPrefab(obj_ptr, prefab_filepath);
    mObjectCopied = true;
}
void EditWindow::onObjectPaste(){
    if (mObjectCopied) {
        std::string prefab_filepath = (engine_ptr->desc->game_dir) + "/.cache/copying_obj.prefab";
        //Call prefab storing
        world.addObjectsFromPrefab(prefab_filepath);
    }
}

void EditWindow::toggleCameras(){
    //This slot toggles cameras
    if(Input::getMouseState().RelativeMode)
        if(isWorldCamera)
            SDL_SetRelativeMouseMode(SDL_FALSE);
        else
            SDL_SetRelativeMouseMode(SDL_TRUE);
    this->isWorldCamera = !this->isWorldCamera;
}

void EditWindow::glRender(){
    
    edit_camera.updateTick(game_data->time->GetDeltaTime()); //Update camera, if it is moving
    //Look for objects names changes
    GO_W_I::updateObjsNames(&world);

    sceneWalkWASD();

    if (game_data->out_manager->RE_TYPE == RuntimeErrorType::RE_TYPE_SCRIPT_ERROR) {
        showErrorMessageBox("Error(s) in scripts", "In one or more scripts there are one or more errors! \nPlease fix them, to get your scene running!\nPress CTRL+L to get look at them");
        game_data->out_manager->unsetRuntimeError();
    }
    if (game_data->out_manager->RE_TYPE == RuntimeErrorType::RE_TYPE_SCENE_OPEN_ERROR) {
        showErrorMessageBox("Error(s) opening scene", "Perhaps, your scene is corrupted");
        game_data->out_manager->unsetRuntimeError();
        onNewScene();
    }

    //Update inspector position variables
    this->settings.inspector_win_pos_X = _inspector_win->pos().x();
    this->settings.inspector_win_pos_Y = _inspector_win->pos().y();
    //if scene is running
    if(isSceneRun){
        //Update physics
        world.physical_world->stepSimulation(game_data->time->GetDeltaTime());
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
        render->render(mWindow, this);
}

RenderPipelineEditor* EditWindow::getRenderPipeline(){
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
    _editor_win->mComponentManager->startManager(settings_manager);
    //Initialize EditWindow class
    _editor_win->init();

    _editor_win->close_reason = EW_CLOSE_REASON_UNCLOSED;
    //Make a vector of all resource files
    _editor_win->lookForResources(QString::fromStdString(_editor_win->project.root_path)); 
    game_data->script_manager->AddScriptFiles();
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
    Engine::IGameObjectComponent* prop_ptr = static_cast<Engine::IGameObjectComponent*>(obj_ptr->getPropertyPtr<Engine::TransformProperty>());
    getActionManager()->newPropertyAction(prop_ptr->go_link, PROPERTY_TYPE::GO_PROPERTY_TYPE_TRANSFORM);
}

void EditWindow::updateDeltaTime(){
    if(mComponentManager->GetComponentsCount() > 0)
        game_data->time->Tick();
}

void EditWindow::setGameViewWindowSize(int W, int H){
    //Set SDL window size
    mWindow->SetSize(W, H);
    //Apply new viewport to cameras
    edit_camera.setViewport(static_cast<unsigned int>(W), static_cast<unsigned int>(H));
}

QTreeWidget* EditWindow::getObjectListWidget(){
    return this->ui->objsList;
}
QListWidget* EditWindow::getFilesListWidget(){
    return this->ui->fileList;
}
