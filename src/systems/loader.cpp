#include "systems/loader.hpp"
#include "appData.hpp"

void initLoaderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity ent(app);
	AssetManager* assets = &app->assetManager;
	ComponentManager* comps = &app->componentManager;

	// register classes

	comps->addPrefab("misc", 5, CharacterBit | InputBit | TransformBit | MeshBit | ChildBit);
	comps->addPrefab("player", 1, CharacterBit | InputBit | TransformBit | RigidbodyBit);
	comps->addPrefab("model", 10, TransformBit | MeshBit, PREFAB_MEM_PACK);
	comps->addPrefab("camera", 2, CameraBit | InputBit | TransformBit | ChildBit | ListenerBit | MeshBit);
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
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/AnotherOne");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("cobble/diffuse");
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();


	//* music
	ent.create("sound");
	ent.refTransform();
	ent.Transform->pos = vec3(20, 4, 10);
	ent.Transform->scale = vec3(.5);

	ent.copySound();
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/SickBeat");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("crate");
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
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/PlopXD");
	ent.Sound->volume = .8f;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;
	ent.syncSound();

	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("crate");
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
		ent.Transform->pos = vec3(4 * (i + 1), 10 + (4*i), -20);
		ent.Transform->rot = quat(1, 0, 0, 0);
		ent.Transform->scale = vec3(2);
		ent.refMesh();
		ent.Mesh->meshId = assets->getModelIndex("sphere");
		ent.Mesh->diffuseId = assets->getTextureIndex("crate");
		ent.Mesh->normalId = INVALID_INDEX;
		ent.Mesh->specularId = INVALID_INDEX;
		ent.Mesh->tiling = vec2(1);
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
	ent.Character->speed = 100;
	
	ent.refTransform();
	ent.Transform->pos = vec3(0,5,0);
	ent.refInput();
	ent.Input->controllerId = 0;
	ent.Input->sensitivity = 60.0f / 225;
	ent.refRigidbody();
	ent.Rigidbody->vel = vec3();
	ent.Rigidbody->invMass = 0;

	u32 playerIndex;
	ent.getGlobalIndex(&playerIndex);

	// Camera 2
	ent.create("camera");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("cobble");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refTransform();
	ent.Transform->pos = vec3(8, 8, 0);
	ent.Transform->rot = rotate(rotate(rotate(quat(0, 0, 0, 1), 180/57.3f, vec3(0, 0, 1)), 45/57.3f, vec3(1, 0, 0)), -45/57.3f, vec3(0, 1, 0));
	ent.Transform->scale = vec3(1);
	ent.refCamera();
	ent.Camera->fboId = assets->getFboIndex("fbo-split");
	ent.Camera->fov = 60.0f * TO_RADS;
	ent.refInput();
	ent.Input->controllerId = -1;
	ent.refChild();
	ent.Child->parent = -1;
	ent.refListener();
	ent.Listener->volume = 0.0f;
	ent.Listener->focus = .3f;

	// Camera
	ent.create("camera");
	ent.copyMesh();
	ent.Mesh->meshId = assets->getModelIndex("sphere");
	ent.Mesh->diffuseId = assets->getTextureIndex("cobble");
	ent.Mesh->normalId = -1;
	ent.Mesh->specularId = -1;
	ent.Mesh->tiling = vec2(1);
	ent.syncMesh();
	ent.refTransform();
	ent.Transform->scale = vec3(1);
	ent.refCamera();
	ent.Camera->fboId = assets->getFboIndex("fbo-default");
	ent.Camera->fov = 60.0f * TO_RADS;
	ent.refInput();
	ent.Input->controllerId = 0;
	ent.Input->sensitivity = .002f;
	ent.Input->deceleration = 0.1f;
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
	ent.Gui->color = vec4(.8, .8, .8, 1);
	ent.Gui->anchor = vec3(1.1, 1.15, 0);
	ent.refTransform();
	ent.Transform->flags = 0;
	ent.Transform->pos = vec3(1, 1, .8);
	ent.Transform->scale = vec3(300, 300, 1);

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