#include "CollisionObject.h"

CollisionObject::CollisionObject() : RigidObject() {}
CollisionObject::CollisionObject(string t, string s) : RigidObject(t, s) {} //ok for now
CollisionObject::CollisionObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, string path) : RigidObject(t, s, position, rotation, size, 0.0f, path, COL_NONE, COL_NONE) 
{
	initBullet(path);
}

CollisionObject::~CollisionObject() 
{
	dynamicsWorld->removeCollisionObject(ghost);
	delete ghost;
}

int CollisionObject::initBullet(string path)
{
	ghost = new btPairCachingGhostObject(this);
	ghost->setWorldTransform(body->getWorldTransform());
	ghost->setCollisionShape (PhysicsObject::getCollisionShape("sphere10"));
	if (ghost->getCollisionShape() == NULL) return -1;

	ghost->setCollisionFlags(ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->setCollisionFlags(body->getCollisionFlags());// | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	//ghost->setActivationState(DISABLE_DEACTIVATION);
	//body->setActivationState(DISABLE_DEACTIVATION); 
	dynamicsWorld->addCollisionObject(ghost, COL_DEFAULT, COL_DEFAULT);
}

int CollisionObject::update(long elapsedTime)
{
	RigidObject::update(elapsedTime);
	ghost->setWorldTransform(body->getWorldTransform());
	collide();
	return 0;
};

int CollisionObject::render(int i, long totalElapsedTime)
{
	return RigidObject::render(i, totalElapsedTime);
}

void CollisionObject::collide()
{
	
} 

void CollisionObject::collisionCallback(RigidObject * otherObject)
{
	
}