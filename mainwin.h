#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <vector>

#include "ProjEd/headers/ProjectEdit.h"
#include "ProjEd/headers/ProjCreateWin.h"

#include "triggers.h"
#ifdef USE_ZSPIRE
#include "engine.h" //include engine headers
#endif

typedef struct ProjectConf{
    QString projLabel;
    QString projFilePath;
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
    void runEngineClickedGL();
    void runEngineClickedVK();

private:
    QMenu* menu; //Menu object to contain everything

    QAction* action_run_engine;
    QAction* action_run_engine_vk;
    QAction* action_delete; //Button to delete project entry from list
public:
    MainWin* win;

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
public slots:
    //Executes on add project in file
    void onAddProjButtonClicked();
    void onShowCreateNewProjectWindow();
    void onSelectProjectToOpen();

    void showCtxMenu(QPoint point);

public:
    Ui::MainWin *ui;
#ifdef USE_ZSPIRE
    ZSpireEngine* engine;
#endif
    EditWindow* edit_win_ptr;
    std::vector<ProjectConf> projects;

    void setupProjectsConfFile();
    void loadProjectsConfigurations();
    void saveProjectsConfiguration();
    void addProjectToVector(ProjectConf conf);

    void updateListWidgetContent();

    explicit MainWin(QWidget *parent = nullptr);
    ~MainWin();
};

#endif // MAINWIN_H
