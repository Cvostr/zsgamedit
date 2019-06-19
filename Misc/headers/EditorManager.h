#ifndef editor_manager
#define editor_manager

#include "zs_types.h"

class EditorComponentManager{
private:

public:
    int WIDTH;
    int HEIGHT;

    Project* project_struct_ptr;

    void setDpMetrics(int W, int H);
    void setProjectStructPtr(Project* ptr);

    float deltaTime;

    virtual void init();
    virtual void destroy();
    virtual void updateWindowSize(int W, int H);

    EditorComponentManager();
    virtual ~EditorComponentManager();
};

#endif
