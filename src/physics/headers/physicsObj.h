#pragma once
//I have used https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/HelloWorld.html
//as the reference for the following code.

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <memory>
#include "physXCallbacks.h"
#include "physxFilters.hpp"


namespace physics
{
	//using singleton design pattern. 
//define all physics related constants here.
#define GRAVITY = -9.18f;
	//define the timeout time for physx
#define timeout = 100;

	


	class PhysXEngine
	{
	private:
		PhysXEngine();
		//the following objects are used for physx initialisation and is common:
		//References: snippetArticulation/snippetsHelloworld. 
		physx::PxFoundation* mFoundation = nullptr;
		physx::PxPhysics* mPhysics = nullptr;
		physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
		physx::PxScene* mScene = nullptr;
		physx::PxMaterial* mMaterial = nullptr;
		//check cuda support;
		//remove. doesnt work in 4.1
		bool mCudaSupport = false;
		bool mOutstandingAllocation = false;

		//debugger this is for using physx debugger in the engine if required.
		physx::PxPvd* mPvd = nullptr;
		physx::PxPvdTransport* mTransport = nullptr;

		//host and port
		//refered from hello world.
		const std::string mHost = "127.0.0.1";
		const unsigned int mPort = 5425;
		unsigned int mTimeout = 10;

		//error callback
		physx::PxDefaultAllocator mAllocator;

		//scene elements
		bool mInitialised;

		//singleton obj
		static PhysXEngine* physxE;

		//error list
		

	public:
		//constructor
		PhysXEngine(PhysXEngine const&) = delete;
		void operator = (PhysXEngine const&) = delete;
		//operator for logging
		//this function takes bool as input to enable/disable physx debugging and to link to the physx debugger.
		//use this function to enable and disable debugger.
		void initPhysxEngine(bool enableDebugger);
		bool isInitialised() const { return mInitialised; };
		~PhysXEngine();
		static PhysXEngine* getInstance();
		void createSceneDescriptor(physx::PxSceneDesc& sceneDesc);
		//check for cuda and opengl compatibility else go with cpu.
		//add 
		physx::PxPhysics* createPhysics();

		//define the operations here.
		//projectile motion of a rigidBody.


		//linear motion of rigid body

		//create a force at a position.

		//create particles motion.

		//create a bezier

	};

	//static objects needed for physX
static bool EnablePhysics = true;
static bool EnableDebugging = true;
PhysXEngine* PhysXEngine::physxE = nullptr;
bool staticRunOnGpu = false;
}

