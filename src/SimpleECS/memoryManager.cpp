#include "SimpleECS/simpleECS.hpp"


#define CHUNK_SIZE (1<<12)


MemoryManager::MemoryManager() {
	memStart = nullptr;
	memSize = 0;
	head = 0;
}

MemoryManager::~MemoryManager() {}


size_t MemoryManager::getHead() {
	return head;
}

void* MemoryManager::getMemStart() {
	return memStart;
}

void MemoryManager::freeTo(size_t freePoint) {
	if (inMemory(freePoint)) {
		head = freePoint;
	}
}

u8* MemoryManager::getBlock(size_t blockSize) {
	u8* blockStart = getVolatileBlock(blockSize);
	std::cout << "allocate " << head << " - ";
	head += blockSize;
	std::cout << head << '\n';
	return blockStart;
}

u8* MemoryManager::getVolatileBlock(size_t blockSize) {
	if (head + blockSize >= memSize) {
		setMemSize(((head + blockSize) / CHUNK_SIZE + 2) * CHUNK_SIZE);
	}
	return memStart + head;
}

void MemoryManager::setMemSize(size_t newSize) {
	if (newSize <= memSize) {
		return;
	}

	void* newMemStart;
	if (memStart == nullptr) {
		newMemStart = malloc(newSize);
	}
	else {
		newMemStart = realloc(memStart, newSize);
	}

	if (newMemStart == nullptr) {
		std::cout << "ERROR: MemoryManager.setMemSize: Memory allocation failed\n";
		return;
	}

	memStart = (u8*)newMemStart;
	memSize = newSize;
}

void MemoryManager::freeAll() {
	if (memStart) {
		free(memStart);
	}
	memStart = nullptr;
	memSize = 0;
	head = 0;
}

bool MemoryManager::inMemory(size_t point) {
	return (0 <= point) && (point < memSize);
}

void MemoryManager::printMemory(size_t address) {
	float occupied = (float)address / memSize;
	u16 charCount = (u16)(occupied * 100);
	while (charCount--) {
		std::cout << '#';
	}
	charCount = (u16)((1-occupied) * 100);
	while (charCount--) {
		std::cout << '-';
	}
	std::cout << ' ' << address << '/' << memSize << '\n';
}