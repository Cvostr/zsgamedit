#include "headers/ProjectEdit.h"
#include "headers/InspectorWin.h"
#include <world/ObjectsComponents/TerrainComponent.hpp>
#include <input/Input.hpp>
#include "ui_editor.h"

#define MONEMENT_COEFF 40.3f

extern InspectorWin* _inspector_win;
//Hack to support resources
extern ZSGAME_DATA* game_data;

void EditWindow::onLeftBtnClicked(int X, int Y) {
    //Terrain painting
    if (_inspector_win->gameobject_ptr != nullptr) {
        Engine::GameObject* obj = static_cast<Engine::GameObject*>(_inspector_win->gameobject_ptr);
        Engine::TerrainProperty* terrain = obj->getPropertyPtr<Engine::TerrainProperty>();

        if (terrain != nullptr && !isWorldCamera)
            terrain->onMouseMotion(X, Y,
                settings.gameViewWin_Height,
                this->input_state.isLeftBtnHold,
                this->input_state.isLCtrlHold);
    }

    //Stop camera moving
    this->edit_camera.stopMoving();
    this->obj_ctx_menu->close(); //Close ctx menu

    if (obj_trstate.isTransforming || isWorldCamera)
        return;
    unsigned int clicked = render->render_getpickedObj(this, X, Y);

    if (clicked > world.objects.size() || clicked >= 256 * 256 * 256) {
        world.unpickObject();
        obj_trstate.obj_ptr = nullptr;
        return;
    }
    Engine::GameObject* obj_ptr = world.objects[clicked]; //Obtain pointer to selected object by label

    obj_trstate.obj_ptr = obj_ptr;
    obj_trstate.tprop_ptr = obj_ptr->getTransformProperty();
    _inspector_win->ShowObjectProperties(obj_ptr);
    this->ui->objsList->setCurrentItem(GO_W_I::getItem(obj_ptr->array_index)); //item selected in tree
}


void EditWindow::onRightBtnClicked(int X, int Y) {
    //Exit function if playcamera used to render world
    if (isWorldCamera) return;

    //Stop camera moving
    this->edit_camera.stopMoving();
    //disabling object transform
    this->obj_trstate.isTransforming = false;
    unsigned int clicked = render->render_getpickedObj(this, X, Y);
    //Check, if picked ID more than object size or ID is incorrect
    if (clicked > world.objects.size() || clicked >= 256 * 256 * 256)
        return;
    //Obtain pointer to selected object by label
    Engine::GameObject* obj_ptr = world.objects[clicked];
    //Clear isPicked property from all objects
    world.unpickObject();
    obj_ptr->pick(); //mark object picked
    this->obj_trstate.obj_ptr = obj_ptr;

    this->obj_ctx_menu->setObjectPtr(obj_ptr);
    this->obj_ctx_menu->displayTransforms = true;
    this->obj_ctx_menu->show(QPoint(settings.gameView_win_pos_x + X, settings.gameView_win_pos_y + Y));
    this->obj_ctx_menu->displayTransforms = false;

}

void EditWindow::onMouseWheel(int x, int y) {
    if (isWorldCamera) return;
    //Stop camera moving
    this->edit_camera.stopMoving();
    //If we are in 3D project
    if (project.perspective == PERSP_3D) {
        Vec3 front = edit_camera.getCameraFrontVec(); //obtain front vector
        Vec3 pos = edit_camera.getCameraPosition(); //obtain position

        edit_camera.setPosition(pos + front * y);
    }
    //2D project
    float Scale2D = edit_camera.mViewScale.X + static_cast<float>(y) / 50.F;
    if (project.perspective == PERSP_2D && this->input_state.isLCtrlHold &&
        Scale2D >= 0.2f &&
        Scale2D <= 1.7f) {
        edit_camera.mViewScale = Vec3(Scale2D, Scale2D, Scale2D);
        edit_camera.updateViewMat();
    }
    //Common camera movement
    if (project.perspective == PERSP_2D && !this->input_state.isLCtrlHold)
        edit_camera.setPosition(edit_camera.getCameraPosition() + Vec3(x * 10, y * 10, 0));
}

void EditWindow::onMouseMotion(int relX, int relY) {
    //Terrain painting
    if (_inspector_win->gameobject_ptr != nullptr) {
        Engine::GameObject* obj = static_cast<Engine::GameObject*>(_inspector_win->gameobject_ptr);
        Engine::TerrainProperty* terrain = obj->getPropertyPtr<Engine::TerrainProperty>();

        if (terrain != nullptr && !isWorldCamera)
            terrain->onMouseMotion(this->input_state.mouseX, input_state.mouseY,
                settings.gameViewWin_Height,
                this->input_state.isLeftBtnHold,
                this->input_state.isLCtrlHold);
    }

    //Property painting
    if (this->ppaint_state.enabled && input_state.isLeftBtnHold == true) { //we just move on map
        if (ppaint_state.time == 0.0f && !isSceneRun) {
            ppaint_state.time += game_data->time->GetDeltaTime();

            unsigned int clicked = render->render_getpickedObj(this, input_state.mouseX, input_state.mouseY);
            //if we pressed on empty space
            if (clicked > world.objects.size() || clicked >= 256 * 256 * 256 || ppaint_state.last_obj == static_cast<int>(clicked))
                return;

            Engine::GameObject* obj_ptr = world.objects[clicked]; //Obtain pointer to selected object by label

            ppaint_state.last_obj = static_cast<int>(clicked); //Set clicked as last object ID
            //Obtain pointer to object's property
            Engine::IGameObjectComponent* prop_ptr = obj_ptr->getPropertyPtrByType(this->ppaint_state.prop_ptr->type);

            if (prop_ptr == nullptr) { //if no property with that type in object
                //add new property
                obj_ptr->addProperty(this->ppaint_state.prop_ptr->type);
                //update pointer
                prop_ptr = obj_ptr->getPropertyPtrByType(this->ppaint_state.prop_ptr->type);
            }

            getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
            //Copy property data
            ppaint_state.prop_ptr->copyTo(prop_ptr);
        }
        else {
            ppaint_state.time += game_data->time->GetDeltaTime();
            if (ppaint_state.time >= 100) ppaint_state.time = 0;
        }

    }
    //We are in 2D project, move camera by the mouse
    if (project.perspective == PERSP_2D && !isWorldCamera) { //Only affective in 2D
        //If middle button of mouse pressed
        if (input_state.isMidBtnHold == true) { //we just move on map
            //Stop camera moving
            this->edit_camera.stopMoving();

            Vec3 cam_pos = edit_camera.getCameraPosition();
            cam_pos.X += relX;
            cam_pos.Y += relY;
            edit_camera.setPosition(cam_pos);
        }
    }

    //We are in 2D project, move camera by the mouse and rotate it
    if (project.perspective == PERSP_3D && !isWorldCamera) {//Only affective in 3D
        //IF mouse wheel is held
        if (input_state.isMidBtnHold == true) {
            this->cam_yaw += relX * 0.16f;
            cam_pitch += relY * 0.16f;
            //Limit camera look
            if (cam_pitch > 89.0f)
                cam_pitch = 89.0f;
            if (cam_pitch < -89.0f)
                cam_pitch = -89.0f;

            Vec3 front;
            front.X = cosf(DegToRad(cam_yaw)) * cosf(DegToRad(cam_pitch));
            front.Y = -sinf(DegToRad(cam_pitch));
            front.Z = sinf(DegToRad(cam_yaw)) * cosf(DegToRad(cam_pitch));
            front.Normalize();
            edit_camera.setFront(front);
        }
    }
    if (obj_trstate.isModifying && input_state.isLeftBtnHold == false) {
        //if we in editing mode and user clicked mouse left button
        //then exit editing mode
        obj_trstate.isModifying = false;

    }
    //Visual transform control
    if (obj_trstate.isTransforming == true && input_state.isLeftBtnHold == true) { //Only affective if object is transforming
        RGBAColor color = render->getColorOfPickedTransformControl(
            this->input_state.mouseX,
            this->input_state.mouseY,
            this);
        //If transformation method isn't set
        if (obj_trstate.isModifying == false) {
            //Set all coordinates to 0
            obj_trstate.Xcf = 0;
            obj_trstate.Ycf = 0;
            obj_trstate.Zcf = 0;
            //if mouse under red color, we modifying X axis
            if (color.r == 255) obj_trstate.Xcf = 1; else obj_trstate.Xcf = 0;
            //if mouse under green color, we modifying Y axis
            if (color.g == 255) obj_trstate.Ycf = 1; else obj_trstate.Ycf = 0;
            //if mouse under blue color, we modifying Z axis
            if (color.b == 255) obj_trstate.Zcf = 1; else obj_trstate.Zcf = 0;
        }

        obj_trstate.isModifying = true;

        Vec3* vec_ptr = nullptr; //pointer to modifying vector
        if (obj_trstate.transformMode == GO_TRANSFORM_MODE_TRANSLATE) {
            vec_ptr = &obj_trstate.tprop_ptr->translation;
        }
        //if we scaling
        if (obj_trstate.transformMode == GO_TRANSFORM_MODE_SCALE) {
            vec_ptr = &obj_trstate.tprop_ptr->scale;
        }
        //if we rotating
        if (obj_trstate.transformMode == GO_TRANSFORM_MODE_ROTATE) {
            vec_ptr = &obj_trstate.tprop_ptr->rotation;
        }
        if (obj_trstate.transformMode > 0)
            *vec_ptr = *vec_ptr + Vec3(-relX * sign(edit_camera.getCameraFrontVec().Z), -relY, relX * sign(edit_camera.getCameraFrontVec().X)) * Vec3(obj_trstate.Xcf, obj_trstate.Ycf, obj_trstate.Zcf);
    }
}

void EditWindow::sceneWalkWASD() {
    float deltaTime = game_data->time->GetDeltaTime();
    if (Input::isKeyHold(SDLK_a) && !isSceneRun && !input_state.isLCtrlHold) {
        Vec3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * -MONEMENT_COEFF * deltaTime;
        edit_camera.setPosition(pos);
    }
    if (Input::isKeyHold(SDLK_d) && !isSceneRun && !input_state.isLCtrlHold) {
        Vec3 pos = edit_camera.getCameraPosition(); //obtain position
        pos = pos + edit_camera.getCameraRightVec() * MONEMENT_COEFF * deltaTime;
        edit_camera.setPosition(pos);
    }
    if (Input::isKeyHold(SDLK_w) && !isSceneRun && !input_state.isLCtrlHold) {
        Vec3 pos = edit_camera.getCameraPosition(); //obtain position
        if (project.perspective == PERSP_2D)
            pos.Y += 2.2f * deltaTime;
        else
            pos = pos + edit_camera.getCameraFrontVec() * MONEMENT_COEFF * deltaTime;
        edit_camera.setPosition(pos);
    }
    if (Input::isKeyHold(SDLK_s) && !isSceneRun && !input_state.isLCtrlHold) {
        Vec3 pos = edit_camera.getCameraPosition(); //obtain position
        if (project.perspective == PERSP_2D)
            pos.Y -= 2.2f * deltaTime;
        else
            pos = pos - edit_camera.getCameraFrontVec() * MONEMENT_COEFF * deltaTime;
        edit_camera.setPosition(pos);
    }
}

void EditWindow::onKeyDown(SDL_Keysym sym) {

    if (input_state.isLAltHold && sym.sym == SDLK_t) {
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_TRANSLATE);
    }
    if (input_state.isLAltHold && sym.sym == SDLK_e) {
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_SCALE);
    }
    if (input_state.isLAltHold && sym.sym == SDLK_r) {
        obj_trstate.setTransformOnObject(GO_TRANSFORM_MODE_ROTATE);
    }

    if (sym.sym == SDLK_DELETE) {
        if (this->obj_trstate.obj_ptr != nullptr) {
            Engine::GameObjectLink link = this->obj_trstate.obj_ptr->getLinkToThisObject();
            callObjectDeletion(link);
        }
    }
    //If we pressed CTRL + O
    if (input_state.isLCtrlHold && sym.sym == SDLK_o) {
        emit onOpenScene();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_s) {
        emit onSceneSave();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_z) {
        emit onUndoPressed();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_y) {
        emit onRedoPressed();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_r) {
        emit onRunProject();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_n) {
        emit onAddNewGameObject();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_c) {
        emit onObjectCopy();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_v) {
        emit onObjectPaste();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_h) {
        emit toggleCameras();
    }
    if (input_state.isLCtrlHold && sym.sym == SDLK_l) {
        emit onOpenConsoleLog();
    }
}

void EditWindow::keyPressEvent(QKeyEvent* ke) {
    if (ke->key() == Qt::Key_Delete) { //User pressed delete button
        QTreeWidgetItem* object_toRemove = this->ui->objsList->currentItem();
        QListWidgetItem* file_toRemove = this->ui->fileList->currentItem();
        if (object_toRemove != nullptr && ui->objsList->hasFocus()) { //if user wish to delete object
            Engine::GameObject* obj = this->world.getGameObjectByLabel(object_toRemove->text(0).toStdString());
            _inspector_win->clearContentLayout(); //Prevent variable conflicts
            Engine::GameObjectLink link = obj->getLinkToThisObject();
            obj_trstate.isTransforming = false; //disabling object transform
            callObjectDeletion(link); //removing object
        }
        if (file_toRemove != nullptr && ui->fileList->hasFocus()) { //if user wish to remove file
            FileDeleteDialog* dialog = new FileDeleteDialog(this->current_dir + "/" + file_toRemove->text());
            dialog->exec();
            delete dialog;
            updateFileList();
        }
    }
    if (ke->key() == Qt::Key_F2) { //User pressed f2 key
        QListWidgetItem* item_toRename = this->ui->fileList->currentItem();
        if (item_toRename != nullptr && ui->fileList->hasFocus()) {
            FileRenameDialog* dialog = new FileRenameDialog(this->current_dir + "/" + item_toRename->text(), item_toRename->text(), this);
            dialog->exec();
            delete dialog;
            updateFileList();
        }
    }
    if (ke->key() == Qt::Key_F5) { //User pressed f5 key
        if (ui->fileList->hasFocus()) {
            updateFileList();
        }
    }

    QMainWindow::keyPressEvent(ke); // base class implementation
}
