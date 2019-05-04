#ifndef zs_gizmosRenderer
#define zs_gizmosRenderer

#include "zs-shader.h"
#include "zs-mesh.h"

class GizmosRenderer{
private:
    //pointer to mark shader
    ZSPIRE::Shader* mark_shader_ptr;
public:

    void drawPickedMeshWireframe(ZSPIRE::Mesh* mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawCube(ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawTransformControls(ZSVECTOR3 position, int tall, int dim);

    GizmosRenderer(ZSPIRE::Shader* mark_shader);
};


#endif
