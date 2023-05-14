#pragma once

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include "../../src/logging.h"

namespace physics
{
	//link to the logging function.
	class physXErrorCallback : public physx::PxErrorCallback 
	{
		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
	};


	//refered from 
	class CollisionCallbacks : public physx::PxSimulationEventCallback
	{
	public:
		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
		void onWake(physx::PxActor** actors, physx::PxU32 count);
		void onSleep(physx::PxActor** actors, physx::PxU32 count);
		//this is the function we are concerned with:
		//https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/AdvancedCollisionDetection.html#extracting-contact-information
		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);
	};

}
