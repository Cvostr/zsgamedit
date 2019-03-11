#ifndef matshaderprops
#define matshaderprops

#include "zs-texture.h"
#include "zs-shader.h"
#include <vector>
#include <QString>

#define MATSHPROP_TYPE_NONE 0
#define MATSHPROP_TYPE_TEXTURE 1
#define MATSHPROP_TYPE_INTEGER 2
#define MATSHPROP_TYPE_FLOAT 3
#define MATSHPROP_TYPE_FVEC3 4

class MaterialShaderProperty{
public:
    int type;
    QString prop_caption;

    MaterialShaderProperty();
};

class MaterialShaderPropertyConf{
public:
    int type;

    MaterialShaderPropertyConf();
};

class MtShaderPropertiesGroup{
public:
    QString str_path;
    ZSPIRE::Shader* render_shader; //Pointer to shader, that binds on object render
    std::vector<MaterialShaderProperty*> properties;

    MaterialShaderProperty* addProperty(int type);
    void loadFromFile(const char* fpath);
    MtShaderPropertiesGroup();
};

class Material{
private:
    std::string file_path; //path to material file
public:
    MtShaderPropertiesGroup* group_ptr;
    std::vector<MaterialShaderPropertyConf*> confs;

    MaterialShaderPropertyConf* addPropertyConf(int type);
    void loadFromFile(std::string fpath);
    void saveToFile();
    void setPropertyGroup(MtShaderPropertiesGroup* group_ptr);
    void clear();

    Material();
};

namespace MtShProps {
    MaterialShaderProperty* allocateProperty(int type);
    MaterialShaderPropertyConf* allocatePropertyConf(int type);

    MtShaderPropertiesGroup* genDefaultMtShGroup(ZSPIRE::Shader* shader3d);
    MtShaderPropertiesGroup* getDefaultMtShGroup();
}

class TextureMaterialShaderProperty : public MaterialShaderProperty{
public:
    unsigned int slotToBind; //Slot to texture
    std::string ToggleUniform; //Uniform to set to 1

    TextureMaterialShaderProperty();
};
class TextureMtShPropConf : public MaterialShaderPropertyConf{
public:
     ZSPIRE::Texture* texture;
     QString path;

     TextureMtShPropConf();
};

#endif
