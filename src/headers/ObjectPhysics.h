/*
* This header declares a physics object which we can attach to a game entity to control physics in the simulated
* game environment.
*/
#pragma once

#include "Simplex/Simplex.h"

namespace Simplex
{
	class ObjectPhysics
	{
		private:
			vector3 object_acceleration = ZERO_V3;
			vector3 object_position = ZERO_V3;
			vector3 object_size = vector3(1.0f);
			vector3 object_velocity = ZERO_V3;
			float object_mass = 1.0f;

			void Release(void);
			void Init(void);
		public:

		#pragma region Constructors / Destructors
			ObjectPhysics(void);
			ObjectPhysics(ObjectPhysics const& other);
			ObjectPhysics& operator=(ObjectPhysics const& other);
			~ObjectPhysics(void);
			void Swap(ObjectPhysics& other);
		#pragma endregion

		#pragma region Simulation Parameters
			void SetPosition(vector3 position);
			vector3 GetPosition(void);
			void SetSize(vector3 size);
			vector3 GetSize(void);
			void SetVelocity(vector3 velocity);
			vector3 GetVelocity(void);
			void SetMass(float mass);
			float GetMass(void);
			void ApplyFriction(float friction = 0.1f);
			void ApplyForce(vector3 force);
		#pragma endregion

			void Update(void);
			void ResolveCollision(ObjectPhysics* a_pOther);
	};
}
