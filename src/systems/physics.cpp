#include "systems/physics.hpp"
#include "appData.hpp"


void initPhysicsSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	appData->physicsInfo.steps = 5;
	appData->physicsInfo.gravity = vec3(0, -9.8, 0);
}


void localToGlobalTransform(TransformComp* trans, TransformComp* other) {
	other->rot = normalize(trans->rot * other->rot);
	other->pos = trans->rot * other->pos;
	other->pos += trans->pos;
}


void getGlobalJointTransforms(COMP_TYPE(Transform)* globalA, COMP_TYPE(Transform)* globalB, Entity* constraint, Entity* bodyA = nullptr, Entity* bodyB = nullptr) {
	globalA->pos = vec3(constraint->Constraint->offsetA.pos);
	globalA->rot = quat(constraint->Constraint->offsetA.rot);
	globalB->pos = vec3(constraint->Constraint->offsetB.pos);
	globalB->rot = quat(constraint->Constraint->offsetB.rot);
	if (bodyA != nullptr) {
		localToGlobalTransform(bodyA->Transform, globalA);
	}
	if (bodyB != nullptr) {
		localToGlobalTransform(bodyB->Transform, globalB);
	}
};


void updatePhysicsSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	Entity body(app), body2(app), constraint(app);
	Entity *bodyA, *bodyB;
	TransformComp globalA, globalB;

	float dt = evnt->dt / appData->physicsInfo.steps;

	for (u8 i = 0; i < appData->physicsInfo.steps; i++) {
		body.set(app->componentManager.getPrefabID("rigidbody"));
		while (body.next()) {
			// move body
			body.copyTransform();
			body.copyRigidbody();

			body.Rigidbody->lastPos = body.Transform->pos;
			body.Rigidbody->lastRot = body.Transform->rot;
			body.Rigidbody->vel += appData->physicsInfo.gravity * dt;
			body.Transform->pos += body.Rigidbody->vel * dt;
			body.Transform->rot *= body.Rigidbody->spin * dt;

			body.syncTransform();
			body.syncRigidbody();
		}

		constraint.set(app->componentManager.getPrefabID("constraint"));
		while (constraint.next()) {
			// update constraints

			constraint.copyConstraint();

			if (constraint.Constraint->bodyA == INVALID_INDEX) {
				bodyA = nullptr;
			}
			else {
				bodyA = &body;
				body.set(-1, constraint.Constraint->bodyA);
			}
			if (constraint.Constraint->bodyB == INVALID_INDEX) {
				bodyB = nullptr;
			}
			else {
				bodyB = &body2;
				body2.set(-1, constraint.Constraint->bodyB);
			}

			body.copyTransform();
			body2.copyTransform();

			getGlobalJointTransforms(&globalA, &globalB, &constraint, bodyA, bodyB);

			// update body orientations

			// update body positions

			

			body.syncTransform();
			body2.syncTransform();
		}

		body.set(app->componentManager.getPrefabID("rigidbody"));
		while (body.next()) {
			// update body velocities

			body.refTransform();
			body.copyRigidbody();
			
			//std::cout << body.Transform->pos.y - body.Rigidbody->lastPos.y << '\n';

			body.Rigidbody->vel = (body.Transform->pos - body.Rigidbody->lastPos) / dt;
			body.Rigidbody->spin = (body.Transform->rot * conjugate(body.Rigidbody->lastRot)) / dt;

			body.syncRigidbody();

		}
	}
}