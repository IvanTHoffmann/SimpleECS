#include "SimpleECS/eventManager.hpp"


EventManager::EventManager() {
	/*
	eventsCapacity = 0;
	newEventsCapacity = 0;
	*/

	//events = new Event[eventsCapacity];
	//newEvents = new Event[newEventsCapacity];

	newEventCount = 0;
	for (u8 i = 0; i < eventsCapacity; i++) {
		events[i].deleted = true;
	}
}

EventManager::~EventManager() {
	
}

/*
void EventManager::setEventsCapacity(u16 n) {
	if (eventsCapacity) {
		delete events;
	}

}

void EventManager::setNewEventsCapacity(u16 n) {
	newEventsCapacity = n;
}
*/

void EventManager::dispatch(cbFunc func, void* data) {
	if (newEventCount >= newEventsCapacity) {
		printf("ERROR: max new events reached\n");
		return;
	}
	newEvents[newEventCount].func = func;
	newEvents[newEventCount].data = data;
	newEvents[newEventCount].lastTime = CUR_TIME;
	newEvents[newEventCount].targetTime = newEvents[newEventCount].lastTime;
	newEventCount++;
}

void EventManager::processEvent(Application* app) {

	// TODO: replace this with some sort of standard library queue
	// or a binary tree where elements are sorted by their timepoint values

	timePoint time;

	for (u8 eventId = 0; eventId < eventsCapacity; eventId++) {
		if (events[eventId].deleted) {
			// Replace this deleted event with a new event if there is one waiting.
			if (newEventCount) {
				events[eventId] = newEvents[--newEventCount];
				events[eventId].deleted = false;
			}
		}
		else if (events[eventId].targetTime <= (time = CUR_TIME)) {
			events[eventId].dt = TIME_DIFF(events[eventId].lastTime, time).count() / 1000000000.0f;
			events[eventId].lastTime = time;
			events[eventId].deleted = true; // this can be changed from within the callback for repeating events
			events[eventId].func(app, events + eventId);
		}
	}
}