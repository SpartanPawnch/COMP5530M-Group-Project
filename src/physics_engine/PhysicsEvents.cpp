#include "PhysicsEvents.h"
#include "../ECS/Component/RigidBodyComponent.h"
#include <iostream>

void PhysicsEvents::onContact(const CollisionCallback::CallbackData& callbackData) {
    std::cout << "contact detected" << std::endl;
    for (uint p = 0; p < callbackData.getNbContactPairs(); p++) {

        CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

        for (uint c = 0; c < contactPair.getNbContactPoints(); c++) {

            CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

            Vector3 worldPoint = contactPair.getCollider1()->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();

            RigidBodyComponent* body1 = static_cast<RigidBodyComponent*>(contactPair.getBody1()->getUserData());
            RigidBodyComponent* body2 = static_cast<RigidBodyComponent*>(contactPair.getBody2()->getUserData());

            body1->collidedAsBody1 = true;
            body2->collidedAsBody2 = true;
        }
    }
}