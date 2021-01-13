#pragma once

#include <QMainWindow>
#include <QTreeWidget>
#include <QListWidget>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QMenu>
#include <fstream>

#include "EdActions.h"
#include "EditorSettings.h"
#include "../../Render/headers/zs-renderer-editor.hpp"
#include <world/Camera.hpp>
#include <audio/OALManager.hpp>
#include <engine/Window.hpp>
#include <engine/EngineComponentManager.hpp>

struct Resource;
struct Project;

#include "../../World/headers/World.h"
#include "../../Misc/headers/ProjBuilder.h"
#include "../../Misc/headers/zs_types.h"
#include "misc/GlyphManager.h"
#include "../../Misc/headers/ThumbnailsMaster.h"

enum GO_TRANSFORM_MODE{
    GO_TRANSFORM_MODE_NONE,
    GO_TRANSFORM_MODE_TRANSLATE,
    GO_TRANSFORM_MODE_SCALE,
    GO_TRANSFORM_MODE_ROTATE
};

#define EW_CLOSE_REASON_UNCLOSED 0
#define EW_CLOSE_REASON_PROJLIST 1
#define EW_CLOSE_REASON_BUILD 2

#include <vector>
#include <QString>

namespace Ui {
class EditWindow;
}

struct EditorInputState{
    bool isLeftBtnHold; //Left mouse button
    bool isRightBtnHold; //Right mouse button
    bool isMidBtnHold; //Middle button (wheel) hold
    bool isLCtrlHold;
    bool isRCtrlHold;
    bool isLAltHold;

    int mouseX;
    int mouseY;
};

struct ObjectTransformState{
    bool isTransforming; //To draw controls or not
    bool isModifying;
    Engine::GameObject* obj_ptr;
    Engine::TransformProperty* tprop_ptr; //Pointer to transform property of obj_ptr
    GO_TRANSFORM_MODE transformMode;

    int Xcf = 0, Ycf = 0, Zcf = 0;

    void setTransformOnObject(GO_TRANSFORM_MODE transformMode);

    ObjectTransformState(){ //Default construct
        isTransforming = false;
        isModifying = false;
        obj_ptr = nullptr;
        tprop_ptr = nullptr;
        transformMode = GO_TRANSFORM_MODE_NONE;
    }
};

struct PropertyPaintState{
    bool enabled;
    Engine::IGameObjectComponent* prop_ptr;

    float time;
    int last_obj = 0;

    PropertyPaintState(){
        time = 0;
        last_obj = -1;
        enabled = false;
        prop_ptr = nullptr;
    }
};



class ObjectCtxMenu;
class FileCtxMenu;
class FileBrowserCtxMenu;

class EditWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void keyPressEvent(QKeyEvent*);

    void onFileListItemClicked();
    void onObjectListItemClicked();
    void onObjectCtxMenuShow(QPoint point);
    void onFileCtxMenuShow(QPoint point);
    void onCameraToObjTeleport();

    Engine::GameObject* onAddNewGameObject();
    void addNewCube();
    void addNewLight();
    void addNewTile();
    void addNewTerrain();
    void addNewAudsource();

    void onSceneSave();
    void onSceneSaveAs();
    void onOpenScene();

    void onNewScene();
    void onNewScript();
    void onNewMaterial();
    void onNewLocalizedStringFile();

    void onImportResource();
    bool onCloseProject();
    void onBuildProject();
    void onRunProject();

    void onUndoPressed();
    void onRedoPressed();

    void onObjectCopy();
    void onObjectPaste();

    void toggleCameras();

    void openRenderSettings();
    void openPhysicsSettings();

    void onOpenConsoleLog();
    void onOpenInspectorWin();

    void onResourceFileChanged(QString path);
    void onDirectoryChanged(QString path);
private:
    QApplication* app_ptr;
    QFileSystemWatcher* mFsWatcher;
    

    QString current_dir; //current directory path string
    QString scene_path;
    bool hasSceneFile; //Is scene saved or loaded

    Engine::Window* mWindow;
    //Render pipeline manager
    RenderPipelineEditor* render;

    ObjectCtxMenu* obj_ctx_menu; //Context menu on object right click
    FileCtxMenu* file_ctx_menu;

    GlyphManager* glyph_manager;

    float cam_pitch = 0;
    float cam_yaw = 0;

    QString sheduled_world;
    bool hasSheduledWorld;

    bool mObjectCopied;

    void createResourceDesc(Engine::ZsResource* _resource, QString absfpath, std::string resource_label);
public:
    bool ready; //Is everything loaded?
    int close_reason;
    bool isSceneRun; //Is scene running right now
    //Use World's camera to render world?
    bool isWorldCamera;

    void* mainwin_ptr;
    World world;
    Engine::Camera edit_camera; //Camera to show editing scene
    Project project; //info about current project
    EditorInputState input_state;
    ObjectTransformState obj_trstate; //Describes object transform
    PropertyPaintState ppaint_state; //Describes property painting feature state
    Engine::WorldSnapshot run_world_snapshot; //useful to recover world state after running

    //Vector to store all editor managers
    Engine::EngineComponentManager* mComponentManager;

    //Thumbnails master
    ThumbnailsMaster* thumb_master;

    EditorSettings settings; //seetings for editor

    QString getCurrentDirectory();

    void init();
    void updateFileList(); //Updates content in file list widget
    void updateFileListItemIcon(QString file);
    void setupObjectsHieList();
    void setViewDirectory(QString dir_path);
    //Recursively walk through game directory and load founded resources
    void lookForResources(QString path); 
    void processResourceFile(QFileInfo fileInfo);
    void ImportResource(QString pathToResource);
    bool checkExtension(QString fpath, QString ext);

    //Create new text file with some content
    QString createNewTextFile(QString directory, QString name, QString ext, const char* content, uint32_t size);
    //Useful to open a file
    void openFile(QString file_path); 
    void addFileToObjectList(QString file_path);
    void assignIconFile(QListWidgetItem* item);

    void runWorld();
    void stopWorld();
    void sheduleWorldLoad(QString file_path);

    void glRender(); //Invoke opengl rendering

    RenderPipelineEditor* getRenderPipeline();
    InspectorWin* getInspector();
    //SDL2 input events stuff
    void onLeftBtnClicked(int X, int Y);
    void onRightBtnClicked(int X, int Y);
    void onMouseMotion(int relX, int relY);
    void onMouseWheel(int x, int y);
    void onKeyDown(SDL_Keysym sym);
    void sceneWalkWASD();

    void resizeEvent(QResizeEvent* event);

    void updateDeltaTime();

    void setGameViewWindowSize(int W, int H);

    Ui::EditWindow* ui;

    QTreeWidget* getObjectListWidget();
    QListWidget* getFilesListWidget();

    explicit EditWindow(QApplication* app, QWidget *parent = nullptr);
    ~EditWindow();

    void callObjectDeletion(Engine::GameObjectLink link);
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

    void onStorePrefabPressed();
    void onInfoPressed();
private:
    Engine::GameObject* obj_ptr;
    QMenu* menu; //Menu object to contain everything

    QAction* action_dub; //Button to dublicate object
    QAction* action_delete; //Button to delete object

    QAction* action_move;
    QAction* action_scale;
    QAction* action_rotate;

    QAction* store_to_prefab;

    QAction* object_info; //Show dialog with detailed object information
public:
    bool displayTransforms;

    ObjectCtxMenu(QWidget* parent = nullptr);
    void setObjectPtr(Engine::GameObject* obj_ptr);
    void show(QPoint point);
    void close();
};

class FileCtxMenu : public QObject{
    Q_OBJECT
public slots:
    void onDeleteClicked();
    void onRename();

    void onOpenInExplorerPressed();
private:
    QMenu* menu; //Menu object to contain everything

    QAction* action_rename; //Button to dublicate object
    QAction* action_delete; //Button to delete object

    QAction* action_open_in_explorer; //Button to delete object
public:
    QString file_path; //path to selected file
    QString file_name;
    QString directory;

    FileCtxMenu(QWidget* parent = nullptr);
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

class FileRenameDialog : public QDialog{
    Q_OBJECT
private:
    QPushButton del_btn;
    QPushButton close_btn;

    QGridLayout contentLayout;
    QLabel rename_message;
    QLineEdit edit_field;
public slots:
    void onRenameButtonPressed();

public:
    QString file_path;
    QString file_name;

    FileRenameDialog(QString file_path, QString file_nam, QWidget* parent = nullptr);
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
    FileListWgt(QWidget* parent = nullptr);

    void dropEvent(QDropEvent* event);

    World* world_ptr;
};

namespace ZSEditor {
    EditWindow* openProject(QApplication* app, Project& project);
    EditWindow* openEditor();
}

EdActions* getActionManager();
