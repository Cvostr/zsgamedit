#include "headers/ProjectEdit.h"
#include <QDir>

//Hack to support resources
extern ZSGAME_DATA* game_data;

void EditWindow::lookForResources(QString path) {
    QDir directory(path); //Creating QDir object
    directory.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks | QDir::NoDot | QDir::NoDotDot);
    directory.setSorting(QDir::DirsLast); //I want to recursive call this function after all files

    QFileInfoList list = directory.entryInfoList(); //Get folder content iterator

    for (int i = 0; i < list.size(); i++) { //iterate all files, skip 2 last . and ..
        QFileInfo fileInfo = list.at(i);  //get iterated file info
        //if it is common file
        if (fileInfo.isFile() == true) {
            //try to process file as resource
            processResourceFile(fileInfo);
        }

        if (fileInfo.isDir() == true) { //If it is directory
            QString newdir_str = path + "/" + fileInfo.fileName();
            lookForResources(newdir_str); //Call this function inside next dir
        }
    }
}


void EditWindow::processResourceFile(QFileInfo fileInfo) {
    QString name = fileInfo.fileName();
    QString absfpath = fileInfo.absoluteFilePath();
    if (checkExtension(name, ".ttf")) { //if its font
        Engine::ZsResource* _resource = new Engine::GlyphResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if (checkExtension(name, ".dds")) { //If its an texture
        Engine::ZsResource* _resource = new Engine::TextureResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if (checkExtension(name, ".zs3m")) {
        ZS3M::ImportedSceneFile isf;
        isf.loadFromFile(absfpath.toStdString());

        for (unsigned int mesh_i = 0; mesh_i < isf.meshes_toRead.size(); mesh_i++) {
            QString rel_path = absfpath;
            rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

            Engine::ZsResource* _resource = new Engine::MeshResource;
            _resource->rel_path = rel_path.toStdString();
            _resource->blob_path = _resource->rel_path;
            _resource->resource_label = isf.meshes_toRead[mesh_i]->mesh_label;
            game_data->resources->pushResource(_resource);
        }
        isf.clearMeshes();
    }
    if (checkExtension(name, ".zsanim")) {

        ZS3M::ImportedAnimationFile* iaf = new ZS3M::ImportedAnimationFile;
        iaf->loadFromFile(absfpath.toStdString());

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        Engine::ZsResource* _resource = new Engine::AnimationResource;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = iaf->anim_ptr->name;
        game_data->resources->pushResource(_resource);

        delete iaf;
    }

    if (checkExtension(name, ".wav")) { //If its an mesh
        Engine::ZsResource* _resource = new Engine::AudioResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);
    }
    if (checkExtension(name, ".zsmat")) { //If its an mesh
        Engine::ZsResource* _resource = new Engine::MaterialResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->size = 0;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);

    }
    if (checkExtension(name, ".as") || checkExtension(name, ".zscr")) { //If its an mesh
        Engine::ZsResource* _resource = new Engine::ScriptResource;

        QString rel_path = absfpath;
        rel_path.remove(0, static_cast<int>(project.root_path.size()) + 1); //Get relative path by removing length of project root from start

        _resource->size = 0;
        _resource->rel_path = rel_path.toStdString();
        _resource->blob_path = _resource->rel_path;
        _resource->resource_label = _resource->rel_path;
        game_data->resources->pushResource(_resource);

    }
}

void EditWindow::ImportResource(QString pathToResource) {

    bool copyResource = false;
    bool workWithFbx = false;

    if (checkExtension(pathToResource, ".dds")) {
        copyResource = true;
    }
    if (checkExtension(pathToResource, ".fbx") || checkExtension(pathToResource, ".dae")) {
        workWithFbx = true;
    }
    if (checkExtension(pathToResource, ".wav")) {
        copyResource = true;
    }
    if (checkExtension(pathToResource, ".lua")) {
        copyResource = true;
    }

    if (workWithFbx) {
        unsigned int num_meshes = 0;
        unsigned int num_anims = 0;
        unsigned int num_textures = 0;
        unsigned int num_materials = 0;

        ZS3M::SceneFileExport exporter;
        //Get amount of meshes, animations, textures, materials in file
        Engine::getSizes(pathToResource.toStdString(), &num_meshes, &num_anims, &num_textures, &num_materials);
        //Allocate array for meshes
        Engine::_ogl_Mesh* meshes = static_cast<Engine::_ogl_Mesh*>(Engine::allocateMesh(num_meshes));
        //Allocate array for animations
        Engine::Animation* anims = new Engine::Animation[num_anims];

        ZS3M::SceneNode rootNode;
        //Load all meshes in file
        for (unsigned int mesh_i = 0; mesh_i < num_meshes; mesh_i++) {
            Engine::loadMesh(pathToResource.toStdString(), &meshes[mesh_i], static_cast<int>(mesh_i));
            //Add loaded mesh to exporter
            exporter.pushMesh(&meshes[mesh_i]);
        }
        //Load all animations in file
        for (unsigned int anim_i = 0; anim_i < num_anims; anim_i++) {
            Engine::loadAnimation(pathToResource.toStdString(), &anims[anim_i], static_cast<int>(anim_i));
            ZS3M::AnimationFileExport ex(&anims[anim_i]);
            QString newanim_path = this->current_dir + "/" + QString::fromStdString(anims[anim_i].name + ".zsanim");
            ex.write(newanim_path.toStdString());
            //Register new resource file
            processResourceFile(QFileInfo(newanim_path));
        }
        //if FBX has at least one mesh inside
        if (num_meshes > 0) {
            Engine::loadNodeTree(pathToResource.toStdString(), &rootNode);
            //Set root node to exporter
            exporter.setRootNode(&rootNode);

            QString _file_name;
            int step = 1;
            while (pathToResource[pathToResource.length() - step] != "/") {
                _file_name.push_front(pathToResource[pathToResource.length() - step]);
                step += 1;
            }
            _file_name = this->current_dir + "/" + _file_name;

            QString new_path;
            if (checkExtension(_file_name, ".fbx"))
                new_path = _file_name.replace(".fbx", ".zs3m");
            if (checkExtension(_file_name, ".dae"))
                new_path = _file_name.replace(".dae", ".zs3m");
            //Write 3D model scene
            exporter.write(new_path.toStdString());
            //Free all meshes
            for (unsigned int mesh_i = 0; mesh_i < num_meshes; mesh_i++) {
                meshes[mesh_i].Destroy();
            }
            //Register new resource file
            processResourceFile(QFileInfo(new_path));
        }
    }

    if (copyResource) {
        std::ifstream res_stream;
        res_stream.open(pathToResource.toStdString(), std::iostream::binary | std::iostream::ate);
        //Failed opening file
        if (res_stream.fail()) return;
        //Get size of importing file
        int size = static_cast<int>(res_stream.tellg());
        unsigned char* data_buffer = new unsigned char[size];
        //Return to Zero position
        res_stream.seekg(0);
        //Read file
        res_stream.read(reinterpret_cast<char*>(data_buffer), size);
        res_stream.close();

        QString _file_name;
        int step = 1;
        while (pathToResource[pathToResource.length() - step] != "/") {
            _file_name.push_front(pathToResource[pathToResource.length() - step]);
            step += 1;
        }

        std::ofstream resource_write_stream;
        resource_write_stream.open((this->current_dir + "/" + _file_name).toStdString(), std::iostream::binary);
        resource_write_stream.write(reinterpret_cast<char*>(data_buffer), size);
        resource_write_stream.close();
        delete[] data_buffer;
        //Register new resource
        processResourceFile(QFileInfo(this->current_dir + "/" + _file_name));
    }
    updateFileList();
}