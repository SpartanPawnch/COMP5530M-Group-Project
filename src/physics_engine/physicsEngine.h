#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace reactphysics3d;

class PhysicsEngine {
private:
	PhysicsEngine() = default;
public:
	static PhysicsEngine* instance;
	static PhysicsEngine* getInstance();
	void createWorld();

	RigidBody* createRigidBody();
	RigidBody* createRigidBody(glm::vec3 p, glm::quat r);
	void deleteRidigBody(RigidBody* body);

	PhysicsCommon physicsCommon;
	PhysicsWorld* world;
	bool isSimulating = false;

	std::vector<RigidBody*> rigidBodies;
};