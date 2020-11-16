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

void MemoryManager::freeTo(u8* freePoint) {
	if (inMemory(freePoint)) {
		size_t oldHead = head;
		head = (freePoint - memStart);
		std::cout << oldHead << "->" << head << '\n';
	}
}

void* MemoryManager::getBlock(size_t blockSize) {
	void* blockStart = getVolatileBlock(blockSize);
	head += blockSize;
	printMemory(head);
	return blockStart;
}

void* MemoryManager::getVolatileBlock(size_t blockSize) {
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
		std::cout << "ERROR: Memory allocation failed\n";
		return;
	}

	std::cout << "--- allocate --- ";
	if (newMemStart != memStart) {
		std::cout << "memStart changed on allocation..." << '\n';
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

bool MemoryManager::inMemory(u8* ptr) {
	return ptr >= memStart && (ptr - memStart) < memSize;
}

void MemoryManager::printMemory(size_t address) {
	float occupied = (double)address / memSize;
	u16 charCount = occupied * 100;
	while (charCount--) {
		std::cout << '#';
	}
	charCount = (1-occupied) * 100;
	while (charCount--) {
		std::cout << '-';
	}
	std::cout << ' ' << address << '/' << memSize << '\n';
}