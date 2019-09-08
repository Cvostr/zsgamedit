#include "mainwin.h"
#include <QApplication>
#include <SDL2/SDL.h>
#include <iostream>

#include "Scripting/headers/zsensdk.h"

static uint64_t NOW = SDL_GetPerformanceCounter();
static uint64_t last = 0;
static float deltaTime = 0;

bool working = true; //Application started and it is working

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.show();

    while (working) {
        a.processEvents();

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
            ZSENSDK::Input::MouseState* mstate = ZSENSDK::Input::getMouseStatePtr();
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_WINDOWEVENT) { //If user caused SDL window to do something

                    if(event.window.event == SDL_WINDOWEVENT_MINIMIZED){
                        w.edit_win_ptr->hide();
                        w.edit_win_ptr->getInspector()->hide();
                    }
                    if(event.window.event == SDL_WINDOWEVENT_RESTORED){
                        w.edit_win_ptr->show();
                        w.edit_win_ptr->getInspector()->show();
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
                            mstate->isLButtonDown = false;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            w.edit_win_ptr->input_state.isRightBtnHold = false;
                            mstate->isRButtonDown = false;
                        }
                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            w.edit_win_ptr->input_state.isMidBtnHold = false;
                            mstate->isMidBtnDown = false;
                        }
                    }
                    if (event.type == SDL_MOUSEWHEEL) {
                        w.edit_win_ptr->onMouseWheel(event.wheel.x, event.wheel.y);
                    }
                    if (event.type == SDL_KEYDOWN) { //if user pressed a key on keyboard
                        w.edit_win_ptr->onKeyDown(event.key.keysym); //Call press function on EditWindow
                        ZSENSDK::Input::addPressedKeyToQueue(event.key.keysym.sym);
                        ZSENSDK::Input::addHeldKeyToQueue(event.key.keysym.sym);

                        if(event.key.keysym.sym == SDLK_LCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isLCtrlHold = true;
                        if(event.key.keysym.sym == SDLK_RCTRL) //if it is ctrl
                            w.edit_win_ptr->input_state.isRCtrlHold = true;
                        if(event.key.keysym.sym == SDLK_LALT) //if it is ctrl
                            w.edit_win_ptr->input_state.isLAltHold = true;
                    }
                    if (event.type == SDL_KEYUP) { //if user pressed a key on keyboard
                        ZSENSDK::Input::removeHeldKeyFromQueue(event.key.keysym.sym);
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

                            mstate->isLButtonDown = true;
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            w.edit_win_ptr->input_state.isRightBtnHold = true;
                            w.edit_win_ptr->onRightBtnClicked(event.motion.x, event.motion.y);

                            mstate->isRButtonDown = true;
                        }

                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            w.edit_win_ptr->input_state.isMidBtnHold = true;
                            mstate->isMidBtnDown = true;
                        }
                    }
                    if (event.type == SDL_MOUSEMOTION) { //If user moved mouse
                        int newX = event.motion.xrel;
                        int newY = event.motion.yrel;
                        w.edit_win_ptr->onMouseMotion(newX, newY);
                        //update state in Editor
                        w.edit_win_ptr->input_state.mouseX = event.motion.x;
                        w.edit_win_ptr->input_state.mouseY = event.motion.y;
                        //update state in ZSENSDK
                        mstate->mouseX = event.motion.x;
                        mstate->mouseY = event.motion.y;
                        mstate->mouseRelX = event.motion.xrel;
                        mstate->mouseRelY = event.motion.yrel;
                    }

                }
                if(w.edit_win_ptr->ready == true){ //If GL is ready to draw
                    w.edit_win_ptr->getInspector()->updateAreasChanges();
                    w.edit_win_ptr->edit_camera.updateTick(deltaTime); //Update camera, if it is moving

                    w.edit_win_ptr->glRender(); //Draw OpenGL window
                }
           }
    ZSENSDK::Input::clearMouseState();
    ZSENSDK::Input::clearPressedKeys();
    }
    return 0;
}
