#include <world/Physics.h>
#include "headers/World.h"
#include "headers/obj_properties.h"
#include "ProjEd/headers/InspEditAreas.h"

void PhysicalProperty::init(){
    TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
    //Set collision shape
    updateCollisionShape();

    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        shape->calculateLocalInertia(mass, localInertia);
    //Declare start transform
    btTransform startTransform;
    startTransform.setIdentity();
    //Set start transform
    startTransform.setOrigin(btVector3( btScalar(transform->_last_translation.X),
                                                btScalar(transform->_last_translation.Y),
                                                btScalar(transform->_last_translation.Z)));

    btQuaternion b;
    b.setEuler(transform->_last_rotation.Z, transform->_last_rotation.Y, transform->_last_rotation.X);

    startTransform.setRotation(btQuaternion(transform->_last_rotation.X, transform->_last_rotation.Y, transform->_last_rotation.Z));
    //startTransform.setRotation(b);

     //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
     btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

     btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);

     rigidBody = new btRigidBody(cInfo);

     rigidBody->setUserIndex(go_link.updLinkPtr()->array_index);
     //add rigidbody to world
     go_link.world_ptr->physical_world->addRidigbodyToWorld(rigidBody);

     created = true;
}

void PhysicalProperty::updateCollisionShape(){
    TransformProperty* transform = this->go_link.updLinkPtr()->getPropertyPtr<TransformProperty>();
    MeshProperty* mesh = this->go_link.updLinkPtr()->getPropertyPtr<MeshProperty>();
    Engine::Mesh* m = mesh->mesh_ptr->mesh_ptr;

    ZSVECTOR3 scale = transform->_last_scale;
    if(isCustomPhysicalSize){
        scale = cust_size;
    }

    switch(coll_type){
        case COLLIDER_TYPE_NONE: {
            break;
        }
        case COLLIDER_TYPE_CUBE: {
            shape = new btBoxShape(btVector3(btScalar(scale.X),
                                         btScalar(scale.Y),
                                         btScalar(scale.Z)));
            break;
        }
        case COLLIDER_TYPE_BOX: {
            shape = new btBox2dShape(btVector3(btScalar(scale.X),
                                         btScalar(scale.Y),
                                         0));
            break;
        }
        case COLLIDER_TYPE_SPHERE:{
            shape = new btSphereShape(btScalar(scale.Y));
            break;
        }
        case COLLIDER_TYPE_CONVEX_HULL:{

            shape = new btConvexHullShape(m->vertices_coord, m->vertices_num, sizeof (float) * 3);
            break;
        }
        case COLLIDER_TYPE_MESH:{

            float* vertices = m->vertices_coord;
            int* indices = reinterpret_cast<int*>(m->indices_arr);

            btTriangleIndexVertexArray* va = new btTriangleIndexVertexArray(m->indices_num / 3,
                                                                            indices,
                                                                            3 * sizeof (int),
                                                                            m->vertices_num,
                                                                            reinterpret_cast<btScalar*>(vertices),
                                                                            sizeof (ZSVECTOR3));
            shape = new btBvhTriangleMeshShape(va, false);
            break;
        }
    }
}

void PhysicalProperty::addColliderRadio(InspectorWin* inspector){
    AreaRadioGroup* group = new AreaRadioGroup; //allocate button layout
    group->value_ptr = reinterpret_cast<uint8_t*>(&this->coll_type);
    group->go_property = static_cast<void*>(this);

    QRadioButton* box_radio = new QRadioButton; //allocate first radio
    box_radio->setText("Box");
    QRadioButton* cube_radio = new QRadioButton;
    cube_radio->setText("Cube");
    QRadioButton* sphere_radio = new QRadioButton;
    sphere_radio->setText("Sphere");
    QRadioButton* convex_radio = new QRadioButton;
    convex_radio->setText("Convex Hull");
    QRadioButton* mesh_radio = new QRadioButton;
    mesh_radio->setText("Mesh");
    //add created radio buttons
    group->addRadioButton(box_radio);
    group->addRadioButton(cube_radio);
    group->addRadioButton(sphere_radio);
    group->addRadioButton(convex_radio);
    group->addRadioButton(mesh_radio);
    //Register in Inspector
    inspector->registerUiObject(group);
    inspector->getContentLayout()->addLayout(group->btn_layout);
}
void PhysicalProperty::addMassField(InspectorWin* inspector){
    FloatPropertyArea* mass_area = new FloatPropertyArea;
    mass_area->setLabel("Mass"); //Its label
    mass_area->value = &this->mass;
    mass_area->go_property = static_cast<void*>(this);
    inspector->addPropertyArea(mass_area);
}

void PhysicalProperty::addCustomSizeField(InspectorWin* inspector){
    BoolCheckboxArea* cust = new BoolCheckboxArea;
    cust->setLabel("Custom size ");
    cust->go_property = static_cast<void*>(this);
    cust->bool_ptr = &this->isCustomPhysicalSize;
    inspector->addPropertyArea(cust);

    if(isCustomPhysicalSize){
        Float3PropertyArea* cust = new Float3PropertyArea; //New property area
        cust->setLabel("Scale"); //Its label
        cust->vector = &this->cust_size; //Ptr to our vector
        cust->go_property = static_cast<void*>(this);
        inspector->addPropertyArea(cust);

    }
}

PhysicalProperty::PhysicalProperty(){
    created = false;
    isCustomPhysicalSize = false;

    rigidBody = nullptr;
}

void PhysicalProperty::copyTo(GameObjectProperty* dest){

    //Do base things
    GameObjectProperty::copyTo(dest);

    PhysicalProperty* rigi_prop = static_cast<PhysicalProperty*>(dest);
    rigi_prop->mass = this->mass;
    rigi_prop->coll_type = this->coll_type;
    rigi_prop->isCustomPhysicalSize = this->isCustomPhysicalSize;
    rigi_prop->cust_size = this->cust_size;
}

void PhysicalProperty::onUpdate(float deltaTime){
    if(!rigidBody->isActive())
        rigidBody->activate();
}
