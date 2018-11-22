#ifndef DTILEPROPERTIES
#define DTILEPROPERTIES

#include "World.h"

#define GO_PROPERTY_TYPE_TILE_GROUP 1000
#define GO_PROPERTY_TYPE_TILE 1001

typedef struct TileGeometry{
    unsigned int tileWidth;
    unsigned int tileHeight;

    TileGeometry(){
        tileWidth = 128;
        tileHeight = 128;
    }

}TileGeometry;

typedef struct TileAnimation{
    bool isAnimated;
    int framesX;
    int framesY;

    TileAnimation(){
        isAnimated = false;
    }

}TileAnimation;

class TileGroupProperty : public GameObjectProperty{
public:
    unsigned int tiles_amount_X;
    unsigned int tiles_amount_Y;

    TileGeometry geometry;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);

    TileGroupProperty();
};

class TileProperty : public GameObjectProperty{
public:
    TileGeometry geometry;

    ZSPIRE::Texture* texture_diffuse;
    TileAnimation anim_property;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);

    TileProperty();
};

#endif // 2DTILEPROPERTIES_H
