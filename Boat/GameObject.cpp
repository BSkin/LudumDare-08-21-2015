#include "GameObject.h"

glm::mat4 * GameObject::viewMatrix = NULL;
glm::mat4 * GameObject::projMatrix = NULL;
list<GameObject *> * GameObject::gameObjectList = NULL;

void GameObject::init()
{
	alive = true;
	gameObjectList->push_back(this);
}

GameObject::GameObject()
{
	init();
}

GameObject::GameObject(string tPath, string sPath) : RenderObject(tPath, sPath)
{
	init();
}

GameObject::GameObject(string tPath, string sPath, glm::vec2 position, float rotation, glm::vec2 size) : RenderObject(tPath, sPath, position, rotation, size)
{
	init();
}

GameObject::~GameObject()
{
	
}

char * GameObject::serialize()
{
	return NULL;
}

void GameObject::deserialize(string buffer)
{

}

int GameObject::update(long elapsedTime)
{
	return RenderObject::update(elapsedTime);
}

int GameObject::render(int i, long totalElapsedTime)
{
	return RenderObject::render(i, totalElapsedTime);
}