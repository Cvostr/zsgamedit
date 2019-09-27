#ifndef ZSUIBUTTON_H
#define ZSUIBUTTON_H

#include "zs-uiwidget.h"

namespace ZSUI {
    class Button : public Widget{
    public:
        void resize(unsigned int Width, unsigned int Height);
        void move(unsigned int x, unsigned int y);

        void draw();

        Button();
        ~Button();
    };
}

#endif // ZSUIBUTTON_H
