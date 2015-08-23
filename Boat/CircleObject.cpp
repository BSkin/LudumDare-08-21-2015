#include "CircleObject.h"

CircleObject::CircleObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, int colGroup, int colMask) : RigidObject(t, s, position, rotation, size, mass, "circle", colGroup, colMask)
{
	initBullet(colGroup, colMask);
}

CircleObject::~CircleObject()
{
	delete colShape;
}

int CircleObject::initBullet(int colGroup, int colMask)
{
	colShape = new btCylinderShapeZ(btVector3(size.x / 2.0f, size.y / 2.0f, 0.5f));
	return RigidObject::initBullet("circle", colGroup, colMask);
}