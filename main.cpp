#include "mainwin.h"
#include <QApplication>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.show();
    bool working = true; //Application started and it is working
    while (working) {
       // if (a.hasPendingEvents())
            a.processEvents();
            if(w.edit_win_ptr != nullptr){ //Check if project editor window is created
                w.edit_win_ptr->getInspector()->area_update();
                w.edit_win_ptr->edit_camera.updateTick(); //Update camera, if it is moving
                if(w.edit_win_ptr->ready == true) //If GL is ready to draw
                    w.edit_win_ptr->glRender(); //Draw OpenGL window
             }

            SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT) { //If user caused SDL window to close
                        working = false;
                    }
                    if (event.type == SDL_MOUSEBUTTONUP) { //If user released mouse button

                        if (event.button.button == SDL_BUTTON_LEFT) {
                            w.edit_win_ptr->input_state.isLeftBtnHold = false;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            w.edit_win_ptr->input_state.isRightBtnHold = false;
                        }
                    }
                    if (event.type == SDL_MOUSEBUTTONDOWN) { //If user pressed mouse btn

                        if (event.button.button == SDL_BUTTON_LEFT) {
                            w.edit_win_ptr->input_state.isLeftBtnHold = true;
                            w.edit_win_ptr->onLeftBtnClicked(event.motion.x, event.motion.y);
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            w.edit_win_ptr->input_state.isRightBtnHold = true;
                            w.edit_win_ptr->onRightBtnClicked(event.motion.x, event.motion.y);
                        }
                    }
                    if (event.type == SDL_MOUSEMOTION) { //If user moved mouse
                        int newX = event.motion.xrel;
                        int newY = event.motion.yrel;
                        w.edit_win_ptr->onMouseMotion(newX, newY);
                    }

                }
    }
    return 0;
}
