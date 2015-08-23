#ifndef SOUND_H
#define SOUND_H

#include "OpenAL/al.h"
#include "OpenAL/alc.h"
#include <string>

using std::string;

class Sound
{
public:
	Sound();
	~Sound();

	static Sound * loadSound(string path);
private:
	friend class SoundSource;
	static Sound * loadWAV(FILE * file);
	
	ALuint buffer;
	ALsizei size, frequency;
	ALenum format;
};

#endif