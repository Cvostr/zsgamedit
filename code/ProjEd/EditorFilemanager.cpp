#include "headers/ProjectEdit.h"
#include <QDir>
#include "ui_editor.h"

void EditWindow::updateFileList() {
    ui->fileList->clear(); //Clear widget content

    QDir directory(this->current_dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsFirst | QDir::Name | QDir::Reversed);

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator
    if (this->current_dir.compare(QString::fromStdString(project.root_path))) {
        QListWidgetItem* backBtn_item = new QListWidgetItem("(back)", ui->fileList);
        backBtn_item->setIcon(QIcon(":/icons/res/icons/dir_back.png"));
    }

    for (int i = 0; i < list.size(); i++) { //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info

        //If file is project info, or terrain, then skip it.
        if (checkExtension(fileInfo.fileName(), ".inf") || checkExtension(fileInfo.fileName(), ".terrain"))
            continue;

        QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName(), ui->fileList);
        if (fileInfo.isDir()) {
            item->setIcon(QIcon(":/icons/res/icons/dir.png"));
        }
        else {
            assignIconFile(item);
        }
    }
}


//Signal
void EditWindow::onFileListItemClicked() {
    QListWidgetItem* selected_file_item = ui->fileList->currentItem();
    //if user pressed back button
    if (selected_file_item->text().compare("(back)") == 0) {
        QDir cur_folder = QDir(this->current_dir);
        cur_folder.cdUp();
        setViewDirectory(cur_folder.absolutePath());
        return;
    }

    QDir directory(this->current_dir);

    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::Name | QDir::Reversed);

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator

    for (int i = 0; i < list.size(); i++) { //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        if (fileInfo.fileName().compare(selected_file_item->text()) == 0) { //Find pressed item
            if (fileInfo.isDir()) { //If we pressed on directory
                QString new_path = this->current_dir + "/" + fileInfo.fileName(); //Get directory path
                setViewDirectory(new_path); //go to this
                return; // Exit function to prevent crash
            }
            else {
                QString new_path = this->current_dir + "/" + fileInfo.fileName(); //Get file path
                openFile(new_path); //Do something to open this file
                return;
            } //If it isn't directory, it is a file
        }
    }
}

void EditWindow::onFileCtxMenuShow(QPoint point) {
    QListWidgetItem* selected_item = ui->fileList->currentItem(); //get selected item
    if (selected_item != nullptr) { //if selected file
        QString file_name = selected_item->text();

        this->file_ctx_menu->file_path = current_dir + "/" + file_name; //set file path
        this->file_ctx_menu->file_name = file_name; //set file name
        this->file_ctx_menu->directory = this->current_dir; //set dirctory
        this->file_ctx_menu->show(point); //show menu
    }
}