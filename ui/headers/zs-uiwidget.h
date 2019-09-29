#ifndef zsuiwidget
#define zsuiwidget

typedef struct WidgetMetrics{
    unsigned int WIDTH;
    unsigned int HEIGHT;

    WidgetMetrics(){
        WIDTH = 0;
        HEIGHT = 0;
    }
}WidgetMetrics;

typedef struct WidgetPosition{
    unsigned int posX;
    unsigned int posY;

    WidgetPosition(){
        posX = 100;
        posY = 100;
    }
}WidgetPosition;

namespace ZSUI {
    class Widget{
    private:
        WidgetMetrics metrics;
        WidgetPosition pos;
    public:
        virtual void resize(unsigned int Width, unsigned int Height);
        virtual void move(unsigned int x, unsigned int y);
        bool isHoveredByMouse();

        virtual void draw();

        Widget();
        virtual ~Widget();
    };
}

#endif
