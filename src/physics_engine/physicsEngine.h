#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace reactphysics3d;

decimal findDistanceBetweenTwoPoints(Vector3 p1, Vector3 p2);

enum ColliderTypes {
	CUBE,
	SPHERE,
	CAPSULE,
	MESH
};

enum CollisionCategories {
	CATEGORY1 = 0x0001,
	CATEGORY2 = 0x0002,
	CATEGORY3 = 0x0004
};

struct ColliderObject {
	Collider* collider;
	ColliderTypes shape;
	CollisionCategories category;
	CollisionCategories currentCollideWith;
	std::vector<CollisionCategories> collidesWith;
	float materialBounciness=0.4f;
	float materialFrictionCoefficient=0.2f;
	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.f, .0f, .0f, .0f);
};

struct CubeColliderObject : ColliderObject {
	glm::vec3 extents = glm::vec3(1.0f);
	BoxShape* colliderShape;
};

struct SphereColliderObject : ColliderObject {
	float radius = 1.0f;
	SphereShape* colliderShape;
};

struct CapsuleColliderObject : ColliderObject {
	float radius = 1.0f;
	float height = 3.0f;
	CapsuleShape* colliderShape;
};

struct MeshColliderObject : ColliderObject {
	//model descriptor
	ConcaveMeshShape* colliderShape;
};

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
	//moved the following functions to rigidBodyComponent
	//void performRandomMovement(RigidBody* rigidBody);
	//actor change orientation.
	//Quaternion getRotationBasedOnForce(Vector3 force);
	//void findClosestRigidBody(RigidBody* dst, decimal radius, RigidBody* src);
	//void goTowardsRigidBody(RigidBody* close);
	//Vector3 findDirection(Vector3 src, Vector3 d);
	//void moveTowardsABody(RigidBody* src, RigidBody* dst, decimal linearVelocity);
	//void moveTowardsABodyForce(RigidBody* src, RigidBody* dst);
	//void createExplosion(Vector3 pos, decimal radius, decimal force);

};


