#include "headers/World.h"

PhysicalWorld::PhysicalWorld(PhysicalWorldSettings* settings){
            this->collisionConfiguraton = new btDefaultCollisionConfiguration();
            //m_collisionConfiguration->setConvexConvexMultipointIterations();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            this->dispatcher = new btCollisionDispatcher(this->collisionConfiguraton);

            this->broadphase = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            this->solver = new btSequentialImpulseConstraintSolver;

            this->physic_world = new btDiscreteDynamicsWorld(this->dispatcher, this->broadphase, this->solver, this->collisionConfiguraton);

            this->physic_world->setGravity(btVector3(settings->gravity.X,
                                                     settings->gravity.Y,
                                                     settings->gravity.Z));
}
