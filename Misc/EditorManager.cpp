#include "headers/EngineManager.h"

EngineComponentManager::EngineComponentManager(){

}

EngineComponentManager::~EngineComponentManager(){

}

void EngineComponentManager::setDpMetrics(int W, int H){
    this->WIDTH = W;
    this->HEIGHT = H;
}

void EngineComponentManager::setProjectStructPtr(Project* ptr){
    this->project_struct_ptr = ptr;
}

void EngineComponentManager::init(){

}
