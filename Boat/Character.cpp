#include "Character.h"

Character::Character(string t, string s, glm::vec2 position, glm::vec2 size, float mass) : CircleObject(t, s, position, 0.0f, size, mass)
{
	airborne = false;
	alive = true;
	body->setFriction(0.5f);
	setMass(5.0f);
	floorDist = 0.1f;
	body->setAngularFactor(btVector3(0, 0, 0));
	createAirborneTest();
	speed = 1.0f;
	direction = glm::vec2(1, 0);

	string foo = "asdf";
	airborneContactCallback = new AirborneContactCallback(*airborneTest, foo, &airborne, &groundNormal);
}

Character::~Character() 
{
	delete airborneContactCallback;
	delete airborneTest;
	delete airborneColShape;
}

void Character::createAirborneTest()
{
	airborneColShape = new btCylinderShapeZ(btVector3(size.x / 2.0f + 0.2f, size.y / 2.0f + 0.2f, 0.1f));
	
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0,0,0);
	airborneColShape->calculateLocalInertia(mass, localInertia);
		
	startTransform.setOrigin(btVector3(body->getWorldTransform().getOrigin()));

	btDefaultMotionState * myMotionState = new btDefaultMotionState(startTransform);
	airborneTest = new btRigidBody(0.0f, myMotionState, airborneColShape, localInertia, this);
	delete myMotionState;
}

int Character::update(long elapsedTime)
{	
	body->activate(true);
	double e = elapsedTime*0.001;
	
	airborne = true;
	testGround();
	
	if (airborne) {
		if (magSqr(walkDirection) != 0.0 && walkDirection != glm::vec2(0,0))
			airwalk(walkDirection * (float)speed*0.01f);
	}
	else {
		if (walkDirection != glm::vec2(0,0))
			walk(glm::normalize(walkDirection) * (float)speed);
		if (walkDirection == glm::vec2(0,0)) {
			if (magSqr(&body->getLinearVelocity()) < maxSpeed*100.0f) {
				stopWalking();
				//setGravity(0,0,0);
			}
		}
	}
	
	if (walkDirection != glm::vec2(0,0)) direction = walkDirection;
	walkDirection = glm::vec2(0,0);

	int ret = RigidObject::update(elapsedTime);

	return ret;
}

int Character::render(int i, long totalElapsedTime) 
{
	return RigidObject::render(i, totalElapsedTime);
}

void Character::walkRight() { walkDirection += glm::vec2(1, 0); }
void Character::walkLeft() { walkDirection += glm::vec2(-1, 0); }
void Character::walkUp() { walkDirection += glm::vec2(0, 1); }
void Character::walkDown() { walkDirection += glm::vec2(0, -1); }

float changeSpeed(float input, float target, float dif) {
	if (target == input) return target;
	if (abs(target-input) < dif) return target;
	float x = (target-input)/abs(target-input)*dif;
	return input + x;
}

btVector3 changeVelocity(btVector3 * currentVel, btVector3 * targetVel, float dif) {
	btVector3 newVel;

	newVel.setX(changeSpeed(currentVel->x(), targetVel->x(), dif));
	newVel.setY(changeSpeed(currentVel->y(), targetVel->y(), dif));
	newVel.setZ(0.0f);
	return newVel;
}

void Character::walk(float x, float y)
{
	if (magnitude(&body->getLinearVelocity()) <= maxSpeed)
	{
		/*if (groundNormal.x() == 0 && groundNormal.y() == 0) return;
		btVector3 sideV = (btVector3(walkDirection.x, walkDirection.y, 0.0f).cross(btVector3(0,-1,0))).normalized();
		btVector3 forwardV = (sideV.cross(groundNormal)).normalized();

		btVector3 forward = forwardV * 10.0f*magnitude(&btVector3(x,y,0.0f));*/
		btVector3 v = body->getLinearVelocity();
		btVector3 forward = 20 * btVector3(walkDirection.x, walkDirection.y, 0.0f);

		body->setLinearVelocity(changeVelocity(&v, &forward, 5.0f));
		//body->setLinearVelocity(10.0f*forward);
	}
}

void Character::stopWalking() { 
	btVector3 v = body->getLinearVelocity();
	body->setLinearVelocity(changeVelocity(&v, &btVector3(0,0,0), 5.0f));
}

void Character::airwalk(float x, float y)
{
	body->applyCentralImpulse(10.0f*btVector3(x,y, 0.0f));
	return;
}

void Character::jump()
{
	if (airborne) return;
	body->setLinearVelocity(body->getLinearVelocity() + btVector3(0,70.0f,0));
	body->translate(btVector3(0,0.5f,0));
}

void Character::testGround()
{
	airborneTest->setWorldTransform(body->getWorldTransform());
	//airborneContactCallback(*airborneTest, foo, &airborne, &groundNormal);
	dynamicsWorld->updateAabbs();
	dynamicsWorld->contactTest(airborneTest, *airborneContactCallback);
}

void Character::setDirection(glm::vec2 direction) {
	this->direction = glm::normalize(direction);
}