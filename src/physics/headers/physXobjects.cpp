#include "physicsObj.h"



namespace physics
{
	//callback abstracts.
	physXErrorCallback errorCallback2;
	CollisionCallbacks collisionCallback2;

	void PhysXEngine::createSceneDescriptor(physx::PxSceneDesc& sceneDesc)
	{
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = mDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		//static static.
		sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; 
		//static and kinetic.
		sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; 
		sceneDesc.simulationEventCallback = &collisionCallback2;
	    sceneDesc.filterShader = CollisionfilterShader;
		/*if(!mDispatcher)
			log*/
	}

	physx::PxPhysics* PhysXEngine::createPhysics()
	{
		return PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), mOutstandingAllocation, mPvd);

	}


	PhysXEngine::PhysXEngine()
	{
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, errorCallback2);
		if (!mFoundation)
			logging::logErr("%s \n", 64, "PHYSX ERROR: Foundation not created \n");
		return;
		if (EnableDebugging)
		{
			mPvd = PxCreatePvd(*mFoundation);
			mTransport = physx::PxDefaultPvdSocketTransportCreate(mHost.c_str(), mPort, mTimeout);
			mPvd->connect(*mTransport, physx::PxPvdInstrumentationFlag::eALL);
			mOutstandingAllocation = true;

		}
		if ((mPhysics = createPhysics()) == nullptr)
		{
			logging::logErr("%s \n", 64, "PHYSX ERROR: physics not created \n ");
			return;
		}
		if ((mDispatcher = physx::PxDefaultCpuDispatcherCreate(4)) == nullptr)
		{
			logging::logErr("%s \n", 64, "PHYSX ERROR: dispatcher not created \n ");
			return;
		}

		physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
		//create callbacks for collision

		createSceneDescriptor(sceneDesc);

	}
}