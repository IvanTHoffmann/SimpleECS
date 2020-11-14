#define WIN32_LEAN_AND_MEAN

#include "SimpleECS/application.hpp"
#include "events.hpp"
#include "appData.hpp"

/* TODO

Write a function to convert inches to pixels

Add features to the audio system such as...
 -sound deactivation :|
 -playback speed (+/-)
 -3D sound
 -Sound Generation


Check data types regarding entity indices. Some functions may accept u16 where they should accept u32
Hardcode some limits for how many entities/components are allowed
Clean up engine code. Check for obsolete variables and places to optimize.
Allocate temporary memory for event data. Possibly just an array of chars in AppDataAttr where the oldest data is overwritten.

organize util.hpp?


*/

int main() {
	AppData appData;
	Application app(&appData);
	app.eventManager.dispatch(&onInit);
	return app.run();
}
