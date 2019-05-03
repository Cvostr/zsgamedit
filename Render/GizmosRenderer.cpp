#include "headers/GizmosRenderer.h"

GizmosRenderer::GizmosRenderer(ZSPIRE::Shader* mark_shader){
    //set shader pointer
    this->mark_shader_ptr = mark_shader;
}

void GizmosRenderer::drawPickedMeshWireframe(ZSPIRE::Mesh *mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color){
    this->mark_shader_ptr->Use();
    this->mark_shader_ptr->setTransform(transform);
    this->mark_shader_ptr->setGLuniformColor("color", color);

    mesh_ptr->DrawLines();
}
