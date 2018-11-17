#ifndef project_edit_h
#define project_edit_h

#include <QMainWindow>
#include <QTreeWidget>
#include <QListWidget>
#include <SDL2/SDL.h>
#include <vector>
#include "../../Render/headers/zs-pipeline.h"
#include "../../World/headers/zs-camera.h"

struct Resource;
struct Project;

#include "../../World/headers/World.h"

#define RESOURCE_TYPE_TEXTURE 0
#define RESOURCE_TYPE_MESH 1

namespace Ui {
class EditWindow;
}

struct Resource{
    QString file_path; //Resource file
    QString rel_path; //Resource project dir related path
    unsigned int type; //Resource type
    void* class_ptr; //Pointer to resource class
};


struct Project{
    QString label; //Label of the project
    QString root_path; //Project root folder

    int version; //Project version
    int perspective;

    std::vector<Resource> resources;

};

class EditWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onFileListItemClicked();
    void onObjectListItemClicked();

    void onAddNewGameObject();
    void onSceneSave();
    void onSceneSaveAs();
    void onOpenScene();

private:
    QString current_dir;
    QString scene_path;
    bool hasSceneFile;

    SDL_Window* window; //Editor preview sdl2 window
    SDL_GLContext glcontext; //Editor preview window opengl context

    RenderPipeline* render;

    QTreeWidgetItem* column_item_go;

public:
    bool ready; //Is everything loaded?

    World world;
    ZSPIRE::Camera edit_camera;
    Project project;

    void init();
    void updateFileList(); //Updates content in file list widget
    void setupObjectsHieList();
    void setViewDirectory(QString dir_path);

    void lookForResources(QString path); //Recursively walk through game directory and load founded resources
    void loadResource(Resource* resource);

    void openFile(QString file_path); //Useful to open a file
    void assignIconFile(QListWidgetItem* item);

    void glRender(); //Invoke opengl rendering

    InspectorWin* getInspector();

    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();


private:
    Ui::EditWindow *ui;
};

class ObjTreeWgt : public QTreeWidget{
    Q_OBJECT

public:
    ObjTreeWgt(QWidget* parent = nullptr);

    void dropEvent(QDropEvent* event);

    World* world_ptr;
};

namespace ZSEditor {
    EditWindow* openProject(QString conf_file_path);
    EditWindow* openEditor();
}

#endif
