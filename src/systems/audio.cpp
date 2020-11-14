#include "systems/audio.hpp"
#include "appData.hpp"

#include "SimpleECS/application.hpp"
#include "SimpleECS/entity.hpp"
#include "SimpleECS/componentManager.hpp"
#include "SimpleECS/util.hpp"


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

	u32 nextIndex;

	bool firstSound = true;
	ent.set(app->componentManager.getPrefabID("sound"));
	while (ent.next()) {
		ent.copySound();
		CONTINUE_IF_DISABLED(ent.Sound);

		for (unsigned long i = 0; i < framesPerBuffer * 2; i+=2) {
			if (firstSound) {
				out[i] = 0;
				out[i + 1] = 0;
			}

			soundInfo = app->assetManager.getSound(ent.Sound->soundIndex);

			ent.Sound->subIndex += ent.Sound->speed * SOUND_SPEED_PRECISION;
			nextIndex = ent.Sound->sampleIndex + ent.Sound->subIndex / SOUND_SPEED_PRECISION;

			//PRINT_VAR(nextIndex, d);

			if (nextIndex > soundInfo->sampleCount / 2) { // finished playing sample
				ent.Sound->sampleIndex = (soundInfo->sampleCount / 2) * (ent.Sound->speed < 0);
				ent.Sound->subIndex = 0;
				if (!(ent.Sound->flags & SOUND_LOOP)) {
					ent.Sound->flags |= COMP_DISABLED;
					break;
				}
			}
			else {
				ent.Sound->sampleIndex = nextIndex;
				ent.Sound->subIndex %= SOUND_SPEED_PRECISION;
				//ent.Sound->subIndex -= SOUND_SPEED_PRECISION * (ent.Sound->speed < 0); // fixes negative modulo

				out[i + 0] += soundInfo->data[ent.Sound->sampleIndex * 2] * ent.Sound->leftVolume;
				out[i + 1] += soundInfo->data[ent.Sound->sampleIndex * 2 + 1] * ent.Sound->rightVolume;
			}
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

void updateAudioSystem(CB_PARAMS) {
	Entity ent(app);
	ent.set(app->componentManager.getPrefabID("sound"));
	while (ent.next()) {
		ent.refSound();
		ent.Sound->speed += .05 * evnt->dt;
		if (ent.Sound->speed > 1.5) {
			ent.Sound->speed = -1.5;
		}
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(evnt->lastTime.time_since_epoch()).count();
		float time = ms / 1000.0;
		ent.Sound->leftVolume = ent.Sound->volume * abs(cos(time)) * .4 + .2;
		ent.Sound->rightVolume = ent.Sound->volume * abs(sin(time)) * .4 + .2;
	}
}

void stopAudioSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	if (checkAudioError(Pa_StopStream(appData->audioInfo.stream))) { return; }
	if (checkAudioError(Pa_CloseStream(appData->audioInfo.stream))) { return; }
	Pa_Terminate();
}