#include "headers/EditorSettings.h"
#include <fstream>

#define ED_SETTINGS_FILE "editor_settings.cfg"

void EditorSettingsManager::OnCreate(){
    std::ifstream stream;
    stream.open(ED_SETTINGS_FILE, std::ifstream::in);

    if(!stream.fail()) // if we successfully read file
        this->settings_ptr->isFirstSetup = false;
    else
        return;

    std::string prefix;
    while(!stream.eof()){
        //reading prefix
        stream >> prefix;
        //if we reached gameview window position
        if(prefix.compare("gameView_win_pos") == 0){
            int x, y;
            stream >> x >> y;
            this->settings_ptr->gameView_win_pos_x = x;
            this->settings_ptr->gameView_win_pos_y = y;
        }
        //if we reached gameview window size
        if(prefix.compare("gameView_win_Size") == 0){
            int Width, Height;
            stream >> Width >> Height;
            this->settings_ptr->gameViewWin_Width = Width;
            this->settings_ptr->gameViewWin_Height = Height;
        }

        if(prefix.compare("inspector_win_pos") == 0){
            int x, y;
            stream >> x >> y;
            this->settings_ptr->inspector_win_pos_X = x;
            this->settings_ptr->inspector_win_pos_Y = y;
        }

        if(prefix.compare("inspector_win_Size") == 0){
            int x, y;
            stream >> x >> y;
            this->settings_ptr->inspector_win_width = x;
            this->settings_ptr->inspector_win_height = y;
        }
        if(prefix.compare("editor_win_pos") == 0){
            int x, y;
            stream >> x >> y;
            this->settings_ptr->editor_win_pos_X = x;
            this->settings_ptr->editor_win_pos_Y = y;
        }
        if(prefix.compare("editor_win_Size") == 0){
            int x, y;
            stream >> x >> y;
            this->settings_ptr->editor_win_width = x;
            this->settings_ptr->editor_win_height = y;
        }
        if(prefix.compare("darkTheme") == 0){
            bool dark;
            stream >> dark;
            this->settings_ptr->isDarkTheme = dark;
        }
    }

    stream.close();
}

EditorSettingsManager::EditorSettingsManager(EditorSettings* settings_ptr){
    this->settings_ptr = settings_ptr;
}
EditorSettingsManager::~EditorSettingsManager(){
    std::ofstream stream;
    stream.open(ED_SETTINGS_FILE, std::ofstream::out);

    stream << "gameView_win_pos " << this->settings_ptr->gameView_win_pos_x << " " << this->settings_ptr->gameView_win_pos_y << "\n";
    stream << "gameView_win_Size " << this->settings_ptr->gameViewWin_Width << " " << this->settings_ptr->gameViewWin_Height << "\n";
    stream << "inspector_win_pos " << this->settings_ptr->inspector_win_pos_X << " " << this->settings_ptr->inspector_win_pos_Y << "\n";
    stream << "inspector_win_Size " << this->settings_ptr->inspector_win_width << " " << this->settings_ptr->inspector_win_height << "\n";
    stream << "editor_win_pos " << this->settings_ptr->editor_win_pos_X << " " << this->settings_ptr->editor_win_pos_Y << "\n";
    stream << "editor_win_Size " << this->settings_ptr->editor_win_width << " " << this->settings_ptr->editor_win_height << "\n";
    stream << "darkTheme " << this->settings_ptr->isDarkTheme << "\n";

    stream.close();
}
