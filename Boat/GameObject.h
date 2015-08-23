#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RenderObject.h"

class GameObject : public RenderObject
{
public:
	GameObject();
	GameObject(string, string); //ok for now
	GameObject(string, string, glm::vec2 position, float rotation, glm::vec2 size); //ok for now
	virtual ~GameObject();

	bool isAlive() { return alive; }
	glm::vec2 getPosition() { return position; }

	virtual char * serialize();
	virtual void deserialize(string buffer);
	virtual int getSerializedSize() { return 0; }

	virtual int update(long elapsedTime);
	virtual int render(int i, long totalElapsedTime);

	void kill() { alive = false; }

	static void setStatics(list<GameObject *> * goList) { gameObjectList = goList; }

protected:
	bool alive;
private:
	void init();

	static list<GameObject *> * gameObjectList;
	static SoundManager * soundManager;
};

#endif