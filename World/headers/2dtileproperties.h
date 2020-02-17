#ifndef DTILEPROPERTIES
#define DTILEPROPERTIES

#include "obj_properties.h"
#include <world/tile_properties.h>

class TileGroupProperty : public Engine::GameObjectProperty{
public:
    int tiles_amount_X; //Tiles to add
    int tiles_amount_Y;

    bool isCreated;
    Engine::TileGeometry geometry; //Tile to add geometry

    std::string diffuse_relpath; //Diffuse texture to generate
    std::string mesh_string; //Mesh to generate

    void addPropertyInterfaceToInspector();
    void process();
    void clear();
    void copyTo(Engine::GameObjectProperty* dest);

    TileGroupProperty();
};


#endif // 2DTILEPROPERTIES_H
