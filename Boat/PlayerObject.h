#ifndef PLAYER_H
#define PLAYER_H

#include "Character.h"
#include "AirborneSpiderContactCallback.h"
#include "InputManager.h"
#include "Camera.h"

class PlayerObject : public Character
{
public:
	PlayerObject();
	PlayerObject(string, string, glm::vec2 position, glm::vec2 size, float mass); //ok for now
	virtual ~PlayerObject();

	static void setStatics(InputManager * i, Camera * c, list<GameObject*> * w) { inputManager = i; camera = c; webbedMen = w; }

	virtual int update(long elapsedTime);
	virtual int render(int i, long totalElapsedTime);

	virtual void collisionCallback(RigidObject * other);
protected:
	inline bool isKeyPressed(long e) { return inputManager->isKeyPressed(e); }
	inline bool isKeyReleased(long e) { return inputManager->isKeyReleased(e); }
	inline bool isKeyDown(long e) { return inputManager->isKeyDown(e); }
	inline bool isKeyUp(long e) { return inputManager->isKeyUp(e); }
	inline bool isMouseScrollUp() { return inputManager->isMouseScrollUp(); }
	inline bool isMouseScrollDown() { return inputManager->isMouseScrollDown(); }

	void leap();
	glm::vec2 getApproxGroundNormal();

private:
	static InputManager * inputManager;
	static Camera * camera;

	static list<GameObject *> * webbedMen;

	GameObject * getClosestWebbedMan();

	bool carrying;
};

#endif