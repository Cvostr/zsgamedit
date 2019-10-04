#include "headers/zs-input.h"

#define KEYS_QUEUE_SIZE 10
#define KEY_NONE -200

static Input::MouseState mouse;

static int pressed_keys_queue[KEYS_QUEUE_SIZE];
static int pressed_keys_q_size = 0;
static int hold_keys_queue[KEYS_QUEUE_SIZE];
static int hold_keys_q_size = 0;

void Input::addPressedKeyToQueue(int keycode){
    //if we have enough free space
    if(pressed_keys_q_size > KEYS_QUEUE_SIZE) return;
    pressed_keys_queue[pressed_keys_q_size] = keycode;
    pressed_keys_q_size += 1;
}
void Input::addHeldKeyToQueue(int keycode){
    //if key already held, do nothing
    if(isKeyHold(keycode)) return;

    bool insertable = false;
    int insert_pos = 0;

    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == KEY_NONE){
            insertable = true;
            insert_pos = iterator;
        }
    }
    if(!insertable){
        if(hold_keys_q_size > KEYS_QUEUE_SIZE) return;
        hold_keys_queue[hold_keys_q_size] = keycode;
        hold_keys_q_size += 1;
    }else{
        hold_keys_queue[insert_pos] = keycode;
    }
}
void Input::removeHeldKeyFromQueue(int keycode){
    for(int iterator = 0; iterator < hold_keys_q_size; iterator ++){
        if(hold_keys_queue[iterator] == keycode){
            hold_keys_queue[iterator] = KEY_NONE;
        }
    }
}
void Input::clearPressedKeys(){
    pressed_keys_q_size = 0;
}
bool Input::isKeyPressed(int keycode){
    for(int i = 0; i < pressed_keys_q_size; i ++){
        if(pressed_keys_queue[i] == keycode)
            return true;
    }
    return false;
}
bool Input::isKeyHold(int keycode){
    for(int i = 0; i < hold_keys_q_size; i ++){
        if(hold_keys_queue[i] == keycode)
            return true;
    }
    return false;
}

Input::MouseState* Input::getMouseStatePtr(){
    return &mouse;
}
Input::MouseState Input::getMouseState(){
    return mouse;
}

void Input::clearMouseState(){
    mouse.mouseRelX = 0;
    mouse.mouseRelY = 0;
}
