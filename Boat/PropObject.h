#ifndef PROP_OBJECT_H
#define PROP_OBJECT_H

#include "RigidObject.h"

class PropObject : public RigidObject
{
public:
	PropObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, string path, int colGroup = COL_DEFAULT, int colMask = COL_DEFAULT); //ok for now
	virtual ~PropObject();
protected:
	int initBullet(string path,int colGroup, int colMask);
};

#endif