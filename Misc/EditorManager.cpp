#include "headers/EditorManager.h"

EditorComponentManager::EditorComponentManager(){

}

EditorComponentManager::~EditorComponentManager(){

}

void EditorComponentManager::setDpMetrics(int W, int H){
    this->WIDTH = W;
    this->HEIGHT = H;
}

void EditorComponentManager::setProjectStructPtr(Project* ptr){
    this->project_struct_ptr = ptr;
}

void EditorComponentManager::init(){

}

void EditorComponentManager::updateWindowSize(int W, int H){

}
