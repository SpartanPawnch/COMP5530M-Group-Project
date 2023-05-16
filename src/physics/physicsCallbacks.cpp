#include "physicsCallbacks.h"

namespace physics
{
	//error callback
	void physXErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		logging::logErr("PhysX Error %d: %s \n", code, message);
	}

	//collison callback
	//to do add extra behaviour
	void CollisionCallbacks::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
	{
		logging::logInfo("%s \n", "onConstraintBreak");
	}
	void CollisionCallbacks::onWake(physx::PxActor** actors, physx::PxU32 count)
	{
		logging::logInfo("%s \n", "onWake");
	}
	void CollisionCallbacks::onSleep(physx::PxActor** actors, physx::PxU32 count)
	{
		logging::logInfo("%s \n", "onSleep");
	}
	//this is the function we are concerned with:
	void CollisionCallbacks::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		logging::logInfo("%s \n", "onTrigger");
	}
	void CollisionCallbacks::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{
		logging::logInfo("%s \n", "onAdvance");
	}
	void CollisionCallbacks::onContact(const physx::PxContactPairHeader& pairHeader,
		const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		logging::logInfo("%s \n", "Collision made");
		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& cp = pairs[i];
			std::string test = std::string(cp.shapes[0]->getName());
			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				std::string msg = std::string(cp.shapes[0]->getName()) + std::string(" and ");
				msg + std::string(cp.shapes[1]->getName());
				logging::logInfo("%s ", msg);
			}
			else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				std::string msg = std::string(cp.shapes[0]->getName()) + std::string(" and ");
				msg + std::string(cp.shapes[1]->getName());
				logging::logInfo("%s ", msg);
			}
		}
	}

}