/*
* The entity manager holds a list of the entities within our game world.
*/
#pragma once

#include "GameEntity.h"

namespace Simplex
{
	class GameEntityManager
	{
		private: 
			uint entity_count = 0;
			GameEntity** entities = nullptr;
			static GameEntityManager* instance;

			GameEntityManager(void);
			GameEntityManager(GameEntityManager const& other);
			GameEntityManager& operator=(GameEntityManager const& other);
			~GameEntityManager(void);
			void Release(void);
			void Init(void);
		public:
		
		#pragma region Constructor / Destructor
			static GameEntityManager* GetInstance();
			static void ReleaseInstance(void);
		#pragma endregion
		
		#pragma region Game State
			int GetEntityIndex(String entity_id);
			void AddEntity(String model_file_name, String entity_id = "NA");
			void RemoveEntity(uint entity_index);
			void RemoveEntity(String entity_id);
			String GetUniqueID(uint entity_index = -1);
			uint GetEntityCount(void);
			GameEntity* GetEntity(uint entity_index = -1);
			void Update(void);
		#pragma endregion

		#pragma region Collision
			void AddDimension(uint entity_index, uint dimension);
			void AddDimension(String entity_id, uint dimension);
			void RemoveDimension(uint entity_index, uint dimension);
			void RemoveDimension(String entity_id, uint dimension);
			void ClearDimensionSetAll(void);
			void ClearDimensionSet(uint entity_index);
			void ClearDimensionSet(String entity_id);
			bool IsInDimension(uint entity_index, uint dimension);
			bool IsInDimension(String entity_id, uint dimension);
			bool SharesDimension(uint entity_index, GameEntity* const other_entity);
			bool SharesDimension(String entity_id, GameEntity* const other_entity);
			void DisableRigidBody(String entity_id);
			ObjectRigidBody* GetRigidBody(uint entity_index = -1);
			ObjectRigidBody* GetRigidBody(String entity_id);
		#pragma endregion

		#pragma region Physics
			void ApplyForce(vector3 force, String entity_id);
			void ApplyForce(vector3 force, uint entity_index = -1);
			void SetPosition(vector3 position, String entity_id);
			void SetPosition(vector3 position, uint entity_index = -1);
			void SetMass(float mass, String entity_id);
			void SetMass(float position, uint entity_index = -1);
			void UsePhysics(bool use_physics, String entity_id);
			void UsePhysics(bool use_physics = true, uint entity_index = -1);
		#pragma endregion

		#pragma region Rendering
			Model* GetModel(uint entity_index = -1);
			Model* GetModel(String entity_id);
			matrix4 GetModelMatrix(uint entity_index = -1);
			matrix4 GetModelMatrix(String entity_id);
			void SetModelMatrix(matrix4 world_matrix, uint entity_index = -1);
			void SetModelMatrix(matrix4 world_matrix, String entity_id);
			void HideModelRender(String entity_id);
			void ShowModelRender(String entity_id);
			void SetAxisVisibility(bool axis_visibility, uint entity_index = -1);
			void SetAxisVisibility(bool axis_visibility, String entity_id);
			void AddEntityToRenderList(uint entity_index = -1, bool rigid_body = false);
			void AddEntityToRenderList(String entity_id, bool rigid_body = false);
		#pragma endregion
	};
}
