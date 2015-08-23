#ifndef EVENT_SERIALIZER_H
#define EVENT_SERIALIZER_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	

#include "DataBuffer.h"

#include <string>
using std::string;

class EventSerializer
{
public:
	static DataBuffer * SpawnFireball(glm::vec3 position, glm::vec3 direction, int ownerIndex = -1);
	static DataBuffer * DamagePlayer(float damage, int targetIndex, int senderIndex = -1);
	static DataBuffer * KillPlayer(int targetIndex, int senderIndex = -1);
	static DataBuffer * SpawnPlayer(glm::vec3 position, glm::vec3 direction, int targetIndex);
	static DataBuffer * ChangeMap(string mapName);
};

#endif