#ifndef HEIGHT_FIELD_OBJECT_H
#define HEIGHT_FIELD_OBJECT_H

#include "RigidObject.h"
#include "Bullet\src\BulletCollision\CollisionShapes\btHeightfieldTerrainShape.h"

class HeightFieldObject : public RigidObject
{
public:
	HeightFieldObject(string, string, glm::vec2 position, float * heightField, int width, float offsetY, int colGroup = COL_DEFAULT, int colMask = COL_DEFAULT); //ok for now
	virtual ~HeightFieldObject();
protected:
	int initBullet(float * heightField, int width, float offsetY, int colGroup, int colMask);
	float * heightData;
};

#endif