#ifndef MAN_H
#define MAN_H

#include "Character.h"

class Man : public Character
{
public:
	Man();
	Man(string, string, glm::vec2 position, glm::vec2 size, float mass); //ok for now
	virtual ~Man();

	virtual int update(long elapsedTime);
	virtual int render(int i, long totalElapsedTime);

	static void setStatics(RigidObject * s) { spider = s; }

private:
	void detectSpider();

	#define IDLE 1
	#define WALK_LEFT 2
	#define WALK_RIGHT 3
	#define RUNNING 0
	int idleChangeTimer, idleChangeCounter;

	static RigidObject * spider;
};

#endif