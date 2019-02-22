#include "headers/ProjectEdit.h"

#include <QMouseEvent>

ObjTreeWgt::ObjTreeWgt(QWidget* parent) : QTreeWidget (parent){
    this->world_ptr = nullptr; //Not assigned by default
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

ObjectCtxMenu::ObjectCtxMenu(EditWindow* win, QWidget* parent ) : QObject(parent){
    this->win_ptr = win;
    //Allocting menu container
    this->menu = new QMenu(win);
    //Allocating actions
    this->action_dub = new QAction("Dublicate", win);
    this->action_delete = new QAction("Delete", win);

    action_move = new QAction("Move", win);
    action_scale = new QAction("Scale", win);
    action_rotate = new QAction("Rotate", win);
    //Adding actions to menu container
    this->menu->addAction(action_dub);
    this->menu->addAction(action_delete);

    this->menu->addAction(action_move);
    this->menu->addAction(action_scale);
    this->menu->addAction(action_rotate);
    //Connect actions to slots
    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_dub, SIGNAL(triggered(bool)), this, SLOT(onDublicateClicked()));

    QObject::connect(this->action_move, SIGNAL(triggered(bool)), this, SLOT(onMoveClicked()));
    QObject::connect(this->action_scale, SIGNAL(triggered(bool)), this, SLOT(onScaleClicked()));
    QObject::connect(this->action_rotate, SIGNAL(triggered(bool)), this, SLOT(onRotateClicked()));
}

void ObjectCtxMenu::show(QPoint point){
    menu->popup(point);
}

FileCtxMenu::FileCtxMenu(EditWindow* win, QWidget* parent) : QObject(parent){
    //Store win pointer
    this->win_ptr = win;
    //Allocate Qt stuff
    this->menu = new QMenu(win);

    this->action_delete = new QAction("Delete", win);
    this->action_rename = new QAction("Rename", win);

    menu->addAction(action_rename);
    menu->addAction(action_delete);

    QObject::connect(this->action_delete, SIGNAL(triggered(bool)), this, SLOT(onDeleteClicked()));
    QObject::connect(this->action_rename, SIGNAL(triggered(bool)), this, SLOT(onRename()));

}

void FileCtxMenu::show(QPoint point){
    this->menu->popup(point);
}
void FileCtxMenu::onDeleteClicked(){
    FileDeleteDialog* dialog = new FileDeleteDialog(file_path);
    dialog->exec();
    delete dialog;
    this->win_ptr->updateFileList();
}
void FileCtxMenu::onRename(){
    FileRenameDialog* dialog = new FileRenameDialog(file_path, file_name);
    //dialog->file_name = file_name;
    dialog->exec();
    delete dialog;
    this->win_ptr->updateFileList();
}

FileDeleteDialog::FileDeleteDialog(QString file_path, QWidget* parent) : QDialog(parent){
    del_message.setText("Are you sure to delete " + file_path);
    this->file_path = file_path;

    del_btn.setText("Delete");
    close_btn.setText("Close");

    contentLayout.addWidget(&del_message, 0, 0);
    contentLayout.addWidget(&del_btn, 1, 0);
    contentLayout.addWidget(&close_btn, 1, 1);
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

    del_btn.setText("Rename");
    close_btn.setText("Close");
    edit_field.setText(file_name);

    contentLayout.addWidget(&rename_message, 0, 0);
    contentLayout.addWidget(&edit_field, 1, 0);
    contentLayout.addWidget(&del_btn, 2, 0);
    contentLayout.addWidget(&close_btn, 2, 1);
    setLayout(&contentLayout);

    this->setWindowTitle("File rename");

    QObject::connect(&this->del_btn, SIGNAL(clicked()), this, SLOT(onRenameButtonPressed()));
    QObject::connect(&this->close_btn, SIGNAL(clicked()), this, SLOT(reject()));
}

void FileRenameDialog::onRenameButtonPressed(){
    QFile file(file_path);
    file.rename(edit_field.text()); //remove it!
    accept();
}
