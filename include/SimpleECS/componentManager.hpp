#pragma once

class ComponentManager;

#include "SimpleECS/util.hpp"
#include "compData.hpp"

#define compMask std::bitset<COMP_COUNT>

#define COMP_DELETED 0x20 // This component belongs to a deleted entity.
#define	COMP_DISABLED 0x40 // This component has been disabled.
#define	COMP_NO_SYNC 0x80 // This component can only be changed by direct reference. Changes to a copy of this component cannot be copied back to the original.
#define	COMP_IS_COPY 0xf0 // This component is a copy of the original.


// TODO: COMP_COUNT should be sent to the component manager at runtime to remove ecsConstants.hpp dependency


enum {
	// The first two bits in the class flags are for the type of memory management.

	PREFAB_MEM_LIMIT,		// if space runs out, new entities will simply be ignored
	PREFAB_MEM_OVERFLOW,		// if space runs out, new entities should be added to misc memory
	PREFAB_MEM_OVERWRITE,	// if space runs out, the entity after the newest added entity will be overwritten.
	PREFAB_MEM_EXPAND,		// if space runs out, memory will be moved to expand the array.

	// bit flags begin here

	PREFAB_MEM_PACK = 0x4, // when an entity is deleted, the last entity should be moved into the vacant space. 
					 // Otherwise, deleted components will be flagged as deleted until they are overwritten.


	PREFAB_ACTIVE = 0x80,
};

// keeps track of the components dedicated to a specific class of entity
struct PrefabData {
	u8 flags;
	u32 entityIndex;
	u16 capacity, size, newest;
	u32 indices[COMP_COUNT];
	compMask mask;
};

// TODO: This should be a user defined variable
#define MAX_PREFABS 10

class ComponentManager {
private:
	u8* data; // pointer to the beggining of component data
	u32 dataSize;
	u32 arrays[COMP_COUNT]; // logs where each component array begins in data

	PrefabData prefabs[MAX_PREFABS];
	u8 prefabCount;


public:

	nameMap prefabNames, entityNames;
	u8 maxPrefabs = MAX_PREFABS;
	
	ComponentManager();
	~ComponentManager();

	u8 getPrefabCount() { return prefabCount; }
	u16 addPrefab(std::string name, u32 capacity, compMask mask, u8 flags = 0);
	void recalculateMemory();
	
	void removePrefab(u16 prefabID);
	void removePrefab(std::string name);
	void clearPrefabs();

	PrefabData* getPrefab(u16 id);
	PrefabData* getPrefab(std::string name);
	u16 getPrefabID(std::string name);

	bool getCompPtr(u8** p, u16 compID, u16 prefabID, u32 entIndex);
	bool getCompPtr(u8** p, u16 compID, u32 compIndex = 0);

};