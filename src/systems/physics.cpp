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


void getGlobalJointTransforms(TransformComp* globalA, TransformComp* globalB, Entity* constraint, Entity* bodyA = nullptr, Entity* bodyB = nullptr) {
	ConstraintComp* c = (ConstraintComp*)constraint->get("Constraint");
	TransformComp* trans = (TransformComp*)bodyA->get("Transform");
	globalA->pos = vec3(c->offsets[0].pos);
	globalA->rot = quat(c->offsets[0].rot);
	globalB->pos = vec3(c->offsets[1].pos);
	globalB->rot = quat(c->offsets[1].rot);
	if (bodyA != nullptr) {
		localToGlobalTransform(trans, globalA);
	}
	if (bodyB != nullptr) {
		localToGlobalTransform(trans, globalB);
	}
};


void updatePhysicsSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	Entity* body = appData->ent;
	Entity* c0 = appData->ent2;
	TransformComp globalA, globalB;

	float dt = evnt->dt / appData->physicsInfo.steps;

	vec3 gravityDt = appData->physicsInfo.gravity * dt;

	for (u8 i = 0; i < appData->physicsInfo.steps; i++) {
		body->setPrefab("rigidbody");

		// predict position
		while (body->next()) { // move body
			if (GET(body, Rigidbody)->invMass)
			{
				GET(body, Rigidbody)->lastPos = GET(body, Transform)->pos;
				GET(body, Rigidbody)->lastRot = GET(body, Transform)->rot;
				GET(body, Rigidbody)->lastVel = GET(body, Rigidbody)->vel;
				GET(body, Rigidbody)->vel += gravityDt;
				GET(body, Transform)->pos += GET(body, Rigidbody)->vel * dt;
				GET(body, Transform)->rot *= GET(body, Rigidbody)->spin * dt;
			}
		}

		// correct position
		c0->setPrefab("constraint");
		while (c0->next()) { // update constraints

			if (IS_SHAPE(GET(c0, Constraint)->type)) {
				if (!GET(c0, Constraint)->body) {
					continue;
				}
				body->setGlobalIndex(GET(c0, Constraint)->body);

				if (GET(body, Transform)->pos.y < GET(body, Transform)->scale.y) {
					// colliding with ground
					GET(body, Transform)->pos.y = GET(body, Transform)->scale.y;
					GET(c0, Constraint)->flags |= COMP_CONSTRAINT_ACTIVE;
				}
				else {
					GET(c0, Constraint)->flags &= ~COMP_CONSTRAINT_ACTIVE;
				}
			}
		}

		// predict velocity
		body->setPrefab("rigidbody");
		while (body->next()) { // update body velocities
			//std::cout << GET(body, Transform)->pos.y - GET(body, Rigidbody)->lastPos.y << '\n';

			GET(body, Rigidbody)->vel = (GET(body, Transform)->pos - GET(body, Rigidbody)->lastPos) / dt;
			GET(body, Rigidbody)->spin = (GET(body, Transform)->rot * conjugate(GET(body, Rigidbody)->lastRot)) / dt;

		}

		// correct velocity
		c0->setPrefab("constraint");
		while (c0->next()) { // solve friction and restitution
			if (GET(c0, Constraint)->flags & COMP_CONSTRAINT_ACTIVE && IS_SHAPE(GET(c0, Constraint)->type)) {
				body->setGlobalIndex(GET(c0, Constraint)->body);

				GET(body, Rigidbody)->lastVel.y *= -1;
				GET(body, Rigidbody)->vel.y = GET(body, Rigidbody)->lastVel.y;
				GET(c0, Constraint)->flags &= ~COMP_CONSTRAINT_ACTIVE;
			}
		}
	}
}