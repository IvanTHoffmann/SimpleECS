#include "SimpleECS/util.hpp"

bool NameMap::getName(std::string* out, u16 index) {
	if (IDToStr.find(index) != IDToStr.end()) {
		*out = IDToStr[index];
		return true;
	}
	return false;
}

bool NameMap::getIndex(u16* index, std::string name) { 
	if (StrToID.find(name) != StrToID.end()) {
		*index = StrToID[name];
		return true;
	}
	return false;
}

void NameMap::add(std::string name, u16 index) { 
	StrToID[name] = index; 
	IDToStr[index] = name;
}

void NameMap::clear() { 
	StrToID.clear(); 
	IDToStr.clear(); 
}

void NameMap::remove(std::string name) {
	if (StrToID.find(name) != StrToID.end()) {
		IDToStr.erase(StrToID[name]);
		StrToID.erase(name);
	}
}

void NameMap::remove(u16 index) {
	if (IDToStr.find(index) != IDToStr.end()) {
		StrToID.erase(IDToStr[index]);
		IDToStr.erase(index);
	}
}

void printMatrix(mat4 m) {
	printf("\n| %f %f %f %f |\n| %f %f %f %f |\n| %f %f %f %f |\n| %f %f %f %f |\n",
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]
	);
}