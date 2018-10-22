#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <vector>

#include "ProjEd/headers/ProjectEdit.h"

typedef struct ProjectConf{
    QString projLabel;
    QString projFilePath;
}ProjectConf;

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public slots:
    //Executes on add project in file
    void onAddProjButtonClicked();
    void onShowCreateNewProjectWindow();
    void onSelectProjectToOpen();

public:
    EditWindow* edit_win_ptr;
    std::vector<ProjectConf> projects;

    void setupProjectsConfFile();
    void loadProjectsConfigurations();
    void saveProjectsConfiguration();
    void addProjectToVector(ProjectConf conf);

    void updateListWidgetContent();


    explicit MainWin(QWidget *parent = nullptr);
    ~MainWin();


private:
    Ui::MainWin *ui;
};

#endif // MAINWIN_H
