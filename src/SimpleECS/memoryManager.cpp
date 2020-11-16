#include "SimpleECS/simpleECS.hpp"


#define CHUNK_SIZE 1024


MemoryManager::MemoryManager() {
	memStart = nullptr;
	memSize = 0;
	head = 0;
}

MemoryManager::~MemoryManager() {}


void* MemoryManager::getHead() {
	return memStart + head;
}

void* MemoryManager::getMemStart() {
	return memStart;
}

void MemoryManager::freeTo(u8* freePoint) {
	if (inMemory(freePoint)) {
		head = (freePoint - memStart);
	}
}

void* MemoryManager::getBlock(size_t blockSize) {
	if (head + blockSize >= memSize) {
		setMemSize(((head + blockSize) / CHUNK_SIZE + 2) * CHUNK_SIZE);
	}
	u8* blockStart = memStart + head;
	head += blockSize;
	return blockStart;
}

void MemoryManager::setMemSize(size_t newSize) {
	if (newSize <= memSize) {
		return;
	}

	void* newMemStart = realloc(memStart, newSize);

	if (newMemStart) {
		memStart = (u8*)newMemStart;
		memSize = newSize;
	}
}

void MemoryManager::freeAll() {
	if (memStart) {
		free(memStart);
	}
	memStart = nullptr;
	memSize = 0;
	head = 0;
}

bool MemoryManager::inMemory(u8* ptr) {
	return ptr >= memStart && (ptr - memStart) < memSize;
}