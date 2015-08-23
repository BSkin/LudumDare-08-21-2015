#ifndef BOX_OBJECT_H
#define	BOX_OBJECT_H

#include "RigidObject.h"

class BoxObject : public RigidObject
{
public:
	BoxObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, int colGroup = COL_DEFAULT, int colMask = COL_DEFAULT); //ok for now
	virtual ~BoxObject();
protected:
	int initBullet(int colGroup, int colMask);
};

#endif