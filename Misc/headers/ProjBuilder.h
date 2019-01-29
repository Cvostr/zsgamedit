#ifndef PROJBUILDER_H
#define PROJBUILDER_H

#ifndef project_edit_h
#define INCLUDE_PRJ_ONLY
#include "../../ProjEd/headers/ProjectEdit.h"
#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include "ui_buildconsole.h"
#endif

namespace Ui {
class BuilderWindow;
}

class BuilderWindow : public QMainWindow{
    Q_OBJECT
private:
    QGridLayout layout;
    Ui::BuilderWindow* ui;
    QString outputTextBuf;
public:
    explicit BuilderWindow(QWidget* parent = nullptr);
    QLabel* getTextWgt();
    void addToOutput(QString text);
    ~BuilderWindow();
};

class ProjBuilder{
private:
    BuilderWindow* window;
    Project* proj_ptr;

    void prepareDirectory(); //Prepare output directory
public:
    void showWindow();
    void start();
    ProjBuilder(Project* proj);
};

#endif // PROJBUILDER_H
