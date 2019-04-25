#include "headers/ProjBuilder.h"
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <QDir>

void ProjBuilder::showWindow(){
    window->show();
}
void ProjBuilder::start(){
    window->addToOutput("Build started!");

    prepareDirectory(); //prepare output directory

    window->addToOutput("Packing resources...");
    writer = new BlobWriter(proj_ptr->root_path + "/.build/resources.map", window);
    writer->directory = proj_ptr->root_path + "/.build";
    writer->name_prefix = "blob_";
    writer->max_blob_size = 50*1024*1024; //50 megabytes limit

    for(unsigned int res_i = 0; res_i < proj_ptr->resources.size(); res_i ++){ //iterate over all resources
        Resource* res_ptr = &proj_ptr->resources[res_i];
        QString type_str;
        switch(res_ptr->type){
            case RESOURCE_TYPE_TEXTURE: type_str = "TEXTURE";
                 break;
            case RESOURCE_TYPE_MESH : type_str = "MESH";
                break;
            case RESOURCE_TYPE_AUDIO : type_str = "AUDIO";
                break;
        }
        window->addToOutput("Resource #" + QString::number(res_i) + " type: " + type_str + " " + res_ptr->rel_path);
        writer->writeToBlob((proj_ptr->root_path + "/" + res_ptr->rel_path).toStdString(), res_ptr->rel_path.toStdString(), res_ptr);
    }
    delete writer;
}

ProjBuilder::ProjBuilder(Project* proj){
    this->window = new BuilderWindow;
    this->proj_ptr = proj;

    this->writer = nullptr; //No writer by default
}

void ProjBuilder::prepareDirectory(){
    QDir out_dir(this->proj_ptr->root_path + "/.build");
    if(out_dir.exists() == false){ //if there is no .build directory
        window->addToOutput("Create .build directory");
        QDir(this->proj_ptr->root_path).mkdir(".build"); //create ddirectory
    }
}

BuilderWindow::BuilderWindow(QWidget* parent) : QMainWindow(parent),
ui(new Ui::BuilderWindow){
    ui->setupUi(this);
}

BuilderWindow::~BuilderWindow(){

}

QLabel* BuilderWindow::getTextWgt(){
    return ui->outputText;
}
void BuilderWindow::addToOutput(QString text){
    this->outputTextBuf += text + "\n";
    this->ui->outputText->setText(outputTextBuf);
}

BlobWriter::BlobWriter(QString map_path, BuilderWindow* window){
    this->map_path = map_path;
    this->map_stream.open(map_path.toStdString(), std::ofstream::binary);

    this->window = window;

    bl_stream_opened = false;
    written_bytes = 0;

    created_blobs = 0;
}

BlobWriter::~BlobWriter(){
    this->map_stream << "_END";
    this->map_stream.close();
}

int BlobWriter::getFileSize(std::string file_path){
    FILE* file = fopen(file_path.c_str(), "rb");
    struct stat buff;

    fstat(fileno(file), &buff); //Getting file info

    fclose(file);

    return buff.st_size;
}

void BlobWriter::writeToBlob(std::string file_path, std::string rel_path, Resource* res_ptr){
    if(!this->bl_stream_opened){ //if blob not opened, open it
        QString blob = directory + "/" + this->name_prefix + QString::number(created_blobs);
        //Open binary blob stream
        this->blob_stream.open(blob.toStdString(), std::ofstream::binary);
        window->addToOutput("Creating Blob /" + this->name_prefix + QString::number(created_blobs));
        bl_stream_opened = true;
        created_blobs += 1;
    }

    int size = getFileSize(file_path);
    unsigned char* data = new unsigned char[size]; //allocate memory
    //open input stream of resource file
    std::ifstream file_stream;
    file_stream.open(file_path, std::ifstream::binary);
    //read resource file
    file_stream.read(reinterpret_cast<char*>(data), size);
    file_stream.close();

    //Write data
    blob_stream.write(reinterpret_cast<char*>(data), size);

    delete[] data; //free data

    //Write data to map
    map_stream << "entry " << rel_path << " " << res_ptr->resource_label << " "; //write header
    map_stream.write(reinterpret_cast<char*>(&written_bytes), sizeof(int64_t));
    map_stream.write(reinterpret_cast<char*>(&size), sizeof(int));
    map_stream.write(reinterpret_cast<char*>(&res_ptr->type), sizeof(RESOURCE_TYPE));
    map_stream << "\n";

    this->written_bytes += size; //Increase amount of written bytes

    window->addToOutput("   Written to /" + this->name_prefix + QString::number(created_blobs - 1) + " " + QString::number(size) + " bytes");

}
