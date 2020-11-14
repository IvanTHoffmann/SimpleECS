#pragma once

#include "SimpleECS/assetManager.hpp"
#include "events.hpp"
#include "portaudio.h"

#define FRAMES_PER_BUFFER 220

static int paSoundCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
static void AudioStreamFinished(void* userData);
int checkAudioError(PaError err);

void initAudioSystem(CB_PARAMS);
void updateAudioSystem(CB_PARAMS);
void stopAudioSystem(CB_PARAMS);
