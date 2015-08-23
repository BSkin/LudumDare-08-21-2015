#include "HeightFieldObject.h"

HeightFieldObject::HeightFieldObject(string t, string s, glm::vec2 position, float * heightData, int width, float offsetY, int colGroup, int colMask) : RigidObject(t, s, position, 0.0f, glm::vec2(1.0f, 1.0f), 0.0f, "heightField", colGroup, colMask)
{
	this->heightData = 0;
	initBullet(heightData, width, offsetY, colGroup, colMask);
}

HeightFieldObject::~HeightFieldObject()
{
	delete colShape; colShape = 0;
	delete[] heightData;
}

int HeightFieldObject::initBullet(float * heightData, int width, float offsetY, int colGroup, int colMask)
{
	#define MAX_HEIGHT 1000.0f
	#define MIN_HEIGHT -1000.0f
	this->heightData = heightData;
	this->position.y += offsetY;
	colShape = new btHeightfieldTerrainShape(width, 3, heightData, 1.0f, MIN_HEIGHT, MAX_HEIGHT, 1, PHY_FLOAT, true);

	return RigidObject::initBullet("heightField", colGroup, colMask);
}