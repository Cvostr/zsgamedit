#ifndef EDITORSETTINGS_H
#define EDITORSETTINGS_H

#include "../../Misc/headers/EditorManager.h"

struct EditorSettings{
    bool isFirstSetup; //false, if editor runs not first time

    int gameViewWin_Width;
    int gameViewWin_Height;

    int editor_win_pos_X;
    int editor_win_pos_Y;

    int gameView_win_pos_x;
    int gameView_win_pos_y;

    int inspector_win_pos_X;
    int inspector_win_pos_Y;
    //Maximum size of one blob
    unsigned int _blob_writer_divide_size_kb;

    EditorSettings(){
        isFirstSetup = true;

        gameViewWin_Width = 640;
        gameViewWin_Height = 480;

        _blob_writer_divide_size_kb = 100 * 1024;
    }
};

class EditorSettingsManager : public EditorComponentManager{
private:
    EditorSettings* settings_ptr;
public:
    void init();

    EditorSettingsManager(EditorSettings* settings_ptr);
    ~EditorSettingsManager();
};

#endif // EDITORSETTINGS_H
