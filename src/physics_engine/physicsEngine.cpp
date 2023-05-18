#include "physicsEngine.h"

PhysicsEngine* PhysicsEngine::getInstance()
{
	if (instance == nullptr) {
		instance = new PhysicsEngine();
	}
	return instance;
}

void PhysicsEngine::createWorld()
{
	PhysicsWorld::WorldSettings settings;
	settings.gravity = Vector3(0, -9.81, 0);

	world = physicsCommon.createPhysicsWorld(settings);
	physicsEvents = new PhysicsEvents();
	world->setEventListener(physicsEvents);
}

RigidBody* PhysicsEngine::createRigidBody()
{
	Vector3 position(0,0,0);
	Quaternion orientation = Quaternion::identity();
	Transform transform(position, orientation);
	return world->createRigidBody(transform);
}

RigidBody* PhysicsEngine::createRigidBody(glm::vec3 p, glm::quat r)
{
	//there are 3 body types: static, kinematic or dynamic
	//a static body has 0 velocity and does not collide with other static or kinematic bodies
	//a kinematic can have velocity and does not collide with other static or kinematic bodies
	//a dynamic body has velocity and can collide with other dynamic, static or kinematic bodies
	//to change type use body->setType(BodyType::KINEMATIC);
	//to disable/enable gravity use rigidBody->enableGravity(false);
	//to apply a force Vector3 force(2.0, 0.0, 0.0); rigidBody->applyForceToCenterOfMass(force);
	//A body can only collide against an other body if it has at least one collider
	//look at 11.1 Collision Shapes
	Vector3 position(p.x, p.y, p.z);
	Quaternion orientation = Quaternion(r.x,r.y,r.z,r.w);
	Transform transform(position, orientation);
	return world->createRigidBody(transform);
}

void PhysicsEngine::deleteRidigBody(RigidBody* body)
{
	world->destroyRigidBody(body);
}
