#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <vector>

#include "ProjEd/headers/ProjectEdit.h"
#include "ProjEd/headers/ProjCreateWin.h"

#include <engine.h> //include engine headers

typedef struct ProjectConf{
    QString projLabel; //Label of project
    QString projFilePath; //Path to .inf file of project
    QString projectRootPath; //Root path of object

    bool removed; //mark project as marked

    Project editwin_proj;

    ProjectConf(){
        removed = false;
    }
}ProjectConf;

namespace Ui {
class MainWin;
}

class MainWin;

class ProjectListWgt : public QListWidget{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void onRightClick(QPoint pos);
    void onLeftClick(QPoint pos);
public:
    ProjectListWgt(QWidget* parent = nullptr);
};

class ProjectCtxMenu : public QObject{
    Q_OBJECT
public slots:
    void onDeleteClicked();
    void onOpenDirClicked();

    void runEngineClickedGL();
    void runEngineClickedVK();

private:
    QMenu* menu; //Menu object to contain everything

    QAction* action_run_engine;
    QAction* action_run_engine_vk;
    QAction* action_delete; //Button to delete project entry from list
    QAction* action_open_folder; //Button to show explorer with root project folder
public:
    MainWin* win;
    ProjectConf* project_conf_ptr; //pointer to conf of selected project

    ProjectCtxMenu(MainWin* win, QWidget* parent = nullptr);
    void show(QPoint point);

    void runEngineClicked(ZSGAPI gapi);
    void close();
};

class MainWin : public QMainWindow
{
    Q_OBJECT
private:
    ProjectCtxMenu* project_menu;
    QApplication* app_ptr;
public slots:
    //Executes on add project in file
    void onAddProjButtonClicked();
    void onShowCreateNewProjectWindow();
    void onSelectProjectToOpen();

    void showCtxMenu(QPoint point);

    void onDestroy();

public:
    Ui::MainWin *ui;
    ZSpireEngine* engine;

    EditWindow* edit_win_ptr;
    std::vector<ProjectConf> projects;

    void setupProjectsConfFile();
    void loadProjectsConfigurations();
    void saveProjectsConfiguration();
    void addProjectToVector(ProjectConf conf);

    void updateListWidgetContent();

    void resizeEvent(QResizeEvent* event);
    virtual void closeEvent ( QCloseEvent * event );

    explicit MainWin(QApplication* app, QWidget *parent = nullptr);
    ~MainWin();
};

#endif // MAINWIN_H
