#ifndef ZSUIBUTTON_H
#define ZSUIBUTTON_H

#include <render/zs-texture.h>
#include "zs-uiwidget.h"

namespace ZSUI {
    class Button : public Widget{
    public:
        Engine::Texture* common_sprite;
        Engine::Texture* hovered_sprite;

        void resize(unsigned int Width, unsigned int Height);
        void move(unsigned int x, unsigned int y);

        void draw();

        Button();
        ~Button();
    };
}

#endif // ZSUIBUTTON_H
