#include "sound.h"
#include <iostream>

#define CHECK_FLAGS(F, NEW_F) ((F) == 0 ? (NEW_F) : (F) & (NEW_F))

#define SOUNDS_N 3

SoundFile SOUNDS[SOUNDS_N] = {
	// {name, path, loop, spatial}
	{"music", "data/sounds/background-music.wav", true, false},
	{"jump", "data/sounds/jump.wav", false, true},
	{"sneak", "data/sounds/sneak.wav", false, true}
};

// without the redefinition below, the linker does not work
std::map<std::string, Audio*> Audio::sAudiosLoaded;

Audio::Audio(SoundFile soundfile) {
	DWORD flags = 0;
	if (soundfile.loop)
		flags = CHECK_FLAGS(flags, BASS_SAMPLE_LOOP);
	if (soundfile.spatial)
		flags = CHECK_FLAGS(flags, BASS_SAMPLE_3D);

	std::cout << " + Sound File Loading: " << soundfile.path << std::endl;

	// flags: BASS_SAMPLE_LOOP, BASS_SAMPLE_3D, ...
	hSample = BASS_SampleLoad(
		false,  					// From internal memory
		soundfile.path.c_str(), 	// Filepath
		0,							// Offset
		0,							// Length
		1,							// Max playbacks
		flags 						// Flags
	);

	if (hSample == 0) { // Error loading
		std::cout << "ERROR LOADING FILE " << soundfile.path << std::endl;
		return;
	}

	// Store sample channel in handler
	hSampleChannel = BASS_SampleGetChannel(hSample, false);
}

Audio::~Audio() {
	Stop(hSampleChannel);
	BASS_ChannelFree(hSampleChannel);
	BASS_SampleFree(hSample);
}

HCHANNEL Audio::play(float volume) {
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, volume);
	BASS_ChannelPlay(hSampleChannel, true);
	return hSampleChannel;
}

bool Audio::Init() {
	// initialize sound device
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		std::cout << "ERROR WITH SOUND DEVICE" << std::endl;
		return false;
	}

	// load predefined samples here
	for (int i = 0; i < SOUNDS_N; i++) {
		SoundFile file = SOUNDS[i];
		Audio::sAudiosLoaded[file.name] = new Audio(file);
	}

	return true;
}

Audio* Audio::Get(const char* name) {
	if (Audio::sAudiosLoaded.count(name) == 0)
		return nullptr;
	return Audio::sAudiosLoaded[name];
}

HCHANNEL Audio::Play(const char* name) {
	// Play channel
	Audio* audio = Get(name);
	if (audio == nullptr) return 0;
	return audio->play(1.0);
}

HCHANNEL Audio::Play3D(const char* name, Vector3 position) {
	Audio* audio = Get(name);
	if (audio == nullptr) return 0;

	// Set audio position in space
	BASS_3DVECTOR bass_position(position.x, position.y, position.z);
	BASS_ChannelSet3DPosition(audio->hSampleChannel, &bass_position, NULL, NULL);

	// Apply changes to 3D system
	BASS_Apply3D();

	// Play sample
	return audio->play(1.0);
}

bool Audio::Stop(HCHANNEL channel) {
	return BASS_ChannelStop(channel);
}

bool Audio::SetListener(Vector3 position) {
	BASS_3DVECTOR bass_position(position.x, position.y, position.z);
	bool result = BASS_Set3DPosition(&bass_position, NULL, NULL, NULL);
	// Apply changes to 3D system
	BASS_Apply3D();
	return result;
}
