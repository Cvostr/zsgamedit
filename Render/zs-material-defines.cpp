#include "headers/MatShaderProps.h"

//Integer stuff
IntegerMaterialShaderProperty::IntegerMaterialShaderProperty(){
    type = MATSHPROP_TYPE_INTEGER;
}
IntegerMtShPropConf::IntegerMtShPropConf(){
    type = MATSHPROP_TYPE_INTEGER;
    value = 0;
}
//Float stuff
FloatMaterialShaderProperty::FloatMaterialShaderProperty(){
    type = MATSHPROP_TYPE_FLOAT;
}
FloatMtShPropConf::FloatMtShPropConf(){
    type = MATSHPROP_TYPE_FLOAT;
    value = 0.0f;
}
//Float3 stuff
Float3MaterialShaderProperty::Float3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC3;
}
Float3MtShPropConf::Float3MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC3;
    value = ZSVECTOR3(0.0f, 0.0f, 0.0f);
}
Float2MaterialShaderProperty::Float2MaterialShaderProperty(){
    type = MATSHPROP_TYPE_FVEC2;
}
Float2MtShPropConf::Float2MtShPropConf(){
    type = MATSHPROP_TYPE_FVEC2;
    value = ZSVECTOR2(0.0f, 0.0f);
}
//Color stuff
ColorMaterialShaderProperty::ColorMaterialShaderProperty(){
    type = MATSHPROP_TYPE_COLOR;
}
ColorMtShPropConf::ColorMtShPropConf(){
    type = MATSHPROP_TYPE_COLOR;
}
//Textures3D stuff
Texture3MaterialShaderProperty::Texture3MaterialShaderProperty(){
    type = MATSHPROP_TYPE_TEXTURE3;
    this->slotToBind = 0;
}



MaterialShaderProperty* MtShProps::allocateProperty(int type){
    MaterialShaderProperty* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new TextureMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new IntegerMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new FloatMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Float2MaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new ColorMaterialShaderProperty); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderProperty*>(new Texture3MaterialShaderProperty); //Allocation of transform in heap
            break;
        }

    }
    return _ptr;
}
MaterialShaderPropertyConf* MtShProps::allocatePropertyConf(int type){
    MaterialShaderPropertyConf* _ptr = nullptr;
    switch (type) {
        case MATSHPROP_TYPE_TEXTURE:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new TextureMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_INTEGER:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new IntegerMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FLOAT:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new FloatMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float3MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_FVEC2:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Float2MtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_COLOR:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new ColorMtShPropConf); //Allocation of transform in heap
            break;
        }
        case MATSHPROP_TYPE_TEXTURE3:{ //If type is transfrom
            _ptr = static_cast<MaterialShaderPropertyConf*>(new Texture3MtShPropConf); //Allocation of transform in heap
            break;
        }
    }
    return _ptr;
}
