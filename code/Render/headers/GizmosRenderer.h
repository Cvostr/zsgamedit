#pragma once

#include <render/Texture.h>
#include <render/Shader.hpp>
#include <render/Mesh.hpp>
#include <render/UniformBuffer.hpp>
#include <GL/glew.h>

#define GRID_STROKE_COUNT 45
#define GRID_DIST 4
#define GRID_STROKE_WIDTH 0.03f

class GizmosRenderer{
private:
    //pointer to mark shader
    Engine::Shader* mark_shader_ptr;
    Engine::Shader* grid_shader_ptr;
    Engine::Shader* ui_shader_ptr;
    Engine::UniformBuffer* transformBuffer;
    Engine::UniformBuffer* editorBuffer;
    Engine::UniformBuffer* instBuffer;
    Engine::UniformBuffer* uiBuffer;
    bool cullFaceEnabled;
    int projectPerspective;
    Mat4 grid_strokes_transf[GRID_STROKE_COUNT * 2];

    
public:
    Engine::Texture* SunTextureSprite;
    void drawPickedMeshWireframe(Engine::Mesh* mesh_ptr, Mat4 transform, RGBAColor color);
    void drawCube(Mat4 transform, RGBAColor color);
    void drawTransformControls(Vec3 position, float tall, float dim);
    void drawObjectRigidbodyShape(void* phys_property);
    void drawGrid();
    void drawGizmoSprite(Engine::Texture* texture, Vec3 position, Vec3 scale, Mat4 CamView, RGBAColor color);

    void glFeaturesOff();
    void glFeaturesOn();

    GizmosRenderer(Engine::Shader* mark_shader,
                   Engine::Shader* grid_shader,
                   Engine::Shader* ui_shader,
                   bool cullFaceEnabled,
                   int projectPerspective,
                   Engine::UniformBuffer* buf,
                   Engine::UniformBuffer* editor,
                   Engine::UniformBuffer* inst,
                   Engine::UniformBuffer* _uiBuffer);
    ~GizmosRenderer();
};