#pragma once

#include "SimpleECS/componentManager.hpp"
#include "SimpleECS/decl.hpp"

#include "compData.hpp" // TODO: remove dependency if possible

#define GET(ent, comp) ((comp ## Comp*)(ent)->get(comp ## Enum))

/* TODO: Entity

Make entity finctions safer. Several of them rely on the entity being valid and the component data packed.
Consider writing general copy, sync, and ref functions accepting bitmasks.
compMask mask; or hasComponents(BitMask);

//*/
class Entity {
private:
	Application* app;
	u32 index;
	u16 prefabID;

public:
	vector<u8*> pointers;
	Entity(Application* app);

	bool create(std::string prefabName);
	bool create(u16 prefabID);
	
	bool byName(std::string name);
	bool setGlobalIndex(u32 globalIndex);

	bool setPrefab(std::string prefabName);
	bool setPrefab(u16 prefabID);
	bool setIndex(u16 localIndex);
	void setName(std::string name);

	bool next();
	bool next(compMask reqMask, compMask exclMask = 0);
	bool isDeleted();
	bool destroy();

	u32 getIndex() { return index; }
	u16 getPrefabID() { return prefabID; }
	bool getGlobalIndex(u32* id);

	bool ref(string name);
	bool ref(u16 id);
	void* get(string name);
	void* get(u16 id);
};