#define WIN32_LEAN_AND_MEAN

#include "SimpleECS/application.hpp"
#include "events.hpp"
#include "appData.hpp"
#include "constants.hpp"

/* TODO main

MISC
-Clean up engine code. Check for obsolete variables and places to optimize.
-Pack structs more efficiently
-Use more dynamic allocation

ENGINE SEPARATION
-The component manager should accept the number of components and their sizes
-Entities should be retrieved from the component manager where they can get memory allocated for component data

NETWORKING
-add a network manager

INPUT
-Bindings should have an option to adjust their graph's curvature
-Bindings should have inAdd and outAdd variables to set deadzones and to invert signals
-Remove floats from binding settings. Use signed bytes or words instead.
-Actions should contain the indices of their bindings rather than the bindings themselves
-Consider changing Binding variables back to booleans rather than using a single byte. Packing appears to be the same.
-add human readable names for mouse inputs (first add it as an enum)
-add InputManager.unbindInput(action)
-read input bindings from a file

COMPONENTS
-Create an entity cache

RENDER
-add proper normal and specular maps

GUI
-Write a basic menu interface
-Write a function to convert inches to pixels

Audio
-Fix incorrect behavior when using multiple audio listeners (possibly just add a limiter and/or some compression)
-Add sound generation to the audio system
-Support changing audio devices at runtime

Assets
-Fix some unsecure functions
-Remove uniform location array macro dependencies in shader compiler function
-Allow user to set the asset path at runtime
-Allow asset reloading
-Allow asset unloading

Memory
-Fix the need to copy/sync mesh and audio components
-Reserve seperate memory pools for each manager and a temporary stack

Crash Prevention
-Check data types regarding entity indices. Some functions may accept u16 where they should accept u32
-Hardcode some limits for how many entities/components are allowed
-Secure ComponentManager methods
-Secure Entity methods

*/

int main() {
	AppData appData;
	Application app(&appData);

	// register components
	ComponentManager* comps = &app.componentManager;
	comps->addComponent("Character", sizeof(CharacterComp)); // 0
	comps->addComponent("Camera", sizeof(CameraComp)); // 1
	comps->addComponent("Transform", sizeof(TransformComp)); // 2
	comps->addComponent("Child", sizeof(ChildComp)); // 3
	comps->addComponent("Mesh", sizeof(MeshComp)); // 4
	comps->addComponent("Text", sizeof(TextComp)); // 5
	comps->addComponent("Gui", sizeof(GuiComp)); // 6
	comps->addComponent("Rigidbody", sizeof(RigidbodyComp)); // 7
	comps->addComponent("Constraint", sizeof(ConstraintComp)); // 8
	comps->addComponent("Sound", sizeof(SoundComp)); // 9
	comps->addComponent("Listener", sizeof(ListenerComp)); // 10

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

	Entity ent(&app), ent2(&app);
	appData.ent = &ent;
	appData.ent2 = &ent2;

	app.eventManager.dispatch(&onInit);

	return app.run();
}
