#pragma once

#include "dtypes.hpp"

/* 
Allocation frequency
	(1) - Application Core
	(2) - Common Assets
	(3) - Common Assets 2
	(4) - Common Assets 3
	(5) - Component Data
	(6) - Data Stream
	(7) - Frame Data
*/

/*

ComponentManager
(1) - component array pointers
(2) - component data
(2) - prefab data
(2) - prefab name map

AssetManager
(1) - asset name maps
(1) - asset ids
(1) - font offsets
(2) - sound data

EventManager
(1) - event tree

Components
(3) - Text.str

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

	void freeTo(u8*);
	void freeAll();
	void setMemSize(size_t size);
	bool inMemory(u8*);

	void* getBlock(size_t);
	void* getVolatileBlock(size_t);

	void printMemory(size_t);
};