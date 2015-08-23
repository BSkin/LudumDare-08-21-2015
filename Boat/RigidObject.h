#ifndef RIGIDOBJECT_H
#define RIGIDOBJECT_H

#include "PhysicsObject.h"

class RigidObject : public PhysicsObject
{
public:
	RigidObject();
	RigidObject(string, string); //ok for now
	RigidObject(string, string, glm::vec2 position, float rotation, glm::vec2 size, float mass, string path, int colGroup, int colMask); //ok for now
	virtual ~RigidObject() = 0;

	virtual int render(int i, long totalElapsedTime);
	virtual int update(long elapsedTime);

	btVector3 getWorldPosFromLocalPos(float x, float y) { if (!physInit) return btVector3(0,0,0); return body->getWorldTransform() * btVector3(x, y, 0); }
	btVector3 getLocalPosFromWorldPos(float x, float y) { if (!physInit) return btVector3(0,0,0); return body->getWorldTransform().inverse() * btVector3(x, y, 0); }
	btVector3 getLocalPosFromWorldPos(glm::vec2 vec)			{ return getLocalPosFromWorldPos(vec.x, vec.y); }

	virtual btVector3 getAngularVelocity()						{ if (!physInit) return btVector3(0,0,0); return body->getAngularVelocity(); }
	virtual btVector3 getVelocity()								{ if (!physInit) return btVector3(0,0,0); return body->getLinearVelocity(); }
	virtual void setAngularVelocity(float x, float y, float z)	{ if (!physInit) return; body->setAngularVelocity(btVector3(x, y, z)); }
	virtual void setAngularVelocity(glm::vec3 vel)				{ setAngularVelocity(vel.x, vel.y, vel.z); }
	virtual void setVelocity(float x, float y, float z)			{ if (!physInit) return; body->setLinearVelocity(btVector3(x, y, z)); }
	virtual void setVelocity(glm::vec3 vel)						{ setVelocity(vel.x, vel.y, vel.z); }

	virtual void setGravity(glm::vec2 grav)						{ if (!physInit) return; body->setGravity(btVector3(grav.x, grav.y, 0.0f)); }
	virtual void setGravity(float x, float y)					{ if (!physInit) return; body->setGravity(btVector3(x, y, 0)); }
	virtual void setFriction(float x)							{ if (!physInit) return; body->setFriction(x); }
	virtual void setMass(float x);
	virtual void setRotation(float rotation);
	virtual void applyCentralImpulse(float x, float y)			{ if (!physInit) return; body->applyCentralImpulse(btVector3(x, y, 0.0f)); }
	virtual btVector3 getPosition()								{ if (!physInit) return btVector3(0,0,0); return body->getCenterOfMassPosition(); }
	virtual void wakeUp()										{ if (!physInit) return; body->activate(true); }

	virtual void collisionCallback(RigidObject * other);
protected:
	int initBullet(string path, int colGroup, int colMask);
	int collisionGroup, collisionMask;
	string collisionPath;

	//btTriangleMesh *mTriMesh;// = new btTriangleMesh();
	btRigidBody * body;
};

#endif