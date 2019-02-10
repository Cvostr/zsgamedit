#ifndef project_edit_h
#define project_edit_h


#ifndef INCLUDE_PRJ_ONLY

#include <QMainWindow>
#include <QTreeWidget>
#include <QListWidget>
#include <QMenu>
#include <SDL2/SDL.h>

#include "EdActions.h"
#include "../../Render/headers/zs-pipeline.h"
#include "../../World/headers/zs-camera.h"

#include "../../Misc/headers/oal_manager.h"

struct Resource;
struct Project;

#include "../../World/headers/World.h"

#include "../../Misc/headers/ProjBuilder.h"

#define GO_TRANSFORM_MODE_NONE 0
#define GO_TRANSFORM_MODE_TRANSLATE 1
#define GO_TRANSFORM_MODE_SCALE 2
#define GO_TRANSFORM_MODE_ROTATE 3

#define EW_CLOSE_REASON_UNCLOSED 0
#define EW_CLOSE_REASON_PROJLIST 1
#define EW_CLOSE_REASON_BUILD 2

#endif

#define RESOURCE_TYPE_TEXTURE 0
#define RESOURCE_TYPE_MESH 1
#define RESOURCE_TYPE_AUDIO 2

#include <vector>
#include <QString>

#ifndef INCLUDE_PRJ_ONLY
namespace Ui {
class EditWindow;
}
#endif

typedef struct Resource{
    QString file_path; //Resource file
    QString rel_path; //Resource project dir related path
    unsigned int type; //Resource type
    void* class_ptr; //Pointer to resource class
}Resource;

struct Project{
    QString label; //Label of the project
    QString root_path; //Project root folder

    int version; //Project version
    int perspective; //3D or 2D

    std::vector<Resource> resources;
};
#ifndef INCLUDE_PRJ_ONLY
struct EditorInputState{
    bool isLeftBtnHold;
    bool isRightBtnHold;
    bool isLCtrlHold;
    bool isRCtrlHold;
};

struct ObjectTransformState{
    bool isTransforming;
    GameObject* obj_ptr;
    TransformProperty* tprop_ptr; //Pointer to transform property of obj_ptr
    int transformMode;

    ObjectTransformState(){ //Default construct
        isTransforming = false;
        obj_ptr = nullptr;
        tprop_ptr = nullptr;
        transformMode = GO_TRANSFORM_MODE_NONE;
    }
};

class ObjectCtxMenu;
class FileCtxMenu;

class EditWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onFileListItemClicked();
    void onObjectListItemClicked();
    void onObjectCtxMenuShow(QPoint point);
    void onFileCtxMenuShow(QPoint point);
    void onCameraToObjTeleport();

    void onAddNewGameObject();
    void onSceneSave();
    void onSceneSaveAs();
    void onOpenScene();
    void onNewScene();

    void onCloseProject();
    void onBuildProject();
    void onRunProject();

    void onUndoPressed();
    void onRedoPressed();

private:
    QString current_dir; //current directory path string
    QString scene_path;
    bool hasSceneFile; //Is scene saved or loaded

    SDL_Window* window; //Editor preview sdl2 window
    SDL_GLContext glcontext; //Editor preview window opengl context

    RenderPipeline* render;

    ObjectCtxMenu* obj_ctx_menu; //Context menu on object right click
    FileCtxMenu* file_ctx_menu;

    float cam_pitch = 0;
    float cam_yaw = 0;
public:
    bool ready; //Is everything loaded?
    int close_reason;
    bool isSceneRun; //Is scene running right now

    World world;
    ZSPIRE::Camera edit_camera; //Camera to show editing scene
    Project project;
    EditorInputState input_state;
    ObjectTransformState obj_trstate; //Describes object transform

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
    //SDL2 input events stuff
    void onLeftBtnClicked(int X, int Y);
    void onRightBtnClicked(int X, int Y);
    void onMouseMotion(int relX, int relY);
    void onMouseWheel(int x, int y);
    void onKeyDown(SDL_Keysym sym);

    Ui::EditWindow *ui;
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

    void callObjectDeletion(GameObjectLink link);
private:

};

class ObjectCtxMenu : public QObject{
    Q_OBJECT
public slots:
    void onDeleteClicked();
    void onDublicateClicked();

    void onMoveClicked();
    void onScaleClicked();
    void onRotateClicked();
private:
    GameObject* obj_ptr;
    QMenu* menu; //Menu object to contain everything

    QAction* action_dub; //Button to dublicate object
    QAction* action_delete; //Button to delete object

    QAction* action_move;
    QAction* action_scale;
    QAction* action_rotate;
public:
    bool displayTransforms;
    EditWindow* win_ptr;

    ObjectCtxMenu(EditWindow* win, QWidget* parent = nullptr);
    void setObjectPtr(GameObject* obj_ptr);
    void show(QPoint point);
    void close();
};

class FileCtxMenu : public QObject{
    Q_OBJECT
public slots:
    void onDeleteClicked();
    void onRename();
private:
    QMenu* menu; //Menu object to contain everything

    QAction* action_rename; //Button to dublicate object
    QAction* action_delete; //Button to delete object
public:
    EditWindow* win_ptr;
    QString file_path; //path to selected file

    FileCtxMenu(EditWindow* win, QWidget* parent = nullptr);
    void show(QPoint point);
    void close();
};

class FileDeleteDialog : public QDialog{
    Q_OBJECT
private:
    QPushButton del_btn;
    QPushButton close_btn;

    QGridLayout contentLayout;
    QLabel del_message;
public slots:
    void onDelButtonPressed();

public:
    QString file_path;

    FileDeleteDialog(QString file_path, QWidget* parent = nullptr);
};

//Class to represent tree widget
class ObjTreeWgt : public QTreeWidget{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void onRightClick(QPoint pos);
    void onLeftClick(QPoint pos);
public:
    EditWindow* win_ptr;
    ObjTreeWgt(QWidget* parent = nullptr);

    void dropEvent(QDropEvent* event);

    World* world_ptr;
};

class FileListWgt : public QListWidget{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void onRightClick(QPoint pos);
    void onLeftClick(QPoint pos);
public:
    EditWindow* win_ptr;
    FileListWgt(QWidget* parent = nullptr);

    World* world_ptr;
};

namespace ZSEditor {
    EditWindow* openProject(QString conf_file_path);
    EditWindow* openEditor();
}

EdActions* getActionManager();
#endif

#endif
