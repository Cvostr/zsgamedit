#include "headers/ProjectEdit.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QMimeData>

#include "ui_editor.h"
#include "ui_inspector_win.h"

extern EditWindow* _editor_win;
extern InspectorWin* _inspector_win;
extern EdActions* _ed_actions_container;
extern ZSGAME_DATA* game_data;

ObjTreeWgt::ObjTreeWgt(QWidget* parent) : QTreeWidget (parent){
    this->world_ptr = nullptr; //Not assigned by default
}

void ObjTreeWgt::dropEvent(QDropEvent* event){
    _ed_actions_container->newSnapshotAction(&_editor_win->world); //Add new snapshot action
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    //User dropped object item
    QList<QTreeWidgetItem*> kids = this->selectedItems(); //Get list of selected object(it is moving object)
    QList<QListWidgetItem*> file_dropped = _editor_win->getFilesListWidget()->selectedItems();

    if(kids.length() > 0){ //if we dropped gameobject
        //Block internal move to avoid bugs
        _editor_win->getObjectListWidget()->setDragDropMode(QAbstractItemView::InternalMove);

        GameObject* obj_ptr = (GameObject*)world_ptr->getGameObjectByLabel(kids.at(0)->text(0).toStdString()); //Receiving pointer to moving object

        QTreeWidgetItem* pparent = kids.at(0)->parent(); //parent of moved object
        if(pparent == nullptr){ //If object hadn't any parent
            //this->removeItemWidget(obj_ptr->item_ptr, 0);
        }else{ //If object already parented
            Engine::GameObjectLink link = obj_ptr->getLinkToThisObject();
            Engine::GameObject* pparent_go = world_ptr->getGameObjectByLabel(pparent->text(0).toStdString());
            pparent_go->removeChildObject(link); //Remove object from previous parent
        }
        if(file_dropped.length() == 0) //if we didn't move a file
            QTreeWidget::dropEvent(event);

        QTreeWidgetItem* nparent = obj_ptr->item_ptr->parent(); //new parent
        if(nparent != nullptr){ //If we moved obj to another parent
            Engine::GameObject* nparent_go = world_ptr->getGameObjectByLabel(nparent->text(0).toStdString());
            nparent_go->addChildObject(obj_ptr->getLinkToThisObject());
        }else{ //Object hasn't received a parent
            if(pparent != nullptr){//We unparented object
                obj_ptr->hasParent = false;
                this->addTopLevelItem(obj_ptr->item_ptr);
            }
        }
    }

    if(file_dropped.length() > 0){ //if we dropped some file
        QString file_path = _editor_win->getCurrentDirectory() + "/" + file_dropped.at(0)->text();
        //call function, that performs some things after object dropped
        _editor_win->addFileToObjectList(file_path);
        //this hack clears selection from file list
        _editor_win->updateFileList();
    }
    //return drg&drop mode to normal
    _editor_win->getObjectListWidget()->setDragDropMode(QAbstractItemView::DragDrop);
}

void FileListWgt::dropEvent(QDropEvent* event){
     const QMimeData* mimeData = event->mimeData();

       // check for our needed mime type, here a file or a list of files
       if (mimeData->hasUrls())
       {
         QStringList pathList;
         QList<QUrl> urlList = mimeData->urls();

         // extract the local paths of the files
         for (int i = 0; i < urlList.size() && i < 32; +i)
         {
           pathList.append(urlList.at(i).toLocalFile());
         }

         // call a function to open the files
         //openFiles(pathList);
       }
}

void ObjTreeWgt::mousePressEvent(QMouseEvent *event){
    QTreeWidget::mousePressEvent(event);
    if(event->button() == Qt::RightButton)
    {
        emit onRightClick(event->pos());
    }
}

FileListWgt::FileListWgt(QWidget* parent) : QListWidget (parent){
    this->world_ptr = nullptr; //Not assigned by default
}

void FileListWgt::mousePressEvent(QMouseEvent *event){
    QListWidget::mousePressEvent(event);
    if(event->button() == Qt::RightButton)
    {
        QPoint point = event->pos();
        point = QPoint(point.x(), point.y() + this->y());
        emit onRightClick(point);
    }
}

ObjectCtxMenu::ObjectCtxMenu(QWidget* parent ) : QObject(parent){

    //Allocting menu container
    this->menu = new QMenu(_editor_win);
    //Allocating actions
    this->action_dub = new QAction("Dublicate", _editor_win);
    this->action_delete = new QAction("Delete", _editor_win);

    action_move = new QAction("Move", _editor_win);
    action_scale = new QAction("Scale", _editor_win);
    action_rotate = new QAction("Rotate", _editor_win);


    store_to_prefab = new QAction("Store to Prefab", _editor_win);

    object_info = new QAction("Info", _editor_win);
    //Adding actions to menu container
    this->menu->addAction(action_dub);
    this->menu->addAction(action_delete);

    this->menu->addAction(action_move);
    this->menu->addAction(action_scale);
    this->menu->addAction(action_rotate);
    this->menu->addAction(store_to_prefab);
    this->menu->addAction(object_info);
    //Connect actions to slots
    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_dub, SIGNAL(triggered(bool)), this, SLOT(onDublicateClicked()));

    QObject::connect(this->action_move, SIGNAL(triggered(bool)), this, SLOT(onMoveClicked()));
    QObject::connect(this->action_scale, SIGNAL(triggered(bool)), this, SLOT(onScaleClicked()));
    QObject::connect(this->action_rotate, SIGNAL(triggered(bool)), this, SLOT(onRotateClicked()));

    QObject::connect(this->store_to_prefab, SIGNAL(triggered(bool)), this, SLOT(onStorePrefabPressed()));
    QObject::connect(this->object_info, SIGNAL(triggered(bool)), this, SLOT(onInfoPressed()));

}

void ObjectCtxMenu::show(QPoint point){
    menu->popup(point);
}

void ObjectCtxMenu::close(){
    menu->close();
}

void ObjectCtxMenu::setObjectPtr(Engine::GameObject* obj_ptr){
    this->obj_ptr = obj_ptr;
}

//Object Ctx menu slots
void ObjectCtxMenu::onDeleteClicked(){
    //delete all ui from inspector
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    Engine::GameObjectLink link = obj_ptr->getLinkToThisObject();
    _editor_win->obj_trstate.isTransforming = false; //disabling object transform
    _editor_win->callObjectDeletion(link);
}
void ObjectCtxMenu::onMoveClicked(){
    _editor_win->obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_TRANSLATE);
}
void ObjectCtxMenu::onScaleClicked(){
    _editor_win->obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_SCALE);
}
void ObjectCtxMenu::onRotateClicked(){
    //Set state to rotate object
    _editor_win->obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_ROTATE);
}
void ObjectCtxMenu::onDublicateClicked(){
    //Make snapshot actions
    _ed_actions_container->newSnapshotAction(&_editor_win->world);
    _inspector_win->clearContentLayout(); //Prevent variable conflicts
    Engine::GameObjectLink link = obj_ptr->getLinkToThisObject();
    GameObject* result = _editor_win->world.dublicateObject((GameObject*)link.ptr);

    if(result->hasParent){ //if object parented
        ((GameObject*)result->parent.ptr)->item_ptr->addChild(result->item_ptr);
    }else{
        _editor_win->getObjectListWidget()->addTopLevelItem(result->item_ptr);
    }
}
void ObjectCtxMenu::onStorePrefabPressed(){
    QString prefab_filepath = _editor_win->getCurrentDirectory() + "/" + QString::fromStdString(*obj_ptr->label_ptr) + ".prefab";
    //Call prefab storing
    _editor_win->world.storeObjectToPrefab((GameObject*)this->obj_ptr, prefab_filepath);
    //update file list in current directory
    _editor_win->updateFileList();
}

void ObjectCtxMenu::onInfoPressed(){
    QString out = "";
    out += ("Label : " + QString::fromStdString(*obj_ptr->label_ptr) + QString(" \n"));
    out += "STR ID : " + QString::fromStdString(obj_ptr->str_id) + QString(" \n");
    out += "ARRAY ID : " + QString::number(obj_ptr->array_index) + QString(" \n");

    QMessageBox msgBox;
    msgBox.setWindowTitle("Object");
    msgBox.setText(out);
    msgBox.exec();
}

FileCtxMenu::FileCtxMenu(QWidget* parent) : QObject(parent){
    //Allocate Qt stuff
    this->menu = new QMenu(_editor_win);

    this->action_delete = new QAction("Delete", _editor_win);
    this->action_rename = new QAction("Rename", _editor_win);
    this->action_open_in_explorer = new QAction("Open in explorer", _editor_win);

    menu->addAction(action_rename);
    menu->addAction(action_delete);
    menu->addAction(action_open_in_explorer);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_rename, SIGNAL(triggered(bool)), this, SLOT(onRename()));
    QObject::connect(this->action_open_in_explorer, SIGNAL(triggered(bool)), this, SLOT(onOpenInExplorerPressed()));

}

void FileCtxMenu::onOpenInExplorerPressed(){
    QDesktopServices::openUrl( QUrl::fromLocalFile( this->directory ) );
}

void FileCtxMenu::show(QPoint point){
    this->menu->popup(point);
}
void FileCtxMenu::onDeleteClicked(){
    FileDeleteDialog* dialog = new FileDeleteDialog(file_path);
    dialog->exec();
    delete dialog;
    _editor_win->updateFileList();
}
void FileCtxMenu::onRename(){
    FileRenameDialog* dialog = new FileRenameDialog(file_path, file_name, _editor_win);
    dialog->exec();
    delete dialog;
    _editor_win->updateFileList();
}

FileDeleteDialog::FileDeleteDialog(QString file_path, QWidget* parent) : QDialog(parent){
    del_message.setText("Are you sure to delete " + file_path);
    this->file_path = file_path;

    del_btn.setText("Delete");
    close_btn.setText("Close");

    contentLayout.addWidget(&del_message, 0, 0);
    contentLayout.addWidget(&del_btn, 1, 1);
    contentLayout.addWidget(&close_btn, 1, 2);
    setLayout(&contentLayout);

    this->setWindowTitle("File deletion");

    QObject::connect(&this->del_btn, SIGNAL(clicked()), this, SLOT(onDelButtonPressed()));
    QObject::connect(&this->close_btn, SIGNAL(clicked()), this, SLOT(reject()));
}

void FileDeleteDialog::onDelButtonPressed(){
    QFile file(file_path);
    file.remove(); //remove it!
    accept();
}

FileRenameDialog::FileRenameDialog(QString file_path, QString file_name, QWidget* parent) : QDialog(parent){
    QFile file(file_path);
    rename_message.setText("Rename file  " + file_name + " to ");
    this->file_path = file_path;
    this->file_name = file_name;

    del_btn.setText("Rename");
    close_btn.setText("Close");
    edit_field.setText(file_name);

    contentLayout.addWidget(&rename_message, 0, 0);
    contentLayout.addWidget(&edit_field, 1, 0);
    contentLayout.addWidget(&del_btn, 2, 0);
    contentLayout.addWidget(&close_btn, 2, 1);
    setLayout(&contentLayout);

    this->setWindowTitle("File rename");
    //Connect signals to slots
    QObject::connect(&this->del_btn, SIGNAL(clicked()), this, SLOT(onRenameButtonPressed()));
    QObject::connect(&this->close_btn, SIGNAL(clicked()), this, SLOT(reject()));
}

void FileRenameDialog::onRenameButtonPressed(){
    //Old file path
    QString cur_path = this->file_path;
    cur_path.resize(cur_path.size() - file_name.size()); //Calculate current directory path

    QFile file(file_path);
    file.rename(cur_path + edit_field.text()); //remove it!
    accept();

    QString rel_path = file_path;
    rel_path = file_path.remove(0, static_cast<int>(_editor_win->project.root_path.size() + 1));

    Engine::ZsResource* _res = game_data->resources->getMaterialByLabel(rel_path.toStdString());
    if(_res != nullptr){ //if resource found
        //Store old relative path
        QString old_rel_path = QString::fromStdString(_res->rel_path);
        //No need to do that with mesh resources
        if(_res->resource_type == RESOURCE_TYPE_MESH) return;
        QString new_relpath = cur_path + edit_field.text();
        new_relpath = new_relpath.remove(0, static_cast<int>(_editor_win->project.root_path.size() + 1));

        _res->rel_path = new_relpath.toStdString();
        _res->blob_path = _res->rel_path;
        _res->resource_label = _res->rel_path;

        if(_res->resource_type == RESOURCE_TYPE_MATERIAL){
            //if we renamed material, update its file path
            Material* mat = static_cast<Engine::MaterialResource*>(_res)->material;
            mat->file_path = (cur_path + edit_field.text()).toStdString();
            //Remove thumbnail with old path
            _editor_win->thumb_master->texture_thumbnails.erase(old_rel_path.toStdString());
            //Store thumbnail of new material name
            _editor_win->thumb_master->createMaterialThumbnail(_res->rel_path);
        }
    }
}

void EditWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    int new_width = event->size().width();
    int new_height = event->size().height();

    QTreeWidget* objs = this->ui->objsList;
    objs->resize(new_width, (new_height / 3) * 2);

    QListWidget* file_list = this->ui->fileList;
    file_list->move(0, (new_height / 3) * 2);
    file_list->resize(new_width, (new_height / 3));

    this->settings.editor_win_width = new_width;
    this->settings.editor_win_height = new_height;
}

void InspectorWin::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);

    int new_width = event->size().width();
    int new_height = event->size().height();

    ui->scrollArea->resize(new_width, new_height - 40);

    _editor_win->settings.inspector_win_width = new_width;
    _editor_win->settings.inspector_win_height = new_height;
}
