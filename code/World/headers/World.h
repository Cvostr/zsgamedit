#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <list>
#include <QString>
#include <QTreeWidget>
#include <fstream>
#include "../../ProjEd/headers/InspectorWin.h"

#include "../../Render/headers/zs-renderer-editor.hpp"
#include <render/Mesh.hpp>
#include <render/Math.hpp>
#include <render/Shader.hpp>
#include <render/Material.hpp>
#include <audio/SoundSource.hpp>

#include "../../Misc/headers/AssimpMeshLoader.h"

#include <world/Physics.h>

#include <game.h>

#include "world/World.hpp"

#include "../../ProjEd/headers/GO_widget_item.h"

class World;
class GameObjectProperty;
class GameObjectSnapshot;

class ObjectPropertyLink{
public:
    Engine::GameObjectLink object;
    PROPERTY_TYPE prop_type;
    Engine::IGameObjectComponent* ptr;

    Engine::IGameObjectComponent* updLinkPtr();
    ObjectPropertyLink();
};

class World : public Engine::World{
public:
    //-----------------DATA FOR QT INTERFACE-----------
    QTreeWidget* obj_widget_ptr;
    bool isPicked(Engine::GameObject* obj);
    void unpickObject();
    
    Engine::GameObject* dublicateObject(Engine::GameObject* original, bool parent = true);
    Engine::GameObject* Instantiate(Engine::GameObject* original);
    
    Engine::GameObject* updateLink(Engine::GameObjectLink* link);
    //Save loaded world to file
    void saveToFile(std::string file);
    //open world from file
    void openFromFile(std::string file, QTreeWidget* w_ptr);
    //Load world from bytes
    void loadFromMemory(const char* bytes, unsigned int size, QTreeWidget* w_ptr);
    //Write gameobject data to specified stream
    void writeGameObject(Engine::GameObject* object_ptr, ZsStream* world_stream);

    void storeObjectToPrefab(Engine::GameObject* object_ptr, QString file);
    void writeObjectToPrefab(Engine::GameObject* object_ptr, ZsStream* stream);

    void addObjectsFromPrefab(std::string file);
    void addObjectsFromPrefab(char* data, unsigned int size);

    void addMeshGroup(std::string file_path);
    Engine::GameObject* addMeshNode(ZS3M::SceneNode* node);

    void recoverFromSnapshot(Engine::WorldSnapshot* snapshot);

    World();

};

QString getPropertyString(PROPERTY_TYPE type);

#endif
