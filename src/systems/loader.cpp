#include "systems/loader.hpp"
#include "appData.hpp"

void initLoaderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity ent(app);
	AssetManager* assets = &app->assetManager;

	assets->loadFbo("fbo-copy", 1920, 1000);
	assets->loadFbo("fbo-default", 1920, 1000);

	// Pre-loading assets isn't necessary, but we should keep track of the assets we use. We might want to free gpu memory between levels.
	/* 
	assets->getShaderIndex("simple");
	assets->getShaderIndex("ui");

	assets->getModelIndex("wheel");
	assets->getModelIndex("cube");
	assets->getModelIndex("rect");
	assets->getModelIndex("crate");
	assets->getModelIndex("sphere");

	assets->getTextureIndex("boop");
	assets->getTextureIndex("crosshair");
	assets->getTextureIndex("cobble");
	assets->getTextureIndex("crate");

	assets->getFontIndex("calibri");
	assets->getFontIndex("cour");
	//*/

	//* music
	ent.getNew(app->componentManager.getPrefabID("sound"));
	ent.refTransform();
	ent.Transform->pos = vec3(0, 4, -15);
	ent.Transform->scale = vec3(1, 1, 1);

	ent.refSound();
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/AnotherOne");
	ent.Sound->volume = .8;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;

	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("sphere");
	ent.Mesh->texId = app->assetManager.getTextureIndex("crate");
	ent.Mesh->tiling = vec2(1);


	//* music
	ent.getNew(app->componentManager.getPrefabID("sound"));
	ent.refTransform();
	ent.Transform->pos = vec3(20, 4, 10);
	ent.Transform->scale = vec3(.5);

	ent.refSound();
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/SickBeat");
	ent.Sound->volume = .8;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;

	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("sphere");
	ent.Mesh->texId = app->assetManager.getTextureIndex("crate");
	ent.Mesh->tiling = vec2(1);
	//*/


	//* music
	ent.getNew(app->componentManager.getPrefabID("sound"));
	ent.refTransform();
	ent.Transform->pos = vec3(-20, 4, 10);
	ent.Transform->scale = vec3(.5);

	ent.refSound();
	ent.Sound->flags |= SOUND_LOOP;
	ent.Sound->soundIndex = assets->getSoundIndex("music/PlopXD");
	ent.Sound->volume = .8;
	ent.Sound->sampleIndex = 0;
	ent.Sound->subIndex = 0;
	ent.Sound->speed = 1;
	ent.Sound->fade = 50;

	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("sphere");
	ent.Mesh->texId = app->assetManager.getTextureIndex("crate");
	ent.Mesh->tiling = vec2(1);
	//*/

	//* screen
	ent.getNew(app->componentManager.getPrefabID("model"));
	
	ent.refTransform();
	ent.Transform->pos = vec3(-5, 0, -15);
	ent.Transform->scale = vec3(10, 5.2, 1);
	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("rect");
	ent.Mesh->texId = app->assetManager.getTextureIndex("fbo-copy");
	ent.Mesh->tiling = vec2(1);
	//*/ 

	//* balls
	u32 lastBodyIndex = INVALID_INDEX;
	u32 curBodyIndex = INVALID_INDEX;
	for (int i = 0; i < 12; i++) {
		ent.getNew(app->componentManager.getPrefabID("rigidbody"));
		ent.refTransform();
		ent.Transform->pos = vec3(4 * (i + 1), 50 + (4*i), -20);
		ent.Transform->rot = quat(1, 0, 0, 0);
		ent.Transform->scale = vec3(2);
		ent.refMesh();
		ent.Mesh->meshId = app->assetManager.getModelIndex("sphere");
		ent.Mesh->texId = app->assetManager.getTextureIndex("crate");
		ent.Mesh->tiling = vec2(1);

		curBodyIndex = ent.getGlobalIndex();

		//*
		ent.getNew(app->componentManager.getPrefabID("constraint"));
		ent.refConstraint();
		ent.Constraint->bodyA = lastBodyIndex;
		ent.Constraint->bodyB = curBodyIndex;
		ent.Constraint->offsetA.pos = i ? vec3(0) : vec3(0, 10, -20);
		ent.Constraint->offsetA.rot = quat(1, 0, 0, 0);
		ent.Constraint->offsetB.pos = vec3(-4, 0, 0);
		ent.Constraint->offsetB.rot = quat(1, 0, 0, 0);

		lastBodyIndex = curBodyIndex;
		//*/
	}
	//*/

	// big box
	ent.getNew(app->componentManager.getPrefabID("model"));
	
	ent.refTransform();
	ent.Transform->pos = vec3(0, 0, 0);
	ent.Transform->scale = vec3(30);
	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("map");
	ent.Mesh->texId = app->assetManager.getTextureIndex("cobble");
	ent.Mesh->tiling = vec2(20);

	// Player
	ent.getNew(app->componentManager.getPrefabID("player"));
	ent.refCharacter();
	ent.Character->speed = 10;
	
	ent.refTransform();
	ent.Transform->pos = vec3(0,5,0);
	ent.refInput();
	ent.Input->controllerId = 0;
	ent.Input->sensitivity = 60.0f / 225;

	u16 playerIndex = ent.getGlobalIndex();

	// Camera
	ent.getNew(app->componentManager.getPrefabID("camera"));
	
	ent.refTransform();
	ent.Transform->scale = vec3(1);
	ent.refCamera();
	ent.Camera->fboId = app->assetManager.getFboIndex("fbo-default");
	ent.Camera->fov = 60.0f * TO_RADS;
	ent.refInput();
	ent.Input->controllerId = 0;
	ent.Input->sensitivity = .002f;
	ent.Input->deceleration = 0.1f;
	ent.refChild();
	ent.Child->parent = playerIndex;
	ent.Child->offsetRot = {0, 0, 0};
	ent.refListener();
	ent.Listener->volume = .5;
	ent.Listener->focus = .3;


	// FBO Rect
	ent.getNew(app->componentManager.getPrefabID("gui"));
	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("rect");
	ent.Mesh->texId = app->assetManager.getTextureIndex("fbo-default");
	ent.Mesh->tiling = vec2(1);
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
	ent.Transform->scale = vec3(1);

	//* crosshair
	ent.getNew(app->componentManager.getPrefabID("gui"));
	ent.refMesh();
	ent.Mesh->meshId = app->assetManager.getModelIndex("rect");
	ent.Mesh->texId = app->assetManager.getTextureIndex("crosshair"); 
	ent.Mesh->tiling = vec2(1);
	ent.refGui();
	ent.Gui->flags |= COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(1, 1, 1, 1);
	ent.Gui->anchor = vec3(.5, .5, 0);
	ent.refTransform();
	ent.Transform->flags = 0;
	ent.Transform->pos = vec3(.5, .5, 0);
	ent.Transform->scale = vec3(25, 25, 0);
	//*/

	//* Fps Counter
	ent.getNew(app->componentManager.getPrefabID("text"));
	ent.refText();
	ent.Text->fontSize = 50;
	ent.Text->fontIndex = app->assetManager.getFontIndex("calibri");
	ent.refGui();
	ent.Gui->flags = COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(0, 1, 0, 1);
	ent.Gui->anchor = vec3(.5, 0, 0);
	ent.refTransform();
	ent.Transform->pos = vec3(.5, 0, 0);
	ent.Transform->rot = vec3(0);
	//*/

	//* Controls
	ent.getNew(app->componentManager.getPrefabID("text"));
	ent.refText();
	ent.Text->fontSize = 30;
	ent.Text->fontIndex = app->assetManager.getFontIndex("calibri");
	strcpy_s(ent.Text->str, " ESC - Toggle Window Focus\0");
	ent.refGui();
	ent.Gui->flags = COMP_GUI_PIXEL_SCALE;
	ent.Gui->color = vec4(0, 1, 0, 1);
	ent.Gui->anchor = vec3(0, 1, 0);
	ent.refTransform();
	ent.Transform->pos = vec3(0, 1, 0);
	ent.Transform->rot = vec3(0);
	//*/

	PrefabData* prefab = app->componentManager.getPrefab("model");
	std::cout << "initialized " << prefab->size << "/" << prefab->capacity << " models\n";
}