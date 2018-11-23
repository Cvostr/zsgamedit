#ifndef DTILEPROPERTIES
#define DTILEPROPERTIES

#include "World.h"

#define GO_PROPERTY_TYPE_TILE_GROUP 1000
#define GO_PROPERTY_TYPE_TILE 1001

typedef struct TileGeometry{
    int tileWidth;
    int tileHeight;

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
    int tiles_amount_X; //Tiles to add
    int tiles_amount_Y;

    TileGeometry geometry; //Tile to add geometry

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void process();

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
