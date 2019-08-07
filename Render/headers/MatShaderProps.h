#ifndef matshaderprops
#define matshaderprops

#include "zs-texture.h"
#include "zs-shader.h"
#include <vector>
#include <QString>

enum MATSHPROP_TYPE{
    MATSHPROP_TYPE_NONE,
    MATSHPROP_TYPE_TEXTURE,
    MATSHPROP_TYPE_INTEGER,
    MATSHPROP_TYPE_FLOAT,
    MATSHPROP_TYPE_FVEC3,
    MATSHPROP_TYPE_COLOR,
    MATSHPROP_TYPE_TEXTURE3
};

class MaterialShaderProperty{
public:
    MATSHPROP_TYPE type; //type of property
    QString prop_caption;
    QString prop_identifier; //String identifier, that will appear in material file

    MaterialShaderProperty();
};

class MaterialShaderPropertyConf{
public:
    MATSHPROP_TYPE type;

    MaterialShaderPropertyConf();
};

class MtShaderPropertiesGroup{
public:
    std::string str_path;
    QString groupCaption;
    bool acceptShadows;

    ZSPIRE::Shader* render_shader; //Pointer to shader, that binds on object render
    std::vector<MaterialShaderProperty*> properties;

    MaterialShaderProperty* addProperty(int type);
    void loadFromFile(const char* fpath);
    MtShaderPropertiesGroup();
};

class Material{
private:
    std::string file_path; //path to material file
    std::string group_str;
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

    MtShaderPropertiesGroup* genDefaultMtShGroup(ZSPIRE::Shader* shader3d, ZSPIRE::Shader* skybox);
    MtShaderPropertiesGroup* getDefaultMtShGroup();

    void addMtShaderPropertyGroup(MtShaderPropertiesGroup* group);
    MtShaderPropertiesGroup* getMtShaderPropertyGroup(std::string group_name);
    MtShaderPropertiesGroup* getMtShaderPropertyGroupByLabel(QString group_label);
    //Get amount of registered material shader properties
    unsigned int getMaterialShaderPropertyAmount();
    //Get shader properties by index
    MtShaderPropertiesGroup* getMtShaderPropertiesGroupByIndex(unsigned int index);

    void clearMtShaderGroups();
}

class TextureMaterialShaderProperty : public MaterialShaderProperty{
public:
    int slotToBind; //Slot to texture
    std::string ToggleUniform; //Uniform to set to 1

    TextureMaterialShaderProperty();
};
class IntegerMaterialShaderProperty : public MaterialShaderProperty{
public:
    std::string integerUniform;
    //Construct
    IntegerMaterialShaderProperty();
};
class FloatMaterialShaderProperty : public MaterialShaderProperty{
public:
    std::string floatUniform;
    //Construct
    FloatMaterialShaderProperty();
};
class Float3MaterialShaderProperty : public MaterialShaderProperty{
public:
    std::string floatUniform;
    //Construct
    Float3MaterialShaderProperty();
};
class ColorMaterialShaderProperty : public MaterialShaderProperty{
public:
    std::string colorUniform;

    ColorMaterialShaderProperty();
};
class Texture3MaterialShaderProperty : public MaterialShaderProperty{
public:
    int slotToBind; //Slot to texture
    std::string ToggleUniform; //Uniform to set to 1

    Texture3MaterialShaderProperty();
};

//Property configurations
class TextureMtShPropConf : public MaterialShaderPropertyConf{
public:
     ZSPIRE::Texture* texture;
     QString path;

     TextureMtShPropConf();
};
class IntegerMtShPropConf : public MaterialShaderPropertyConf{
public:
    int value;
    //Construct
    IntegerMtShPropConf();
};
class FloatMtShPropConf : public MaterialShaderPropertyConf{
public:
    float value;
    //Construct
    FloatMtShPropConf();
};
class Float3MtShPropConf : public MaterialShaderPropertyConf{
public:
    ZSVECTOR3 value;
    //Construct
    Float3MtShPropConf();
};
class ColorMtShPropConf : public MaterialShaderPropertyConf{
public:
    ZSRGBCOLOR color;

    ColorMtShPropConf();
};
class Texture3MtShPropConf : public MaterialShaderPropertyConf{
public:
    int texture_count;
    QString texture_str[6];
    QString rel_path;

    ZSPIRE::Texture3D* texture3D;

     Texture3MtShPropConf();
};
#endif
