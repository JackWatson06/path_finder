/*
* Declare a rigid body which handles the bounding box of a entity. We use this mainly for checking collisions with other
* entities in the game scene.
*/

#pragma once

#include "Simplex\Mesh\Model.h"

namespace Simplex
{
	class ObjectRigidBody
	{
		protected:
			MeshManager* mesh_manager = nullptr;

			bool m_bVisibleBS = false;
			bool m_bVisibleOBB = true;
			bool m_bVisibleARBB = true;

			float m_fRadius = 0.0f;

			vector3 m_v3ColorColliding = C_RED;
			vector3 m_v3ColorNotColliding = C_WHITE;

			vector3 m_v3CenterL = ZERO_V3;
			vector3 m_v3CenterG = ZERO_V3;

			vector3 m_v3MinL = ZERO_V3;
			vector3 m_v3MaxL = ZERO_V3;

			vector3 m_v3MinG = ZERO_V3;
			vector3 m_v3MaxG = ZERO_V3;

			vector3 m_v3HalfWidth = ZERO_V3;
			vector3 m_v3ARBBSize = ZERO_V3;

			matrix4 world_matrix = IDENTITY_M4;

			uint m_uCollidingCount = 0;
			ObjectRigidBody** m_CollidingArray = nullptr;

		public:
		#pragma region Constructors / Destructors
			ObjectRigidBody(std::vector<vector3> point_list);
			ObjectRigidBody(ObjectRigidBody const& other);
			ObjectRigidBody& operator=(ObjectRigidBody const& other);
			~ObjectRigidBody(void);
			void Swap(ObjectRigidBody& other);
		#pragma endregion

		#pragma region Rigid Body Parameters
			bool GetVisibleBS(void);
			void SetVisibleBS(bool visible);
			bool GetVisibleOBB(void);
			void SetVisibleOBB(bool visible);
			bool GetVisibleARBB(void);
			void SetVisibleARBB(bool visible);
			float GetRadius(void);
			vector3 GetColorColliding(void);
			void SetColorColliding(vector3 color);
			vector3 GetColorNotColliding(void);
			void SetColorNotColliding(vector3 color);
			vector3 GetCenterLocal(void);
			vector3 GetMinLocal(void);
			vector3 GetMaxLocal(void);
			vector3 GetCenterGlobal(void);
			vector3 GetMinGlobal(void);
			vector3 GetMaxGlobal(void);
			vector3 GetHalfWidth(void);
		#pragma endregion

		#pragma region Collision
			void ClearCollidingList(void);
			void AddCollisionWith(ObjectRigidBody* other);
			void RemoveCollisionWith(ObjectRigidBody* other);
			bool IsColliding(ObjectRigidBody* const other);
			ObjectRigidBody** GetColliderArray(void);
			uint GetCollidingCount(void);
			bool IsInCollidingArray(ObjectRigidBody* other_rigid_body);
		#pragma endregion

		#pragma region Rendering
			void AddToRenderList(void);
			matrix4 GetModelMatrix(void);
			void SetModelMatrix(matrix4 model_matrix);
		#pragma endregion
		
		private:
			void Release(void);
			void Init(void);
	};
}
