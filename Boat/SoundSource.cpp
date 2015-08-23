#include "SoundSource.h"

SoundSource::SoundSource(Sound * sound)
{
	initSoundSource(sound);
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
}

SoundSource::SoundSource(Sound * sound, glm::vec3 position)
{
	initSoundSource(sound);
	setPosition(position);
}

SoundSource::SoundSource(Sound * sound, float x, float y, float z)
{
	initSoundSource(sound);
	setPosition(x, y, z);
}

SoundSource::~SoundSource()
{
	alDeleteSources(1, &source);
}

void SoundSource::initSoundSource(Sound * s)
{
	alGenSources((ALuint)1, &source);
	sound = s;
	alSourcei(source, AL_BUFFER, sound->buffer);

	setPitch(1.0f);
	setGain(1.0f);
	setPosition(0.0f, 0.0f, 0.0f);
	setVelocity(0.0f, 0.0f, 0.0f);
	setLooping(false);
	alSourcePlay(source);
}

void SoundSource::setPitch(float x) 
{
	pitch = x;
	alSourcef(source, AL_PITCH, pitch);
}
void SoundSource::setGain(float x) 
{ 
	gain = x; 
	alSourcef(source, AL_GAIN, gain);
}
void SoundSource::setPosition(glm::vec3 x) 
{ 
	position = x; 
	alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}
void SoundSource::setVelocity(glm::vec3 x)
{
	velocity = x;
	alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}
void SoundSource::setLooping(bool x) 
{ 
	looping = x; 
	alSourcei(source, AL_LOOPING, looping);
}

ALint SoundSource::getState()
{
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return state;
}