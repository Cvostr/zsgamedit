#include "mainwin.h"
#include <QApplication>
#include <SDL2/SDL.h>
#include <iostream>
#include <windows.h>
#include <input/zs-input.h>

#define SHOW_WIN_CONSOLE

static uint64_t NOW = SDL_GetPerformanceCounter();
static uint64_t last = 0;
static float deltaTime = 0;

bool infocus = true;
bool working = true; //Application started and it is working

QApplication* a;

int main(int argc, char *argv[]){
#ifdef SHOW_WIN_CONSOLE
    if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
#endif

    a = new QApplication(argc, argv);
    MainWin w(a);
    w.show();

    while (working) {
        //Work on application
        a->processEvents();
        if(w.edit_win_ptr != nullptr){ //Check if project editor window is created
            //Time calculation
            if(w.edit_win_ptr->ready){
                last = NOW;
                NOW = SDL_GetPerformanceCounter();
                deltaTime = (NOW - last) * 1000 / SDL_GetPerformanceFrequency();
            }
            //Send delta time to all editor managers
            w.edit_win_ptr->updateDeltaTime(deltaTime);
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                Input::processEventsSDL(&event);
                if (event.type == SDL_WINDOWEVENT) { //If user caused SDL window to do something
                    
                    if(event.window.event == SDL_WINDOWEVENT_MINIMIZED){
                        w.edit_win_ptr->hide();
                        w.edit_win_ptr->getInspector()->hide();
                        infocus = false;
                    }
                    if(event.window.event == SDL_WINDOWEVENT_RESTORED){
                        w.edit_win_ptr->show();
                        w.edit_win_ptr->getInspector()->show();
                        infocus = true;
                    }

                    if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                        w.edit_win_ptr->setGameViewWindowSize(event.window.data1, event.window.data2);
                        //Write new settings
                        w.edit_win_ptr->settings.gameViewWin_Width = event.window.data1;
                        w.edit_win_ptr->settings.gameViewWin_Height = event.window.data2;
                    }
                    if(event.window.event == SDL_WINDOWEVENT_MOVED){
                        //Write new settings
                        w.edit_win_ptr->settings.gameView_win_pos_x = event.window.data1;
                        w.edit_win_ptr->settings.gameView_win_pos_y = event.window.data2;
                        }
                    }
                    if (event.type == SDL_QUIT) { //If user caused SDL window to close
                        //Close all managers and release everything
                        if(w.edit_win_ptr->onCloseProject())
                            working = false;
                    }
                    if (event.type == SDL_MOUSEBUTTONUP) { //If user released mouse button

                        if (event.button.button == SDL_BUTTON_LEFT) {
                            w.edit_win_ptr->input_state.isLeftBtnHold = false;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            w.edit_win_ptr->input_state.isRightBtnHold = false;
                        }
                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            w.edit_win_ptr->input_state.isMidBtnHold = false;
                        }
                    }
                    if (event.type == SDL_MOUSEWHEEL) {
                        w.edit_win_ptr->onMouseWheel(event.wheel.x, event.wheel.y);
                    }
                    if (event.type == SDL_KEYDOWN) { //if user pressed a key on keyboard
                        w.edit_win_ptr->onKeyDown(event.key.keysym); //Call press function on EditWindow

                        if(event.key.keysym.sym == SDLK_LCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isLCtrlHold = true;
                        if(event.key.keysym.sym == SDLK_RCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isRCtrlHold = true;
                        if(event.key.keysym.sym == SDLK_LALT) //if it is ctrl
                            w.edit_win_ptr->input_state.isLAltHold = true;
                    }
                    if (event.type == SDL_KEYUP) { //if user pressed a key on keyboard
                        if(event.key.keysym.sym == SDLK_LCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isLCtrlHold = false;
                        if(event.key.keysym.sym == SDLK_RCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isRCtrlHold = false;
                        if(event.key.keysym.sym == SDLK_LALT) //if it is ctrl
                            w.edit_win_ptr->input_state.isLAltHold = false;
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

                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            w.edit_win_ptr->input_state.isMidBtnHold = true;
                        }
                    }
                    if (event.type == SDL_MOUSEMOTION) { //If user moved mouse
                        int newX = event.motion.xrel;
                        int newY = event.motion.yrel;
                        w.edit_win_ptr->onMouseMotion(newX, newY);
                        //update state in Editor
                        w.edit_win_ptr->input_state.mouseX = event.motion.x;
                        w.edit_win_ptr->input_state.mouseY = event.motion.y;
                    }

                }
                if(w.edit_win_ptr->ready == true && infocus){ //If GL is ready to draw
                    w.edit_win_ptr->getInspector()->updateAreasChanges();
                    w.edit_win_ptr->edit_camera.updateTick(deltaTime); //Update camera, if it is moving
                    w.edit_win_ptr->glRender(); //Draw OpenGL window
                }
                Input::clearMouseState();
                Input::clearPressedKeys();
           }
    }
    return 0;
}
