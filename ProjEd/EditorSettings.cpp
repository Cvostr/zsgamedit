#include "headers/EditorSettings.h"
#include <fstream>

#define ED_SETTINGS_FILE "editor_settings.cfg"

void EditorSettingsManager::init(){
    std::ifstream stream;
    stream.open(ED_SETTINGS_FILE, std::ifstream::in);

    if(!stream.fail()){ // if we successfully read file
        this->settings_ptr->isFirstSetup = false;
    }

    std::string prefix;
}

EditorSettingsManager::EditorSettingsManager(EditorSettings* settings_ptr){
    this->settings_ptr = settings_ptr;
}
EditorSettingsManager::~EditorSettingsManager(){

}
