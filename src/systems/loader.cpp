#include "systems/loader.hpp"
#include "systems/physics.hpp"
#include "constants.hpp"
#include "appData.hpp"

void initLoaderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity ent(app);
	AssetManager* assets = &app->assetManager;
	ComponentManager* comps = &app->componentManager;

	InputManager* inp = &app->inputManager;

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
	inp->bindInput("MOVE_RIGHT",		DEVICE_KEYBOARD, KEY_D);
	float mouseSensitivity = .003f;
	inp->bindInput("LOOK_LEFT",	DEVICE_MOUSE, 2, 0.0f, -1.0f, mouseSensitivity);
	inp->bindInput("LOOK_RIGHT",	DEVICE_MOUSE, 2, 0.0f, 1.0f, mouseSensitivity);
	inp->bindInput("LOOK_UP",		DEVICE_MOUSE, 3, 0.0f, 1.0f, mouseSensitivity);
	inp->bindInput("LOOK_DOWN",	DEVICE_MOUSE, 3, 0.0f, -1.0f, mouseSensitivity);

	inp->bindInput("JUMP",	DEVICE_KEYBOARD, KEY_SPACE);
	inp->bindInput("FOCUS",	DEVICE_KEYBOARD, KEY_ESCAPE);
	inp->bindInput("QUIT",	DEVICE_KEYBOARD, KEY_DELETE);

	float moveSensitivity = 1.25f;
	float deadzone = .2f;
	inp->bindInput("MOVE_FORWARD",	0, AXIS_LY, deadzone, -1.0f, moveSensitivity);
	inp->bindInput("MOVE_LEFT",		0, AXIS_LX, deadzone, -1.0f, moveSensitivity);
	inp->bindInput("MOVE_BACKWARD",	0, AXIS_LY, deadzone,  1.0f, moveSensitivity);
	inp->bindInput("MOVE_RIGHT",		0, AXIS_LX, deadzone,  1.0f, moveSensitivity);
	inp->bindInput("MOVE_FORWARD",	0, BUTTON_UP);
	inp->bindInput("MOVE_LEFT",		0, BUTTON_LEFT);
	inp->bindInput("MOVE_BACKWARD",	0, BUTTON_DOWN);
	inp->bindInput("MOVE_RIGHT",		0, BUTTON_RIGHT);
	float gpSensitivity = 5.0f;
	inp->bindInput("LOOK_UP",		0, AXIS_RY, deadzone,  1.0f, gpSensitivity,  true);
	inp->bindInput("LOOK_LEFT",	0, AXIS_RX, deadzone, -1.0f, gpSensitivity, true);
	inp->bindInput("LOOK_DOWN",	0, AXIS_RY, deadzone, -1.0f, gpSensitivity, true);
	inp->bindInput("LOOK_RIGHT",	0, AXIS_RX, deadzone,  1.0f, gpSensitivity,  true);

	inp->bindInput("JUMP",	0, BUTTON_A);
	inp->bindInput("FOCUS",	0, BUTTON_SELECT);
	inp->bindInput("QUIT",	0, BUTTON_START);

	// register prefabs
	comps->addPrefab("misc", 5, CharacterBit | TransformBit | MeshBit | ChildBit);
	comps->addPrefab("player", 1, CharacterBit | TransformBit | RigidbodyBit);
	comps->addPrefab("model", 10, TransformBit | MeshBit, PREFAB_MEM_PACK);
	comps->addPrefab("camera", 2, CameraBit | TransformBit | ChildBit | ListenerBit | MeshBit);
	comps->addPrefab("text", 3, TextBit | GuiBit | TransformBit);
	comps->addPrefab("gui", 5, MeshBit | GuiBit | TransformBit);
	comps->addPrefab("rigidbody", 20, MeshBit | TransformBit | RigidbodyBit);
	comps->addPrefab("constraint", 40, TransformBit | ConstraintBit);
	comps->addPrefab("sound", 10, TransformBit | SoundBit | MeshBit);
	comps->recalculateMemory();

	// 

	assets->loadFbo("fbo-copy", 1920, 1000);
	assets->loadFbo("fbo-default", 1920, 1000);
	assets->loadFbo("fbo-split", 1920 / 4, 1000);

	//* music
	ent.create("sound");
	ent.refTransform();
	ent.Transform->pos = vec3(0, 4, -15);
	ent.Transform->scale = vec3(1, 1, 1);

	ent.copySound();
	ent.Sound->flags |= COMP_SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/AnotherOne");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("crash");
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();

	//* music
	ent.create("sound");
	ent.refTransform();
	ent.Transform->pos = vec3(20, 4, 10);
	ent.Transform->scale = vec3(.5);

	ent.copySound();
	ent.Sound->flags |= COMP_SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/SickBeat");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("jukebox");
	ent.Mesh->diffuseId = assets->getTextureIndex("jukebox");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();

	//* music
	ent.create("sound");
	ent.refTransform();
	ent.Transform->pos = vec3(-20, 4, 10);
	ent.Transform->scale = vec3(.5);

	ent.copySound();
	ent.Sound->flags |= COMP_SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/PlopXD");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("jukebox");
	ent.Mesh->diffuseId = assets->getTextureIndex("jukebox");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	//*/

	//* screen
	ent.create("model");
	ent.refTransform();
	ent.Transform->pos = vec3(-5, 0, -15);
	ent.Transform->scale = vec3(10, 5.2, 1);
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("rect");
	ent.Mesh->diffuseId = assets->getTextureIndex("fbo-copy");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	//*/ 

	//* Ground Collider

	ent.create("constraint");
	ent.refConstraint();
	ent.Constraint->type = CONSTRAINT_PLANE;
	ent.Constraint->restitution = .9f;
	ent.Constraint->staticFriction = .7f;
	ent.Constraint->dynamicFriction = .2f;

	ent.refTransform();
	ent.Transform->pos = vec3(0, 0, 0);
	ent.Transform->rot = quat(1, 0, 0, 0);
	ent.Transform->scale = vec3(2);

	//

	//* balls
	u32 lastShapeIndex = INVALID_INDEX;
	u32 curShapeIndex = INVALID_INDEX;
	u32 curBodyIndex = INVALID_INDEX;
	for (int i = 0; i < 12; i++) {
		ent.create("rigidbody");
		ent.refTransform();
		ent.Transform->pos = vec3(4 * (i + 1) - 20, 10 + (4*i), -20);
		ent.Transform->rot = quat(1, 0, 0, 0);
		ent.Transform->scale = vec3(4);
		ent.copyMesh();
		ent.Mesh->meshId = assets->getModelIndex("crate");
		ent.Mesh->diffuseId = assets->getTextureIndex("crate");
		ent.Mesh->normalId = INVALID_INDEX;
		ent.Mesh->specularId = INVALID_INDEX;
		ent.Mesh->tiling = vec2(1);
		ent.syncMesh();
		ent.refRigidbody();
		ent.Rigidbody->invMass = 1;

		ent.getGlobalIndex(&curBodyIndex);

		ent.create("constraint");
		ent.refConstraint();
		ent.Constraint->type = CONSTRAINT_SPHERE;
		ent.Constraint->body = curBodyIndex;
		ent.Constraint->restitution = .9f;
		ent.Constraint->staticFriction = .7f;
		ent.Constraint->dynamicFriction = .2f;

		ent.getGlobalIndex(&curShapeIndex);

		//*
		ent.create("constraint");
		ent.refConstraint();
		ent.Constraint->connections[0] = lastShapeIndex;
		ent.Constraint->connections[1] = curShapeIndex;
		ent.Constraint->offsets[0].pos = i ? vec3(0) : vec3(0, 10, -20);
		ent.Constraint->offsets[0].rot = quat(1, 0, 0, 0);
		ent.Constraint->offsets[1].pos = vec3(-4, 0, 0);
		ent.Constraint->offsets[1].rot = quat(1, 0, 0, 0);

		lastShapeIndex = curShapeIndex;
		//*/
	}
	//*/

	// big box
	ent.create("model");
	
	ent.refTransform();
	ent.Transform->pos = vec3(0, 0, 0);
	ent.Transform->scale = vec3(30);
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("map");
	ent.Mesh->diffuseId = assets->getTextureIndex("cobble/diffuse");
	ent.Mesh->normalId = assets->getTextureIndex("cobble/normal");
	ent.Mesh->specularId = assets->getTextureIndex("cobble/specular");
	ent.Mesh->tiling = vec2(20);
	ent.syncMesh();

	// Player
	ent.create("player");
	ent.refCharacter();
	ent.Character->speed = 250;
	
	ent.refTransform();
	ent.Transform->pos = vec3(0,5,0);
	ent.refRigidbody();
	ent.Rigidbody->vel = vec3();
	ent.Rigidbody->invMass = 0;

	u32 playerIndex;
	ent.getGlobalIndex(&playerIndex);

	// Camera 2
	ent.create("camera");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("monkey");
	ent.Mesh->diffuseId = assets->getTextureIndex("monkey");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refTransform();
	ent.Transform->pos = vec3(8, 5, 0);
	ent.Transform->rot = angleAxis(0.0f, vec3(1, 0, 0)) * angleAxis(.4f, vec3(0, 1, 0)) * angleAxis(0.0f, vec3(0, 0, 1));
	ent.Transform->scale = vec3(1);
	ent.refCamera();
	ent.Camera->fboId = assets->getFboIndex("fbo-split");
	ent.Camera->fov = 60.0f * TO_RADS;
	ent.refChild();
	ent.Child->parent = INVALID_INDEX;
	ent.refListener();
	ent.Listener->volume = 0.0f;
	ent.Listener->focus = .3f;

	// Camera
	ent.create("camera");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("monkey");
	ent.Mesh->diffuseId = assets->getTextureIndex("monkey");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refTransform();
	ent.Transform->scale = vec3(1, 1, 1);
	ent.refCamera();
	ent.Camera->fboId = assets->getFboIndex("fbo-default");
	ent.Camera->fov = 60.0f * TO_RADS;
	ent.refChild();
	ent.Child->parent = playerIndex;
	ent.Child->offsetRot = {0, 0, 0};
	ent.refListener();
	ent.Listener->volume = .5f;
	ent.Listener->focus = .3f;


	// FBO Rect
	ent.create("gui");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("rect");
	ent.Mesh->diffuseId = assets->getTextureIndex("fbo-default");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refGui();
	ent.Gui->flags |= 0;
		//ATTR_GUI_PIXEL_POS | 
		//ATTR_GUI_PIXEL_SCALE;
		//ATTR_GUI_PIXEL_ANCHOR;
	ent.Gui->color = vec4(.8, .8, .8, 1);
	ent.Gui->anchor = vec3(0, 0, 0);
	ent.refTransform();
	ent.Transform->flags = 0;
	ent.Transform->pos = vec3(0, 0, .9);
	ent.Transform->scale = vec3(1, 1, 1);

	// FBO Rect
	ent.create("gui");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("rect");
	ent.Mesh->diffuseId = assets->getTextureIndex("fbo-split");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refGui();
	ent.Gui->flags |= COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(1);
	ent.Gui->anchor = vec3(1.1, 1.15, 0);
	ent.refTransform();
	ent.Transform->flags = 0;
	ent.Transform->pos = vec3(1, 1, .8);
	ent.Transform->scale = vec3(200, 200, 1);

	// GLFWmonitor* monitor = glfwGetWindowMonitor(appData->window);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	float scaleW, scaleH;
	glfwGetMonitorContentScale(monitor, &scaleW, &scaleH);

	//* crosshair
	ent.create("gui");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("rect");
	ent.Mesh->diffuseId = assets->getTextureIndex("crosshair");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refGui();
	ent.Gui->flags |= COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(1, 0, 1, 1);
	ent.Gui->anchor = vec3(.5, .5, 0);
	ent.refTransform();
	ent.Transform->flags = 0;
	ent.Transform->pos = vec3(.5, .5, 0);
	ent.Transform->scale = vec3(25 * scaleW, 25 * scaleH, 0);
	//*/

	//* Fps Counter
	ent.create("text");
	ent.copyText();
	ent.Text->fontSize = 30 * scaleW;
	ent.Text->fontIndex = assets->getFontIndex("calibri");
	ent.syncText();
	ent.refGui();
	ent.Gui->flags = COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(0, 1, 0, 1);
	ent.Gui->anchor = vec3(1, 1, 0);
	ent.refTransform();
	ent.Transform->pos = vec3(1, 1, 0);
	ent.Transform->rot = vec3(0);
	//*/

	//* Controls
	ent.create("text");
	ent.copyText();
	ent.Text->fontSize = 20 * scaleW;
	ent.Text->fontIndex = assets->getFontIndex("calibri");
	strcpy_s(ent.Text->str, " ESC - Toggle Window Focus\0");
	ent.syncText();
	ent.refGui();
	ent.Gui->flags = COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(0, 1, 0, 1);
	ent.Gui->anchor = vec3(0, 1, 0);
	ent.refTransform();
	ent.Transform->pos = vec3(0, 1, 0);
	ent.Transform->rot = vec3(0);
	//*/

	PrefabData* prefab;
	app->componentManager.getPrefab(&prefab, "model");
	std::cout << "initialized " << prefab->size << "/" << prefab->capacity << " models\n";
}