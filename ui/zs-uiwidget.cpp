#include "headers/zs-uiwidget.h"

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
void ZSUI::Widget::draw(){

}
