#ifndef EDITORSETTINGS_H
#define EDITORSETTINGS_H

#include "../../Misc/headers/EditorManager.h"

struct EditorSettings{
    bool isFirstSetup; //false, if editor runs not first time


    int editor_win_width;
    int editor_win_height;
    int editor_win_pos_X;
    int editor_win_pos_Y;

    int gameViewWin_Width;
    int gameViewWin_Height;
    int gameView_win_pos_x;
    int gameView_win_pos_y;

    int inspector_win_width;
    int inspector_win_height;
    int inspector_win_pos_X;
    int inspector_win_pos_Y;
    //Maximum size of one blob
    unsigned int _blob_writer_divide_size_kb;
    bool isDarkTheme;

    EditorSettings(){
        isFirstSetup = true;
        isDarkTheme = false;

        gameViewWin_Width = 640;
        gameViewWin_Height = 480;

        _blob_writer_divide_size_kb = 100 * 1024;
    }
};

class EditorSettingsManager : public IEngineComponent{
private:
    EditorSettings* settings_ptr;
public:
    void OnCreate();

    EditorSettingsManager(EditorSettings* settings_ptr);
    ~EditorSettingsManager();
};

#endif // EDITORSETTINGS_H
