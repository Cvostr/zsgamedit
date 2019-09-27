#include "headers/zs-uibutton.h"

//extern ZSENSDK::Input::MouseState mouse;

ZSUI::Button::Button(){

}
ZSUI::Button::~Button(){

}
void ZSUI::Button::draw(){

}
void ZSUI::Button::resize(unsigned int Width, unsigned int Height){
    Widget::resize(Width, Height);
}
void ZSUI::Button::move(unsigned int x, unsigned int y){
    Widget::move(x, y);
}
