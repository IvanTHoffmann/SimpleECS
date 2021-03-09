#include "systems/audio.hpp"
#include "appData.hpp"

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

	i16* p;

	bool firstSound = true;
	ent.setPrefab("sound");
	while (ent.next()) {
		ent.copySound();
		CONTINUE_IF_DISABLED(ent.Sound);
		soundInfo = app->assetManager.getSound(ent.Sound->soundIndex);
		p = (i16*)(app->memoryManager.memStart + soundInfo->data);

		for (unsigned long i = 0; i < framesPerBuffer * 2; i+=2) {
			if (firstSound) {
				out[i + 0] = 0;
				out[i + 1] = 0;
			}

			soundInfo = app->assetManager.getSound(ent.Sound->soundIndex);

			ent.Sound->subIndex += ent.Sound->speed;
			ent.Sound->sampleIndex = ent.Sound->sampleIndex + (int)ent.Sound->subIndex;

			if (ent.Sound->sampleIndex > soundInfo->sampleCount / soundInfo->numChannels) { // finished playing sample
				ent.Sound->sampleIndex = (soundInfo->sampleCount / soundInfo->numChannels) * (ent.Sound->speed < 0);
				ent.Sound->subIndex = 0.0f;
				if (!(ent.Sound->flags & COMP_SOUND_LOOP)) {
					ent.Sound->flags |= COMP_DISABLED;
					break;
				}
			}
			else {
				ent.Sound->subIndex = MOD1(ent.Sound->subIndex);
			}

			out[i + 0] += p[ent.Sound->sampleIndex * soundInfo->numChannels] * ent.Sound->leftVolume;
			out[i + 1] += p[ent.Sound->sampleIndex * soundInfo->numChannels + (soundInfo->numChannels > 1)] * ent.Sound->rightVolume;
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
	Entity sound(app), listener(app);

	vec3 diff, right, forward;
	float dist, inv_sqr;
	float leftVolume, rightVolume, forwardVolume;
	float invFocus;

	listener.setPrefab(0);
	while (listener.next(ListenerBit)) {
		listener.refListener();
		listener.refTransform();

		if (listener.Listener->volume <= 0) {
			continue;
		}

		invFocus = 1 - listener.Listener->focus;
		right = vec3(1, 0, 0) * listener.Transform->rot;
		forward = vec3(0, 0, -1) * listener.Transform->rot;

		sound.setPrefab("sound");
		while (sound.next()) {
			sound.refSound();
			sound.refTransform();

			
			diff = sound.Transform->pos - listener.Transform->pos;
			dist = length(diff);
			if (dist) {
				diff /= dist;

				rightVolume = dot(right, diff);
				leftVolume = -rightVolume;
				forwardVolume = dot(forward, diff);
				rightVolume = MAX(0, rightVolume * listener.Listener->focus + invFocus);
				leftVolume = MAX(0, leftVolume * listener.Listener->focus + invFocus);
				forwardVolume = MAX(0, forwardVolume * listener.Listener->focus + invFocus);

				inv_sqr = sound.Sound->fade / (pow(dist, 2) + sound.Sound->fade);

				leftVolume *= inv_sqr * forwardVolume;
				rightVolume *= inv_sqr * forwardVolume;
			}
			else {
				rightVolume = listener.Listener->volume;
				leftVolume = listener.Listener->volume;
			}
			
			sound.Sound->leftVolume = sound.Sound->volume * leftVolume;
			sound.Sound->rightVolume = sound.Sound->volume * rightVolume;
		}
	}
}

void stopAudioSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	if (checkAudioError(Pa_StopStream(appData->audioInfo.stream))) { return; }
	if (checkAudioError(Pa_CloseStream(appData->audioInfo.stream))) { return; }
	Pa_Terminate();
}