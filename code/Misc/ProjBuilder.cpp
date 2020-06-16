#include "headers/ProjBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <QDir>

extern QApplication* a;
extern ZSGAME_DATA* game_data;

void ProjBuilder::showWindow(){
    window->show();
}
void ProjBuilder::start(){
    window->addToOutput("Build started!");

    prepareDirectory(); //prepare output directory

    window->addToOutput("Packing resources...");
    writer = new BlobWriter(QString::fromStdString(proj_ptr->root_path) + "/.build/resources.map", window);
    writer->directory = QString::fromStdString(proj_ptr->root_path) + "/.build";
    writer->name_prefix = "blob_";
    writer->max_blob_size = 100*1024*1024; //100 megabytes limit

    for(unsigned int res_i = 0; res_i < game_data->resources->getResourcesSize(); res_i ++){ //iterate over all resources
        Engine::ZsResource* res_ptr = game_data->resources->getResourceByIndex(res_i);
        //Not store base resources
        if(res_ptr->rel_path[0] == '@')
            continue;

        QString type_str;
        switch(res_ptr->resource_type){
            case RESOURCE_TYPE_NONE: type_str = "NONE";
                break;
            case RESOURCE_TYPE_FILE: type_str = "FILE";
                break;
            case RESOURCE_TYPE_TEXTURE: type_str = "TEXTURE";
                 break;
            case RESOURCE_TYPE_MESH : type_str = "MESH";
                break;
            case RESOURCE_TYPE_AUDIO : type_str = "AUDIO";
                break;
            case RESOURCE_TYPE_SCRIPT : type_str = "SCRIPT";
                break;
            case RESOURCE_TYPE_MATERIAL : type_str = "MATERIAL";
                break;
            case RESOURCE_TYPE_ANIMATION : type_str = "ANIMATION";
                break;
            case RESOURCE_TYPE_FONT : type_str = "FONT";
                break;
        }
        window->addToOutput("Resource #" + QString::number(res_i) + " type: " + type_str + " " + QString::fromStdString(res_ptr->rel_path));
        //Update window content
        a->processEvents();
        std::string abs_path = (proj_ptr->root_path + "/" + res_ptr->rel_path);
        //write resource to blob
        writer->writeToBlob(abs_path, res_ptr);
    }

    window->addToOutput("All Resources are Written!");

    copyOtherFiles();

    delete writer;
}

void ProjBuilder::copyOtherFiles() {
    _copyOtherFilesDir(QString::fromStdString(this->proj_ptr->root_path));
}

void ProjBuilder::_copyOtherFilesDir(const QString dir) {
    QDir directory(dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsFirst | QDir::Name | QDir::Reversed);

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator
    
    for (int i = 0; i < list.size(); i++) { //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        
        if (fileInfo.isDir())
            _copyOtherFilesDir(fileInfo.absoluteFilePath());
        
        if (fileInfo.fileName().endsWith(".terrain")) {
            QString abs_path = fileInfo.absoluteFilePath();

            QString rel_path;
            rel_path = abs_path.remove(0, static_cast<int>(proj_ptr->root_path.size() + 1));

            Engine::ZsResource resource;
            resource.rel_path = rel_path.toStdString();
            resource.resource_label = rel_path.toStdString();
            resource.resource_type = RESOURCE_TYPE_NONE;

            writer->writeToBlob(fileInfo.absoluteFilePath().toStdString(), &resource);
        }
    }
}

ProjBuilder::ProjBuilder(Project* proj){
    this->window = new BuilderWindow;
    this->proj_ptr = proj;

    this->writer = nullptr; //No writer by default
}

void ProjBuilder::prepareDirectory(){
    QDir out_dir(QString::fromStdString(this->proj_ptr->root_path) + "/.build");
    if(out_dir.exists() == false){ //if there is no .build directory
        window->addToOutput("Create .build directory");
        QDir(QString::fromStdString(this->proj_ptr->root_path)).mkdir(".build"); //create ddirectory
    }
}

BuilderWindow::BuilderWindow(QWidget* parent) : QMainWindow(parent),
ui(new Ui::BuilderWindow){
    ui->setupUi(this);
}

BuilderWindow::~BuilderWindow(){

}

void BuilderWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    int new_width = event->size().width();
    int new_height = event->size().height();

    ui->scrollArea->resize(new_width - 20, new_height);
    //ui->outputText->resize(new_width - 40, new_height - 20);
}

QLabel* BuilderWindow::getTextWgt(){
    return ui->outputText;
}
void BuilderWindow::addToOutput(QString text){
    this->outputTextBuf += text + "\n";
    this->ui->outputText->setText(outputTextBuf);
    ui->outputText->resize(ui->outputText->size().width(), ui->outputText->size().height() + 10);
}

BlobWriter::BlobWriter(QString map_path, BuilderWindow* window){
    this->map_path = map_path;
    this->map_stream.open(map_path.toStdString(), std::ofstream::binary);
    //Store pointer to qt window
    this->window = window;
    //Set all stats to zero
    bl_stream_opened = false;
    written_bytes = 0;
    created_blobs = 0;
}

BlobWriter::~BlobWriter(){
    //Write terminating string "_END"
    this->map_stream << "_END";
    //Close map stream
    this->map_stream.close();
}

unsigned int BlobWriter::getFileSize(std::string file_path){
    std::ifstream file_stream;
    //Open file from end
    file_stream.open(file_path, std::iostream::binary | std::iostream::ate);
    //if file doesn't exist, then exit function with 0
    if (file_stream.fail()) return 0;
    //get file size
    unsigned int result = static_cast<unsigned int>(file_stream.tellg());
    //Clse file stream
    file_stream.close();

    return result;
}

void BlobWriter::writeToBlob(std::string& file_path, Engine::ZsResource* res_ptr){
    //check, if we got over the blob limit
    if(written_bytes >= this->max_blob_size){
        written_bytes = 0;
        this->bl_stream_opened = false;
        blob_stream.close();
    }
    //if blob not opened, open it
    if(!this->bl_stream_opened){ 
        QString blob = directory + "/" + this->name_prefix + QString::number(created_blobs);
        //Open binary blob stream
        this->blob_stream.open(blob.toStdString(), std::ofstream::binary);
        window->addToOutput("Creating Blob /" + this->name_prefix + QString::number(created_blobs));
        bl_stream_opened = true;
        //Increase amount of created blobs
        created_blobs += 1;
    }
    //Get resource file size
    unsigned int size = getFileSize(file_path);
    //allocate memory for resource file
    unsigned char* data = new unsigned char[size]; 
    //open input stream of resource file
    std::ifstream file_stream;
    file_stream.open(file_path, std::ifstream::binary);
    //read resource file
    file_stream.read(reinterpret_cast<char*>(data), size);
    file_stream.close();
    //Write data
    blob_stream.write(reinterpret_cast<char*>(data), size);
    //free data
    delete[] data; 
    //Make string with blob path
    QString blob_path = this->name_prefix + QString::number(created_blobs - 1);
    //Write data to map
    map_stream << "entry " << res_ptr->rel_path << " " << res_ptr->resource_label << " " << blob_path.toStdString() << " "; //write header
    map_stream.write(reinterpret_cast<char*>(&written_bytes), sizeof(uint64_t));
    map_stream.write(reinterpret_cast<char*>(&size), sizeof(unsigned int));
    map_stream.write(reinterpret_cast<char*>(&res_ptr->resource_type), sizeof(RESOURCE_TYPE));
    map_stream << "\n";
    //Increase amount of written bytes
    this->written_bytes += static_cast<unsigned int>(size); 
    //Add to output
    window->addToOutput("   Written to /" + this->name_prefix + QString::number(created_blobs - 1) + " " + QString::number(size) + " bytes");
}
