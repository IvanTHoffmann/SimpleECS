#define WIN32_LEAN_AND_MEAN

#include "SimpleECS/application.hpp"
#include "events.hpp"
#include "appData.hpp"
#include "constants.hpp"

/* TODO main

MISC
-Clean up engine code. Check for obsolete variables and places to optimize.
-Pack structs more efficiently
-Use more run-time variables instead of compile-time

ENGINE SEPARATION
-The component manager should accept the number of components and their sizes
-Entities should be retrieved from the component manager where they can have memory allocated for component data

NETWORKING
-add a network manager

INPUT
-Add name map for actions
-Consider changing Binding variables back to booleans rather than using a single byte. Packing appears to be the same.
-add human readable names for mouse inputs (first add it as an enum)
-add InputManager.unbindInput(action)
-read input bindings from a file

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

#include <array>

int main() {
	AppData appData;
	Application app(&appData);
	app.eventManager.dispatch(&onInit);

	return app.run();
}
