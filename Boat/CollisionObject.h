#ifndef COLLISIONOBJECT_H
#define COLLISIONOBJECT_H

#include "RigidObject.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class CollisionObject : public RigidObject
{
public:
	CollisionObject();
	CollisionObject(string, string); //ok for now
	CollisionObject(string, string, glm::vec2 position, float rotation, glm::vec2 size, string path); //ok for now
	virtual ~CollisionObject();

	virtual int render(int i, long elapsedTime);
	virtual int update(long totalElapsedTime);

	virtual void collisionCallback(RigidObject * other);
protected:
	virtual int initBullet(string path);

	virtual void collide();
	btPairCachingGhostObject * ghost;
private:

};

#endif