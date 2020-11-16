#include "SimpleECS/simpleECS.hpp"

#include "compData.hpp"

bool shouldSync(u8* reference, u8* copy) {
	if (reference == copy) {
		if (*copy & COMP_NO_SYNC) {
			// This is used for easy debugging. You can insert a breakpoint here to pause the program when something tries to sync changes to a COMP_NO_SYNC flagged component
			return false;
		}
		return true;
	}
	return reference == nullptr;
}

#define COPY_DEF(c) bool Entity::copy ## c() { \
if (!ref ## c()) {return false;} \
c ## Copy = *c; c = &(c ## Copy); return true; }

#define SYNC_DEF(c) bool Entity::sync ## c() { \
if (!shouldSync((u8*)c, (u8*)&c##Copy)) { return false; } \
if (!ref ## c()) { return false; } \
*c = c ## Copy; return true; }

#define REF_DEF(c) bool Entity::ref ## c() { return app->componentManager.getCompPtr((u8**)(&c), COMP_ENUM(c), prefabID, index); }

FOREACH_COMP(COPY_DEF);
FOREACH_COMP(SYNC_DEF);
FOREACH_COMP(REF_DEF);


Entity::Entity(Application* appPtr) {
	app = appPtr;
	set();
}

#define POINT_TO_COPY(c) c = &c##Copy; 
#define CLEAR_FLAGS(c) c##Copy.flags = 0; 


void Entity::clearVars() {
	FOREACH_COMP(POINT_TO_COPY)
	FOREACH_COMP(CLEAR_FLAGS)
}

bool Entity::set(u16 cID, u16 arrayIndex) {
	clearVars();
	//mask = 0;
	prefabID = cID;
	if (prefabID > app->componentManager.getPrefabCount()) {
		// The class wasn't specified.
		if (arrayIndex == (u8)(-1)) {
			// The index wasn't specified either.
			prefabID = -1;
			index = -1;
			return false;
		}
		// The index was specified. Search for entity using its global index.
		PrefabData* prefabData;
		prefabID = app->componentManager.getPrefabCount();
		while (prefabID > 0) {
			prefabData = app->componentManager.getPrefab(--prefabID);
			if (arrayIndex >= prefabData->entityIndex) {
				// If the CLASS_COUNT-1 array is selected, the index may be outside the bounds, so we check that ( index < maxSize; )
				index = arrayIndex - prefabData->entityIndex;
				if (index >= prefabData->capacity) { break; }
				return true;
			}
		}
		// The global index was out of bounds.
		prefabID = -1;
		index = -1;
		return false;
	}
	index = arrayIndex;
	if (index > app->componentManager.getPrefab(prefabID)->capacity) {
		// The class was specified, but the local index was out of bounds.
		index = -1;
		return false;
	}
	// The class and local index were both valid.
	return true;
}

bool Entity::getNew(u16 cID) {
	clearVars();
	// this accounts for overflow and pack, but not overwrite.
	prefabID = cID;
	if (prefabID > app->componentManager.getPrefabCount()) { return false; }

	index = app->componentManager.getPrefab(prefabID)->size - 1;// *(app->componentManager.getPrefab(prefabID)->flags & PREFAB_MEM_PACK != 0) - 1;
	while (next() && !isDeleted());

	if (index < app->componentManager.getPrefab(cID)->capacity) {
		// found space in class array
		app->componentManager.getPrefab(cID)->size++;
		return true;
	}

	if (app->componentManager.getPrefab(cID)->flags & PREFAB_MEM_OVERFLOW) {
		// misc class should not be set to overflow. 
		// If this function gets stuck in a loop, thats why. 
		// Add "&& cID" to this conditional if you just cant help yourself.
		return getNew(0);
	}
	return false;
}

// Advances to the next entity in its class. Returns false when it reaches the end of the class's array.
bool Entity::next() {
	return ++index < app->componentManager.getPrefab(prefabID)->size; // doesnt support classes with overflow enabled or pack disabled
}

// Advances to the next entity that satisfies the masks. Returns false when there are no more to check
bool Entity::next(compMask reqMask, compMask exclMask) {
	if (prefabID > app->componentManager.getPrefabCount()) {
		prefabID = -1;
	}
	else if (next()) {
		return true;
	}
	// finished searching class
	while (++prefabID < app->componentManager.getPrefabCount()) {
		index = -1;
		if ((((app->componentManager.getPrefab(prefabID)->mask & reqMask) == reqMask) && ((app->componentManager.getPrefab(prefabID)->mask & exclMask) == 0)) && next())
		{
			// masks match class and next returned true;
			return true;
		}
	}
	return false;
}

bool Entity::isDeleted() {
	u8 *compFlags = nullptr;
	for (u16 compID = 0; compID < COMP_COUNT; compID++) {
		if (app->componentManager.getCompPtr(&compFlags, compID, prefabID, index)) {
			return *compFlags & COMP_DELETED;
		}
	}
	return true;
}

bool Entity::destroy() {
	if (index > app->componentManager.getPrefab(prefabID)->size) { return false; }

	if (app->componentManager.getPrefab(prefabID)->flags & PREFAB_MEM_PACK) {
		if (index < app->componentManager.getPrefab(prefabID)->size - 1) {
			// overwrite with attributes from entity[size-1]
			for (u8 compID = 0; compID < COMP_COUNT; compID++) {
				if (app->componentManager.getPrefab(prefabID)->mask.test(compID)) {
					u8 *dst, *src;
					app->componentManager.getCompPtr(&dst, compID, prefabID, index);
					app->componentManager.getCompPtr(&src, compID, prefabID, app->componentManager.getPrefab(prefabID)->size - 1);
					memcpy(dst, src, compSize[compID]);
				}
			}
		}
		app->componentManager.getPrefab(prefabID)->size--;
	}
	else {
		// set components to deleted
		u8* compFlags = nullptr;
		for (u8 compID = 0; compID < COMP_COUNT; compID++) {
			if (app->componentManager.getCompPtr(&compFlags, compID, prefabID, index)) {
				*compFlags = COMP_DELETED;
			}
		}
	}
}


bool Entity::byName(std::string name) {
	return byIndex(app->componentManager.entityNames.getIndex(name));
}

bool Entity::byIndex(u32 globalIndex) {
	for (prefabID = 0; prefabID < app->componentManager.maxPrefabs; prefabID++) {
		if (globalIndex < app->componentManager.getPrefab(prefabID)->capacity) {
			index = globalIndex;
			return true;
		}
		globalIndex -= app->componentManager.getPrefab(prefabID)->capacity;
	}
	set(); // reset entity variables to default
	return false;
}

void Entity::setName(std::string name) {
	u32 globalIndex = getGlobalIndex();
	app->componentManager.entityNames.setIndex(name, globalIndex);
}

u32 Entity::getGlobalIndex() {
	return app->componentManager.getPrefab(prefabID)->entityIndex + index;
}
