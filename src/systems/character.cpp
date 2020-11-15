#include "systems/character.hpp"


void updateCharacterSystem(CB_PARAMS) {

	Entity cam(app);
	cam.set(app->componentManager.getPrefabID("camera"));
	Entity player(app);
	player.set(app->componentManager.getPrefabID("player"));

	vec2 inputDir;

	while (player.next()) {
		player.refInput();
		player.refCharacter();
		player.copyTransform();


		while (cam.next()) {
			cam.refInput();
			if (cam.Input->controllerId == player.Input->controllerId) {
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

		vec3 velocity(player.Input->axis[AXIS_LH], 0, player.Input->axis[AXIS_LV]);

		float mag = length(velocity);
		if (mag > 1) {
			velocity /= mag;
		}
		velocity *= player.Character->speed;

		player.Transform->pos.x += (velocity[0] * inputDir.x + velocity[2] * -inputDir.y) * evnt->dt;
		player.Transform->pos.y += velocity[1] * evnt->dt;
		player.Transform->pos.z += (velocity[0] * inputDir.y + velocity[2] * inputDir.x) * evnt->dt;
		player.syncTransform();
	}
}