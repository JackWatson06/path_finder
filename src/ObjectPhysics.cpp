/*
* This file implements the core code for our physics object which we apply to our game entities to give them
* the ability to interact with the world.
*/
#include "headers/ObjectPhysics.h"

using namespace Simplex;

#pragma region Constructors / Destructors
void ObjectPhysics::Release(void) { }

void ObjectPhysics::Init(void)
{
	object_acceleration = ZERO_V3;
	object_position = ZERO_V3;
	object_velocity = ZERO_V3;
	object_mass = 1.0f;
}

ObjectPhysics::ObjectPhysics(void) { Init(); }

ObjectPhysics::ObjectPhysics(ObjectPhysics const& other)
{
	object_acceleration = other.object_acceleration;
	object_velocity = other.object_velocity;
	object_position = other.object_position;
	object_mass = other.object_mass;
}
ObjectPhysics& ObjectPhysics::operator=(ObjectPhysics const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		ObjectPhysics temp(other);
		Swap(temp);
	}
	return *this;
}
ObjectPhysics::~ObjectPhysics() { Release(); }

void ObjectPhysics::Swap(ObjectPhysics& other)
{
	std::swap(object_acceleration, other.object_acceleration);
	std::swap(object_velocity, other.object_velocity);
	std::swap(object_position, other.object_position);
	std::swap(object_mass, other.object_mass);
}
#pragma endregion

#pragma region Simulation Parameters
void ObjectPhysics::SetPosition(vector3 position) { object_position = position; }
vector3 ObjectPhysics::GetPosition(void) { return object_position; }

void ObjectPhysics::SetSize(vector3 size) { object_size = size; }
vector3 ObjectPhysics::GetSize(void) { return object_size; }

void ObjectPhysics::SetVelocity(vector3 velocity) { object_velocity = velocity; }
vector3 ObjectPhysics::GetVelocity(void) { return object_velocity; }

void ObjectPhysics::SetMass(float mass) { object_mass = mass; }
float ObjectPhysics::GetMass(void) { return object_mass; }

void ObjectPhysics::ApplyFriction(float friction)
{
	if (friction < 0.01f) {
		friction = 0.01f;
	}

	object_velocity *= 1.0f - friction;

	if (glm::length(object_velocity) < 0.01f) {
		object_velocity = ZERO_V3;
	}
}
void ObjectPhysics::ApplyForce(vector3 force)
{
	if (object_mass < 0.01f) {
		object_mass = 0.01f;
	}
	
	object_acceleration += force / object_mass;
}
#pragma endregion

vector3 CalculateMaxVelocity(vector3 velocity, float max_velocity)
{
	if (glm::length(velocity) > max_velocity) {
		velocity = glm::normalize(velocity);
		velocity *= max_velocity;
	}
	return velocity;
}
vector3 RoundSmallVelocity(vector3 velocity, float min_velocity = 0.01f)
{
	if (glm::length(velocity) < min_velocity) {
		velocity = ZERO_V3;
	}
	return velocity;
}
void ObjectPhysics::Update(void)
{
	ApplyForce(vector3(0.0f, -0.035f, 0.0f));

	object_velocity += object_acceleration;

	float fMaxVelocity = 5.0f;
	object_velocity = CalculateMaxVelocity(object_velocity, fMaxVelocity);

	ApplyFriction(0.1f);
	object_velocity = RoundSmallVelocity(object_velocity, 0.028f);

	object_position += object_velocity;

	if (object_position.y <= 0) {
		object_position.y = 0;
		object_velocity.y = 0;
	}

	object_acceleration = ZERO_V3;
}
void ObjectPhysics::ResolveCollision(ObjectPhysics* other_physics)
{
	float fMagThis = glm::length(object_velocity);
	float fMagOther = glm::length(object_velocity);

	if (fMagThis > 0.015f || fMagOther > 0.015f) {
		ApplyForce(-object_velocity);
		other_physics->ApplyForce(object_velocity);
	} else {
		vector3 v3Direction = object_position - other_physics->object_position;
		
		if (glm::length(v3Direction) != 0) {
			v3Direction = glm::normalize(v3Direction);
		}

		v3Direction *= 0.04f;
		ApplyForce(v3Direction);
		other_physics->ApplyForce(-v3Direction);
	}
}
