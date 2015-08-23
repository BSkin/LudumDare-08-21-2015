#ifndef CAMERA_H
#define CAMERA_H

#include <WinSock2.h>
#include <Windows.h>
#include <math.h>
#include "OpenGL/GLU.h"
#include "OpenGL/GL.h"

#include "glm/glm.hpp"						//glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm/gtc/matrix_transform.hpp"		//glm::translate, glm::rotate, glm::scale
#include "glm/gtc/type_ptr.hpp"				//glm::make_mat4
#include "glm/gtx/rotate_vector.hpp"
#include "Bullet\src\btBulletDynamicsCommon.h"

#include "Utilities.h"
using namespace Utilities;

class Camera
{
public:
	Camera();
	~Camera();
	static Camera * getActiveCamera() { return activeCamera; }
	int activate();

	int calculateView(glm::mat4 * view);
	int calculateProjection(glm::mat4 * proj);
	int calculateOrthographicProjection(glm::mat4 * proj, float width, float height);

	void move(glm::vec3 value) { position += value; }
	void move(float x, float y, float z) { move(glm::vec3(x, y, z)); } 
	void move(float x, float z) { move(x, 0, z); }
	void moveForward(float speed) { position += lookAtVector * speed; }
	void moveBack(float speed) { position -= lookAtVector * speed; }
	void moveLeft(float speed) { position += leftVector * speed; }
	void moveRight(float speed) { position -= leftVector * speed; }
	void moveUp(float speed) { position += upVector * speed; }
	void moveDown(float speed) { position -= upVector * speed; }
	void setPosition(glm::vec3 value) { position = value; }
	void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }
	void zeroVelocity() { velocity = glm::vec3(0,0,0); }
	void turn(long x, long y);
	void turn(POINT v) { turn(v.x, v.y); }
	void roll(float z);
	void toggleFreeRotation();
	void setFreeRotation(bool x) { if (freeRotation != x) toggleFreeRotation(); }
	void setLookAtVector(glm::vec3 x) { 
		lookAtVector = x; 
		int y = 2;
	}
	void setUpVector(glm::vec3 x) { upVector = x; }

	glm::vec3 getPos()			{ return position; }
	glm::vec3 getAngle()		{ return glm::vec3(xAngle, yAngle, zAngle); }
	glm::vec3 getLookAtVector()	{ return lookAtVector; }
	glm::vec3 getUpVector()		{ return upVector; }
	glm::vec3 getLeftVector()	{ return leftVector; }

	void update(long dt);
private:
	inline void calculateLeftVector() { leftVector = glm::normalize(glm::cross(upVector, lookAtVector)); }
	static Camera * activeCamera;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 lookAtVector;
	glm::vec3 upVector;
	glm::vec3 leftVector;

	float vFOV, aspectRatio, nearPlane, farPlane;
	bool freeRotation;
	float xAngle, yAngle, zAngle;
};

#endif