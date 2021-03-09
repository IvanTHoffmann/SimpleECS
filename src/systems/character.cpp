#include "systems/character.hpp"


void updateCharacterSystem(CB_PARAMS) {
	Entity cam(app);
	Entity player(app);
	cam.setPrefab("camera");
	player.setPrefab("player");

	vec2 inputDir;

	while (player.next()) {
		player.refInput();
		player.refCharacter();
		player.copyTransform();
		player.copyRigidbody();


		while (cam.next()) {
			cam.refInput();
			if (cam.Input->HidMask & player.Input->HidMask) {
				cam.refTransform();
				inputDir.y = 2 * (cam.Transform->rot.x * cam.Transform->rot.z + cam.Transform->rot.w * cam.Transform->rot.y);
				inputDir.x = cam.Transform->rot.w * cam.Transform->rot.w -
					cam.Transform->rot.x * cam.Transform->rot.x -
					cam.Transform->rot.y * cam.Transform->rot.y +
					cam.Transform->rot.z * cam.Transform->rot.z;
				inputDir = normalize(inputDir);
				break;
			}
		}

		player.Rigidbody->vel.y -= 20 * evnt->dt;


		if (player.Transform->pos.y <= 5) { // grounded
			player.Transform->pos.y = 5;
			player.Rigidbody->vel.y = 0;

			player.Rigidbody->vel.x *= 1 - 5 * evnt->dt;
			player.Rigidbody->vel.z *= 1 - 5 * evnt->dt;

			float lh = app->inputManager.getButton(player.Input->HidMask, KEY_D) - app->inputManager.getButton(player.Input->HidMask, KEY_A) + app->inputManager.getAxis(player.Input->HidMask, AXIS_LH);
			float lv = app->inputManager.getButton(player.Input->HidMask, KEY_S) - app->inputManager.getButton(player.Input->HidMask, KEY_W) + app->inputManager.getAxis(player.Input->HidMask, AXIS_LV);

			lh *= abs(lh) > .2;
			lv *= abs(lv) > .2;


			vec3 localAccel(lh * inputDir.x - lv * inputDir.y, 0, lh * inputDir.y + lv * inputDir.x);
			float mag = length(localAccel);
			if (mag > 1) {
				localAccel /= mag;
			}
			localAccel *= player.Character->speed;
			player.Rigidbody->vel += localAccel * evnt->dt;

			u8 jumpButton = app->inputManager.getButton(player.Input->HidMask, 0);

			if (jumpButton == 3) {
				player.Rigidbody->vel.y += 10;
			}
		}

		player.Transform->pos += player.Rigidbody->vel * evnt->dt;
		player.syncTransform();
		player.syncRigidbody();
	}
}