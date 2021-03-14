#include "SimpleECS/simpleECS.hpp"

Entity::Entity(Application* appPtr) {
	app = appPtr;
	index = -1;
	prefabID = -1;
	pointers.resize(app->componentManager.components.size());
	fill(pointers.begin(), pointers.end(), nullptr);
}

bool Entity::ref(string name) {
	u16 index = -1;
	app->componentManager.componentNames.getIndex(&index, name);
	return ref(index);
}

bool Entity::ref(u16 id) {
	if (id >= app->componentManager.components.size()) { // invalid component id
		return false;
	}
	app->componentManager.getCompPtr(&pointers[id], id, prefabID, index);
}

void* Entity::get(string name) {
	u16 index = -1;
	app->componentManager.componentNames.getIndex(&index, name);
	return get(index);
}

void* Entity::get(u16 id) {
	if (id >= pointers.size()) {
		return nullptr;
	}
	if (pointers[id] == nullptr) {
		ref(id);
	}
	return pointers[id];
}

bool Entity::setPrefab(std::string prefabName) {
	u16 pID;
	if (!app->componentManager.getPrefabID(&pID, prefabName)) {
		std::cout << "WARNING: Entity.setPrefab could not set prefab using name \"" << prefabName << "\"\n";
		return false;
	}
	return setPrefab(pID);
}

bool Entity::setPrefab(u16 pID) {
	fill(pointers.begin(), pointers.end(), nullptr);
	PrefabData* prefab;
	if (!app->componentManager.getPrefab(&prefab, pID)) {
		return false;
	}
	prefabID = pID;
	index = -1;
	return true;
}

bool Entity::setIndex(u16 localIndex) {
	fill(pointers.begin(), pointers.end(), nullptr);
	PrefabData* prefab;
	if (!app->componentManager.getPrefab(&prefab, prefabID)) {
		std::cout << "ERROR: Entity.setIndex: Could not find prefab with ID \"" << prefabID << "\"\n";
		return false;
	}

	if (localIndex >= prefab->capacity) {
		std::cout << "ERROR: Entity.setIndex: index \"" << localIndex << "\" exceeds the prefab's capacity (" << prefab->capacity << ")\n";
		return false;
	}
	index = localIndex;
	return true;
}

bool Entity::setGlobalIndex(u32 globalIndex) {
	fill(pointers.begin(), pointers.end(), nullptr);
	PrefabData* prefab;
	prefabID = -1;
	while (app->componentManager.getPrefab(&prefab, ++prefabID)) {
		if (globalIndex < prefab->capacity) {
			index = globalIndex;
			return true;
		}
		globalIndex -= prefab->capacity;
	}
	prefabID = -1;
	index = -1;
	return false;
}

bool Entity::create(std::string prefabName) {
	u16 prefabID;
	if (!app->componentManager.getPrefabID(&prefabID, prefabName)) {
		std::cout << "ERROR: Entity.create: Could not locate prefab by name \"" << prefabName << "\"\n";
		return false;
	}
	return create(prefabID);
}

bool Entity::create(u16 pID) {
	// this accounts for overflow and pack, but not overwrite.

	PrefabData* prefab;
	if (!app->componentManager.getPrefab(&prefab, pID)) {
		std::cout << "ERROR: Entity.create: Could not locate prefab by ID \"" << pID << "\"\n";
		return false;
	}

	prefabID = pID;
	index = prefab->size - 1;// *(app->componentManager.getPrefab(prefabID)->flags & PREFAB_MEM_PACK != 0) - 1;
	while (next() && !isDeleted());

	if (index >= prefab->capacity) {
		std::cout << "ERROR: entity.create: Index exceeded capacity";
		return false;
		
	}

	// found space in class array
	prefab->size++;
	return true;

	/*
	if (prefab->flags & PREFAB_MEM_OVERFLOW) {
		// misc class should not be set to overflow. 
		// If this function gets stuck in a loop, thats why. 
		// Add "&& cID" to this conditional if you just cant help yourself.
		return create(0);
	}
	*/
}

// Advances to the next entity in its class. Returns false when it reaches the end of the class's array.
bool Entity::next() {
	fill(pointers.begin(), pointers.end(), nullptr);
	PrefabData* prefab; // TODO: Consider storing a pointer to prefab in entity class or ensuring that prefabID has been confirmed as valid
	if (!app->componentManager.getPrefab(&prefab, prefabID)) {
		return false;
	}
	return ++index < prefab->size; // doesnt support classes with overflow enabled or pack disabled
}

// Advances to the next entity that satisfies the masks. Returns false when there are no more to check
bool Entity::next(compMask reqMask, compMask exclMask) {
	fill(pointers.begin(), pointers.end(), nullptr);
	PrefabData* prefab;
	if (app->componentManager.getPrefab(&prefab, prefabID) && next()) {
		// We assume the prefab is matching
		return true;
	}
	
	// finished searching prefab

	while (app->componentManager.getPrefab(&prefab, ++prefabID)) {
		index = -1;
		if ((((prefab->mask & reqMask) == reqMask) && ((prefab->mask & exclMask) == 0)) && next())
		{
			// masks match class and next returned true;
			return true;
		}
	}
	return false;
}

bool Entity::isDeleted() {
	u8* compFlags = nullptr;
	for (u16 compID = 0; compID < app->componentManager.components.size(); compID++) {
		if (app->componentManager.getCompPtr(&compFlags, compID, prefabID, index)) {
			return *compFlags & COMP_DELETED;
		}
	}
	return true;
}

bool Entity::destroy() {
	PrefabData* prefab;

	if (!app->componentManager.getPrefab(&prefab, prefabID)) { return false; }
	if (index >= prefab->size) { return false; }

#if 0
	if (prefab->flags & PREFAB_MEM_PACK) {
		if (index < prefab->size - (u16)1) {
			// overwrite with attributes from entity[size-1]
			for (u8 compID = 0; compID < COMP_COUNT; compID++) {
				if (prefab->mask.test(compID)) {
					u8* dst, * src;
					app->componentManager.getCompPtr(&dst, compID, prefabID, index);
					app->componentManager.getCompPtr(&src, compID, prefabID, prefab->size - 1);
					memcpy(dst, src, app->componentManager.components[compID].size);
				}
			}
		}
		prefab->size--;
	}
	else {
#endif
		// set components to deleted
		u8* compFlags = nullptr;
		for (u8 compID = 0; compID < COMP_COUNT; compID++) {
			if (app->componentManager.getCompPtr(&compFlags, compID, prefabID, index)) {
				*compFlags = COMP_DELETED;
			}
		}
	//}
	return true;
}

bool Entity::byName(std::string name) {
	u16 index;
	app->componentManager.entityNames.getIndex(&index, name);
	return setGlobalIndex(index);
}

void Entity::setName(std::string name) {
	u32 globalIndex;
	if (!getGlobalIndex(&globalIndex)) {
		std::cout << "ERROR: Entity.setName: Could not set entity name to \"" << name << "\"\n";
		return;
	}
	app->componentManager.entityNames.add(name, globalIndex);
}

bool Entity::getGlobalIndex(u32* id) {
	PrefabData* prefab;
	if (!app->componentManager.getPrefab(&prefab, prefabID)) { 
		std::cout << "ERROR: Entity.getGlobalIndex: Could not find prefab with ID \"" << prefabID << "\"\n";
		return false; 
	}
	*id = prefab->entityIndex + index;
	return true;
}
