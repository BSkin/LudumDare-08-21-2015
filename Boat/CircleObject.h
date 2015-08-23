#ifndef CIRCLE_OBJECT_H
#define CIRCLE_OBJECT_H

#include "RigidObject.h"

class CircleObject : public RigidObject
{
public:
	CircleObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, int colGroup = COL_DEFAULT, int colMask = COL_DEFAULT); //ok for now
	virtual ~CircleObject();
protected:
	int initBullet(int colGroup, int colMask);
};

#endif