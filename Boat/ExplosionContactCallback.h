#ifndef EXPLOSION_CONTACT_CALLBACK_H
#define EXPLOSION_CONTACT_CALLBACK_H

#include "Bullet\src\btBulletDynamicsCommon.h"
#include "Utilities.h"
#include <string>

using std::string;
using namespace Utilities;

struct ExplosionContactCallback : public btCollisionWorld::ContactResultCallback 
{
	btRigidBody& body; //!< The body the sensor is monitoring
    string ctxt; //!< External information for contact processing
	float force;
	float damage;

    //! Constructor, pass whatever context you want to have available when processing contacts
    /*! You may also want to set m_collisionFilterGroup and m_collisionFilterMask
     *  (supplied by the superclass) for needsCollision() */
    ExplosionContactCallback(btRigidBody& tgtBody , string context , float Force, float Damage /*, ... */)
        : btCollisionWorld::ContactResultCallback(), body(tgtBody), ctxt(context) { force = Force; damage = Damage; }

    //! If you don't want to consider collisions where the bodies are joined by a constraint, override needsCollision:
    /*! However, if you use a btCollisionObject for #body instead of a btRigidBody,
     *  then this is unnecessary—checkCollideWithOverride isn't available */
    virtual bool needsCollision(btBroadphaseProxy* proxy) const {
        // superclass will check m_collisionFilterGroup and m_collisionFilterMask
        if(!btCollisionWorld::ContactResultCallback::needsCollision(proxy))
            return false;
        // if passed filters, may also want to avoid contacts between constraints
        return body.checkCollideWithOverride(static_cast<btCollisionObject*>(proxy->m_clientObject));
    }

    //! Called with each contact for your own processing (e.g. test if contacts fall in within sensor parameters)
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0,int partId0,int index0,
        const btCollisionObjectWrapper* colObj1,int partId1,int index1)
    {
		if (((btRigidBody*)colObj1->getCollisionObject())->getOwner() != 0 &&
			((PhysicsObject*)((btRigidBody*)colObj1->getCollisionObject())->getOwner())->getCollisionType() == "projectile") 
			return 0;
		((btRigidBody*)colObj1->getCollisionObject())->activate();
		btVector3 direction = colObj1->getCollisionObject()->getWorldTransform().getOrigin() - colObj0->getCollisionObject()->getWorldTransform().getOrigin();
		float mSqr = magSqr(&direction);
		if (direction == btVector3(0,0,0)) 
			{}
		else if ((25.0f - mSqr) > 0.0f) {
			if ((direction.normalized() * (25.0f-mSqr) * force) == btVector3(0,0,0)) return 0;
	        ((btRigidBody*)colObj1->getCollisionObject())->applyCentralImpulse(direction.normalized() * (25.0f-mSqr) * force);
		}
        return 0; // There was a planned purpose for the return value of addSingleResult, but it is not used so you can ignore it.
    }
};

#endif