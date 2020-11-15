#include "systems/ui.hpp"
#include "appData.hpp"


void updateUISystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity ent(app);

	
	
	ent.set(app->componentManager.getPrefabID("text"));
	while (ent.next()) {
		ent.copyText();

		if (ent.getIndex() == 0) { // This assumes that the zeroth text entity is the fps counter
			float dtSum = evnt->dt;
			//*
			for (u8 i = 0; i < 9; i++) {
				appData->dtHistory[i] = appData->dtHistory[i + 1];
				dtSum += appData->dtHistory[i];
			}
			appData->dtHistory[9] = evnt->dt;
			dtSum /= 10.0;
			//*/

			u16 fps = (u16)(10 / dtSum) + 1;
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
			strcpy(ent.Text->str + i, " fps \0");
		}
		
		
		vec3 textSize = app->assetManager.getTextSize(ent.Text->str, ent.Text->fontIndex);
		ent.Text->lineCount = textSize.y;
		ent.syncText();

		ent.copyTransform();
		ent.Transform->scale = ent.Text->fontSize * textSize;
		ent.syncTransform();
	}
}