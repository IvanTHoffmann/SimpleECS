#pragma once

#include "util.hpp"
#include "SimpleECS/application.hpp"
#include "compData.hpp" // TODO: remove depenency if possible

#define COMP_VAR(c) COMP_TYPE(c) c ## Copy;
#define COMP_PTR(c) COMP_TYPE(c)* c;
#define COPY_DECL(c) bool copy ## c();
#define SYNC_DECL(c) bool sync ## c();
#define REF_DECL(c) bool ref ## c();

/* TODO: 

Make entity finctions safer. Several of them rely on the entity being valid and the component data packed.
Consider writing general copy, sync, and ref functions accepting bitmasks.

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

	bool set(u16 cID = -1, u16 arrayIndex = -1);
	bool next();
	bool next(compMask reqMask, compMask exclMask = 0);
	bool getNew(u16 cID);
	bool isDeleted();
	bool destroy();
	bool byName(std::string name);
	bool byIndex(u32 globalIndex);
	u16 getIndex() { return index; }
	u16 getPrefab() { return prefabID; }
	u16 getGlobalIndex() { return app->componentManager.getPrefab(prefabID)->entityIndex + index; }

	void setName(std::string name);

	compMask mask;
	FOREACH_COMP(COMP_PTR);
	FOREACH_COMP(COPY_DECL);
	FOREACH_COMP(SYNC_DECL);
	FOREACH_COMP(REF_DECL);

	//void _setMask(attrMask newMask); // this might be convenient for quickly checking if the necessary attributes are all enabled.
};