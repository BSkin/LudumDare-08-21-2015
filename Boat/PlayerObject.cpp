#include "PlayerObject.h"

InputManager * PlayerObject::inputManager = NULL;
Camera * PlayerObject::camera = NULL;
list<GameObject*> * PlayerObject::webbedMen = NULL;

PlayerObject::PlayerObject(string t, string s, glm::vec2 position, glm::vec2 size, float mass) : Character(t, s, position, size, mass)
{
	collisionType = "spider";
	minUV = glm::vec2(0, 0);
	maxUV = glm::vec2(1, 1);
	delete airborneContactCallback;
	string foo = "asdf";
	airborneContactCallback = new AirborneSpiderContactCallback(*airborneTest, foo, &airborne, &groundNormal);
	carrying = false;
	speed = 2.0f;
}
PlayerObject::~PlayerObject() {}

void PlayerObject::leap()
{
	if (airborne) return;
	glm::vec2 cWP = inputManager->getMouseWorldPosition();
	btVector3 cursorWorldPos = btVector3(cWP.x, cWP.y, 0.0f);
	btVector3 position = body->getWorldTransform().getOrigin();
	if (cursorWorldPos == position) return;
	btVector3 dir = (cursorWorldPos - position).normalized();
	body->setLinearVelocity(dir * 90.0f);
	body->setGravity(btVector3(0, -98.1f*2.0f, 0));

	SoundSource * leapSoundSource = new SoundSource(getSound("Sounds\\leap.wav"));
	leapSoundSource->setPosition(btToGLM3(&getPosition()));
	addSoundSource(leapSoundSource);
}

GameObject * PlayerObject::getClosestWebbedMan()
{
	GameObject * closestWebbedMan = NULL;
	float lowestDistance = 2000.0f;
	for (list<GameObject*>::iterator it = webbedMen->begin(); it != webbedMen->end(); it++) {
		btVector3 pos = getPosition();
		float dist = magnitude(glm::vec2(pos.x(), pos.y()) - (*it)->getPosition());
		if (dist < lowestDistance) {
			lowestDistance = dist;
			closestWebbedMan = *it;
		}
	}
	return closestWebbedMan;
}

int PlayerObject::update(long elapsedTime)
{
	if (isKeyDown(IM_A)) walkLeft();
	if (isKeyDown(IM_D)) walkRight();
	if (isKeyDown(IM_W)) walkUp();
	if (isKeyDown(IM_S)) walkDown();
	if (isKeyPressed(IM_SPACE)) {
		btVector3 pos = getPosition();
		if (carrying) {
			if (!airborne) {
				float webRotation;
				glm::vec2 approxGN = getApproxGroundNormal();
				if (approxGN == glm::vec2(0, 1)) webRotation = 0.0f;
				else if (approxGN == glm::vec2(0, -1)) webRotation = 180.0f;
				else if (approxGN == glm::vec2(1, 0)) webRotation = 270.0f;
				else if (approxGN == glm::vec2(-1, 0)) webRotation = 90.0f;
				webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", glm::vec2(pos.x(), pos.y()) - approxGN*5.0f, webRotation, glm::vec2(10.0f, 10.0f)));
				carrying = false;
			}
		}
		else {
			GameObject * closestWebbedMan = getClosestWebbedMan();
			if (closestWebbedMan != NULL) {
				if (magnitude(closestWebbedMan->getPosition() - glm::vec2(pos.x(), pos.y())) < 10.0f) {
					closestWebbedMan->kill();
					webbedMen->remove(closestWebbedMan);
					carrying = true;
				}
			}
		}
	}

	if (isKeyPressed(IM_M1))	primaryClick();
	if (isKeyReleased(IM_M1))	primaryRelease();
	if (isKeyDown(IM_M1))		primaryDown();
	if (isKeyUp(IM_M1))			primaryUp();

	if (isKeyPressed(IM_M2))	secondaryClick();
	if (isKeyReleased(IM_M2))	secondaryRelease();
	if (isKeyDown(IM_M2))		secondaryDown();
	if (isKeyUp(IM_M2))			secondaryUp();

	if (isMouseScrollUp())
	{
		
	}
	if (isMouseScrollDown())
	{
		
	}
	
	if (isKeyPressed(IM_M2)) leap(); 

	int ret = Character::update(elapsedTime);

	if (airborne) setGravity(0, -98.1*2.0f);
	else {
		btVector3 gravity = btVector3(groundNormal.x(), groundNormal.y(), 0.0f) * -98.1*2.0f;
		setGravity(gravity.x(), gravity.y());
		setGravity(0,0);
	}

	if (body->getLinearVelocity().length() > maxSpeed) body->setLinearVelocity(body->getLinearVelocity().normalized()*maxSpeed);

	//setDirection(camera->getLookAtVector());
	//camera->setPosition(btToGLM3(&getPosition()) + glm::vec3(0,0,-1));
	//camera->setLookAtVector(glm::vec3(0, 0, 1));
	//camera->update(elapsedTime);

	return ret;
}

glm::vec2 PlayerObject::getApproxGroundNormal()
{
	if (groundNormal.x() > 0.707f) return glm::vec2(1, 0);
	if (groundNormal.x() < -0.707f) return glm::vec2(-1, 0);
	if (groundNormal.y() > 0.707f) return glm::vec2(0, 1);
	if (groundNormal.y() < -0.707f) return glm::vec2(0, -1);
	return glm::vec2(0, 0);
}

int PlayerObject::render(int i, long totalElapsedTime)
{
	bool flipUV = false;
	glm::vec2 approxGN = getApproxGroundNormal();
	if (approxGN == glm::vec2(0, 0)) {
		if (direction == glm::vec2(1, 0)) {
			minUV = glm::vec2(0, 0);
			maxUV = glm::vec2(1, 1);
		}
		if (direction == glm::vec2(-1, 0)) {
			minUV = glm::vec2(1, 0);
			maxUV = glm::vec2(0, 1);
		}
	}
	else if (approxGN == glm::vec2(0, 1)) {
		if (direction.x == 1) {
			minUV = glm::vec2(0, 0);
			maxUV = glm::vec2(1, 1);
		}
		if (direction.x == -1) {
			minUV = glm::vec2(1, 0);
			maxUV = glm::vec2(0, 1);
		}
	}
	else if (approxGN == glm::vec2(0, -1)) {
		if (direction.x == 1) {
			minUV = glm::vec2(0, 1);
			maxUV = glm::vec2(1, 0);
		}
		if (direction.x == -1) {
			minUV = glm::vec2(1, 1);
			maxUV = glm::vec2(0, 0);
		}
	}
	else if (approxGN == glm::vec2(1, 0)) {
		if (direction.y == 1) {
			minUV = glm::vec2(0, 0);
			maxUV = glm::vec2(1, 1);
			flipUV = true;
		}
		if (direction.y == -1) {
			minUV = glm::vec2(0, 1);
			maxUV = glm::vec2(1, 0);
			flipUV = true;
		}
	}
	else if (approxGN == glm::vec2(-1, 0)) {
		if (direction.y == 1) {
			minUV = glm::vec2(1, 0);
			maxUV = glm::vec2(0, 1);
			flipUV = true;
		}
		if (direction.y == -1) {
			minUV = glm::vec2(1, 1);
			maxUV = glm::vec2(0, 0);
			flipUV = true;
		}
	}

	shader->activate();
	shader->setUniformf2("minUV", minUV.x, minUV.y);
	shader->setUniformf2("maxUV", maxUV.x, maxUV.y);
	shader->setUniformf1("flipUV", flipUV ? 1.0f : 0.0f);
	shader->setUniformf2("spriteOffset", 0.0f, carrying ? 0.0f : 0.5f);

	return Character::render(i, totalElapsedTime);
}

void PlayerObject::collisionCallback(RigidObject * other)
{
	if (other == NULL) return;
	if (other->getCollisionType() == "man") {
		btVector3 pos = other->getPosition();
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", glm::vec2(pos.x(), pos.y()) - 5.0f, 0.0f, glm::vec2(10.0f, 10.0f)));
		other->kill();

		SoundSource * leapSoundSource = new SoundSource(getSound("Sounds\\wrap.wav"));
		leapSoundSource->setPosition(btToGLM3(&getPosition()));
		addSoundSource(leapSoundSource);
	}
	else if (other->getCollisionType() == "soldier") {
		kill();
	}
}