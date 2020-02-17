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
#include <fstream>
#include <misc/zs3m-master.h>
#include "../World/headers/Misc.h"
//Hack to support meshes
extern ZSpireEngine* engine_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

EditWindow* _editor_win;
InspectorWin* _inspector_win;
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

    QObject::connect(ui->actionImport_Resource, SIGNAL(triggered()), this, SLOT(onImportResource()));
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

    //init render
    render = new RenderPipeline;
    this->startManager(render);
    renderer = render;
    //init glyph manager
    this->glyph_manager = new GlyphManager;
    this->startManager(glyph_manager);
    //Init thumbnails manager
    this->thumb_master = new ThumbnailsMaster;
    this->startManager(thumb_master);


    game_data = new ZSGAME_DATA;
    game_data->resources = new Engine::ResourceManager;
    startManager(game_data->resources);

    game_data->glyph_manager = this->glyph_manager;
    game_data->pipeline = this->render;

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
            edit_camera.setPosition(ZSVECTOR3(0,0,0));
            edit_camera.setFront(ZSVECTOR3(0,0,1));
            edit_camera.setZplanes(0.1f, 5000.f);
            break;
        }
    }
    Engine::ZSVIEWPORT viewport = Engine::ZSVIEWPORT(0,0,static_cast<unsigned int>(this->settings.gameViewWin_Width),static_cast<unsigned int>( this->settings.gameViewWin_Height));
    edit_camera.setViewport(viewport);

    world.world_camera = edit_camera;
    //World gameplay camera will work with openal listener
    world.world_camera.isAlListenerCamera = true;

    project_ptr = &this->project;
}

void EditWindow::assignIconFile(QListWidgetItem* item){
    //Set base icon
    item->setIcon(QIcon::fromTheme("application-x-executable"));

    //File is plaintext
    if(checkExtension(item->text(), (".txt")) || checkExtension(item->text(), (".inf")) || checkExtension(item->text(), (".scn"))){
        item->setIcon(QIcon::fromTheme("text-x-generic"));
    }
    if(checkExtension(item->text(), (".dds"))){
        QString path = this->current_dir + "/" + item->text();
        //Check, if we have thumbnail for this texture
        if(thumb_master->isAvailable(path.toStdString())){
            //Thumbnail exists
            QImage* img = thumb_master->texture_thumbnails.at(path.toStdString());
            item->setIcon(QIcon(QPixmap::fromImage(*img)));
        }
    }
    //File is .FBX .DAE .ZS3M scene
    if(checkExtension(item->text(), (".fbx")) || checkExtension(item->text(), (".dae")) || checkExtension(item->text(), (".zs3m"))){
        item->setIcon(QIcon::fromTheme("applications-graphics"));
    }
    //File is .zsanim animation
    if(checkExtension(item->text(), (".zsanim"))){
        item->setIcon(QIcon::fromTheme("media-playback-start"));
    }
    //File is .WAV sound
    if(checkExtension(item->text(), (".wav"))){
        item->setIcon(QIcon::fromTheme("audio-x-generic"));
    }
    if(checkExtension(item->text(), (".lua"))){
        item->setIcon(QIcon::fromTheme("text-x-script"));
    }
    //File is .ZSMAT material
    if(checkExtension(item->text(), (".zsmat"))){
        QString path = this->current_dir + "/" + item->text();
        if(thumb_master->isAvailable(path.toStdString())){
            QImage* img = thumb_master->texture_thumbnails.at(path.toStdString());
            item->setIcon(QIcon(QPixmap::fromImage(*img)));
        }
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
        world.openFromFile(file_path.toStdString(), ui->objsList); //Open this scene

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
    if(checkExtension(file_path, ".prefab")){
        this->world.addObjectsFromPrefab(file_path.toStdString());
    }
    if(checkExtension(file_path, ".fbx") || checkExtension(file_path, ".dae") || checkExtension(file_path, ".zs3m")){
        this->world.addMeshGroup(file_path.toStdString());
    }
}

bool EditWindow::checkExtension(QString fpath, QString ext){
    return fpath.toLower().endsWith(ext);
}

QString EditWindow::getCurrentDirectory(){
    return this->current_dir;
}

QString EditWindow::createNewTextFile(QString directory, QString name, QString ext, std::string& content){
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

    //Register new material in list
    //First, get relative path to new material
    QString rel_path = picked_name; //Preparing to get relative path
    rel_path = rel_path.remove(0, static_cast<int>(project.root_path.size() + 1)); //Get relative path by removing length of project root from start

    Engine::ZsResource* _resource = new Engine::MaterialResource;
    _resource->size = 0;
    _resource->rel_path = rel_path.toStdString();
    _resource->blob_path = _resource->rel_path;
    _resource->resource_label = _resource->rel_path;
    game_data->resources->pushResource(_resource);

    thumb_master->createMaterialThumbnail(_resource->rel_path);

    updateFileList(); //Make new file visible
}

void EditWindow::openRenderSettings(){
    _inspector_win->clearContentLayout(); //clear everything, that was before

    Engine::RenderSettings* ptr = this->render->getRenderSettings();

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Ambient light color");
    lcolor->color = &ptr->ambient_light_color;
    _inspector_win->addPropertyArea(lcolor);

    PropertyPickArea* sky_ppickarea = new PropertyPickArea(GO_PROPERTY_TYPE_SKYBOX);
    sky_ppickarea->property_ptr_ptr = reinterpret_cast<Engine::GameObjectProperty**>(&ptr->skybox_ptr);
    sky_ppickarea->setLabel("Skybox");
    _inspector_win->addPropertyArea(sky_ppickarea);

    PropertyPickArea* shadow_ppickarea = new PropertyPickArea(GO_PROPERTY_TYPE_SHADOWCASTER);
    shadow_ppickarea->property_ptr_ptr = reinterpret_cast<Engine::GameObjectProperty**>(&ptr->shadowcaster_ptr);
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
        GameObject* obj = new GameObject;
        obj->alive = false;
        obj->world_ptr = &world;
        obj->array_index = free_ind;
        world.objects.push_back(obj);
    }
    //Create action
    _ed_actions_container->newGameObjectAction(((GameObject*)world.objects[static_cast<unsigned int>(free_ind)])->getLinkToThisObject());

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
    *obj->label_ptr = "Cube_" + std::to_string(add_num);
    obj->item_ptr->setText(0, QString::fromStdString(*obj->label_ptr));

    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@cube";
    mesh->updateMeshPtr();

    Engine::MaterialProperty* mat = obj->getPropertyPtr<Engine::MaterialProperty>();
    mat->setMaterial("@default");

}
void EditWindow::addNewLight(){
    GameObject* obj = onAddNewGameObject();
    obj->addProperty(GO_PROPERTY_TYPE_LIGHTSOURCE);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Light_", &add_num);
    *obj->label_ptr = "Light_" + std::to_string(add_num);
    obj->item_ptr->setText(0, QString::fromStdString(*obj->label_ptr));
}

void EditWindow::addNewTile(){
    GameObject* obj = onAddNewGameObject();
    obj->addProperty(GO_PROPERTY_TYPE_TILE); //Creates tile inside
    obj->addProperty(GO_PROPERTY_TYPE_MESH); //Creates mesh inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Tile_", &add_num);
    *obj->label_ptr = "Tile_" + std::to_string(add_num);
    obj->item_ptr->setText(0, QString::fromStdString(*obj->label_ptr));
    //Assign @mesh
    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@plane";
    mesh->updateMeshPtr();
    //Assign new scale
    Engine::TransformProperty* transform =
            static_cast<Engine::TransformProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    transform->scale = ZSVECTOR3(100, 100, 1);
    transform->updateMat();
}
void EditWindow::addNewTerrain(){
    GameObject* obj = onAddNewGameObject();

    obj->addProperty(GO_PROPERTY_TYPE_MATERIAL); //Creates material inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Terrain_", &add_num);
    *obj->label_ptr = "Terrain_" + std::to_string(add_num);

    std::vector<GameObject> objects; obj->item_ptr->setText(0, QString::fromStdString(*obj->label_ptr));
    //Add terrain property
    obj->addProperty(GO_PROPERTY_TYPE_TERRAIN); //Creates terrain inside
    obj->getPropertyPtr<Engine::TerrainProperty>()->onAddToObject();

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
        Engine::Loader::stop();
        stopTerrainThread();

        destroyAllManagers();

        _ed_actions_container->clear();
        Engine::SFX::destroyAL();


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
    //Avoi crash on skybox rendering
    this->render->getRenderSettings()->resetPointers();
    //Prepare world for stopping
    for(unsigned int object_i = 0; object_i < world.objects.size(); object_i ++){
        GameObject* object_ptr = (GameObject*)world.objects[object_i];
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
    if(this->current_dir.compare(QString::fromStdString(project.root_path))){
        QListWidgetItem* backBtn_item = new QListWidgetItem("(back)", ui->fileList);
        backBtn_item->setIcon(QIcon::fromTheme("go-up"));
    }

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info

        //If file is project info, or terrain, then skip it.
        if(checkExtension(fileInfo.fileName(), ".inf") || checkExtension(fileInfo.fileName(), ".terrain"))
            continue;

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

    Engine::GameObject* obj_ptr = world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = static_cast<Engine::TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
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
    GameObject* obj_ptr = (GameObject*)world.getGameObjectByLabel(obj_name.toStdString()); //Obtain pointer to selected object by label

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
    //iterate over all objects in world
    for(unsigned int obj_i = 0; obj_i < world.objects.size(); obj_i ++){
        GameObject* obj_ptr = (GameObject*)world.objects[obj_i];
        //if object is alive
        if(obj_ptr->alive){
            //if object active, and dark theme is on
            if(obj_ptr->active){
                if(settings.isDarkTheme)
                    obj_ptr->item_ptr->setTextColor(0, QColor(Qt::white));
                else {
                    obj_ptr->item_ptr->setTextColor(0, QColor(Qt::black));
                }
            }else
                obj_ptr->item_ptr->setTextColor(0, QColor(Qt::gray));
        }
    }

    this->settings.inspector_win_pos_X = _inspector_win->pos().x();
    this->settings.inspector_win_pos_Y = _inspector_win->pos().y();

    if(isSceneRun){
        //Update physics
        world.physical_world->stepSimulation(deltaTime);
    }

    if(hasSheduledWorld){
        stopWorld(); //firstly, stop world
        //load world
        ui->objsList->clear();
        world.openFromFile(this->sheduled_world.toStdString(), world.obj_widget_ptr);
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

void EditWindow::lookForResources(QString path){
    QDir directory (path); //Creating QDir object
    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsLast); //I want to recursive call this function after all files

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        //if it is common file
        if(fileInfo.isFile() == true){
            //try to process file as resource
            processResourceFile(fileInfo);
        }

        if(fileInfo.isDir() == true){ //If it is directory
            QString newdir_str = path + "/" + fileInfo.fileName();
            lookForResources(newdir_str); //Call this function inside next dir
        }
    }
}

void EditWindow::processResourceFile(QFileInfo fileInfo){
    QString name = fileInfo.fileName();
    QString absfpath = fileInfo.absoluteFilePath();
    if(checkExtension(name, ".ttf")){ //if its font
        Engine::ZsResource* _resource = new Engine::GlyphResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if(checkExtension(name, ".dds")){ //If its an texture
        Engine::ZsResource* _resource = new Engine::TextureResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if(checkExtension(name, ".zs3m")){
        ZS3M::ImportedSceneFile isf;
        isf.loadFromFile(absfpath.toStdString());

        for(unsigned int mesh_i = 0; mesh_i < isf.meshes_toWrite.size(); mesh_i ++){
            QString rel_path = absfpath;
            rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

            Engine::ZsResource* _resource = new Engine::MeshResource;
            _resource->rel_path = rel_path.toStdString();
            _resource->blob_path = _resource->rel_path;
            _resource->resource_label = isf.meshes_toWrite[mesh_i]->mesh_label;
            game_data->resources->pushResource(_resource);
        }

    }
    if(checkExtension(name, ".zsanim")){

        ZS3M::ImportedAnimationFile* iaf = new ZS3M::ImportedAnimationFile;
        iaf->loadFromFile(absfpath.toStdString());

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        Engine::ZsResource* _resource = new Engine::AnimationResource;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = iaf->anim_ptr->name;
        game_data->resources->pushResource(_resource);

        delete iaf;
    }

    if(checkExtension(name, ".wav")){ //If its an mesh
        Engine::ZsResource* _resource = new Engine::AudioResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if(checkExtension(name, ".zsmat")){ //If its an mesh
        Engine::ZsResource* _resource = new Engine::MaterialResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->size = 0;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);

    }
    if(checkExtension(name, ".lua") || checkExtension(name, ".zscr")){ //If its an mesh
        Engine::ZsResource* _resource = new Engine::ScriptResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->size = 0;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);

    }
}

void EditWindow::ImportResource(QString pathToResource){

    bool copyResource = false;
    bool workWithFbx = false;

    if(checkExtension(pathToResource, ".dds")){
        copyResource = true;
    }
    if(checkExtension(pathToResource, ".fbx") || checkExtension(pathToResource, ".dae")){
        workWithFbx = true;
    }
    if(checkExtension(pathToResource, ".wav")){
        copyResource = true;
    }
    if(checkExtension(pathToResource, ".lua")){
        copyResource = true;
    }

    if(workWithFbx){
        unsigned int num_meshes = 0;
        unsigned int num_anims = 0;
        unsigned int num_textures = 0;
        unsigned int num_materials = 0;

        ZS3M::SceneFileExport exporter;
        //Get amount of meshes, animations, textures, materials in file
        Engine::getSizes(pathToResource.toStdString(), &num_meshes, &num_anims, &num_textures, &num_materials);
        //Allocate array for meshes
        Engine::_ogl_Mesh* meshes = static_cast<Engine::_ogl_Mesh*>(Engine::allocateMesh(num_meshes));
        //Allocate array for animations
        Engine::Animation* anims = new Engine::Animation[num_anims];

        ZS3M::SceneNode rootNode;
        //Load all meshes in file
        for(unsigned int mesh_i = 0; mesh_i < num_meshes; mesh_i ++){
            Engine::loadMesh(pathToResource.toStdString(), &meshes[mesh_i], static_cast<int>(mesh_i));
            //Add loaded mesh to exporter
            exporter.pushMesh(&meshes[mesh_i]);
        }
        //Load all animations in file
        for(unsigned int anim_i = 0; anim_i < num_anims; anim_i ++){
            Engine::loadAnimation(pathToResource.toStdString(), &anims[anim_i], static_cast<int>(anim_i));
            ZS3M::AnimationFileExport ex(&anims[anim_i]);
            QString newanim_path = this->current_dir + "/" + QString::fromStdString(anims[anim_i].name + ".zsanim");
            ex.write(newanim_path.toStdString());
            //Register new resource file
            processResourceFile(QFileInfo( newanim_path));
        }
        //if FBX has at least one mesh inside
        if(num_meshes > 0){
            Engine::loadNodeTree(pathToResource.toStdString(), &rootNode);
            //Set root node to exporter
            exporter.setRootNode(&rootNode);

            QString _file_name;
            int step = 1;
            while(pathToResource[pathToResource.length() - step] != "/"){
                _file_name.push_front(pathToResource[pathToResource.length() - step]);
                step += 1;
            }
            _file_name = this->current_dir + "/" + _file_name;

            QString new_path;
            if(checkExtension(_file_name, ".fbx"))
                new_path = _file_name.replace(".fbx", ".zs3m");
            if(checkExtension(_file_name, ".dae"))
                new_path = _file_name.replace(".dae", ".zs3m");
            //Write 3D model scene
            exporter.write(new_path.toStdString());
            //Free all meshes
            for(unsigned int mesh_i = 0; mesh_i < num_meshes; mesh_i ++){
                meshes[mesh_i].Destroy();
            }
            //Register new resource file
            processResourceFile(QFileInfo( new_path));
        }
    }

    if(copyResource){
        std::ifstream res_stream;
        res_stream.open(pathToResource.toStdString(), std::iostream::binary | std::iostream::ate);
        //Failed opening file
        if (res_stream.fail()) return;
        //Get size of importing file
        int size = static_cast<int>(res_stream.tellg());
        unsigned char* data_buffer = new unsigned char[size];
        //Return to Zero position
        res_stream.seekg(0);
        //Read file
        res_stream.read(reinterpret_cast<char*>(data_buffer), size);
        res_stream.close();

        QString _file_name;
        int step = 1;
        while(pathToResource[pathToResource.length() - step] != "/"){
            _file_name.push_front(pathToResource[pathToResource.length() - step]);
            step += 1;
        }

        std::ofstream resource_write_stream;
        resource_write_stream.open((this->current_dir + "/" + _file_name).toStdString(), std::iostream::binary);
        resource_write_stream.write(reinterpret_cast<char*>(data_buffer), size);
        resource_write_stream.close();
        delete[] data_buffer;
        //Register new resource
        processResourceFile(QFileInfo(this->current_dir + "/" + _file_name));
    }
    updateFileList();
}

EditWindow* ZSEditor::openProject(QApplication* app, Project& project){
    _editor_win = new EditWindow(app); //Creating class object
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
    _editor_win->lookForResources(QString::fromStdString(_editor_win->project.root_path)); //Make a vector of all resource files
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

    _ed_actions_container = new EdActions; //Allocating EdActions
    _ed_actions_container->world_ptr = &_editor_win->world; //Put world pointer
    _ed_actions_container->setStoreActions(true);
    //Create thumbnails
    _editor_win->thumb_master->createTexturesThumbnails();
    _editor_win->thumb_master->createMaterialThumbnails();
    _editor_win->thumb_master->createMeshesThumbnails();

    _editor_win->setViewDirectory(QString::fromStdString(_editor_win->project.root_path));

    return _editor_win;
}

InspectorWin* EditWindow::getInspector(){
    return _inspector_win;
}


void EditWindow::onLeftBtnClicked(int X, int Y){
    //Terrain painting
    if(_inspector_win->gameobject_ptr != nullptr){
        GameObject* obj = static_cast<GameObject*>(_inspector_win->gameobject_ptr);
        Engine::TerrainProperty* terrain = obj->getPropertyPtr<Engine::TerrainProperty>();

        if(terrain != nullptr && !isWorldCamera)
            terrain->onMouseMotion(X, Y,
                                  settings.gameViewWin_Height,
                                  this->input_state.isLeftBtnHold,
                                  this->input_state.isLCtrlHold);
    }

    //Stop camera moving
    this->edit_camera.stopMoving();
    this->obj_ctx_menu->close(); //Close ctx menu

    if(obj_trstate.isTransforming || isWorldCamera)
        return;
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    if(clicked > world.objects.size() || clicked >= 256 * 256 * 256){
        world.unpickObject();
        return;
    }
    GameObject* obj_ptr = (GameObject*)world.objects[clicked]; //Obtain pointer to selected object by label

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = static_cast<Engine::TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    _inspector_win->ShowObjectProperties(static_cast<void*>(obj_ptr));
    this->ui->objsList->setCurrentItem(obj_ptr->item_ptr); //item selected in tree
}
void EditWindow::onRightBtnClicked(int X, int Y){
    //Exit function if playcamera used to render world
    if(isWorldCamera) return;

    //Stop camera moving
    this->edit_camera.stopMoving();

    this->obj_trstate.isTransforming = false; //disabling object transform
    unsigned int clicked = render->render_getpickedObj(static_cast<void*>(this), X, Y);

    if(clicked > world.objects.size() || clicked >= 256 * 256 * 256)
        return;
    GameObject* obj_ptr = (GameObject*)world.objects[clicked]; //Obtain pointer to selected object by label

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
    if(project.perspective == PERSP_3D){
        ZSVECTOR3 front = edit_camera.getCameraFrontVec(); //obtain front vector
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position

        edit_camera.setPosition(pos + front * y);
    }
    //2D project
    if(project.perspective == PERSP_2D && this->input_state.isLCtrlHold &&
            edit_camera.orthogonal_factor + static_cast<float>(y) / 50.F >= 0.2f &&
            edit_camera.orthogonal_factor + static_cast<float>(y) / 50.F <= 1.7f){
        edit_camera.orthogonal_factor += static_cast<float>(y) / 50.F;
        edit_camera.updateProjectionMat();
    }
    //Common camera movement
    if(project.perspective == PERSP_2D && !this->input_state.isLCtrlHold)
        edit_camera.setPosition(edit_camera.getCameraPosition() + ZSVECTOR3(x * 10, y * 10, 0));

}
void EditWindow::onMouseMotion(int relX, int relY){
    //Terrain painting
    if(_inspector_win->gameobject_ptr != nullptr){
        GameObject* obj = static_cast<GameObject*>(_inspector_win->gameobject_ptr);
        Engine::TerrainProperty* terrain = obj->getPropertyPtr<Engine::TerrainProperty>();

        if(terrain != nullptr && !isWorldCamera)
            terrain->onMouseMotion(this->input_state.mouseX, input_state.mouseY,
                                   settings.gameViewWin_Height,
                                   this->input_state.isLeftBtnHold,
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

            GameObject* obj_ptr = (GameObject*)world.objects[clicked]; //Obtain pointer to selected object by label

            ppaint_state.last_obj = static_cast<int>(clicked); //Set clicked as last object ID
            //Obtain pointer to object's property
            Engine::GameObjectProperty* prop_ptr = obj_ptr->getPropertyPtrByType(this->ppaint_state.prop_ptr->type);

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
    if(project.perspective == PERSP_2D && !isWorldCamera){ //Only affective in 2D
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
    if(project.perspective == PERSP_3D && !isWorldCamera){//Only affective in 3D

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
        ZSRGBCOLOR color = render->getColorOfPickedTransformControl(
                                                                    this->input_state.mouseX,
                                                                    this->input_state.mouseY,
                                                                    static_cast<void*>(this));
        //If transformation method isn't set
        if(obj_trstate.isModifying == false){
            //Set all coordinates to 0
            obj_trstate.Xcf = 0;
            obj_trstate.Ycf = 0;
            obj_trstate.Zcf = 0;
            //if mouse under red color, we modifying X axis
            if(color.r == 255) obj_trstate.Xcf = 1; else obj_trstate.Xcf = 0;
            //if mouse under green color, we modifying Y axis
            if(color.g == 255) obj_trstate.Ycf = 1; else obj_trstate.Ycf = 0;
            //if mouse under blue color, we modifying Z axis
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
        if(obj_trstate.transformMode > 0)
            *vec_ptr = *vec_ptr + ZSVECTOR3(-relX * sign(edit_camera.camera_front.Z), -relY,relX * sign(edit_camera.camera_front.X)) * ZSVECTOR3(obj_trstate.Xcf, obj_trstate.Ycf, obj_trstate.Zcf);
    }
}

void EditWindow::keyPressEvent(QKeyEvent* ke){
    if(ke->key() == Qt::Key_Delete){ //User pressed delete button
        QTreeWidgetItem* object_toRemove = this->ui->objsList->currentItem();
        QListWidgetItem* file_toRemove = this->ui->fileList->currentItem();
        if(object_toRemove != nullptr && ui->objsList->hasFocus()){ //if user wish to delete object
            GameObject* obj = (GameObject*)this->world.getGameObjectByLabel(object_toRemove->text(0).toStdString());
            _inspector_win->clearContentLayout(); //Prevent variable conflicts
            Engine::GameObjectLink link = obj->getLinkToThisObject();
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
        if(project.perspective == PERSP_2D)
            pos.Y += 2.2f * deltaTime;
        else
            pos = pos + edit_camera.getCameraFrontVec() * 1.2f * deltaTime;
        edit_camera.setPosition(pos);
    }
    if(sym.sym == SDLK_s && !isSceneRun && !input_state.isLCtrlHold){
        ZSVECTOR3 pos = edit_camera.getCameraPosition(); //obtain position
        if(project.perspective == PERSP_2D)
            pos.Y -= 2.2f * deltaTime;
        else
            pos = pos - edit_camera.getCameraFrontVec() * 1.2f * deltaTime;
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
        Engine::GameObjectLink link = this->obj_trstate.obj_ptr->getLinkToThisObject();
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
    if(input_state.isLCtrlHold && sym.sym == SDLK_h){
        emit toggleCameras();
    }
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
    //Calculate pointer to transform property
    this->tprop_ptr = static_cast<Engine::TransformProperty*>(obj_ptr->getPropertyPtrByType(GO_PROPERTY_TYPE_TRANSFORM));
    //We haven't found transform property
    if(tprop_ptr == nullptr) return;

    this->transformMode = transformMode;
    this->isTransforming = true;
    //Add property action
    Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(obj_ptr->getPropertyPtr<Engine::TransformProperty>());
    getActionManager()->newPropertyAction(prop_ptr->go_link, GO_PROPERTY_TYPE_TRANSFORM);
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
