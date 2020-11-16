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

void printMatrix(mat4 m) {
	printf("\n| %f %f %f %f |\n| %f %f %f %f |\n| %f %f %f %f |\n| %f %f %f %f |\n",
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	);
}