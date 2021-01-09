#pragma once

#include "../../Misc/headers/zs_types.h"

#include <misc/misc.h>
#include <QDialog>
#include <QTextBrowser>
#include <QMainWindow>
#include <QGridLayout>
#include <QResizeEvent>
#include "ui_buildconsole.h"
#include <fstream>


namespace Ui {
class BuilderWindow;
}

class ProjBuilder;

class BuilderWindow : public QMainWindow{
    Q_OBJECT
private:
    QGridLayout layout;
    Ui::BuilderWindow* ui;
    QString outputTextBuf;
    
public:
    ProjBuilder* mBuilder;
    explicit BuilderWindow(QWidget* parent = nullptr);
    QTextBrowser* getTextWgt();
    void addToOutput(QString text);

    void resizeEvent(QResizeEvent* event);

    ~BuilderWindow();
};

class BlobWriter{
private:
    ZsStream map_stream;
    std::ofstream blob_stream;

    bool bl_stream_opened;
    uint64_t written_bytes;

    int created_blobs;

    BuilderWindow* window;
public:
    uint64_t max_blob_size; //Limit of one blob size
    QString directory; //Directory to write blobs
    QString name_prefix; //

    QString map_path; // path to resource map file

    void writeToBlob(const std::string& file_path, Engine::ZsResource* res_ptr);
    unsigned int getFileSize(const std::string& file_path);

    BlobWriter(QString map_path, BuilderWindow* window);
    ~BlobWriter();
};

class ProjBuilder{
private:
    BuilderWindow* window;
    Project* proj_ptr;
    BlobWriter* writer;

    void prepareDirectory(); //Prepare output directory
    void copyOtherFiles();
    void _copyOtherFilesDir(const QString dir);
public:
    unsigned int mResourcesSize;
    unsigned int mResourcesWritten;

    void showWindow();
    void start();

    ProjBuilder(Project* proj);
};
