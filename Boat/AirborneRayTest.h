#ifndef AIRBORNE_RAY_TEST
#define AIRBORNE_RAY_TEST

#include "Bullet\src\btBulletDynamicsCommon.h"
#include "RigidObject.h"

class AirborneRayTest : public btCollisionWorld::ClosestRayResultCallback
{
public:
	AirborneRayTest(const btVector3& rayFromWorld, const btVector3& rayToWorld) : btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld) {}
      
private:
	virtual bool needsCollision(btBroadphaseProxy* proxy0) const
	{
		const btCollisionObject* co = (btCollisionObject*)proxy0->m_clientObject;
                 
		if ((co->getOwner() != NULL) && (((PhysicsObject*)co->getOwner())->getCollisionType() == "projectile"))
			return false;
         
		return btCollisionWorld::ClosestRayResultCallback::needsCollision(proxy0);
	}
};

#endif