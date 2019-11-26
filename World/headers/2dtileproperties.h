#ifndef DTILEPROPERTIES
#define DTILEPROPERTIES

#include "obj_properties.h"

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
    int framesX; //columns in atlas
    int framesY; //rows in atlas
    int frame_time; //time to change the frame

    TileAnimation(){
        isAnimated = false;

        framesX = 1; //defaultly we have 1 frame
        framesY = 1;

        frame_time = 1000;
    }

}TileAnimation;

typedef struct TileAnimationState{
    int current_time;
    int current_frame;
    bool playing = false;

    int cur_frameX;
    int cur_frameY;

    TileAnimationState() {
        current_frame = 0; //we start from 0 frame
        current_time = 0;
        playing = false; //we not playing that
    }
}TileAnimationState;

class TileGroupProperty : public GameObjectProperty{
public:
    int tiles_amount_X; //Tiles to add
    int tiles_amount_Y;

    bool isCreated;
    TileGeometry geometry; //Tile to add geometry

    std::string diffuse_relpath; //Diffuse texture to generate
    std::string mesh_string; //Mesh to generate

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void process();
    void clear();
    void copyTo(GameObjectProperty* dest);

    TileGroupProperty();
};

class TileProperty : public GameObjectProperty{
private:
    bool lastAnimState;
public:
    TileGeometry geometry;
    //Defines color texture
    Engine::TextureResource* texture_diffuse;
    std::string diffuse_relpath;
    //Defines texture, that will cover tile
    Engine::TextureResource* texture_transparent;
    std::string transparent_relpath;
    //Defines animation
    TileAnimation anim_property;
    TileAnimationState anim_state;

    void addPropertyInterfaceToInspector(InspectorWin* inspector);
    void onValueChanged(); //Update texture pointer
    void updTexturePtr();
    void copyTo(GameObjectProperty* dest);
    void onAddToObject();
    void onUpdate(float deltaTime);
    void onRender(RenderPipeline* pipeline);

    void playAnim();
    void stopAnim();
    void setDiffuseTexture(std::string texture);

    TileProperty();
};

#endif // 2DTILEPROPERTIES_H
