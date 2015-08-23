#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

#include "Sound.h"
#include "glm/glm.hpp"

class SoundSource
{
public:
	SoundSource(Sound * sound);
	SoundSource(Sound * sound, glm::vec3 position);
	SoundSource(Sound * sound, float x, float y, float z);
	~SoundSource();

	void setPitch(float x);
	void setGain(float x);
	void setPosition(glm::vec3 x);
	void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }
	void setVelocity(glm::vec3 x);
	void setVelocity(float x, float y, float z) { setVelocity(glm::vec3(x, y, z)); }
	void setLooping(bool x);

	float getPitch() { return pitch; }
	float getGain() { return gain; }
	glm::vec3 getPosition() { return position; }
	glm::vec3 getVelocity() { return velocity; }
	bool getLooping() { return looping; }
	ALint getState();

private:
	void initSoundSource(Sound * s);

	ALuint source;

	Sound * sound;
	float pitch, gain;
	glm::vec3 position, velocity;
	bool looping;
};

#endif