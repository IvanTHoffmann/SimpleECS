#pragma once

#include "dtypes.hpp"

/*
	InputManager				- Size can change upon setting a binding
	- bindings					std::vector<Binding>
	
	ComponentManager			- Size can change when a level loads
	- prefabs;					std::vector<Prefab>
		- u32* indices;			std::vector<u32>[numComponents]
	- Component data;			std::vector<u8>
	
	EventManager				- Size can change every frame
	- Event* events;			std::Vector<Event>
	- void* eventData;			std::Vector<u8> (or use the temporary stack)

	AssetManager				- Size can change at any time
	- FontOffsets* fontOffsets;	std::Vector<u16>
	- Sound* soundData;			std::Vector<i16>

	Temporary Stack

*/

class MemoryManager {
private:
	size_t head, memSize;

public:
	u8 *memStart;
	MemoryManager();
	~MemoryManager();

	size_t getHead();
	void* getMemStart();

	void freeTo(size_t);
	void freeAll();
	void setMemSize(size_t);
	bool inMemory(size_t);

	u8* getBlock(size_t);
	u8* getVolatileBlock(size_t);

	void printMemory(size_t);
};