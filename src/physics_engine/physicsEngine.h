#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../asset_import/model.h"

using namespace reactphysics3d;

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
	std::shared_ptr<model::ModelDescriptor> model;
	std::string modelUuid = std::string("");
	ConcaveMeshShape* colliderShape;
	TriangleMesh* triangleMesh;
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
};