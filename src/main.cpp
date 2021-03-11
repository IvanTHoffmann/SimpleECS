#define WIN32_LEAN_AND_MEAN

#include "SimpleECS/application.hpp"
#include "events.hpp"
#include "appData.hpp"
#include "constants.hpp"

/* TODO main

Clean up engine code. Check for obsolete variables and places to optimize.
Pack structs more efficiently
Use more run-time variables instead of compile-time

NETWORKING
-add a network manager

INPUT
-Consider changing Binding variables back to booleans rather than using a single byte. Packing appears to be the same.
-add human readable names for mouse inputs (add it as an enum to start)
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

Memory
-The program crashes when a new texture or model is added, then subsequent runs work fine...
-Fix the necessity to copy/sync components that require allocating memory
-Reserve seperate memory pools for engine configuration, assets, and components
-Allocate temporary memory for event data. Possibly just an array of chars in AppData where the oldest data is overwritten.

Crash Prevention
-Check data types regarding entity indices. Some functions may accept u16 where they should accept u32
-Hardcode some limits for how many entities/components are allowed
-Secure ComponentManager methods
-Secure Entity methods

*/

int main() {
	AppData appData;
	Application app(&appData);
	app.eventManager.dispatch(&onInit);
	return app.run();
}
