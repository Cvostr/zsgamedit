#ifndef project_edit_h
#define project_edit_h

#include <QMainWindow>
#include <QTreeWidget>
#include <SDL2/SDL.h>
#include <vector>
#include "../../Render/headers/zs-pipeline.h"
#include "../../World/headers/World.h"

#define RESOURCE_TYPE_TEXTURE 0
#define RESOURCE_TYPE_MESH 1

namespace Ui {
class EditWindow;
}

struct Resource{
    QString file_path; //Resource file
    QString rel_path; //Resource project dir related path
    int type; //Resource type
    void* class_ptr; //Pointer to resource class
};

struct Project{
    QString label; //Label of the project
    QString root_path; //Project root folder

    int version; //Project version

    std::vector<Resource> resources;

    //std::vector<ZSPIRE::Mesh> meshes;
    //std::vector<ZSPIRE::Texture> textures;
};

class EditWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onFileListItemClicked();

    void onAddNewGameObject();
    void onSceneSave();
    void onSceneSaveAs();

private:
    QString current_dir;
    QString scene_path;
    bool hasSceneFile;

    SDL_Window* window; //Editor preview sdl2 window
    SDL_GLContext glcontext; //Editor preview window opengl context

    RenderPipeline* render;

    World world;

    QTreeWidgetItem* column_item_go;
public:
    bool ready; //Is everything loaded?

    Project project;

    void init();
    void updateFileList(); //Updates content in file list widget
    void updateObjectsHieList(); //Updates content in objects hierarchy list
    void setupObjectsHieList();
    void setViewDirectory(QString dir_path);

    void lookForResources(QString path); //Recursively walk through game directory and load founded resources
    void loadResource(Resource* resource);

    void openFile(QString file_path); //Useful to open a file

    void glRender();

    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();


private:
    Ui::EditWindow *ui;
};


namespace ZSEditor {
    EditWindow* openProject(QString conf_file_path);
    EditWindow* openEditor();
}

#endif
