#include "SimpleECS/simpleECS.hpp"


ComponentManager::ComponentManager() {
	clearPrefabs();
	dataSize = 0;
	data = 0;
	prefabCount = 0;
}

ComponentManager::~ComponentManager() {
	if (dataSize) {
		app->memoryManager.freeTo(data);
	}
}

void ComponentManager::setApp(Application* _app) {
	app = _app;
}

u16 ComponentManager::addPrefab(std::string name, u32 capacity, compMask mask, u8 flags) {
	for (u16 i = 0; i < maxPrefabs; i++) {
		if (!(prefabs[i].flags && PREFAB_ACTIVE)) {
			prefabNames.add(name, i);
			prefabs[i].flags = flags | PREFAB_ACTIVE;
			prefabs[i].capacity = capacity;
			prefabs[i].size = 0;
			prefabs[i].newest = capacity - 1;
			prefabs[i].mask = mask;
			prefabCount++;
			return i;
		}
	}
	std::cout << "ERROR: Couldn't add prefab \"" << name << "\" because the maximum number of prefabs has already been reached\n";
	return INVALID_INDEX_16;
}

void ComponentManager::recalculateMemory() {
	// SET ENTITY INDICES
	u16 entityIndex = 0;
	for (u16 i = 0; i < maxPrefabs; i++) {
		prefabs[i].entityIndex = entityIndex;
		entityIndex += prefabs[i].capacity;
	}

	// count the max number of each component and how many bytes will be needed to store everything
	u32 bytes = 0;
	u32 compCount;

	for (u8 compID = 0; compID < COMP_COUNT; compID++) {
		arrays[compID] = bytes; // set starting position for current component array

		compCount = 0;
		for (u8 prefabID = 0; prefabID < maxPrefabs; prefabID++) {
			prefabs[prefabID].indices[compID] = compCount; // set starting index to current number of components in the array
			compCount += prefabs[prefabID].capacity * prefabs[prefabID].mask[compID]; // add maxCount if component is enabled in mask
		}
		bytes += compCount * compSize[compID];
	}

	// ALLOCATE MEMORY

	if (data) {
		app->memoryManager.freeTo(data);
	}
	std::cout << "Components (" << bytes << ") bytes\n";
	data = app->memoryManager.getHead();
	dataSize = bytes;
	u8* dataPtr = (u8*)app->memoryManager.getBlock(bytes);
	for (u32 i = 0; i < dataSize; i++) {
		dataPtr[i] = 0;
	}
}

// TODO: finish removePrefab functions
void ComponentManager::removePrefab(u16 prefabID) {
	if (prefabID >= prefabCount) { return; }
	prefabs[prefabID].flags = 0;
	// remove prefab name entry from prefabNames
}

void ComponentManager::removePrefab(std::string name) {
	prefabNames.remove(name);
	u16 prefabID;
	if (!getPrefabID(&prefabID, name)) { return; }
	prefabs[prefabID].flags = 0;
}
//

void ComponentManager::clearPrefabs() {
	for (u16 i = 0; i < maxPrefabs; i++) {
		prefabs[i].flags = 0;
	}
	prefabNames.clear();
}

bool ComponentManager::getPrefab(PrefabData** out, u16 id) {
	if (id >= prefabCount) { return false; }
	*out = prefabs + id;
	return true;
}

bool ComponentManager::getPrefab(PrefabData** out, std::string name) {
	u16 index;
	if (!prefabNames.getIndex(&index, name)) {
		return false;
	}
	return getPrefab(out, index);
}

bool ComponentManager::getPrefabID(u16* out, std::string name) {
	return prefabNames.getIndex(out, name);
}

bool ComponentManager::getPrefabName(std::string* out, u16 id) {
	return prefabNames.getName(out, id);
}

// TODO: Make sure these functions can never crash
bool ComponentManager::getCompPtr(u8** p, u16 compID, u16 prefabID, u32 entIndex) {
	*p = nullptr;
	PrefabData prefab = prefabs[prefabID];
	if (!prefab.mask[compID]) { return false; }
	if (entIndex >= prefab.size) { return false; }
	*p = app->memoryManager.memStart + data + arrays[compID] + ((u32)prefabs[prefabID].indices[compID] + entIndex) * compSize[compID];
	return true; 
}

bool ComponentManager::getCompPtr(u8** p, u16 compID, u32 compIndex) {
	*p = nullptr;
	if (arrays[compID] + compSize[compID] * compIndex >= (compID < COMP_COUNT - 1 ? arrays[compID + 1] : dataSize)) { return false; }
	*p = app->memoryManager.memStart + data + arrays[compID] + compIndex * compSize[compID];
	return true;
}
//