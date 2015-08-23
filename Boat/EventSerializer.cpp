#include "EventSerializer.h"

DataBuffer * EventSerializer::SpawnFireball(glm::vec3 position, glm::vec3 direction, int ownerIndex)
{
	//create fireball of owner at position, direction
	return NULL;
}

DataBuffer * EventSerializer::DamagePlayer(float damage, int targetIndex, int senderIndex)
{
	//target received x damage from sender
	return NULL;
}

DataBuffer * EventSerializer::KillPlayer(int targetIndex, int senderIndex)
{
	//sender killed target
	return NULL;
}

DataBuffer * EventSerializer::SpawnPlayer(glm::vec3 position, glm::vec3 direction, int targetIndex)
{
	//spawn target at position, direction
	return NULL;
}