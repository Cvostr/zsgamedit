#include "../../ProjEd/headers/ProjectEdit.h"
#include "../headers/World.h"

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h> //For Sleep();
#endif

#include <world/tile_properties.h>
#include "../headers/World.h"
#include "world/go_properties.h"
#include "../../ProjEd/headers/InspEditAreas.h"

extern InspectorWin* _inspector_win;
static Engine::TileGroupProperty* current_property; //Property, that shown
extern ZSGAME_DATA* game_data;

void onCreateBtnPress(){
    current_property->process();
}

void onClearBtnPress(){
    current_property->clear();
}

void Engine::TileGroupProperty::addPropertyInterfaceToInspector(){
    if(!isCreated){ //If tiles haven't been initialized
        IntPropertyArea* tileSizeXArea = new IntPropertyArea;
        tileSizeXArea->setLabel("Tile Width");
        tileSizeXArea->go_property = this;
        tileSizeXArea->value = &this->geometry.tileWidth;
        _inspector_win->addPropertyArea(tileSizeXArea);

        IntPropertyArea* tileSizeYArea = new IntPropertyArea;
        tileSizeYArea->setLabel("Tile Height");
        tileSizeYArea->go_property = this;
        tileSizeYArea->value = &this->geometry.tileHeight;
        _inspector_win->addPropertyArea(tileSizeYArea);

        IntPropertyArea* tilesAmountX = new IntPropertyArea;
        tilesAmountX->setLabel("Tiles amount X");
        tilesAmountX->go_property = this;
        tilesAmountX->value = &this->tiles_amount_X;
        _inspector_win->addPropertyArea(tilesAmountX);

        IntPropertyArea* tilesAmountY = new IntPropertyArea;
        tilesAmountY->setLabel("Tiles amount Y");
        tilesAmountY->go_property = this;
        tilesAmountY->value = &this->tiles_amount_Y;
        _inspector_win->addPropertyArea(tilesAmountY);
        //Resources pick
        PickResourceArea* mesh_area = new PickResourceArea(RESOURCE_TYPE_MESH);
        mesh_area->setLabel("Tiles Mesh");
        mesh_area->go_property = this;
        mesh_area->rel_path_std = &this->mesh_string;
        mesh_area->isShowNoneItem = false;
        _inspector_win->addPropertyArea(mesh_area);

        PickResourceArea* diffuse_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
        diffuse_area->setLabel("Tiles Diffuse");
        diffuse_area->go_property = this;
        diffuse_area->rel_path_std = &this->diffuse_relpath;
        diffuse_area->isShowNoneItem = true;
        _inspector_win->addPropertyArea(diffuse_area);

        //Add button to add objects
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onCreateBtnPress;
        btn->button->setText("Process"); //Setting text to qt button
        _inspector_win->getContentLayout()->addWidget(btn->button);
        _inspector_win->registerUiObject(btn);
    }else{
        QString out = "";
        out += ("Tiles X : " + QString::number(this->tiles_amount_X) + QString(" \n"));
        out += "Tiles Y : " + QString::number(this->tiles_amount_Y) + QString(" \n");
        out += ("Tile Width : " + QString::number(this->geometry.tileWidth) + QString(" \n"));
        out += "Tile Height : " + QString::number(this->geometry.tileHeight) + QString(" \n");

        AreaText* group_info = new AreaText;
        group_info->label->setText(out);
        _inspector_win->getContentLayout()->addWidget(group_info->label);
        _inspector_win->registerUiObject(group_info);

        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onClearBtnPress;
        btn->button->setText("Clear");
        _inspector_win->getContentLayout()->addWidget(btn->button);
        _inspector_win->registerUiObject(btn);
    }
    current_property = this;
}

void Engine::TileGroupProperty::process(){
    //receive pointer to object that own this property
    getActionManager()->newSnapshotAction(go_link.world_ptr);

    for(int x_i = 0; x_i < tiles_amount_X; x_i ++){
        for(int y_i = 0; y_i < tiles_amount_Y; y_i ++){
#ifdef __linux__
            usleep(1100); //Wait some time to make random generator work properly
#endif
#ifdef _WIN32
            Sleep(1);
#endif
            Engine::GameObject* obj = mWorld->newObject(); //Invoke new object creation

            Engine::GameObject* parent = go_link.updLinkPtr();
            Engine::TransformProperty* parent_transform = parent->getPropertyPtr<Engine::TransformProperty>();
            Engine::LabelProperty* parent_label = parent->getPropertyPtr<Engine::LabelProperty>();

            //obj->render_type = GO_RENDER_TYPE_TILE;
            obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_MESH); //Adding mesh
            obj->addProperty(PROPERTY_TYPE::GO_PROPERTY_TYPE_TILE); //Adding tile
            //Receive properties ptrs
            Engine::TransformProperty* transform = obj->getPropertyPtr<Engine::TransformProperty>();
            Engine::LabelProperty* label = obj->getPropertyPtr<Engine::LabelProperty>();
            Engine::TileProperty* tile_prop = obj->getPropertyPtr<Engine::TileProperty>();
            Engine::MeshProperty* mesh_prop = obj->getPropertyPtr<Engine::MeshProperty>();

            mesh_prop->resource_relpath = this->mesh_string; //Default plane as mesh
            mesh_prop->updateMeshPtr(); //Update mesh pointer in property
            tile_prop->geometry = this->geometry; //Assign geometry property
            tile_prop->diffuse_relpath = this->diffuse_relpath; //Copy texture relpath
            tile_prop->updTexturePtr(); //Find texture pointer

            transform->scale = ZSVECTOR3(geometry.tileWidth, geometry.tileHeight, 1);
            transform->translation = ZSVECTOR3(geometry.tileWidth * x_i * 2, geometry.tileHeight * y_i * 2, 0);
            transform->translation = transform->translation + parent_transform->translation;

            label->label = parent_label->label + std::to_string(x_i) + "," + std::to_string(y_i); //Get new object new name

            parent->addChildObject(obj->getLinkToThisObject()); //Make new object dependent
            GO_W_I::getItem(parent->array_index)->addChild(GO_W_I::getItem(obj->array_index)); //Add widget as a child to tree
        }
    }
    this->isCreated = true;
}

void Engine::TileGroupProperty::clear(){
    this->go_link.updLinkPtr();
    //Create snapshot
    getActionManager()->newSnapshotAction((World*)go_link.world_ptr);

    Engine::GameObject* parent = go_link.updLinkPtr();
    size_t children_am = parent->mChildren.size();
    for(size_t ch_i = 0; ch_i < children_am; ch_i ++){
        Engine::GameObjectLink link_toremove = parent->mChildren[0];
        link_toremove.updLinkPtr();
        this->go_link.updLinkPtr();
        parent = go_link.ptr;
    }

    isCreated = false;
}

void Engine::TileProperty::addPropertyInterfaceToInspector(){
    BoolCheckboxArea* isAnim = new BoolCheckboxArea;
    isAnim->setLabel("Animated ");
    isAnim->go_property = this;
    isAnim->pResultBool = &this->anim_property.isAnimated;
    isAnim->updateInspectorOnChange = true;
    _inspector_win->addPropertyArea(isAnim);

    if(this->anim_property.isAnimated){ //if animation turned on
        IntPropertyArea* rowsAmount = new IntPropertyArea;
        rowsAmount->setLabel("Atlas rows");
        rowsAmount->go_property = this;
        rowsAmount->value = &this->anim_property.framesX;
        _inspector_win->addPropertyArea(rowsAmount);

        IntPropertyArea* colsAmount = new IntPropertyArea;
        colsAmount->setLabel("Atlas cols");
        colsAmount->go_property = this;
        colsAmount->value = &this->anim_property.framesY;
        _inspector_win->addPropertyArea(colsAmount);

        IntPropertyArea* frameTime = new IntPropertyArea;
        frameTime->setLabel("frame time");
        frameTime->go_property = this;
        frameTime->value = &this->anim_property.frame_time;
        _inspector_win->addPropertyArea(frameTime);
    }

    PickResourceArea* area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
    area->setLabel("Texture");
    area->go_property = this;
    area->rel_path_std = &diffuse_relpath;
    _inspector_win->addPropertyArea(area);

    PickResourceArea* transparent_area = new PickResourceArea(RESOURCE_TYPE_TEXTURE);
    transparent_area->setLabel("Secondary Diffuse");
    transparent_area->go_property = this;
    transparent_area->rel_path_std = &transparent_relpath;
    _inspector_win->addPropertyArea(transparent_area);
}

void Engine::TileProperty::onValueChanged(){
    updTexturePtr();
}
