#include "events.hpp"
#include "appData.hpp"
#include "compData.hpp"

#include "SimpleECS/simpleECS.hpp"

#include "systems/render.hpp"
#include "systems/input.hpp"
#include "systems/camera.hpp"
#include "systems/character.hpp"
#include "systems/loader.hpp"
#include "systems/ui.hpp"
#include "systems/physics.hpp"
#include "systems/audio.hpp"


// Note that you don't need to declare system callback functions with CB_PARAMS. 
// They are not being stored or referenced by function pointers. It is convenient though.


void onInit(CB_PARAMS) {

	Entity ent(app);

	ComponentManager* comps = &app->componentManager;
	AssetManager* assets = &app->assetManager;
	AppData *appData = (AppData*)app->getData();
	appData->maxDt = 1000 / 20.0f;
	appData->doubleBuf = false;
	appData->fpsSum = 0;

	initRenderSystem(CB_ARGS);
	// Pre-loading assets isn't necessary, but we should keep track of the assets we use. We might want to free gpu memory between levels.
	//*
	std::cout << "simple\n";
	assets->getShaderIndex("simple");
	std::cout << "ui\n";
	assets->getShaderIndex("ui");

	std::cout << "wheel\n";
	assets->getModelIndex("wheel");
	std::cout << "cube\n";
	assets->getModelIndex("cube");
	std::cout << "rect\n";
	assets->getModelIndex("rect");
	std::cout << "crate\n";
	assets->getModelIndex("crate");
	std::cout << "sphere\n";
	assets->getModelIndex("sphere");
	std::cout << "map\n";
	assets->getModelIndex("map");

	std::cout << "boop\n";
	assets->getTextureIndex("boop");
	std::cout << "crosshair\n";
	assets->getTextureIndex("crosshair");
	std::cout << "cobble\n";
	assets->getTextureIndex("cobble");
	std::cout << "crate\n";
	assets->getTextureIndex("crate");

	std::cout << "calibri\n";
	assets->getFontIndex("calibri");
	//std::cout << "cour\n";
	//assets->getFontIndex("cour");

	std::cout << "anotherone\n";
	assets->getSoundIndex("music/AnotherOne");
	std::cout << "plopxd\n";
	assets->getSoundIndex("music/PlopXD");
	std::cout << "sickbeat\n";
	assets->getSoundIndex("music/SickBeat");

	//*/

	// register classes

	comps->addPrefab("misc", 5, CharacterBit | InputBit | TransformBit | MeshBit | ChildBit);
	comps->addPrefab("player", 1, CharacterBit | InputBit | TransformBit);
	comps->addPrefab("model", 10, TransformBit | MeshBit, PREFAB_MEM_PACK);
	comps->addPrefab("camera", 1, CameraBit | InputBit | TransformBit | ChildBit | ListenerBit);
	comps->addPrefab("text", 3, TextBit | GuiBit | TransformBit);
	comps->addPrefab("gui", 5, MeshBit | GuiBit | TransformBit);
	comps->addPrefab("rigidbody", 20, MeshBit | TransformBit | RigidbodyBit);
	comps->addPrefab("constraint", 20, TransformBit | ConstraintBit);
	comps->addPrefab("sound", 10, TransformBit | SoundBit | MeshBit);
	comps->recalculateMemory();

	// 
	

	initInputSystem(CB_ARGS);
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
		evnt->targetTime = evnt->lastTime + 16 * MILLISECOND; // TODO: set framerate in a more flexible way
	}
}

void onStop(CB_PARAMS) {

	stopAudioSystem(CB_ARGS);

	glfwTerminate();
	app->stop();
}