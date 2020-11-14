#include "systems/audio.hpp"
#include "appData.hpp"

#include "SimpleECS/application.hpp"
#include "SimpleECS/entity.hpp"


// TODO: Support changing audio devices at runtime

// PortAudio functions

static int paSoundCallback(
	const void* inputBuffer, 
	void* outputBuffer,
	unsigned long framesPerBuffer, 
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags, 
	void* userData) {

	Application* app = (Application*)userData;
	Entity ent(app);
	SoundInfo* soundInfo;

	int16_t* out = (int16_t*)outputBuffer;

	bool firstSound = true;
	ent.set(app->componentManager.getPrefabID("sound"));
	while (ent.next()) {
		ent.refSound();
		for (unsigned long i = 0; i < framesPerBuffer * 2; i+=2) {
			if (firstSound) {
				out[i] = 0;
				out[i + 1] = 0;
			}

			soundInfo = app->assetManager.getSound(ent.Sound->soundIndex);

			if (ent.Sound->sampleIndex >= soundInfo->sampleCount) {
				std::cout << "repeat\n";
				//ent.destroy();
				//break;
				ent.Sound->sampleIndex = 0;
			}

			out[i+0] += soundInfo->data[ent.Sound->sampleIndex++] * ent.Sound->volume;
			out[i+1] += soundInfo->data[ent.Sound->sampleIndex++] * ent.Sound->volume;
		}
		ent.syncSound();
		firstSound = false;
	}
	return paContinue;
}

static void AudioStreamFinished(void* userData) {
	std::cout << "INFO: Audio stream finished\n";
}

int checkAudioError(PaError err) {
	if (err == paNoError) {
		return 0;
	}
	Pa_Terminate();
	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return 1;
}

// Events

void initAudioSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	appData->audioInfo.sampleRate = 44100;
	appData->audioInfo.numChannels = 2;

	if (checkAudioError(Pa_Initialize())) { return; }

	appData->audioInfo.outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (appData->audioInfo.outputParameters.device == paNoDevice) {
		fprintf(stderr, "Error: No default output device.\n");
		return;
	}
	appData->audioInfo.outputParameters.channelCount = appData->audioInfo.numChannels;       /* stereo output */
	appData->audioInfo.outputParameters.sampleFormat = paInt16; /* 32 bit floating point output */
	appData->audioInfo.outputParameters.suggestedLatency = Pa_GetDeviceInfo(appData->audioInfo.outputParameters.device)->defaultLowOutputLatency;
	appData->audioInfo.outputParameters.hostApiSpecificStreamInfo = NULL;

	if (checkAudioError(Pa_OpenStream(
		&appData->audioInfo.stream,
		NULL, // no input 
		&(appData->audioInfo.outputParameters),
		appData->audioInfo.sampleRate,
		FRAMES_PER_BUFFER,
		paClipOff,
		paSoundCallback,
		app))) {
		return;
	}

	if (checkAudioError(Pa_SetStreamFinishedCallback(appData->audioInfo.stream, &AudioStreamFinished))) { return; }
	if (checkAudioError(Pa_StartStream(appData->audioInfo.stream))) { return; }
}

void stopAudioSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	if (checkAudioError(Pa_StopStream(appData->audioInfo.stream))) { return; }
	if (checkAudioError(Pa_CloseStream(appData->audioInfo.stream))) { return; }
	Pa_Terminate();
}