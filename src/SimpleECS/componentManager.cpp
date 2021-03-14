#include "SimpleECS/simpleECS.hpp"


ComponentManager::ComponentManager(Application* _app) {
	app = _app;
	clearPrefabs();
}

ComponentManager::~ComponentManager() {}

u16 ComponentManager::addComponent(std::string name, size_t size) {
	u16 index = components.size();
	if (componentNames.getIndex(&index, name)) {
		cout << "WARNING: A component named \"" << name << "\" already exists\n";
		return -1;
	}
	componentNames.add(name, index);
	CompData comp;
	comp.size = size;
	components.push_back(comp);
	return index;
}

u16 ComponentManager::addPrefab(std::string name, u32 capacity, compMask mask, u8 flags) {
	u16 index = prefabs.size();
	if (prefabNames.getIndex(&index, name)) {
		cout << "WARNING: A prefab named \"" << name << "\" already exists\n";
		return -1;
	}
	prefabNames.add(name, index);
	PrefabData prefab;
	prefab.size = 0;
	prefab.capacity = capacity;
	prefab.mask = mask;
	prefab.flags = flags | PREFAB_ACTIVE;
	prefab.newest = capacity - 1;
	prefabs.push_back(prefab);
	return index;
}

void ComponentManager::recalculateMemory() {
	// SET ENTITY INDICES
	u16 entityIndex = 0;
	for (u16 i = 0; i < prefabs.size(); i++) {
		prefabs[i].entityIndex = entityIndex;
		entityIndex += prefabs[i].capacity;
		prefabs[i].indices.resize(components.size());
	}

	// count the max number of each component and how many bytes will be needed to store everything
	u32 bytes = 0;
	u32 compCount;

	for (u8 compID = 0; compID < components.size(); compID++) {
		components[compID].startPos = bytes; // set starting position for current component array

		compCount = 0;
		for (u8 prefabID = 0; prefabID < prefabs.size(); prefabID++) {
			prefabs[prefabID].indices[compID] = compCount; // set starting index to current number of components in the array
			compCount += prefabs[prefabID].capacity * prefabs[prefabID].mask[compID]; // add maxCount if component is enabled in mask
		}
		bytes += compCount * components[compID].size;
	}

	// ALLOCATE MEMORY

	data.resize(bytes);
	fill(data.begin(), data.end(), 0);
}

// TODO: finish removePrefab functions
void ComponentManager::removePrefab(u16 prefabID) {
	if (prefabID >= prefabs.size()) { return; }
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
	prefabs.clear();
	prefabNames.clear();
}

bool ComponentManager::getPrefab(PrefabData** out, u16 id) {
	if (id >= prefabs.size()) { return false; }
	*out = &prefabs[id];
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
	if (compID >= components.size() || prefabID >= prefabs.size() || entIndex >= prefabs[prefabID].size || !prefabs[prefabID].mask[compID]) {
		return false;
	}
	*p = &data[components[compID].startPos + (prefabs[prefabID].indices[compID] + entIndex) * components[compID].size];
	return true; 
}

bool ComponentManager::getCompPtr(u8** p, u16 compID, u32 compIndex) {
	*p = nullptr;
	if (compID >= components.size() || compIndex >= components[compID].count) {
		return false;
	}
	*p = &data[components[compID].startPos + compIndex * components[compID].size];
	return true;
}
//