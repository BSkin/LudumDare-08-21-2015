#include "PropObject.h"

PropObject::PropObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, string path, int colGroup, int colMask) : RigidObject(t, s, position, rotation, size, mass, path, colGroup, colMask)
{
	initBullet(path, colGroup, colMask);
}

PropObject::~PropObject() {}

int PropObject::initBullet(string path, int colGroup, int colMask)
{
	colShape = PhysicsObject::getCollisionShape(path);
	return RigidObject::initBullet(path, colGroup, colMask);
}