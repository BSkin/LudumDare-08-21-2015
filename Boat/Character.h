#ifndef CHARACTER_H
#define CHARACTER_H

#include "glm/gtx/rotate_vector.hpp"

#include "CircleObject.h"
#include "AirborneContactCallback.h"

#define maxSpeed 120.0f

class Spell;

class Character : public CircleObject
{
public:
	Character(string, string, glm::vec2 position, glm::vec2 size, float mass); //ok for now
	virtual ~Character();

	virtual int update(long elapsedTime);
	virtual int render(int i, long totalElapsedTime);

	bool getAir() { return getAirborne(); }
	glm::vec3 getGroundNormal() { return btToGLM3(&groundNormal); }
	int getState() { return state; }
protected:
	void runLeft();
	void runRight();

	void walkUp();
	void walkDown();
	void walkLeft();
	void walkRight();
	
	void setDirection(glm::vec2 direction);

	void primaryClick()		{}
	void primaryDown()		{}
	void primaryUp()		{}
	void primaryRelease()	{}

	void secondaryClick()	{}
	void secondaryDown()	{}
	void secondaryUp()		{}
	void secondaryRelease() {}

	void jump();

	btCollisionWorld::ContactResultCallback * airborneContactCallback;
	btRigidBody * airborneTest;
	bool airborne;
	btVector3 groundNormal;
	glm::vec2 direction;
	glm::vec2 walkDirection;
	float speed;
	int state;

	glm::vec2 minUV;
	glm::vec2 maxUV;
private:
	void walk(float x, float y);
	void walk(glm::vec2 val) { walk(val.x, val.y); }
	void airwalk(float x, float y);
	void airwalk(glm::vec2 val) { airwalk(val.x, val.y); }

	void stopWalking();
	bool getAirborne() { return airborne; }
	void testGround();
		
	void createAirborneTest();
	btRigidBody * airborneRigidBody;
	btCollisionShape * airborneColShape;

	bool sliding;
	float floorDist;
	btVector3 groundPos;
};

#endif