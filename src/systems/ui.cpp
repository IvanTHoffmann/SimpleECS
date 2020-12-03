#include "systems/ui.hpp"
#include "appData.hpp"


void updateUISystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity ent(app);
	
	ent.setPrefab("text");
	while (ent.next()) {
		ent.copyText();

		if (ent.getIndex() == 0) { // This assumes that the zeroth text entity is the fps counter
			appData->fpsSum -= appData->fpsSum >> 2;
			appData->fpsSum += (u16)(10 / evnt->dt) + 1;

			u16 fps = appData->fpsSum >> 2;
			u8 i = 0;
			if (fps >= 10000) {
				ent.Text->str[i++] = (fps / 10000) % 10 + '0';
			}
			if (fps >= 1000) {
				ent.Text->str[i++] = (fps / 1000) % 10 + '0';
			}
			ent.Text->str[i++] = (fps / 100) % 10 + '0';
			ent.Text->str[i++] = (fps / 10) % 10 + '0';
			ent.Text->str[i++] = '.';
			ent.Text->str[i++] = fps % 10 + '0';
			strcpy_s(ent.Text->str + i, 7, " fps \0");
		}
		
		
		vec3 textSize = app->assetManager.getTextSize(ent.Text->str, ent.Text->fontIndex);
		ent.Text->lineCount = (u8)textSize.y;
		ent.syncText();

		ent.copyTransform();
		ent.Transform->scale = ent.Text->fontSize * textSize;
		ent.syncTransform();
	}
}