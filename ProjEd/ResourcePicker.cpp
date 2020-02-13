#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "headers/ProjectEdit.h"
#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QDragEnterEvent>
#include <QObject>
#include <QDir>
#include <iostream>

extern EditWindow* _editor_win;
extern Project* project_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

void ResourcePickDialog::onResourceSelected(){
    if(area->rel_path_std == nullptr) return;

    QListWidgetItem* selected = this->list->currentItem();
    QString resource_path = selected->text(); //Get selected text
    //Make action
    Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(area->go_property);
    getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    //Apply resource change
    *area->rel_path_std = resource_path.toStdString();

    area->PropertyEditArea::callPropertyUpdate();
    this->resource_text->setText(resource_path);
    emit accept(); //Close dailog with positive answer
}

void ResourcePickDialog::onDialogClose(){

}

void ResourcePickDialog::onNeedToShow(){

    if(this->area->resource_type == RESOURCE_TYPE_TEXTURE || this->area->resource_type == RESOURCE_TYPE_MATERIAL || this->area->resource_type == RESOURCE_TYPE_MESH){
        this->list->setViewMode(QListView::IconMode);
        this->list->setIconSize(QSize(75, 75));
    }

    this->extension_mask = area->extension_mask; //send extension mask
    this->list->clear();
    //Receiving pointer to project
    unsigned int resources_num = game_data->resources->getResourcesSize();

    if(area->isShowNoneItem){
        new QListWidgetItem("@none", this->list);
    }

    if(area->resource_type < RESOURCE_TYPE_FILE){ // if it is an resource
        //Iterate over all resources
        for(unsigned int res_i = 0; res_i < resources_num; res_i ++){
            Engine::ZsResource* resource_ptr = game_data->resources->getResourceByIndex(res_i);
            if(resource_ptr->resource_type == area->resource_type){ //if type is the same
                QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(resource_ptr->resource_label), this->list); //add resource to list
                QImage* img = nullptr;
                std::string fpath;
                if(this->area->resource_type == RESOURCE_TYPE_TEXTURE || this->area->resource_type == RESOURCE_TYPE_MATERIAL)
                    fpath = _editor_win->project.root_path + "/" + resource_ptr->resource_label;

                if(this->area->resource_type == RESOURCE_TYPE_MESH)
                    fpath = resource_ptr->resource_label;

                if(_editor_win->thumb_master->isAvailable(fpath))
                    img = _editor_win->thumb_master->texture_thumbnails.at(fpath);
                if(img)
                    item->setIcon(QIcon(QPixmap::fromImage(*img)));
            }
        }
    }else{ //we want to pick common file
        findFiles(QString::fromStdString(project_ptr->root_path));
    }
    this->show();
    //update label content
    area->updateLabel();
}

void ResourcePickDialog::findFiles(QString directory){
    //Obtain pointer to project
    QDir _directory (directory); //Creating QDir object
    _directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    _directory.setSorting(QDir::DirsLast); //I want to recursive call this function after all files

    QFileInfoList list = _directory.entryInfoList(); //Get folder content iterator

    for(int i = 0; i < list.size(); i ++){ //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info

        if(fileInfo.isFile() == true){ //we found a file
            QString name = fileInfo.fileName();
            if(name.endsWith(extension_mask)){ //if extension matches
                QString wlabel = directory + "/" + name;
                wlabel.remove(0, static_cast<int>(project_ptr->root_path.size()) + 1); //Remove path to project
                new QListWidgetItem(wlabel, this->list);
            }
        }

        if(fileInfo.isDir() == true){ //If it is directory
            QString newdir_str = directory + "/"+ fileInfo.fileName();
            findFiles(newdir_str);  //Call this function inside next dir
        }
    }
}

ResourcePickDialog::ResourcePickDialog(QWidget* parent) :
    QDialog (parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint){
    resize(700, 600);
    contentLayout = new QGridLayout(this); // Alocation of layout
    list = new QListWidget;
    this->setWindowTitle("Select Resource");

    contentLayout->addWidget(list);
    connect(this->list, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onResourceSelected())); //Connect to slot
    connect(this, SIGNAL(rejected()), this, SLOT(onDialogClose())); //Connect to slot
    setLayout(contentLayout);

}

ResourcePickDialog::~ResourcePickDialog(){
    delete list;
    delete contentLayout;
}



QLabelResourcePickWgt::QLabelResourcePickWgt(PropertyEditArea* area_ptr, QWidget* parent) : QLabel (parent){
    setAcceptDrops(true);
    this->area_ptr = area_ptr;
    //Set size as slim text
    setMaximumSize(QSize(100000, 50));
}

void QLabelResourcePickWgt::dragEnterEvent( QDragEnterEvent* event ){
    event->acceptProposedAction();
}
void QLabelResourcePickWgt::dropEvent( QDropEvent* event ){

    assert(event);
    QList<QListWidgetItem*> file_dropped = _editor_win->getFilesListWidget()->selectedItems();
    QList<QTreeWidgetItem*> object_dropped = _editor_win->getObjectListWidget()->selectedItems();
    //We drooped common file
    if(file_dropped.length() > 0){
        //Get pointer to resource picker
        PickResourceArea* resource_area = static_cast<PickResourceArea*>(area_ptr);
        if(resource_area->rel_path_std == nullptr) return;
        //if we drooped texture to texture pick area
        if((resource_area->resource_type == RESOURCE_TYPE_TEXTURE && (file_dropped[0]->text().endsWith(".dds") || file_dropped[0]->text().endsWith(".DDS")))
                || (resource_area->resource_type == RESOURCE_TYPE_MATERIAL && (file_dropped[0]->text().endsWith(".zsmat")))){

            QString newpath = _editor_win->getCurrentDirectory() + "/" + file_dropped[0]->text();
            newpath.remove(0, static_cast<int>(_editor_win->project.root_path.size()) + 1);
            //Set new relative path
            *resource_area->rel_path_std = newpath.toStdString();

            Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(area_ptr->go_property);
            getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
            //Apply resource change
            area_ptr->PropertyEditArea::callPropertyUpdate();
        }
    }

    if(object_dropped.length() > 0){
        GameObject* obj = (GameObject*)_editor_win->world.getGameObjectByLabel(object_dropped[0]->text(0).toStdString());
        //if we picking property
        if(area_ptr->type == PEA_TYPE_PROPPICK){
            PropertyPickArea* _area_ptr = static_cast<PropertyPickArea*>(area_ptr);
            Engine::GameObjectProperty* prop = obj->getPropertyPtrByType(_area_ptr->prop_type);
            if(prop != nullptr){ //Property with that type exist
                //Writing pointer
                *_area_ptr->property_ptr_ptr = prop;
                //Assigning object string ID
                *(_area_ptr->oj_label_ptr) = prop->go_link.updLinkPtr()->str_id;
            }
            _area_ptr->setup();
        }

        if(area_ptr->type == PEA_TYPE_GOBJECT_PICK){
            GameobjectPickArea* _area_ptr = static_cast<GameobjectPickArea*>(area_ptr);
            *_area_ptr->gameobject_ptr_ptr = obj;
            _area_ptr->setup();
        }
    }
}

//Pick resoource area stuff
PickResourceArea::PickResourceArea(RESOURCE_TYPE resource_type){
    type = PEA_TYPE_RESPICK;
    this->rel_path_std = nullptr;
    this->resource_type = resource_type; //Default type is texture

    isShowNoneItem = false;

    respick_btn = new QPushButton; //Allocation of QPushButton
    elem_layout->addSpacing(6);
    relpath_label = new QLabelResourcePickWgt(this); //Allocation of resource relpath text
    elem_layout->addWidget(relpath_label);
    //Space between text and button
    elem_layout->addSpacing(6);
    elem_layout->addWidget(respick_btn);
    respick_btn->setText("Select...");

    this->dialog = new ResourcePickDialog; //Allocation of dialog
    dialog->area = this;
    dialog->resource_text = this->relpath_label;

}
PickResourceArea::~PickResourceArea(){

}

void PickResourceArea::destroyContent(){
    delete respick_btn;
    delete dialog;
    delete relpath_label;
}

void PickResourceArea::addToInspector(InspectorWin* win){
    QObject::connect(respick_btn,  SIGNAL(clicked()), dialog, SLOT(onNeedToShow())); //On click on this button dialog will be shown
    win->getContentLayout()->addLayout(elem_layout);
}

void PickResourceArea::setup(){
   // QString resource_relpath = *this->rel_path;
}

void PickResourceArea::updateLabel(){
    if(this->rel_path_std == nullptr) //If vector hasn't been set
        return; //Go out

    bool resource_specified = *rel_path_std != "@none";

    if((this->resource_type == RESOURCE_TYPE_MATERIAL || this->resource_type == RESOURCE_TYPE_TEXTURE) && resource_specified){
        std::string fpath = _editor_win->project.root_path + "/";

        fpath += *rel_path_std;

        QImage* img = nullptr;
        //Set resource pixmap
        if(_editor_win->thumb_master->isAvailable(fpath))
            img = _editor_win->thumb_master->texture_thumbnails.at(fpath);
        if(img)
            relpath_label->setPixmap(QPixmap::fromImage(*img));
        relpath_label->setScaledContents(true);

        relpath_label->setMinimumSize(QSize(50, 50));
        relpath_label->setMaximumSize(QSize(50, 50));
    }else{
        relpath_label->setText(QString::fromStdString(*rel_path_std));
    }
}

void PickResourceArea::updateValues(){
    if(this->rel_path_std == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current value in text field
    QString cur = this->relpath_label->text();

    updateLabel();
}
