#include "physicsObj.h"

namespace physics
{

	//collision filter.
	//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
	physx::PxFilterFlags CollisionFilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		// let triggers through
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlag::eDEFAULT;
		}
		// generate contacts for all that were not filtered above
		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		// trigger the contact callback for pairs (A,B) where
		// the filtermask of A contains the ID of B and vice versa.
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

		return physx::PxFilterFlag::eDEFAULT;
	}

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
		sceneDesc.filterShader = CollisionFilterShader;
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