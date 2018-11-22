#include "headers/World.h"

TileGroupProperty::TileGroupProperty(){
    type = GO_PROPERTY_TYPE_TILE_GROUP;
    active = true;
}

TileProperty::TileProperty(){
    type = GO_PROPERTY_TYPE_TILE;
    active = true;
}

void TileGroupProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){
    FloatPropertyArea* tileSizeXArea = new FloatPropertyArea;
    tileSizeXArea->setLabel("Tile Size X");
    tileSizeXArea->go_property = static_cast<void*>(this);
    //tileSizeXArea->value = &this->geometry.tileWidth;
    inspector->addPropertyArea(tileSizeXArea);
}

void TileProperty::addPropertyInterfaceToInspector(InspectorWin* inspector){

}
