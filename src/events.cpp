#include "events.hpp"
#include "appData.hpp"
#include "compData.hpp"

#include "SimpleECS/application.hpp"
#include "SimpleECS/entity.hpp"
#include "SimpleECS/componentManager.hpp"

#include "systems/render.hpp"
#include "systems/input.hpp"
#include "systems/camera.hpp"
#include "systems/character.hpp"
#include "systems/loader.hpp"
#include "systems/ui.hpp"
#include "systems/physics.hpp"
#include "systems/audio.hpp"


// TODO: Note that you don't need to declare system callback functions with CB_PARAMS. 
// They are not being stored or referenced by function pointers. It is convenient though.


void onInit(CB_PARAMS) {
	Entity ent(app);

	ComponentManager* comps = &app->componentManager;

	// register classes

	comps->addPrefab("misc", 5, CharacterBit | InputBit | TransformBit | MeshBit | ChildBit);
	comps->addPrefab("player", 1, CharacterBit | InputBit | TransformBit);
	comps->addPrefab("model", 5000, TransformBit | MeshBit, PREFAB_MEM_PACK);
	comps->addPrefab("camera", 1, CameraBit | InputBit | TransformBit | ChildBit);
	comps->addPrefab("text", 3, TextBit | GuiBit | TransformBit);
	comps->addPrefab("gui", 5, MeshBit | GuiBit | TransformBit);
	comps->addPrefab("rigidbody", 20, MeshBit | TransformBit | RigidbodyBit);
	comps->addPrefab("constraint", 20, TransformBit | ConstraintBit);
	comps->addPrefab("sound", 10, TransformBit | SoundBit);
	comps->recalculateMemory();

	// 
	
	AppData *appData = (AppData*)app->getData();
	appData->maxDt = 1000 / 20.0f;
	appData->doubleBuf = true;

	initInputSystem(CB_ARGS);
	initRenderSystem(CB_ARGS);
	initLoaderSystem(CB_ARGS);
	initPhysicsSystem(CB_ARGS);
	initAudioSystem(CB_ARGS);

	app->eventManager.dispatch(&onWindowInit);
}

void onWindowInit(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	setCallbacks(appData->window, &appData->inputInfo);
	glfwSetWindowSize(appData->window, appData->inputInfo.windowW, appData->inputInfo.windowH);

	app->eventManager.dispatch(&onUpdate);
}

void onUpdate(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	evnt->dt = MIN(evnt->dt, appData->maxDt);

	updateUISystem(CB_ARGS);
	updateRenderSystem(CB_ARGS);
	updateInputSystem(CB_ARGS);
	updateCameraSystem(CB_ARGS);
	updateCharacterSystem(CB_ARGS);
	updatePhysicsSystem(CB_ARGS);
	updateAudioSystem(CB_ARGS);

	evnt->deleted = false;
	if (!appData->doubleBuf) {
		evnt->targetTime = evnt->lastTime + 10 * MILLISECOND; // TODO: set framerate in a more flexible way
	}
}

void onStop(CB_PARAMS) {

	stopAudioSystem(CB_ARGS);

	glfwTerminate();
	app->stop();
}