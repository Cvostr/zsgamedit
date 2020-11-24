#include "headers/GizmosRenderer.h"
#include <world/go_properties.h>

GizmosRenderer::GizmosRenderer(Engine::Shader* mark_shader,
                               Engine::Shader* grid_shader,
                               Engine::Shader* ui_shader,
                               bool cullFaceEnabled,
                               int projectPerspective,
                               Engine::UniformBuffer* buf,
                               Engine::UniformBuffer* editor,
                               Engine::UniformBuffer* inst) :
    mark_shader_ptr(mark_shader),
    grid_shader_ptr(grid_shader),
    ui_shader_ptr(ui_shader),

    transformBuffer(buf),
    editorBuffer(editor),
    instBuffer(inst)
{

    this->cullFaceEnabled = cullFaceEnabled;
    this->projectPerspective = projectPerspective;

    SunTextureSprite = Engine::allocTexture();
    SunTextureSprite->LoadPNGTextureFromFile("res/icons/sunlight.png");

    //Create grid strokes transforms
    int red = GRID_STROKE_COUNT * GRID_DIST * -0.5f;
    for (int i = 0; i < GRID_STROKE_COUNT; i++) {
        Mat4 transform = getIdentity();

        transform = transform * getScaleMat(GRID_STROKE_WIDTH, 100, GRID_STROKE_WIDTH);
        transform = transform * getRotationMat(ZSVECTOR3(90, 0, 0));
        transform = transform * getTranslationMat(red + GRID_DIST * i, 0, 0);

        grid_strokes_transf[i] = transform;
    }
    for (int i = GRID_STROKE_COUNT; i < GRID_STROKE_COUNT * 2; i++) {
        Mat4 transform = getIdentity();

        transform = transform * getScaleMat(GRID_STROKE_WIDTH, 100, GRID_STROKE_WIDTH);
        transform = transform * getRotationMat(ZSVECTOR3(0, 0, 90));
        transform = transform * getTranslationMat(0, 0, red + GRID_DIST * (i - GRID_STROKE_COUNT));

        grid_strokes_transf[i] = transform;
    }
}

GizmosRenderer::~GizmosRenderer() {

}

void GizmosRenderer::drawPickedMeshWireframe(Engine::Mesh *mesh_ptr, Mat4 transform, ZSRGBCOLOR color){
    glFeaturesOff();

    this->mark_shader_ptr->Use();

    transformBuffer->bind();
    transformBuffer->writeData(sizeof (Mat4) * 2, sizeof (Mat4), &transform);

    editorBuffer->bind();
    ZSVECTOR4 v = ZSVECTOR4(color.gl_r, color.gl_g, color.gl_b, color.gl_a);
    editorBuffer->writeData(0, 16, &v);

    mesh_ptr->DrawLines();

    glFeaturesOn();
}

void GizmosRenderer::drawCube(Mat4 transform, ZSRGBCOLOR color){
    this->mark_shader_ptr->Use();
    transformBuffer->bind();
    transformBuffer->writeData(sizeof (Mat4) * 2, sizeof (Mat4), &transform);

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

    Mat4 transform;
    ZSVECTOR3 scale = ZSVECTOR3(dim, tall, dim);
    Mat4 scale_mat = getScaleMat(scale);

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

void GizmosRenderer::drawObjectRigidbodyShape(void* phys_property){
    if(phys_property == nullptr) return;
    glFeaturesOff();
    this->mark_shader_ptr->Use();

    Engine::PhysicalProperty* property = static_cast<Engine::PhysicalProperty*>(phys_property);
    Engine::TransformProperty* transform = property->go_link.updLinkPtr()->getTransformProperty();
    Engine::Mesh* mesh_toDraw = nullptr;

    switch(property->coll_type){
        case COLLIDER_TYPE::COLLIDER_TYPE_CUBE:{
            mesh_toDraw = Engine::getCubeMesh3D();
            break;
        }
        case COLLIDER_TYPE::COLLIDER_TYPE_SPHERE:{
            mesh_toDraw = Engine::getSphereMesh();
            break;
        }
    }

    ZSVECTOR3 scale = transform->abs_scale;
    if(property->isCustomPhysicalSize){
        scale = property->cust_size;
    }

    editorBuffer->bind();
    ZSVECTOR4 v = ZSVECTOR4(255, 0, 0, 255);
    editorBuffer->writeData(0, 16, &v);

    transformBuffer->bind();
    Mat4 rotation_mat1 = getIdentity();
    transform->getAbsoluteRotationMatrix(rotation_mat1);
    Mat4 rotation_mat = getRotationMat(transform->abs_rotation);
    Mat4 transform_mat = getScaleMat(scale) * rotation_mat * rotation_mat1 * getTranslationMat(transform->abs_translation + property->transform_offset);

    transformBuffer->writeData(sizeof (Mat4) * 2, sizeof (Mat4), &transform_mat);
    //If mesh exist, then draw
    if(mesh_toDraw)
        mesh_toDraw->DrawLines();
    glFeaturesOn();
}

void GizmosRenderer::glFeaturesOff(){
    //Disable DEPTH TEST for current pipeline
    glDisable(GL_DEPTH_TEST);
    //Disable Face Culling
    if(this->cullFaceEnabled == true)
        glDisable(GL_CULL_FACE);
}
void GizmosRenderer::glFeaturesOn(){
    //Enable DEPTH TEST for current pipeline
    glEnable(GL_DEPTH_TEST);
    //Enable Face Culling
    if(this->cullFaceEnabled == true)
        glEnable(GL_CULL_FACE);
}

void GizmosRenderer::drawGrid() {

    editorBuffer->bind();
    ZSVECTOR4 v = ZSVECTOR4(0.3f, 0.3f, 0.3f, 1);
    editorBuffer->writeData(0, 16, &v);

    instBuffer->bind();
    //Send all transforms to instance buffer
    for (int i = 0; i < GRID_STROKE_COUNT * 2; i++) {
        instBuffer->writeData(sizeof(Mat4) * i, sizeof(Mat4), &grid_strokes_transf[i]);
    }

    this->grid_shader_ptr->Use();
       
    Engine::getCubeMesh3D()->DrawInstanced(GRID_STROKE_COUNT * 2);
}

void GizmosRenderer::drawGizmoSprite(Engine::Texture* texture, ZSVECTOR3 position, ZSVECTOR3 scale, Mat4 CamView) {
    Mat4 ScaleMat = getScaleMat(scale);
    Mat4 PosMat = getTranslationMat(position);

    Mat4 Transform = ScaleMat * PosMat;
    Transform = removeRotationFromTransformMat(Transform, CamView);

    transformBuffer->bind();
    transformBuffer->writeData(sizeof(Mat4) * 2, sizeof(Mat4), &Transform);

    ui_shader_ptr->Use();

    texture->Use(0);

    Engine::getPlaneMesh2D()->Draw();
}