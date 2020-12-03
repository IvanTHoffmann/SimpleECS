#pragma once

#include "dtypes.hpp"

/* Allocation frequency
	(1) - Application Core
	(2) - Common Assets
	(3) - Common Assets 2
	(4) - Common Assets 3
	(5) - Component Data
	(6) - Data Stream
	(7) - Frame Data
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