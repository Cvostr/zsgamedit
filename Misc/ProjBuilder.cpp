#include "headers/ProjBuilder.h"

#include <QDir>

void ProjBuilder::showWindow(){
    window->show();
}
void ProjBuilder::start(){
    window->addToOutput("Build started!");

    prepareDirectory(); //prepare output directory

    window->addToOutput("Packing resources...");

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
    }

}

ProjBuilder::ProjBuilder(Project* proj){
    this->window = new BuilderWindow;
    this->proj_ptr = proj;
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
