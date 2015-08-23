#ifndef SOLDIER_H
#define SOLDIER_H

#include "Character.h"

class Soldier : public Character
{
public:
	Soldier();
	Soldier(string, string, glm::vec2 position, glm::vec2 size, float mass); //ok for now
	virtual ~Soldier();

	virtual int update(long elapsedTime);
	virtual int render(int i, long totalElapsedTime);

	static void setStatics(RigidObject * s) { spider = s; }

private:
	void detectSpider();

	#define IDLE 1
	#define ATTACKING 0
	int idleChangeTimer, idleChangeCounter;

	static RigidObject * spider;
};

#endif