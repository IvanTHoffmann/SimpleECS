#include "systems/character.hpp"


void updateCharacterSystem(CB_PARAMS) {
	Entity cam(app);
	Entity player(app);
	cam.setPrefab("camera");
	player.setPrefab("player");

	vec2 inputDir = vec2(1, 0);

	while (player.next()) {
		player.refCharacter();
		player.copyTransform();
		player.copyRigidbody();


		u32 playerIndex;
		player.getGlobalIndex(&playerIndex);

		while (cam.next()) {
			cam.refChild();
			if (cam.Child->parent == playerIndex) {
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

			float dx = app->inputManager.getInput(MOVE_RIGHT) - app->inputManager.getInput(MOVE_LEFT);
			float dy = app->inputManager.getInput(MOVE_BACKWARD) - app->inputManager.getInput(MOVE_FORWARD);

			vec3 localAccel(dx * inputDir.x - dy * inputDir.y, 0, dx * inputDir.y + dy * inputDir.x);
			float mag = length(localAccel);
			if (mag > 1) {
				localAccel /= mag;
			}
			localAccel *= player.Character->speed;
			player.Rigidbody->vel += localAccel * evnt->dt;

			if (app->inputManager.onInputSignal(JUMP)) {
				player.Rigidbody->vel.y += 10;
			}
		}

		player.Transform->pos += player.Rigidbody->vel * evnt->dt;
		player.syncTransform();
		player.syncRigidbody();
	}
}