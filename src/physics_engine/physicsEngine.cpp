#include "physicsEngine.h"

decimal findDistanceBetweenTwoPoints(Vector3 p1, Vector3 p2)
{
	glm::vec3 v1, v2;
	v1 = glm::vec3(p1.x, p1.y, p1.z);
	v2 = glm::vec3(p2.x, p2.y, p2.z);
	glm::vec3 d = v1 - v2;
	return glm::length(d);
	//check distance
}

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

//Quaternion PhysicsEngine::getRotationBasedOnForce(Vector3 force)
//{
//	//pass 2d force.
//	Vector3 up(0.0f, 1.0f, 0.0f);
//	Vector3 r = force.cross(up);
//	reactphysics3d::decimal d = force.dot(up);
//	Quaternion orientation(r, d);
//	return orientation;
//}



//implement for other types of components.
//need to implement a filter
//void PhysicsEngine::findClosestRigidBody(RigidBody* dst, decimal radius, RigidBody* src)
//{
//	//using a hack here.
//	//instead of testing overlap in physx, loop through the list of all object and find the global pos.
//	//then use distance formula to find the distance, if distance is under the radius. move to that entity.
//	//then use callbacks.
//	//this is for rigid bodies at the moment. Convert to all actors.
//	//o(n3) yikes.
//	Vector3 closestPos;
//	float closestDistance = radius;
//	Transform t = src->getTransform();
//	Vector3 positionSrc = t.getPosition();
//	for (auto i : rigidBodies)
//	{
//		Transform transform = i->getTransform();
//		Vector3 position = transform.getPosition();
//		auto d = findDistanceBetweenTwoPoints(position, positionSrc);
//		if (d < closestDistance)
//		{
//			i = dst;
//			radius = d;
//		}
//
//	}
//}

//Vector3 PhysicsEngine::findDirection(Vector3 src, Vector3 d)
//{
//	//src moves towards destination.
//	Vector3 dir = d - src;
//	dir.normalize();
//	return dir;
//}

//void PhysicsEngine::moveTowardsABody(RigidBody* src, RigidBody* dst, decimal linearVelocity)
//{
//	//src.
//	Transform transform = src->getTransform();
//	Vector3 position = transform.getPosition();
//
//	//dest
//	Transform transform2 = dst->getTransform();
//	Vector3 position2 = transform.getPosition();
//
//	Vector3 dir = findDirection(position, position2);
//	src->setLinearVelocity(dir);
//
//}

//void PhysicsEngine::goTowardsRigidBody(RigidBody* close)
//{
//	//only detect if rigid bodies are within the radius of 10f.
//	decimal radius = 10.0;
//	RigidBody* dst;
//	findClosestRigidBody(dst, 10, close);
//	moveTowardsABodyForce(close, dst);
//	//till pathfinding is done, simply add linear movement.
//	//apply linear velocity in that position or force.
//
//}

//void PhysicsEngine::moveTowardsABodyForce(RigidBody* src, RigidBody* dst)
//{
//	Transform transform = src->getTransform();
//	Vector3 position = transform.getPosition();
//
//	//dest
//	Transform transform2 = dst->getTransform();
//	Vector3 position2 = transform.getPosition();
//	Vector3 dir = findDirection(position, position2);
//
//	decimal forceM = 100.0f;  // Example force magnitude
//
//	Vector3 force = forceM * dir;
//	src->applyLocalForceAtCenterOfMass(force);
//}

//void PhysicsEngine::createExplosion(Vector3 pos, decimal radius, decimal force)
//{
//	//find all the objects within the radius and apply explosion.
//	//same logic as closest obj.
//	Vector3 closestPos;
//	for (auto i : rigidBodies)
//	{
//		Transform transform = i->getTransform();
//		Vector3 position = transform.getPosition();
//		auto d = findDistanceBetweenTwoPoints(position, pos);
//		if (d < radius)
//		{
//			//we apply force in the 
//			Vector3 dir = findDirection(pos, position);
//			i->applyLocalForceAtCenterOfMass(force * dir);
//		}
//
//	}
//}

//void PhysicsEngine::performRandomMovement(RigidBody* rigidBody)
//{
//	//this is mainly to have bodies walking around randomly.
//	//randomly generate forces and directions to move around.
//	Transform transform = rigidBody->getTransform();
//	Vector3 position = transform.getPosition();
//	Quaternion orientation = transform.getOrientation();
//	//apply random forces in the x,y plane to have them move around. mainly for rigid bodies
//	//random x,y direction.
//	//random float: https://stackoverflow.com/questions/686353/random-float-number-generation
//	float x = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
//	float y = static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f;
//	Vector3 force(x, y, 0.0);
//	Quaternion newQ = getRotationBasedOnForce(force);
//	rigidBody->applyLocalForceAtCenterOfMass(force);
//	//add here. add rotation manually.
//
//}

