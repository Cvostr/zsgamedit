#include "../ProjEd/headers/ProjectEdit.h"
#include "headers/World.h"
#include <unistd.h>
TileGroupProperty* current_property; //Property, that shown

void onCreateBtnPress(){
    current_property->process();
}

void onClearBtnPress(){
    current_property->clear();
}

TileGroupProperty::TileGroupProperty(){
    type = GO_PROPERTY_TYPE_TILE_GROUP; //Set correct type
    active = true; //And it is active
    size = sizeof(TileGroupProperty);
    data_start = &tiles_amount_X;

    this->isCreated = false;
    this->tiles_amount_X = 0;
    this->tiles_amount_Y = 0;
}

TileProperty::TileProperty(){
    type = GO_PROPERTY_TYPE_TILE;
    active = true;
    size = sizeof(TileProperty);
    data_start = &geometry;

    this->texture_diffuse = nullptr;
}

void TileGroupProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    if(!isCreated){ //If tiles haven't been initialized
        IntPropertyArea* tileSizeXArea = new IntPropertyArea;
        tileSizeXArea->setLabel("Tile Width");
        tileSizeXArea->go_property = static_cast<void*>(this);
        tileSizeXArea->value = &this->geometry.tileWidth;
        inspector->addPropertyArea(tileSizeXArea);

        IntPropertyArea* tileSizeYArea = new IntPropertyArea;
        tileSizeYArea->setLabel("Tile Height");
        tileSizeYArea->go_property = static_cast<void*>(this);
        tileSizeYArea->value = &this->geometry.tileHeight;
        inspector->addPropertyArea(tileSizeYArea);

        IntPropertyArea* tilesAmountX = new IntPropertyArea;
        tilesAmountX->setLabel("Tiles amount X");
        tilesAmountX->go_property = static_cast<void*>(this);
        tilesAmountX->value = &this->tiles_amount_X;
        inspector->addPropertyArea(tilesAmountX);

        IntPropertyArea* tilesAmountY = new IntPropertyArea;
        tilesAmountY->setLabel("Tiles amount Y");
        tilesAmountY->go_property = static_cast<void*>(this);
        tilesAmountY->value = &this->tiles_amount_Y;
        inspector->addPropertyArea(tilesAmountY);
        //Add button to add objects
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onCreateBtnPress;
        btn->button->setText("Process"); //Setting text to qt button
        inspector->getContentLayout()->addWidget(btn->button);
        btn->insp_ptr = inspector; //Setting inspector pointer
        inspector->registerUiObject(btn);
    }else{
        AreaButton* btn = new AreaButton;
        btn->onPressFuncPtr = &onClearBtnPress;
        btn->button->setText("Clear");
        inspector->getContentLayout()->addWidget(btn->button);
        btn->insp_ptr = inspector;
        inspector->registerUiObject(btn);
    }
    current_property = this;
}

void TileGroupProperty::process(){
    //receive pointer to object that own this property
    World* wrld = world_ptr;

    for(int x_i = 0; x_i < tiles_amount_X; x_i ++){
        for(int y_i = 0; y_i < tiles_amount_Y; y_i ++){
            usleep(1100); //Wait some time to make random generator work properly
            GameObject* obj = wrld->newObject(); //Invoke new object creation

            go_link.updLinkPtr();
            GameObject* parent = go_link.ptr;
            TransformProperty* parent_transform = parent->getTransformProperty();
            LabelProperty* parent_label = parent->getLabelProperty();

            obj->render_type = GO_RENDER_TYPE_TILE;
            obj->addProperty(GO_PROPERTY_TYPE_MESH); //Adding mesh
            obj->addProperty(GO_PROPERTY_TYPE_TILE); //Adding tile
            //Receive properties ptrs
            TransformProperty* transform = obj->getTransformProperty();
            LabelProperty* label = obj->getLabelProperty();
            TileProperty* tile_prop = static_cast<TileProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_TILE));
            MeshProperty* mesh_prop = static_cast<MeshProperty*>(obj->getPropertyPtrByType(GO_PROPERTY_TYPE_MESH));

            mesh_prop->resource_relpath = "@plane"; //Default plane as mesh
            mesh_prop->updateMeshPtr(); //Update mesh pointer in property
            tile_prop->geometry = this->geometry; //Assign geometry property
            transform->scale = ZSVECTOR3(geometry.tileWidth, geometry.tileHeight, 1);
            transform->translation = ZSVECTOR3(geometry.tileWidth * x_i * 2, geometry.tileHeight * y_i * 2, 0);
            transform->translation = transform->translation + parent_transform->translation;

            label->label = parent_label->label + QString::number(x_i) + "," + QString::number(y_i); //Get new object new name
            obj->item_ptr->setText(0, label->label);

            parent->addChildObject(obj->getLinkToThisObject()); //Make new object dependent
            parent->item_ptr->addChild(obj->item_ptr); //Add widget as a child to tree
        }
    }
    this->isCreated = true;
}

void TileGroupProperty::clear(){
    go_link.updLinkPtr();
    GameObject* parent = go_link.ptr;
    unsigned int children_am = parent->children.size();
    for(unsigned int ch_i = 0; ch_i < children_am; ch_i ++){
        GameObjectLink link_toremove = parent->children[0];
        world_ptr->removeObj(link_toremove);
        go_link.updLinkPtr();
        parent = go_link.ptr;
    }

    isCreated = false;
}

void TileProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    PickResourceArea* area = new PickResourceArea;
    area->setLabel("Texture");
    area->go_property = static_cast<void*>(this);
    area->rel_path = &diffuse_relpath;
    area->resource_type = RESOURCE_TYPE_TEXTURE; //It should load textures only
    inspector->addPropertyArea(area);
}

void TileProperty::onValueChanged(){
    updTexturePtr();
}

void TileProperty::updTexturePtr(){
    this->texture_diffuse = world_ptr->getTexturePtrByRelPath(diffuse_relpath);
}
