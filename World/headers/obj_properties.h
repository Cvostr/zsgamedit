#ifndef OBJ_PROPERTIES_H
#define OBJ_PROPERTIES_H

#include "World.h"
#include "../../Scripting/headers/LuaScript.h"
#include <render/zs-materials.h>
#include "../../World/headers/terrain.h"

#include "world/go_properties.h"

class ScriptGroupProperty : public Engine::GameObjectProperty {
public:
    int scr_num; //to update amount via IntPropertyArea

    std::vector<ObjectScript> scripts_attached;
    std::vector<std::string> path_names;

    void onValueChanged();
    void addPropertyInterfaceToInspector();
    void shutdown();
    void onUpdate(float deltaTime); //calls update in scripts
    void copyTo(Engine::GameObjectProperty* dest);

    ObjectScript* getScriptByName(std::string name);

    ScriptGroupProperty();
};

#endif // OBJ_PROPERTIES_H
