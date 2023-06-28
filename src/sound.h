#pragma once
#include "extra/bass.h"
#include "framework.h"
#include <map>
#include <string>

// default audio definition
typedef struct {
	std::string name;	// name for easy calling
	std::string path;	// actual path
	bool loop;			// if the sound loops (ex. music)
	bool spatial;		// if it is 3D or not
} SoundFile;

class Audio
{
public: 
	HSAMPLE hSample;	// Handler to store one sample	
	HCHANNEL hSampleChannel; // Handler to store one channel

	Audio(SoundFile soundfile);	// Sample = 0 here
	~Audio(); 					// Free sample (BASS_SampleFree)

	// Play audio and return the channel
	HCHANNEL play(float volume);
	bool pause();
	bool stop();

	// Manager stuff
	// The map!
	static std::map<std::string, Audio*> sAudiosLoaded;

	// Initialize BASS
	static bool Init();
	// Get from manager map
	static Audio* Get(const char* name);
	// Play Manager API
	static HCHANNEL Play(const char* name);
	static HCHANNEL Play(const char* name, float volume);
	static HCHANNEL Play3D(const char* name, Vector3 position);
	// Stop sounds
	static bool Stop(HCHANNEL channel);
	static bool Stop(const char* name);
	static bool Pause(const char* name);
	// Set position of listener
	static bool SetListener(Vector3 position);
	static void SetSoundPosition(Audio* audio, Vector3 position);
};

