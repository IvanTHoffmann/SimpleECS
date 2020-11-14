#include "SimpleECS/application.hpp"


Application::Application(void* d) {
	data = d;
	running = false;
}

Application::~Application() {
	
}

void* Application::getData() {
	return data;
}

int Application::run() {
	running = true;
	while (running) {
		eventManager.processEvent(this);
	}
	return 0;
}

void Application::stop() {
	running = false;
}