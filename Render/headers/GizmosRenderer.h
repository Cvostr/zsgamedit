#ifndef zs_gizmosRenderer
#define zs_gizmosRenderer

#include "zs-shader.h"
#include <render/zs-mesh.h>
#include <render/zs-uniform-buffer.h>
#include <GL/glew.h>

class GizmosRenderer{
private:
    //pointer to mark shader
    ZSPIRE::Shader* mark_shader_ptr;
    Engine::UniformBuffer* transformBuffer;
    bool depthTestEnabled;
    bool cullFaceEnabled;
    int projectPerspective;
public:

    void drawPickedMeshWireframe(Engine::Mesh* mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawCube(ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawTransformControls(ZSVECTOR3 position, float tall, float dim);

    void glFeaturesOff();
    void glFeaturesOn();

    GizmosRenderer(ZSPIRE::Shader* mark_shader, bool depthTestEnabled, bool cullFaceEnabled, int projectPerspective, Engine::UniformBuffer* buf);
};


#endif
