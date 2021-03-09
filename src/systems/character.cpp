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
			if (cam.Input->mask & player.Input->mask) {
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

			vec3 localAccel(player.Input->axis[AXIS_LH] * inputDir.x + player.Input->axis[AXIS_LV] * -inputDir.y, 0,
				player.Input->axis[AXIS_LH] * inputDir.y + player.Input->axis[AXIS_LV] * inputDir.x);
			float mag = length(localAccel);
			if (mag > 1) {
				localAccel /= mag;
			}
			localAccel *= player.Character->speed;
			player.Rigidbody->vel += localAccel * evnt->dt;

			if (player.Input->button[BUTTON_A] == 3) {
				player.Rigidbody->vel.y += 10;
			}
		}

		player.Transform->pos += player.Rigidbody->vel * evnt->dt;
		player.syncTransform();
		player.syncRigidbody();
	}
}