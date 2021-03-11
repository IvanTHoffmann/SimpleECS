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
	AssetManager* assets = &app->assetManager;
	AppData *appData = (AppData*)app->getData();
	appData->maxDt = 1000 / 20;
	appData->doubleBuf = false;
	appData->fpsSum = 0;

	initRenderSystem(CB_ARGS);
	// TODO: Pre-loading assets shouldn't be required. Fix that.
	
	/*
	assets->getFontIndex("calibri");
	assets->getFontIndex("cour");

	assets->getSoundIndex("music/AnotherOne");
	assets->getSoundIndex("music/PlopXD");
	assets->getSoundIndex("music/SickBeat");
	
	assets->getShaderIndex("simple");
	assets->getShaderIndex("ui");

	assets->getModelIndex("wheel");
	assets->getModelIndex("cube");
	assets->getModelIndex("rect");
	assets->getModelIndex("crate");
	assets->getModelIndex("sphere");
	assets->getModelIndex("map");

	assets->getTextureIndex("boop");
	assets->getTextureIndex("crosshair");
	assets->getTextureIndex("cobble");
	assets->getTextureIndex("crate");


	//*/

	initInputSystem(CB_ARGS);
	initLoaderSystem(CB_ARGS);
	initPhysicsSystem(CB_ARGS);
	initAudioSystem(CB_ARGS);

	app->eventManager.dispatch(&onWindowInit);
}

void onWindowInit(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	app->inputManager.setCallbacks(appData->window);
	glfwSetWindowSize(appData->window, app->inputManager.windowW, app->inputManager.windowH);

	app->eventManager.dispatch(&onUpdate);
}

void onUpdate(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	evnt->dt = min(evnt->dt, appData->maxDt);

	updateUISystem(CB_ARGS);
	updateRenderSystem(CB_ARGS);
	updateInputSystem(CB_ARGS);
	updateCameraSystem(CB_ARGS);
	updateCharacterSystem(CB_ARGS);
	updatePhysicsSystem(CB_ARGS);
	updateAudioSystem(CB_ARGS);

	evnt->deleted = false;
	if (!appData->doubleBuf) {
		//evnt->targetTime = evnt->lastTime + 16 * MILLISECOND; // TODO: set framerate in a more flexible way
	}
}

void onStop(CB_PARAMS) {

	stopAudioSystem(CB_ARGS);

	glfwTerminate();
	app->stop();
}