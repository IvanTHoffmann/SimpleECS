#include "systems/ui.hpp"
#include "appData.hpp"


void updateUISystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity* ent = appData->ent;
	
	ent->setPrefab("text");
	while (ent->next()) {
		if (ent->getIndex() == 0) { // This assumes that the zeroth text entity is the fps counter
			appData->fpsSum -= appData->fpsSum >> 4;
			appData->fpsSum += (u16)(10 / evnt->dt) + 1;

			u16 fps = appData->fpsSum >> 4;
			u8 i = 0;
			if (fps >= 10000) {
				GET(ent, Text)->str[i++] = (fps / 10000) % 10 + '0';
			}
			if (fps >= 1000) {
				GET(ent, Text)->str[i++] = (fps / 1000) % 10 + '0';
			}
			GET(ent, Text)->str[i++] = (fps / 100) % 10 + '0';
			GET(ent, Text)->str[i++] = (fps / 10) % 10 + '0';
			GET(ent, Text)->str[i++] = '.';
			GET(ent, Text)->str[i++] = fps % 10 + '0';
			strcpy_s(GET(ent, Text)->str + i, 7, " fps \0");
		}
		
		vec3 textSize = app->assetManager.getTextSize(GET(ent, Text)->str, GET(ent, Text)->fontIndex);
		GET(ent, Text)->lineCount = (u8)textSize.y;
		GET(ent, Transform)->scale = GET(ent, Text)->fontSize * textSize;
	}
}