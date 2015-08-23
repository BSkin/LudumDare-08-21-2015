#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include "SoundSource.h"
#include "Camera.h"
#include <vector>
#include <algorithm>
#include "glm/glm.hpp"

using std::vector;
using std::remove_if;

class SoundManager
{
public:
	SoundManager();
	~SoundManager();
	int addSound(SoundSource * soundSource);
	void update();

	int getNumSoundSources() { return soundSources->size(); }
private:
	#define activeCamera Camera::getActiveCamera()
	#define MAX_SOURCES 16

	void updateListener();
	void updateSounds();

	vector<SoundSource *> * soundSources;
	glm::vec3 prevCamPos;

	ALCdevice * device;
	ALCcontext * context;
};

#endif