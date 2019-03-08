#ifndef matshaderprops
#define matshaderprops

#include "zs-texture.h"

#define MATSHPROP_TYPE_NONE 0
#define MATSHPROP_TYPE_TEXTURE 1
#define MATSHPROP_TYPE_INTEGER 2
#define MATSHPROP_TYPE_FLOAT 3
#define MATSHPROP_TYPE_FVEC3 4

class MaterialShaderProperty{
public:
    int type;

    MaterialShaderProperty();
};

class TextureMaterialShaderProperty : public MaterialShaderProperty{
public:
    ZSPIRE::Texture* texture;
    unsigned int slotToBind;
    TextureMaterialShaderProperty();
};

#endif
