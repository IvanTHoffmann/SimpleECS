#define WIN32_LEAN_AND_MEAN

#include "SimpleECS/application.hpp"
#include "events.hpp"
#include "appData.hpp"
#include "constants.hpp"

/* TODO main

Clean up engine code. Check for obsolete variables and places to optimize.

RENDER
-Fix the camera rotation. The attached model rotates in the opposite direction.

GUI
-Write a function to convert inches to pixels

Audio
-Fix incorrect behaviour when using multiple audio listeners (possibly just add a limiter and/or some compression)
-Add sound generation to the audio system
-Support changing audio devices at runtime

Memory
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
