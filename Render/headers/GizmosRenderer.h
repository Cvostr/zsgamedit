#ifndef zs_gizmosRenderer
#define zs_gizmosRenderer

#include "zs-shader.h"
#include "zs-mesh.h"

#include <GL/glew.h>

class GizmosRenderer{
private:
    //pointer to mark shader
    ZSPIRE::Shader* mark_shader_ptr;
    bool depthTestEnabled;
    bool cullFaceEnabled;
    int projectPerspective;
public:

    void drawPickedMeshWireframe(ZSPIRE::Mesh* mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawCube(ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawTransformControls(ZSVECTOR3 position, int tall, int dim);

    void glFeaturesOff();
    void glFeaturesOn();

    GizmosRenderer(ZSPIRE::Shader* mark_shader, bool depthTestEnabled, bool cullFaceEnabled, int projectPerspective);
};


#endif
