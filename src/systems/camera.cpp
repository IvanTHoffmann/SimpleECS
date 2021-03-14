#include "systems/camera.hpp"
#include "appData.hpp"


void updateCameraSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	Entity* camera = appData->ent;
	Entity* parent = appData->ent2;

	vec3 input;
	quat rot;

	camera->setPrefab("camera");
	while (camera->next()) {
		// adjust the offset to the player

		float dx = app->inputManager.getInput("LOOK_RIGHT", evnt->dt) - app->inputManager.getInput("LOOK_LEFT", evnt->dt);
		float dy = app->inputManager.getInput("LOOK_UP", evnt->dt) - app->inputManager.getInput("LOOK_DOWN", evnt->dt);

		GET(camera, Child)->offsetRot += vec3(dx, dy, 0);
		GET(camera, Child)->offsetRot.y = clamp(GET(camera, Child)->offsetRot.y, -1.0f, 1.0f);

		rot = angleAxis(-GET(camera, Child)->offsetRot.x, vec3(0, 1, 0));
		rot *= angleAxis(-GET(camera, Child)->offsetRot.y, vec3(1, 0, 0));
		rot = normalize(rot);

		//child.offsetRot += angleAxis(input.x * evnt->dt, vec3(0, 1, 0)) + angleAxis(input.y * evnt->dt, vec3(1, 0, 0));

		if (parent->setGlobalIndex(GET(camera, Child)->parent)) {
			// Move the camera's position and orientation to the target offset.
			GET(camera, Transform)->pos = GET(camera, Child)->offsetPos + GET(parent, Transform)->pos;
			//trans.rot = mix(trans.rot, rot, 20.0f * evnt->dt);// +parentTrans.rot;
			GET(camera, Transform)->rot = rot;
		}
	}
}