#ifndef PROJBUILDER_H
#define PROJBUILDER_H

#ifndef project_edit_h
#define INCLUDE_PRJ_ONLY
#include "../../ProjEd/headers/ProjectEdit.h"
#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include "ui_buildconsole.h"
#include <fstream>
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

class BlobWriter{
private:
    std::ofstream map_stream;
    std::ofstream blob_stream;

    bool bl_stream_opened;
    int64_t written_bytes;

    int created_blobs;

    BuilderWindow* window;
public:
    int64_t max_blob_size; //Limit of one blob size
    QString directory; //Directory to write blobs
    QString name_prefix; //

    QString map_path; // path to resource map file

    void writeToBlob(std::string file_path);
    int getFileSize(std::string file_path);

    BlobWriter(QString map_path, BuilderWindow* window);
};

class ProjBuilder{
private:
    BuilderWindow* window;
    Project* proj_ptr;
    BlobWriter* writer;

    void prepareDirectory(); //Prepare output directory
public:
    void showWindow();
    void start();
    ProjBuilder(Project* proj);
};

#endif // PROJBUILDER_H
