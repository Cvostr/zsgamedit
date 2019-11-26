#ifndef ZS_TYPES_H
#define ZS_TYPES_H

#include <QString>
#include <vector>
#include <string>
#include <game.h>

typedef struct Resource{
    QString file_path; //Resource file
    QString rel_path; //Resource project dir related path
    std::string resource_label;
    RESOURCE_TYPE type; //Resource type
    void* class_ptr; //Pointer to resource class
}Resource;

typedef struct Project{
    std::string label; //Label of the project
    std::string root_path; //Project root folder

    int version; //Project version
    ZSPERSPECTIVE perspective; //3D or 2D

    std::string startup_scene;

    std::vector<Resource> resources;
    Resource* getResource(QString rel_path);
}Project;

#endif // ZS_TYPES_H
