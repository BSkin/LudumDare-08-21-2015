#include "BoxObject.h"

BoxObject::BoxObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, int colGroup, int colMask) : RigidObject(t, s, position, rotation, size, mass, "box", colGroup, colMask)
{
	initBullet(colGroup, colMask);
}

BoxObject::~BoxObject() 
{
	delete colShape;
}

int BoxObject::initBullet(int colGroup, int colMask)
{
	colShape = new btBoxShape(btVector3(size.x / 2.0f, size.y / 2.0f, 0.5f));
	return RigidObject::initBullet("box", colGroup, colMask);
}