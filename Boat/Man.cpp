#include "Man.h"

RigidObject * Man::spider = NULL;

Man::Man(string t, string s, glm::vec2 position, glm::vec2 size, float mass) : Character(t, s, position, size, mass)
{
	collisionType = "man";
	state = IDLE;
	idleChangeTimer = idleChangeCounter = 0;
}

Man::~Man() {}

int Man::update(long elapsedTime)
{
	detectSpider();

	if (state != RUNNING) {
		idleChangeCounter += elapsedTime;
		if (idleChangeCounter >= idleChangeTimer) {
			state = rand() % 3 + 1;
			idleChangeCounter = 0;
			idleChangeTimer = (rand() % 4500) + 500;
		}

		if (state == WALK_LEFT) walkDirection = glm::vec2(-1, 0);
		else if (state == WALK_RIGHT) walkDirection = glm::vec2(1, 0);
	}
	else {
		speed = 2.0f;
		btVector3 pos = spider->getPosition() - getPosition();
		if (pos.x() <= 0) walkDirection = glm::vec2(1, 0);
		else walkDirection = glm::vec2(-1, 0);
	}

	return Character::update(elapsedTime);
}

int Man::render(int i, long totalElapsedTime)
{
	if (direction.x == 1) {
		minUV = glm::vec2(0, 0);
		maxUV = glm::vec2(1, 1);
	}
	if (direction.x == -1) {
		minUV = glm::vec2(1, 0);
		maxUV = glm::vec2(0, 1);
	}

	shader->activate();
	shader->setUniformf2("minUV", minUV.x, minUV.y);
	shader->setUniformf2("maxUV", maxUV.x, maxUV.y);
	shader->setUniformf1("flipUV", 0.0f);
	shader->setUniformf2("spriteOffset", 0.0f, state == RUNNING ? 0.0f : 0.5f);

	return Character::render(i, totalElapsedTime);
}

void Man::detectSpider()
{
	btVector3 pos = spider->getPosition() - getPosition();
	if (pos == btVector3(0, 0, 0)) return;
	btVector3 dir = pos.normalized();
	float theta = acos(glm::dot(direction, glm::vec2(dir.x(), dir.y())));

	btVector3 endPosition = getPosition() + dir * 50.0f;
	btCollisionWorld::ClosestRayResultCallback RayCallback(getPosition(), endPosition);
	dynamicsWorld->rayTest(getPosition(), endPosition, RayCallback);

	if (RayCallback.hasHit()) {
		const btCollisionObject * test = RayCallback.m_collisionObject;
		RigidObject * temp = (RigidObject*)test->getOwner();
		if ((temp->getCollisionType() == "spider" && theta < M_PI / 8.0f) ||
			((temp->getCollisionType() == "man" || temp->getCollisionType() == "soldier") && ((Character *)test->getOwner())->getState() == 0)) {
			if (state != RUNNING) {
				SoundSource * screamSoundSource = new SoundSource(getSound("Sounds\\scream.wav"));
				screamSoundSource->setPosition(btToGLM3(&getPosition()));
				addSoundSource(screamSoundSource);
			}
			state = RUNNING;
		}
	}
}