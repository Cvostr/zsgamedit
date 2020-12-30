#include "headers/ProjectEdit.h"

#include <QDir>
#include "ui_editor.h"

//Hack to support resources
extern ZSGAME_DATA* game_data;

QString EditWindow::getCurrentDirectory() {
    return this->current_dir;
}

bool EditWindow::checkExtension(QString fpath, QString ext) {
    return fpath.toLower().endsWith(ext);
}

void EditWindow::setViewDirectory(QString dir_path) {

    this->current_dir = dir_path;
    this->updateFileList();
}

void EditWindow::assignIconFile(QListWidgetItem* item) {
    //Set base icon
    item->setIcon(QIcon(":/icons/res/icons/unknown.png"));

    //File is plaintext
    if (checkExtension(item->text(), (".txt")) || checkExtension(item->text(), (".inf"))) {
        item->setIcon(QIcon::fromTheme("text-x-generic"));
    }
    //Check if this resource is textures or material
    if (checkExtension(item->text(), (".dds")) || checkExtension(item->text(), ".zsmat")) {
        std::string path = (this->current_dir + "/" + item->text()).toStdString();
        //remove extension from back
        RemoveExtension(path);
       
        //Check, if we have thumbnail for this texture
        if (thumb_master->isAvailable(path)) {
            //Thumbnail exists
            QImage* img = thumb_master->texture_thumbnails.at(path);
            item->setIcon(QIcon(QPixmap::fromImage(*img)));
        }
    }

    if (checkExtension(item->text(), ".scn"))
        item->setIcon(QIcon(":/icons/res/icons/3d_scene.png"));
    //File is .FBX .DAE .ZS3M scene
    if (checkExtension(item->text(), (".fbx")) || checkExtension(item->text(), (".dae")) || checkExtension(item->text(), (".zs3m"))) {
        item->setIcon(QIcon(":/icons/res/icons/3dmodel.png"));
    }
    //File is .zsanim animation
    if (checkExtension(item->text(), (".zsanim"))) {
        item->setIcon(QIcon(":/icons/res/icons/bone.png"));
    }
    //File is .WAV sound
    if (checkExtension(item->text(), (".wav"))) {
        item->setIcon(QIcon(":/icons/res/icons/audio-clip.png"));
    }
    if (checkExtension(item->text(), (".as"))) {
        item->setIcon(QIcon(":/icons/res/icons/script.png"));
    }
    if (checkExtension(item->text(), (".prefab"))) {
        item->setIcon(QIcon(":/icons/res/icons/package.png"));
    }
}

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

void EditWindow::updateFileListItemIcon(QString file) {
    unsigned int items = ui->fileList->count();
    for (unsigned int i = 0; i < items; i++) {
        QListWidgetItem* item = ui->fileList->item(i);
        QFileInfo fileInfo = QFileInfo(file);

        if(item->text() == fileInfo.fileName())
            assignIconFile(item);
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
void EditWindow::onNewScript() {
    std::string scriptContent = "class angel : ZPScript{\n\tangel(GameObject@ o){\n\n\t}\n\tvoid Start() {\n\n\t}\n\tvoid Update(){\n\n\t}\n}";
    QString picked_name = this->createNewTextFile(current_dir, "Script", ".as", scriptContent.c_str(), scriptContent.size());
}
void EditWindow::onNewMaterial() {
    char* matContent = "ZSP_MATERIAL\nGROUP @default\nENTRY i_uv_repeat \x0\x0\x0\x1\x0\x0\x0\x1\n";
    QString picked_name = this->createNewTextFile(current_dir, "Material", ".zsmat", matContent, 56);
}

void EditWindow::onNewLocalizedStringFile() {
    char* matContent = "ZSLOCALIZEDSTRINGSBINARY\x0\x0\x0\x0\x0\x0\x0\x0";
    QString picked_name = this->createNewTextFile(current_dir, "LocString", ".lcstr", matContent, 33);
}