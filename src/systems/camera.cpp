#include "systems/camera.hpp"


void updateCameraSystem(CB_PARAMS) {
	Entity camera(app);
	Entity parent(app);
	
	vec3 input;
	quat rot;

	camera.setPrefab("camera");
	while (camera.next()) {
		camera.refInput();
		camera.copyChild();

		// adjust the offset to the player

		camera.Child->offsetRot += vec3(camera.Input->axis[AXIS_RH], camera.Input->axis[AXIS_RV], 0);
		camera.Child->offsetRot.y = CLAMP(camera.Child->offsetRot.y, -1, 1);

		rot = angleAxis(camera.Child->offsetRot.y, vec3(1, 0, 0));
		rot *= angleAxis(camera.Child->offsetRot.x, vec3(0, 1, 0));
		rot = normalize(rot);

		//child.offsetRot += angleAxis(input.x * evnt->dt, vec3(0, 1, 0)) + angleAxis(input.y * evnt->dt, vec3(1, 0, 0));
		camera.syncChild();

		if (camera.Child->parent != (u16)-1) {
			parent.setGlobalIndex(camera.Child->parent);
			parent.refTransform();
			// Move the camera's position and orientation to the target offset.
			camera.copyTransform();
			camera.Transform->pos = camera.Child->offsetPos + parent.Transform->pos;
			//trans.rot = mix(trans.rot, rot, 20.0f * evnt->dt);// +parentTrans.rot;
			camera.Transform->rot = rot;
			camera.syncTransform();
		}
	}
}