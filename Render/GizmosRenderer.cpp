#include "headers/GizmosRenderer.h"

GizmosRenderer::GizmosRenderer(Engine::Shader* mark_shader,
                               bool depthTestEnabled,
                               bool cullFaceEnabled,
                               int projectPerspective,
                               Engine::UniformBuffer* buf,
                               Engine::UniformBuffer* editor){
    //set shader pointer
    this->mark_shader_ptr = mark_shader;

    this->cullFaceEnabled = cullFaceEnabled;
    this->depthTestEnabled = depthTestEnabled;
    this->projectPerspective = projectPerspective;

    this->transformBuffer = buf;
    this->editorBuffer = editor;
}

void GizmosRenderer::drawPickedMeshWireframe(Engine::Mesh *mesh_ptr, ZSMATRIX4x4 transform, ZSRGBCOLOR color){
    glFeaturesOff();

    this->mark_shader_ptr->Use();

    transformBuffer->bind();
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform);

    editorBuffer->bind();
    ZSVECTOR4 v = ZSVECTOR4(color.gl_r, color.gl_g, color.gl_b, color.gl_a);
    editorBuffer->writeData(0, 16, &v);

    mesh_ptr->DrawLines();

    glFeaturesOn();
}

void GizmosRenderer::drawCube(ZSMATRIX4x4 transform, ZSRGBCOLOR color){
    this->mark_shader_ptr->Use();
    transformBuffer->bind();
    transformBuffer->writeData(sizeof (ZSMATRIX4x4) * 2, sizeof (ZSMATRIX4x4), &transform);

    editorBuffer->bind();
    ZSVECTOR4 v = ZSVECTOR4(color.gl_r, color.gl_g, color.gl_b, color.gl_a);
    editorBuffer->writeData(0, 16, &v);

    Engine::getCubeMesh3D()->Draw();
}

void GizmosRenderer::drawTransformControls(ZSVECTOR3 position, float tall, float dim){
    if(projectPerspective == 3){
        tall /= 10;
        dim /= 10;
    }

    ZSMATRIX4x4 transform;
    ZSVECTOR3 scale = ZSVECTOR3(dim, tall, dim);
    ZSMATRIX4x4 scale_mat = getScaleMat(scale);

    glFeaturesOff();

    //X control
    ZSVECTOR3 rotation = ZSVECTOR3(0,0,90);
    transform = scale_mat * getRotationMat(rotation) * getTranslationMat(ZSVECTOR3(position.X + tall, position.Y, position.Z));
    drawCube(transform, ZSRGBCOLOR(255,0,0));
    //Y control
    rotation = ZSVECTOR3(0,0,0);
    transform = scale_mat * getRotationMat(rotation) * getTranslationMat(ZSVECTOR3(position.X, position.Y + tall, position.Z));
    drawCube(transform, ZSRGBCOLOR(0,255,0));
    //Z control
    rotation = ZSVECTOR3(90,0,0);
    transform = scale_mat * getRotationMat(rotation) * getTranslationMat(ZSVECTOR3(position.X, position.Y, position.Z + tall));
    drawCube(transform, ZSRGBCOLOR(0,0,255));

    glFeaturesOn();
}

void GizmosRenderer::glFeaturesOff(){
    if(this->depthTestEnabled == true) //if depth is enabled
        glDisable(GL_DEPTH_TEST);

    if(this->cullFaceEnabled == true)
        glDisable(GL_CULL_FACE);
}
void GizmosRenderer::glFeaturesOn(){
    if(this->depthTestEnabled == true) //if depth is enabled
        glEnable(GL_DEPTH_TEST);

    if(this->cullFaceEnabled == true)
        glEnable(GL_CULL_FACE);
}
