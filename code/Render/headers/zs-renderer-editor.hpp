#pragma once

#include <render/Shader.hpp>
#include <render/Framebuffer.hpp>
#include <render/Mesh.hpp>
#include <render/UniformBuffer.hpp>
#include <world/Camera.hpp>
#include <render/Texture.h>
#include "GizmosRenderer.h"
#include "../../Misc/headers/EditorManager.h"
#include <QMainWindow>
#include <engine/Window.hpp>
#include <ogl/GLRenderer.hpp>

class RenderPipelineEditor : public Engine::GLRenderer {
private:
    GizmosRenderer* gizmos;

    Engine::Shader* pick_shader; //Shader to draw & pick objects
    Engine::Shader* obj_mark_shader; //Shader to draw mark of selected objects
    Engine::Shader* obj_grid_shader;
    Engine::Shader* sprite_shader_3d;

    Engine::Framebuffer* gbuffer;
    Engine::Framebuffer* df_light_buffer;
public:
    Engine::UniformBuffer* editorUniformBuffer;

    void setup(int bufWidth, int bufHeight);
    void initGizmos(int projectPespective);

    void render(Engine::Window* window, void* projectedit_ptr);
    unsigned int render_getpickedObj(void* projectedit_ptr, int mouseX, int mouseY);
    RGBAColor getColorOfPickedTransformControl(int mouseX, int mouseY, void* projectedit_ptr);
    void renderGizmos(void* projectedit_ptr, Engine::Camera* cam);

    void OnUpdateWindowSize(int W, int H);

    void* win_ptr;

    GizmosRenderer* getGizmosRenderer();
    //override virtual function from EngineComponentManager
    void OnCreate();

    Engine::Shader* getPickingShader();

    RenderPipelineEditor();
    ~RenderPipelineEditor();
};
