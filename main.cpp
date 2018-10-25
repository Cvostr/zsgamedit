#include "mainwin.h"
#include <QApplication>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.show();
    bool working = true;
    //return a.exec();
    while (working) {
       // if (a.hasPendingEvents())
            a.processEvents();
            if(w.edit_win_ptr != nullptr){ //Check if project editor window is created
                w.edit_win_ptr->getInspector()->area_update();
                if(w.edit_win_ptr->ready == true) //If GL is ready to draw
                    w.edit_win_ptr->glRender(); //Draw OpenGL window
             }

            SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT) {
                        working = false;
                    }
                }
        //w.updateGL();
    }
    return 0;
}
