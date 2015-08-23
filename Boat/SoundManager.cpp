#include "SoundManager.h"

SoundManager::SoundManager()
{
	device = alcOpenDevice(NULL);
	if (!device) { 
		fprintf(stderr, "Error creating OpenAL Device\n");
	}
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context) {
		fprintf(stderr, "Error creating OpenAL Context\n");
	}

	soundSources = new vector<SoundSource*>();
}

SoundManager::~SoundManager()
{
	while (soundSources->size() > 0) {
		delete soundSources->back();
		soundSources->pop_back();
	}
	delete soundSources;
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

int SoundManager::addSound(SoundSource * soundSource)
{
	if (soundSources->size() == MAX_SOURCES) delete soundSource;
	soundSources->push_back(soundSource);

	return 0;
}

void SoundManager::update()
{
	updateListener();
	updateSounds();
}

void SoundManager::updateListener()
{
	glm::vec3 camPos = activeCamera->getPos();
	glm::vec3 camDir = activeCamera->getLookAtVector();
	glm::vec3 camUp = activeCamera->getUpVector();
	glm::vec3 camVel = camPos - prevCamPos;
	alListener3f(AL_POSITION, camPos.x, camPos.y, camPos.z);
	alListener3f(AL_VELOCITY, camVel.x, camVel.y, camVel.z);
	ALfloat lOr[] = { camDir.x, camDir.y, camDir.z, camUp.x, camUp.y, camUp.z };
	alListenerfv(AL_ORIENTATION, lOr);
	prevCamPos = camPos;
}

bool finished(SoundSource * o)
{
	return (o->getState() == AL_STOPPED);
}

void SoundManager::updateSounds()
{
	for (vector<SoundSource *>::iterator it = soundSources->begin(); it != soundSources->end();) {
		if ((*it)->getState() == AL_STOPPED) {
			delete *it;
			it = soundSources->erase(it);
		}
		else {
			++it;
		}
	}
	
}