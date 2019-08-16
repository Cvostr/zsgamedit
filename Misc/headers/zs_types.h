#ifndef ZS_TYPES_H
#define ZS_TYPES_H

#include <QString>
#include <vector>
#include <string>

enum RESOURCE_TYPE {RESOURCE_TYPE_NONE,
                    RESOURCE_TYPE_TEXTURE,
                    RESOURCE_TYPE_MESH,
                    RESOURCE_TYPE_AUDIO,
                    RESOURCE_TYPE_MATERIAL,
                    RESOURCE_TYPE_SCRIPT,
                    RESOURCE_TYPE_FILE = 1000};

typedef struct Resource{
    QString file_path; //Resource file
    QString rel_path; //Resource project dir related path
    std::string resource_label;
    RESOURCE_TYPE type; //Resource type
    void* class_ptr; //Pointer to resource class
}Resource;

struct Project{
    QString label; //Label of the project
    QString root_path; //Project root folder

    int version; //Project version
    int perspective; //3D or 2D

    std::string startup_scene;

    std::vector<Resource> resources;
    Resource* getResource(QString rel_path);
};

#endif // ZS_TYPES_H
