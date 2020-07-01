#ifndef zs_gizmosRenderer
#define zs_gizmosRenderer

#include <render/zs-shader.h>
#include <render/zs-mesh.h>
#include <render/zs-uniform-buffer.h>
#include <GL/glew.h>

#define GRID_STROKE_COUNT 45
#define GRID_DIST 4
#define GRID_STROKE_WIDTH 0.03f

class GizmosRenderer{
private:
    //pointer to mark shader
    Engine::Shader* mark_shader_ptr;
    Engine::Shader* grid_shader_ptr;
    Engine::UniformBuffer* transformBuffer;
    Engine::UniformBuffer* editorBuffer;
    Engine::UniformBuffer* instBuffer;
    bool cullFaceEnabled;
    int projectPerspective;
    ZSMATRIX4x4 grid_strokes_transf[GRID_STROKE_COUNT * 2];
public:

    void drawPickedMeshWireframe(Engine::Mesh* mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawCube(ZSMATRIX4x4 transform, ZSRGBCOLOR color);
    void drawTransformControls(ZSVECTOR3 position, float tall, float dim);
    void drawObjectRigidbodyShape(void* phys_property);
    void drawGrid();

    void glFeaturesOff();
    void glFeaturesOn();

    GizmosRenderer(Engine::Shader* mark_shader,
                   Engine::Shader* grid_shader,
                   bool cullFaceEnabled,
                   int projectPerspective,
                   Engine::UniformBuffer* buf,
                   Engine::UniformBuffer* editor,
                   Engine::UniformBuffer* inst);
};


#endif
