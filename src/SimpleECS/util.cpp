#include "SimpleECS/util.hpp"

std::string nameMap::getName(u16 index) { 
	return "error :("; 
}

u16 nameMap::getIndex(std::string name) { 
	return map[name] - 1; 
}

void nameMap::setIndex(std::string name, u16 index) { 
	map[name] = index + 1; 
}

void nameMap::clear() { 
	map.clear(); 
}

void nameMap::removeByName(std::string name) { 
	map.erase(name); 
}