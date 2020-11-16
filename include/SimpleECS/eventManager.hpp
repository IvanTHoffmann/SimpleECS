#pragma once

#include "SimpleECS/decl.hpp"
#include "SimpleECS/util.hpp"


struct Event;
#define CB_PARAMS Application* app, Event* evnt
#define CB_ARGS app, evnt
typedef void (*cbFunc)(CB_PARAMS);

struct Event {
	cbFunc func;
	void* data;
	size_t dataSize;
	timePoint lastTime, targetTime;
	float dt;
	bool deleted;
};


// TODO: These should be user defined variables
#define MAX_EVENTS 16
#define MAX_NEW_EVENTS 16


class EventManager {
private:
	Event events[MAX_EVENTS], newEvents[MAX_NEW_EVENTS];
	u8 newEventCount;

	u8 eventsCapacity = 16, newEventsCapacity = 16;

public:
	EventManager();
	~EventManager();

	/*
	void setEventsCapacity(u16 n);
	void setNewEventsCapacity(u16 n);
	*/

	void dispatch(cbFunc func, void* data = 0);
	void processEvent(Application* app);
};
