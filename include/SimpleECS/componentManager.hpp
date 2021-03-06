#pragma once

#include "SimpleECS/decl.hpp"
#include "compData.hpp"

#include <bitset>
#define COMP_COUNT 32
#define compMask std::bitset<COMP_COUNT> // Bitset isn't expandable. Use vector<bool> or custom class instead

#define COMP_DELETED 0x20 // This component belongs to a deleted entity.
#define	COMP_DISABLED 0x40 // This component has been disabled.
#define	COMP_NO_SYNC 0x80 // This component can only be changed by direct reference. Changes to a copy of this component cannot be copied back to the original.
#define	COMP_IS_COPY 0xf0 // This component is a copy of the original.

#define CONTINUE_IF_DISABLED(c) if ((c)->flags & COMP_DISABLED) { continue; }

enum {
	// The first two bits in the class flags are for the type of memory management.

	// TODO: Implement these memory management flags
	PREFAB_MEM_LIMIT,		// if space runs out, new entities will simply be ignored
	PREFAB_MEM_OVERFLOW,	// if space runs out, new entities should be added to misc memory
	PREFAB_MEM_OVERWRITE,	// if space runs out, the entity after the most recently added entity will be overwritten.
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
	compMask mask;
	vector<u32> indices;
};

struct CompData {
	size_t size;
	u32 count, startPos;
};

class ComponentManager {
private:
	Application* app;

	vector<u8> data;
	vector<PrefabData> prefabs;

public:
	vector<CompData> components;

	NameMap prefabNames, entityNames, componentNames;
	
	ComponentManager(Application* _app);
	~ComponentManager();
	void setApp(Application* _app);

	u16 addComponent(string name, size_t size);

	u8 getPrefabCount() { return prefabs.size(); }
	u16 addPrefab(std::string name, u32 capacity, compMask mask, u8 flags = 0);
	void recalculateMemory();
	
	void removePrefab(u16 prefabID);
	void removePrefab(std::string name);
	void clearPrefabs();

	bool getPrefab(PrefabData**, u16 id);
	bool getPrefab(PrefabData**, std::string name);
	bool getPrefabID(u16*, std::string name);
	bool getPrefabName(std::string* out, u16 id);

	bool getCompPtr(u8** p, u16 compID, u16 prefabID, u32 entIndex);
	bool getCompPtr(u8** p, u16 compID, u32 compIndex = 0);
};