#include "systems/character.hpp"
#include "appData.hpp"


void updateCharacterSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	Entity* cam = appData->ent;
	Entity* player = appData->ent2;
	cam->setPrefab("camera");
	player->setPrefab("player");

	vec2 inputDir = vec2(1, 0);

	while (player->next()) {
		u32 playerIndex;
		player->getGlobalIndex(&playerIndex);

		cam->setIndex(0);
		while (cam->next()) {
			if (GET(cam, Child)->parent == playerIndex) {
				inputDir.y = 2 * (GET(cam, Transform)->rot.x * GET(cam, Transform)->rot.z - GET(cam, Transform)->rot.w * GET(cam, Transform)->rot.y);
				inputDir.x = GET(cam, Transform)->rot.w * GET(cam, Transform)->rot.w +
					GET(cam, Transform)->rot.x * GET(cam, Transform)->rot.x -
					GET(cam, Transform)->rot.y * GET(cam, Transform)->rot.y +
					GET(cam, Transform)->rot.z * GET(cam, Transform)->rot.z;
				inputDir = normalize(inputDir);
				break;
			}
		}

		GET(player, Rigidbody)->vel.y -= 20 * evnt->dt;

		if (GET(player, Transform)->pos.y <= 5) { // grounded
			GET(player, Transform)->pos.y = 5;
			GET(player, Rigidbody)->vel.y = 0;

			GET(player, Rigidbody)->vel.x *= 1 - 10 * evnt->dt;
			GET(player, Rigidbody)->vel.z *= 1 - 10 * evnt->dt;

			float dx = app->inputManager.getInput("MOVE_RIGHT") - app->inputManager.getInput("MOVE_LEFT");
			float dy = app->inputManager.getInput("MOVE_BACKWARD") - app->inputManager.getInput("MOVE_FORWARD");

			vec3 localAccel(dx * inputDir.x - dy * inputDir.y, 0, dx * inputDir.y + dy * inputDir.x);
			float mag = length(localAccel);
			if (mag > 1) {
				localAccel /= mag;
			}
			localAccel *= GET(player, Character)->speed;
			GET(player, Rigidbody)->vel += localAccel * evnt->dt;

			if (app->inputManager.onInputSignal("JUMP")) {
				GET(player, Rigidbody)->vel.y += 10;
			}
		}

		GET(player, Transform)->pos += GET(player, Rigidbody)->vel * evnt->dt;
	}
}