#ifndef engine_manager
#define engine_manager

#include "zs_types.h"

class EngineComponentManager{
private:

public:
    int WIDTH;
    int HEIGHT;

    Project* project_struct_ptr;

    void setDpMetrics(int W, int H);
    void setProjectStructPtr(Project* ptr);

    virtual void init();

    EngineComponentManager();
    virtual ~EngineComponentManager();
};

#endif
