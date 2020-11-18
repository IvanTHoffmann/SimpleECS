#pragma once

#include "SimpleECS/componentManager.hpp"
#include "SimpleECS/decl.hpp"

#include "compData.hpp" // TODO: remove dependency if possible

#define COMP_VAR(c) COMP_TYPE(c) c ## Copy;
#define COMP_PTR(c) COMP_TYPE(c)* c;
#define COPY_DECL(c) bool copy ## c();
#define SYNC_DECL(c) bool sync ## c();
#define REF_DECL(c) bool ref ## c();



/* TODO: Entity

Make entity finctions safer. Several of them rely on the entity being valid and the component data packed.
Consider writing general copy, sync, and ref functions accepting bitmasks.
Use the mask variable or get rid of it

//*/
class Entity {
private:
	Application* app;
	u32 index;
	u16 prefabID;

	void clearVars();
	FOREACH_COMP(COMP_VAR);

public:
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


	//compMask mask;
	FOREACH_COMP(COMP_PTR);
	FOREACH_COMP(COPY_DECL);
	FOREACH_COMP(SYNC_DECL);
	FOREACH_COMP(REF_DECL);

	//void _setMask(attrMask newMask); // this might be convenient for quickly checking if the necessary attributes are all enabled.
};