#include "systems/loader.hpp"
#include "systems/physics.hpp"
#include "constants.hpp"
#include "appData.hpp"

void initLoaderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	AssetManager* assets = &app->assetManager;
	ComponentManager* comps = &app->componentManager;
	InputManager* inp = &app->inputManager;
	Entity* ent = appData->ent;

	// strings can be replaced with their indices when getting input, and actions dont necessarily need to be assigned names
	inp->newAction("MOVE_LEFT");
	inp->newAction("MOVE_RIGHT");
	inp->newAction("MOVE_FORWARD");
	inp->newAction("MOVE_BACKWARD");
	inp->newAction("LOOK_LEFT");
	inp->newAction("LOOK_RIGHT");
	inp->newAction("LOOK_UP");
	inp->newAction("LOOK_DOWN");
	inp->newAction("JUMP");
	inp->newAction("QUIT");
	inp->newAction("FOCUS");

	// set key bindings
	inp->bindInput("MOVE_FORWARD",	DEVICE_KEYBOARD, KEY_W);
	inp->bindInput("MOVE_LEFT",		DEVICE_KEYBOARD, KEY_A);
	inp->bindInput("MOVE_BACKWARD",	DEVICE_KEYBOARD, KEY_S);
	inp->bindInput("MOVE_RIGHT",	DEVICE_KEYBOARD, KEY_D);
	float mouseSensitivity = .003f;
	inp->bindInput("LOOK_LEFT",		DEVICE_MOUSE, 2, 0.0f, -1.0f, mouseSensitivity);
	inp->bindInput("LOOK_RIGHT",	DEVICE_MOUSE, 2, 0.0f, 1.0f, mouseSensitivity);
	inp->bindInput("LOOK_UP",		DEVICE_MOUSE, 3, 0.0f, 1.0f, mouseSensitivity);
	inp->bindInput("LOOK_DOWN",		DEVICE_MOUSE, 3, 0.0f, -1.0f, mouseSensitivity);

	inp->bindInput("JUMP",			DEVICE_KEYBOARD, KEY_SPACE);
	inp->bindInput("FOCUS",			DEVICE_KEYBOARD, KEY_ESCAPE);
	inp->bindInput("QUIT",			DEVICE_KEYBOARD, KEY_DELETE);

	float gpMoveSensitivity = 1.25f;
	float gpLookSensitivity = 5.0f;
	float deadzone = .2f;
	inp->bindInput("MOVE_FORWARD",	0, AXIS_LY, deadzone, -1.0f, gpMoveSensitivity);
	inp->bindInput("MOVE_LEFT",		0, AXIS_LX, deadzone, -1.0f, gpMoveSensitivity);
	inp->bindInput("MOVE_BACKWARD",	0, AXIS_LY, deadzone,  1.0f, gpMoveSensitivity);
	inp->bindInput("MOVE_RIGHT",	0, AXIS_LX, deadzone,  1.0f, gpMoveSensitivity);
	inp->bindInput("MOVE_FORWARD",	0, BUTTON_UP);
	inp->bindInput("MOVE_LEFT",		0, BUTTON_LEFT);
	inp->bindInput("MOVE_BACKWARD",	0, BUTTON_DOWN);
	inp->bindInput("MOVE_RIGHT",	0, BUTTON_RIGHT);
	inp->bindInput("LOOK_UP",		0, AXIS_RY, deadzone,  1.0f, gpLookSensitivity, true);
	inp->bindInput("LOOK_LEFT",		0, AXIS_RX, deadzone, -1.0f, gpLookSensitivity, true);
	inp->bindInput("LOOK_DOWN",		0, AXIS_RY, deadzone, -1.0f, gpLookSensitivity, true);
	inp->bindInput("LOOK_RIGHT",	0, AXIS_RX, deadzone,  1.0f, gpLookSensitivity, true);

	inp->bindInput("JUMP",			0, BUTTON_A);
	inp->bindInput("FOCUS",			0, BUTTON_SELECT);
	inp->bindInput("QUIT",			0, BUTTON_START);
	
	assets->loadFbo("fbo-copy", 1920, 1080);
	assets->loadFbo("fbo-default", 1920, 1080);
	assets->loadFbo("fbo-split", 400, 400);

	//* music
	ent->create("sound");

	GET(ent, Transform)->pos = vec3(0, 4, -15);
	GET(ent, Transform)->scale = vec3(1, 1, 1);
	GET(ent, Sound)->flags |= COMP_SOUND_LOOP;
	GET(ent, Sound)->soundIndex = assets->getSoundIndex("music/AnotherOne");
	GET(ent, Sound)->volume = .8f;
	GET(ent, Sound)->sampleIndex = 0;
	GET(ent, Sound)->subIndex = 0;
	GET(ent, Sound)->speed = 1;
	GET(ent, Sound)->fade = 50;
	GET(ent, Mesh)->meshId = assets->getModelIndex("sphere");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("crash");
	GET(ent, Mesh)->tiling = vec2(1);

	//* music
	ent->create("sound");
	GET(ent, Transform)->pos = vec3(20, 4, 10);
	GET(ent, Transform)->scale = vec3(.5);
	GET(ent, Sound)->flags |= COMP_SOUND_LOOP;
	GET(ent, Sound)->soundIndex = assets->getSoundIndex("music/SickBeat");
	GET(ent, Sound)->volume = .8f;
	GET(ent, Sound)->sampleIndex = 0;
	GET(ent, Sound)->subIndex = 0;
	GET(ent, Sound)->speed = 1;
	GET(ent, Sound)->fade = 50;
	GET(ent, Mesh)->meshId = assets->getModelIndex("jukebox");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("jukebox");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);

	//* music
	ent->create("sound");
	GET(ent, Transform)->pos = vec3(-20, 4, 10);
	GET(ent, Transform)->scale = vec3(.5);
	GET(ent, Sound)->flags |= COMP_SOUND_LOOP;
	GET(ent, Sound)->soundIndex = assets->getSoundIndex("music/PlopXD");
	GET(ent, Sound)->volume = .8f;
	GET(ent, Sound)->sampleIndex = 0;
	GET(ent, Sound)->subIndex = 0;
	GET(ent, Sound)->speed = 1;
	GET(ent, Sound)->fade = 50;
	GET(ent, Mesh)->meshId = assets->getModelIndex("jukebox");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("jukebox");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	//*/

	//* screen
	ent->create("model");
	GET(ent, Transform)->pos = vec3(-5, 0, -15);
	GET(ent, Transform)->scale = vec3(10, 5.2, 1);
	GET(ent, Mesh)->meshId = assets->getModelIndex("rect");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("fbo-copy");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	//*/ 

	//* Ground Collider

	ent->create("constraint");
	GET(ent, Constraint)->type = CONSTRAINT_PLANE;
	GET(ent, Constraint)->restitution = .9f;
	GET(ent, Constraint)->staticFriction = .7f;
	GET(ent, Constraint)->dynamicFriction = .2f;
	GET(ent, Transform)->pos = vec3(0, 0, 0);
	GET(ent, Transform)->rot = quat(1, 0, 0, 0);
	GET(ent, Transform)->scale = vec3(2);

	//

	//* balls
	u32 lastShapeIndex = INVALID_INDEX;
	u32 curShapeIndex = INVALID_INDEX;
	u32 curBodyIndex = INVALID_INDEX;
	for (int i = 0; i < 12; i++) {
		ent->create("rigidbody");
		GET(ent, Transform)->pos = vec3(4 * (i + 1) - 20, 10 + (4*i), -20);
		GET(ent, Transform)->rot = quat(1, 0, 0, 0);
		GET(ent, Transform)->scale = vec3(4);
		GET(ent, Mesh)->meshId = assets->getModelIndex("crate");
		GET(ent, Mesh)->diffuseId = assets->getTextureIndex("crate");
		GET(ent, Mesh)->normalId = INVALID_INDEX;
		GET(ent, Mesh)->specularId = INVALID_INDEX;
		GET(ent, Mesh)->tiling = vec2(1);
		GET(ent, Rigidbody)->invMass = 1;

		ent->getGlobalIndex(&curBodyIndex);

		ent->create("constraint");
		GET(ent, Constraint)->type = CONSTRAINT_SPHERE;
		GET(ent, Constraint)->body = curBodyIndex;
		GET(ent, Constraint)->restitution = .9f;
		GET(ent, Constraint)->staticFriction = .7f;
		GET(ent, Constraint)->dynamicFriction = .2f;

		ent->getGlobalIndex(&curShapeIndex);

		//*
		ent->create("constraint");
		GET(ent, Constraint)->connections[0] = lastShapeIndex;
		GET(ent, Constraint)->connections[1] = curShapeIndex;
		GET(ent, Constraint)->offsets[0].pos = i ? vec3(0) : vec3(0, 10, -20);
		GET(ent, Constraint)->offsets[0].rot = quat(1, 0, 0, 0);
		GET(ent, Constraint)->offsets[1].pos = vec3(-4, 0, 0);
		GET(ent, Constraint)->offsets[1].rot = quat(1, 0, 0, 0);

		lastShapeIndex = curShapeIndex;
		//*/
	}
	//*/

	// big box
	ent->create("model");
	GET(ent, Transform)->pos = vec3(0, 0, 0);
	GET(ent, Transform)->scale = vec3(30);
	GET(ent, Mesh)->meshId = assets->getModelIndex("map");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("cobble/diffuse");
	GET(ent, Mesh)->normalId = assets->getTextureIndex("cobble/normal");
	GET(ent, Mesh)->specularId = assets->getTextureIndex("cobble/specular");
	GET(ent, Mesh)->tiling = vec2(20);

	// Player
	ent->create("player");
	GET(ent, Character)->speed = 250;
	GET(ent, Transform)->pos = vec3(0,5,0);
	GET(ent, Rigidbody)->vel = vec3();
	GET(ent, Rigidbody)->invMass = 0;
	u32 playerIndex;
	ent->getGlobalIndex(&playerIndex);

	//* Camera 2
	ent->create("camera");
	GET(ent, Mesh)->meshId = assets->getModelIndex("monkey");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("monkey");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	GET(ent, Transform)->pos = vec3(8, 5, 0);
	GET(ent, Transform)->rot = angleAxis(0.0f, vec3(1, 0, 0)) * angleAxis(.4f, vec3(0, 1, 0)) * angleAxis(0.0f, vec3(0, 0, 1));
	GET(ent, Transform)->scale = vec3(1);
	GET(ent, Camera)->fboId = assets->getFboIndex("fbo-split");
	GET(ent, Camera)->fov = 60.0f * TO_RADS;
	GET(ent, Child)->parent = INVALID_INDEX;
	GET(ent, Listener)->volume = 0.0f;
	GET(ent, Listener)->focus = .3f;
	//*/

	// Camera
	ent->create("camera");
	GET(ent, Mesh)->meshId = assets->getModelIndex("sphere");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("monkey");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	GET(ent, Transform)->scale = vec3(1, 1, 1);
	GET(ent, Camera)->fboId = assets->getFboIndex("fbo-default");
	GET(ent, Camera)->fov = 60.0f * TO_RADS;
	GET(ent, Child)->parent = playerIndex;
	GET(ent, Child)->offsetRot = { 0, 0, 0 };
	GET(ent, Listener)->volume = .5f;
	GET(ent, Listener)->focus = .3f;

	// FBO Rect
	ent->create("gui");
	GET(ent, Mesh)->meshId = assets->getModelIndex("rect");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("fbo-copy");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	GET(ent, Gui)->flags |= 0;
		//ATTR_GUI_PIXEL_POS | 
		//ATTR_GUI_PIXEL_SCALE;
		//ATTR_GUI_PIXEL_ANCHOR;
	GET(ent, Gui)->color = vec4(.8, .8, .8, 1);
	GET(ent, Gui)->anchor = vec3(0, 0, 0);
	GET(ent, Transform)->flags = 0;
	GET(ent, Transform)->pos = vec3(0, 0, .9);
	GET(ent, Transform)->scale = vec3(1, 1, 1);

	// FBO Rect
	ent->create("gui");
	GET(ent, Mesh)->meshId = assets->getModelIndex("rect");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("fbo-split");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	GET(ent, Gui)->flags |= COMP_GUI_PIXEL_SCALE;
	GET(ent, Gui)->color = vec4(1);
	GET(ent, Gui)->anchor = vec3(1.1, 1.15, 0);
	GET(ent, Transform)->flags = 0;
	GET(ent, Transform)->pos = vec3(1, 1, .8);
	GET(ent, Transform)->scale = vec3(400, 400, 1);

	// GLFWmonitor* monitor = glfwGetWindowMonitor(appData->window);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	float scaleW, scaleH;
	glfwGetMonitorContentScale(monitor, &scaleW, &scaleH);

	//* crosshair
	ent->create("gui");
	GET(ent, Mesh)->meshId = assets->getModelIndex("rect");
	GET(ent, Mesh)->diffuseId = assets->getTextureIndex("crosshair");
	GET(ent, Mesh)->normalId = -1;
	GET(ent, Mesh)->specularId = -1;
	GET(ent, Mesh)->tiling = vec2(1);
	GET(ent, Gui)->flags |= COMP_GUI_PIXEL_SCALE;
	GET(ent, Gui)->color = vec4(1, 0, 1, 1);
	GET(ent, Gui)->anchor = vec3(.5, .5, 0);
	GET(ent, Transform)->flags = 0;
	GET(ent, Transform)->pos = vec3(.5, .5, 0);
	GET(ent, Transform)->scale = vec3(25 * scaleW, 25 * scaleH, 0);
	//*/

	//* Fps Counter
	ent->create("text");
	GET(ent, Text)->fontSize = 30 * scaleW;
	GET(ent, Text)->fontIndex = assets->getFontIndex("calibri");
	GET(ent, Gui)->flags = COMP_GUI_PIXEL_SCALE;
	GET(ent, Gui)->color = vec4(0, 1, 0, 1);
	GET(ent, Gui)->anchor = vec3(1, 1, 0);
	GET(ent, Transform)->pos = vec3(1, 1, 0);
	GET(ent, Transform)->rot = vec3(0);
	//*/

	//* Controls
	ent->create("text");
	GET(ent, Text)->fontSize = 20 * scaleW;
	GET(ent, Text)->fontIndex = assets->getFontIndex("calibri");
	strcpy_s(GET(ent, Text)->str, " ESC - Toggle Window Focus\0");
	GET(ent, Gui)->flags = COMP_GUI_PIXEL_SCALE;
	GET(ent, Gui)->color = vec4(0, 1, 0, 1);
	GET(ent, Gui)->anchor = vec3(0, 1, 0);
	GET(ent, Transform)->pos = vec3(0, 1, 0);
	GET(ent, Transform)->rot = vec3(0);
	//*/

	PrefabData* prefab;
	app->componentManager.getPrefab(&prefab, "model");
	std::cout << "initialized " << prefab->size << "/" << prefab->capacity << " models\n";
}