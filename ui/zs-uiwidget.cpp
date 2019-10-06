#include "headers/zs-uiwidget.h"
#include "../Input/headers/zs-input.h"

ZSUI::Widget::Widget(){

}
ZSUI::Widget::~Widget(){

}

void ZSUI::Widget::resize(unsigned int Width, unsigned int Height){
    this->metrics.WIDTH = Width;
    this->metrics.HEIGHT = Height;

}
void ZSUI::Widget::move(unsigned int x, unsigned int y){
    this->pos.posX = x;
    this->pos.posY = y;
}
bool ZSUI::Widget::isHoveredByMouse(){
    Input::MouseState mstate = Input::getMouseState();

    if(pos.posX < mstate.mouseX && pos.posY < mstate.mouseY && pos.posX + metrics.WIDTH > mstate.mouseX && pos.posY + metrics.HEIGHT > mstate.mouseY)
        return true;

    return false;
}
void ZSUI::Widget::draw(){

}
