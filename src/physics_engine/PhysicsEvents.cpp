#include "PhysicsEvents.h"
#include "../ECS/Component/RigidBodyComponent.h"
#include <iostream>

void PhysicsEvents::onContact(const CollisionCallback::CallbackData& callbackData) {
    for (uint p = 0; p < callbackData.getNbContactPairs(); p++) {

        CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

        for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {

            CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

            Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform() *
                contactPoint.getLocalPointOnCollider1();

            RigidBody* rigidBody1 =
                static_cast<RigidBody*>(contactPair.getCollider1()->getUserData());
            RigidBody* rigidBody2 =
                static_cast<RigidBody*>(contactPair.getCollider2()->getUserData());

            BodyType type1 = rigidBody1->getType();
            BodyType type2 = rigidBody2->getType();

            std::string t1 = "not found";
            std::string t2 = "not found";

            // if (type1 == BodyType::DYNAMIC)
            //     t1 = "Dynamic";
            // else if (type1 == BodyType::KINEMATIC)
            //     t1 = "kinematic";
            // else if (type1 == BodyType::STATIC)
            //     t1 = "static";
            //
            // if (type2 == BodyType::DYNAMIC)
            //     t2 = "Dynamic";
            // else if (type2 == BodyType::KINEMATIC)
            //     t2 = "kinematic";
            // else if (type2 == BodyType::STATIC)
            //     t2 = "static";
            //
            // std::cout << t1 << " collided with " << t2 << std::endl;

            CollisionInfo* body1 = static_cast<CollisionInfo*>(rigidBody1->getUserData());
            CollisionInfo* body2 = static_cast<CollisionInfo*>(rigidBody2->getUserData());

            if (body1 != nullptr && body2 != nullptr) {
                body1->collidedAsBody1 = true;
                body1->otherUuid1 = body2->ownUuid;

                body2->collidedAsBody2 = true;
                body2->otherUuid2 = body1->ownUuid;
            }

            /*body1->collidedAsBody1 = true;
            body2->collidedAsBody2 = true;*/
        }
    }
}
