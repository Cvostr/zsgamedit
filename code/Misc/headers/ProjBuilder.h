#ifndef PROJBUILDER_H
#define PROJBUILDER_H


#include "../../Misc/headers/zs_types.h"

#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include <QResizeEvent>
#include "ui_buildconsole.h"
#include <fstream>


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

    void resizeEvent(QResizeEvent* event);

    ~BuilderWindow();
};

class BlobWriter{
private:
    std::ofstream map_stream;
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

    void writeToBlob(std::string& file_path, std::string& rel_path, Engine::ZsResource* res_ptr);
    unsigned int getFileSize(std::string file_path);

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
    void showWindow();
    void start();

    ProjBuilder(Project* proj);
};

#endif // PROJBUILDER_H
