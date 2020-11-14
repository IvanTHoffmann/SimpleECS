#pragma once

#include "SimpleECS/assetManager.hpp"
#include "SimpleECS/componentManager.hpp"
#include "SimpleECS/eventManager.hpp"

class Application {
private:
	bool running;
	void* data;

public:
	AssetManager assetManager;
	ComponentManager componentManager;
	EventManager eventManager;
	
	Application(void* appData);
	~Application();

	void* getData();

	int run();
	void stop();
};


/*

Application - - - - - -	Holds managers. Wraps the entire engine into one class.
~data - - - - - - - - - Pointer to an application data struct where the developer can store singleton data

ComponentManager  -	- -	Holds all user data in a block of memory. Provides functions for accessing it.
~Components - - - - - - Struct of raw data.
~Prefabs  - - - - - - - Holds information about how the memory is segmented.
~Entities - - - - - - - Allows the user to fetch and modify components as if they were member variables of an object.

EventManager  - - - - - stores events and calls them at their target time
~Events - - - - - - - - Holds information about an event such as the time it needs to be called and a pointer to relevant data

AssetManager  - - - - - Loads and stores asset data to be retrieved by systems. 
						This could be stored in a singleton component or in AppData, but it's simpler to use it as a class.


*/