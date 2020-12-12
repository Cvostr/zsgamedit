#include "headers/ProjectEdit.h"
#include "world/go_properties.h"
#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "ui_editor.h"
#include <QFileDialog>

extern InspectorWin* _inspector_win;
extern EdActions* _ed_actions_container;

void EditWindow::onOpenScene() {
    QString filter = tr("ZSpire Scene (*.scn *.SCN);;");
    QString path = QFileDialog::getOpenFileName(this, tr("Scene File"), QString::fromStdString(project.root_path), filter);
    if (path.isNull() == false) //If user specified file path
    {
        openFile(path);
    }
}

void EditWindow::onSceneSave() {
    if (isSceneRun) return; //No save during scene run

    if (hasSceneFile == false) { //If new created scene without file
        onSceneSaveAs(); //Show dialog and save
    }
    else {
        //unset unsavedchanges flag to avoid showing dialog
        _ed_actions_container->hasChangesUnsaved = false;
        //perform scene save
        world.saveToFile(this->scene_path.toStdString());
    }
}

void EditWindow::onSceneSaveAs() {
    _ed_actions_container->hasChangesUnsaved = false;

    QString filename = QFileDialog::getSaveFileName(this, tr("Save scene file"), QString::fromStdString(project.root_path), "*.scn");
    if (!filename.endsWith(".scn")) //If filename doesn't end with ".scn"
        filename.append(".scn"); //Add this extension
    world.saveToFile(filename.toStdString()); //Save to picked file
    scene_path = filename; //Assign scene path
    hasSceneFile = true; //Scene is saved

    updateFileList(); //Make new scene visible in file list
}

void EditWindow::onNewScene() {
    setupObjectsHieList();
    world.clear();
    //if Scene is Running
    if (isSceneRun == true)
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


void EditWindow::openRenderSettings() {
    _inspector_win->clearContentLayout(); //clear everything, that was before

    Engine::RenderSettings* ptr = this->render->getRenderSettings();

    ColorDialogArea* lcolor = new ColorDialogArea;
    lcolor->setLabel("Ambient light color");
    lcolor->color = &ptr->ambient_light_color;
    _inspector_win->addPropertyArea(lcolor);

}

void EditWindow::openPhysicsSettings() {
    _inspector_win->clearContentLayout(); //clear everything, that was before

    PhysicalWorldSettings* ptr = &this->world.phys_settngs;

    Float3PropertyArea* float3_area = new Float3PropertyArea;
    float3_area->setLabel("Gravity"); //Its label
    float3_area->vector = &ptr->gravity;
    float3_area->go_property = reinterpret_cast<Engine::IGameObjectComponent*>(this);
    _inspector_win->addPropertyArea(float3_area);
}

Engine::GameObject* EditWindow::onAddNewGameObject() {
    //get free index in array
    int free_ind = world.getFreeObjectSpaceIndex();
    //if we have no free space inside array
    if (free_ind == static_cast<int>(world.objects.size())) {
        Engine::GameObject* obj = new Engine::GameObject;
        obj->mAlive = false;
        obj->mWorld = &world;
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

void EditWindow::addNewCube() {
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH);
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Cube_", &add_num);
    obj->setLabel("Cube_" + std::to_string(add_num));
    //Set MESH properties
    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@cube";
    mesh->updateMeshPtr();
    //Set MATERIAL properties
    Engine::MaterialProperty* mat = obj->getPropertyPtr<Engine::MaterialProperty>();
    mat->setMaterial("@default");

}
void EditWindow::addNewLight() {
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_LIGHTSOURCE);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Light_", &add_num);
    obj->setLabel("Light_" + std::to_string(add_num));
}

void EditWindow::addNewTile() {
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE); //Creates tile inside
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH); //Creates mesh inside

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Tile_", &add_num);
    obj->setLabel("Tile_" + std::to_string(add_num));
    //Assign @mesh
    Engine::MeshProperty* mesh = obj->getPropertyPtr<Engine::MeshProperty>();
    mesh->resource_relpath = "@plane";
    mesh->updateMeshPtr();
    //Assign new scale
    Engine::TransformProperty* transform = obj->getPropertyPtr<Engine::TransformProperty>();
    transform->scale = Vec3(100, 100, 1);
    transform->updateMatrix();
}
void EditWindow::addNewTerrain() {
    Engine::GameObject* obj = onAddNewGameObject();
    //Creates material inside
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MATERIAL);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Terrain_", &add_num);
    obj->setLabel("Terrain_" + std::to_string(add_num));
    //Add terrain property
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TERRAIN); //Creates terrain inside
    obj->getPropertyPtr<Engine::TerrainProperty>()->onAddToObject();
    //Get pointer to MaterialProperty and set to id Default Terrain Material 
    Engine::MaterialProperty* mat = obj->getPropertyPtr<Engine::MaterialProperty>();
    mat->setMaterial("@defaultHeightmap");

    updateFileList();
}

void EditWindow::addNewAudsource() {
    Engine::GameObject* obj = onAddNewGameObject();
    obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_AUDSOURCE);

    //Set new name to object
    int add_num = 0; //Declaration of addititonal integer
    world.getAvailableNumObjLabel("Audio_", &add_num);
    obj->setLabel("Audio_" + std::to_string(add_num));
}

void EditWindow::onImportResource() {
    QString dir = "/home";
#ifdef _WIN32
    dir = "C:\\\n";
#endif
    QString filter = tr("GPU compressed texture (.DDS) (*.dds *.DDS);; 3D model (*.fbx *.FBX *.dae *.DAE *zs3m);; Sound (*.wav *.WAV);; Script(*.as *.AS);; All files (*.*);;");

    QFileDialog dialog;
    QString path = dialog.getOpenFileName(this, tr("Select Resource"), dir, filter);

    if (path.isNull() == false) //If user specified file path
    {
        ImportResource(path);
    }
}