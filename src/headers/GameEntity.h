/*
* This header file defines a GameEntity which is simply an object that exists in our environment. Things like blocks,
* characters, or enemies are all game entities.
*/
#pragma once

#include "ObjectPhysics.h"
#include "ObjectRigidBody.h"

namespace Simplex
{
	class GameEntity
	{
		private:
		#pragma region Game Parameters
			String id = "";
			static std::map<String, GameEntity*> id_to_entity;
			bool entity_model_loaded_in_memory = false;
		#pragma endregion

		#pragma region Physics
			uint exists_in_dimensions_count = 0;
			uint* dimensions_lives_in = nullptr;
			bool should_use_physics = false;
			bool is_rigid_body_disabled = false;
		#pragma endregion

		#pragma region Rendering
			bool should_render_axis = false;
			matrix4 world_matrix = IDENTITY_M4;
			bool is_model_rendering_disabled = false;
		#pragma endregion

		#pragma region Model Attributes
			Model* model = nullptr;
			ObjectRigidBody* rigid_body = nullptr;
			MeshManager* mesh_manager = nullptr;
			ObjectPhysics* physics = nullptr;
		#pragma endregion

			void Release(void);
			void Init(void);
		public:

		#pragma region Constructor / Destructor
			GameEntity(String model_file, String unique_id = "NA");
			GameEntity(GameEntity const& other);
			GameEntity& operator=(GameEntity const& other);
			~GameEntity(void);
			void Swap(GameEntity& other);
			bool IsInitialized(void);
		#pragma endregion

		#pragma region Game State	
			void Update(void);
			static GameEntity* GetEntity(String unique_id);
			void GenUniqueID(String& unique_id);
			String GetUniqueID(void);
		#pragma endregion

		#pragma region Collision
			void AddDimension(uint dimension);
			void RemoveDimension(uint dimension);
			void ClearDimensionSet(void);
			bool IsInDimension(uint dimension);
			bool SharesDimension(GameEntity* const other_entity);
			void SortDimensions(void);
			bool IsColliding(GameEntity* const other);
			void ResolveCollision(GameEntity* a_pOther);
			ObjectRigidBody** GetColliderArray(void);
			uint GetCollidingCount(void);
			void ClearCollisionList(void);
		#pragma endregion

		#pragma region Physics
			void ApplyForce(vector3 force);
			void SetPosition(vector3 position);
			vector3 GetPosition(void);
			void SetVelocity(vector3 velocity);
			vector3 GetVelocity(void);
			void SetMass(float mass);
			float GetMass(void);
			ObjectPhysics* GetSolver(void);
			void UsePhysics(bool use_physics = true);
			ObjectRigidBody* GetRigidBody(void);
			void DisableRigidBody(void);
			bool HasThisRigidBody(ObjectRigidBody* rigid_body);
		#pragma endregion

		#pragma region Render
			Model* GetModel(void);
			matrix4 GetModelMatrix(void);
			void SetModelMatrix(matrix4 set_world_matrix);
			void AddToRenderList(bool draw_rigid_body = false);
			void SetAxisVisible(bool set_axis_enabled = true);
			void HideModelRender(void);
			void ShowModelRender(void);
		#pragma endregion
	};
}
