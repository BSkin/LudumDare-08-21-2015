#include "Sound.h"

Sound::Sound()
{
	buffer = -1;
}

Sound::~Sound()
{
	alDeleteBuffers(1, &buffer);
}

struct RIFF_Header {
	char chunkID[4];
	long chunkSize;//size not including chunkSize or chunkID
	char format[4];
};

struct WAVE_Format {
	char subChunkID[4];
	long subChunkSize;
	short audioFormat;
	short numChannels;
	long sampleRate;
	long byteRate;
	short blockAlign;
	short bitsPerSample;
};

struct WAVE_Data {
	char subChunkID[4]; //should contain the word data
	long subChunk2Size; //Stores the size of the data block
};

Sound * Sound::loadSound(string path)
{
	FILE * file = fopen(path.c_str(), "rb");
	if (file == NULL) { fclose(file); return NULL; }

	if (strcmp(path.c_str() + path.size() - 4, ".wav") == 0 || strcmp(path.c_str() + path.size() - 4, ".WAV") == 0)
		return loadWAV(file);

	fclose(file);
	return NULL;
}

Sound * Sound::loadWAV(FILE * file)
{
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char * data;

	// Read in the first chunk into the struct
	fread(&riff_header, sizeof(RIFF_Header), 1, file);

	//check for RIFF and WAVE tag in memeory
	if ((riff_header.chunkID[0] != 'R' ||
		riff_header.chunkID[1] != 'I' ||
		riff_header.chunkID[2] != 'F' ||
		riff_header.chunkID[3] != 'F') ||
		(riff_header.format[0] != 'W' ||
		riff_header.format[1] != 'A' ||
		riff_header.format[2] != 'V' ||
		riff_header.format[3] != 'E')) {
		fclose(file);
		printf("Failed to load WAV: Invalid RIFF Tag\n");
		return NULL;
	}

	//Read in the 2nd chunk for the wave info
	fread(&wave_format, sizeof(WAVE_Format), 1, file);
	//check for fmt tag in memory
	if (wave_format.subChunkID[0] != 'f' ||
		wave_format.subChunkID[1] != 'm' ||
		wave_format.subChunkID[2] != 't' ||
		wave_format.subChunkID[3] != ' ') {
		fclose(file);
		printf("Failed to load WAV: Invalid fmt Tag\n");
		return NULL;
	}

	//check for extra parameters;
	if (wave_format.subChunkSize > 16)
		fseek(file, sizeof(short), SEEK_CUR);

	//Read in the the last byte of data before the sound file
	fread(&wave_data, sizeof(WAVE_Data), 1, file);
	//check for data tag in memory
	if (wave_data.subChunkID[0] != 'd' ||
		wave_data.subChunkID[1] != 'a' ||
		wave_data.subChunkID[2] != 't' ||
		wave_data.subChunkID[3] != 'a') {
		fclose(file);
		printf("Failed to load WAV: Invalid Data Tag\n");
		return NULL;
	}

	//Allocate memory for data
	data = new unsigned char[wave_data.subChunk2Size];

	// Read in the sound data into the soundData variable
	if (!fread(data, wave_data.subChunk2Size, 1, file)) {
		fclose(file);
		delete [] data;
		printf("Failed to load WAV: Invalid RIFF Tag\n");
		return NULL;
	}
	
	Sound * sound = new Sound();

	//Now we set the variables that we passed in with the
	//data from the structs
	sound->size = wave_data.subChunk2Size;
	sound->frequency = wave_format.sampleRate;
	//The format is worked out by looking at the number of
	//channels and the bits per sample.
	if (wave_format.numChannels == 1) {
		if (wave_format.bitsPerSample == 8)
			sound->format = AL_FORMAT_MONO8;
		else if (wave_format.bitsPerSample == 16)
			sound->format = AL_FORMAT_MONO16;
	}
	else if (wave_format.numChannels == 2) {
		if (wave_format.bitsPerSample == 8)
			sound->format = AL_FORMAT_STEREO8;
		else if (wave_format.bitsPerSample == 16)
			sound->format = AL_FORMAT_STEREO16;
	}
	//create our openAL buffer and check for success
	alGenBuffers(1, &sound->buffer);

	//now we put our data into the openAL buffer
	alBufferData(sound->buffer, sound->format, (void*)data, sound->size, sound->frequency);

	delete[] data;
	fclose(file);
	return sound;
}