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
	globalA->pos = vec3(constraint->Constraint->offsets[0].pos);
	globalA->rot = quat(constraint->Constraint->offsets[0].rot);
	globalB->pos = vec3(constraint->Constraint->offsets[1].pos);
	globalB->rot = quat(constraint->Constraint->offsets[1].rot);
	if (bodyA != nullptr) {
		localToGlobalTransform(bodyA->Transform, globalA);
	}
	if (bodyB != nullptr) {
		localToGlobalTransform(bodyB->Transform, globalB);
	}
};


void updatePhysicsSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	Entity body(app), c0(app), c1(app);
	Entity *a, *b;
	TransformComp globalA, globalB;

	float dt = evnt->dt / appData->physicsInfo.steps;

	vec3 gravityDt = appData->physicsInfo.gravity * dt;

	for (u8 i = 0; i < appData->physicsInfo.steps; i++) {
		body.setPrefab("rigidbody");

		// predict position
		while (body.next()) { // move body
			body.copyTransform();
			body.copyRigidbody();

			if (body.Rigidbody->invMass)
			{
				body.Rigidbody->lastPos = body.Transform->pos;
				body.Rigidbody->lastRot = body.Transform->rot;
				body.Rigidbody->lastVel = body.Rigidbody->vel;
				body.Rigidbody->vel += gravityDt;
				body.Transform->pos += body.Rigidbody->vel * dt;
				body.Transform->rot *= body.Rigidbody->spin * dt;

				body.syncTransform();
				body.syncRigidbody();
			}
		}

		// correct position
		c0.setPrefab("constraint");
		while (c0.next()) { // update constraints
			c0.refConstraint();

			if (IS_SHAPE(c0.Constraint->type)) {
				if (!c0.Constraint->body) {
					continue;
				}
				body.setGlobalIndex(c0.Constraint->body);

				body.copyTransform();
				body.copyRigidbody();

				if (body.Transform->pos.y < body.Transform->scale.y) {
					// colliding with ground
					body.Transform->pos.y = body.Transform->scale.y;
					c0.Constraint->flags |= CONSTRAINT_ACTIVE;
				}
				else {
					c0.Constraint->flags &= ~CONSTRAINT_ACTIVE;
				}

				body.syncTransform();
				body.syncRigidbody();
			}
		}

		// predict velocity
		body.setPrefab("rigidbody");
		while (body.next()) { // update body velocities
			body.refTransform();
			body.copyRigidbody();
			
			//std::cout << body.Transform->pos.y - body.Rigidbody->lastPos.y << '\n';

			body.Rigidbody->vel = (body.Transform->pos - body.Rigidbody->lastPos) / dt;
			body.Rigidbody->spin = (body.Transform->rot * conjugate(body.Rigidbody->lastRot)) / dt;

			body.syncRigidbody();

		}

		// correct velocity
		c0.setPrefab("constraint");
		while (c0.next()) { // solve friction and restitution
			c0.refConstraint();

			if (c0.Constraint->flags & CONSTRAINT_ACTIVE && IS_SHAPE(c0.Constraint->type)) {
				body.setGlobalIndex(c0.Constraint->body);

				body.copyRigidbody();

				body.Rigidbody->lastVel.y *= -1;
				body.Rigidbody->vel.y = body.Rigidbody->lastVel.y;

				body.syncRigidbody();
				c0.Constraint->flags &= ~CONSTRAINT_ACTIVE;
			}
		}
	}
}