/*
* Define a rigid body which enables collision checking amongst different entities in the game world.
*/

#include "headers/ObjectRigidBody.h"
using namespace Simplex;

#pragma region Constructors / Destructors
void ObjectRigidBody::Release(void)
{
	mesh_manager = nullptr;
	ClearCollidingList();
}

void ObjectRigidBody::Init(void)
{
	mesh_manager = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3CenterL = ZERO_V3;
	m_v3CenterG = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	world_matrix = IDENTITY_M4;

	m_uCollidingCount = 0;
	m_CollidingArray = nullptr;
}

ObjectRigidBody::ObjectRigidBody(std::vector<vector3> point_list)
{
	Init();

	uint uVertexCount = point_list.size();

	if (uVertexCount == 0) {
		return;
	}

	m_v3MaxL = m_v3MinL = point_list[0];

	for (uint i = 1; i < uVertexCount; ++i) {
		if (m_v3MaxL.x < point_list[i].x) m_v3MaxL.x = point_list[i].x;
		else if (m_v3MinL.x > point_list[i].x) m_v3MinL.x = point_list[i].x;

		if (m_v3MaxL.y < point_list[i].y) m_v3MaxL.y = point_list[i].y;
		else if (m_v3MinL.y > point_list[i].y) m_v3MinL.y = point_list[i].y;

		if (m_v3MaxL.z < point_list[i].z) m_v3MaxL.z = point_list[i].z;
		else if (m_v3MinL.z > point_list[i].z) m_v3MinL.z = point_list[i].z;
	}

	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	m_v3CenterL = (m_v3MaxL + m_v3MinL) / 2.0f;
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	m_fRadius = glm::distance(m_v3CenterL, m_v3MinL);
}

ObjectRigidBody::ObjectRigidBody(ObjectRigidBody const& other)
{
	mesh_manager = other.mesh_manager;

	m_bVisibleBS = other.m_bVisibleBS;
	m_bVisibleOBB = other.m_bVisibleOBB;
	m_bVisibleARBB = other.m_bVisibleARBB;

	m_fRadius = other.m_fRadius;

	m_v3ColorColliding = other.m_v3ColorColliding;
	m_v3ColorNotColliding = other.m_v3ColorNotColliding;

	m_v3CenterL = other.m_v3CenterL;
	m_v3CenterG = other.m_v3CenterG;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;
	m_v3ARBBSize = other.m_v3ARBBSize;

	world_matrix = other.world_matrix;

	m_uCollidingCount = other.m_uCollidingCount;
	m_CollidingArray = other.m_CollidingArray;
}

ObjectRigidBody& ObjectRigidBody::operator=(ObjectRigidBody const& other)
{
	if (this != &other) {
		Release();
		Init();
		ObjectRigidBody temp(other);
		Swap(temp);
	}
	return *this;
}

ObjectRigidBody::~ObjectRigidBody() { Release(); };

void ObjectRigidBody::Swap(ObjectRigidBody& other)
{
	std::swap(mesh_manager, other.mesh_manager);
	std::swap(m_bVisibleBS, other.m_bVisibleBS);
	std::swap(m_bVisibleOBB, other.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, other.m_bVisibleARBB);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3ColorColliding, other.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, other.m_v3ColorNotColliding);

	std::swap(m_v3CenterL, other.m_v3CenterL);
	std::swap(m_v3CenterG, other.m_v3CenterG);
	std::swap(m_v3MinL, other.m_v3MinL);
	std::swap(m_v3MaxL, other.m_v3MaxL);

	std::swap(m_v3MinG, other.m_v3MinG);
	std::swap(m_v3MaxG, other.m_v3MaxG);

	std::swap(m_v3HalfWidth, other.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, other.m_v3ARBBSize);

	std::swap(world_matrix, other.world_matrix);

	std::swap(m_uCollidingCount, other.m_uCollidingCount);
	std::swap(m_CollidingArray, other.m_CollidingArray);
}
#pragma endregion

#pragma region Rigid Body Parameters
bool ObjectRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void ObjectRigidBody::SetVisibleBS(bool visible) { m_bVisibleBS = visible; }
bool ObjectRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void ObjectRigidBody::SetVisibleOBB(bool visible) { m_bVisibleOBB = visible; }
bool ObjectRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void ObjectRigidBody::SetVisibleARBB(bool visible) { m_bVisibleARBB = visible; }
float ObjectRigidBody::GetRadius(void) { return m_fRadius; }
vector3 ObjectRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
void ObjectRigidBody::SetColorColliding(vector3 color) { m_v3ColorColliding = color; }
vector3 ObjectRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void ObjectRigidBody::SetColorNotColliding(vector3 color) { m_v3ColorNotColliding = color; }
vector3 ObjectRigidBody::GetCenterLocal(void) { return m_v3CenterL; }
vector3 ObjectRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 ObjectRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 ObjectRigidBody::GetCenterGlobal(void) { return m_v3CenterG; }
vector3 ObjectRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 ObjectRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 ObjectRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
#pragma endregion

#pragma region Collision
void ObjectRigidBody::ClearCollidingList(void)
{
	m_uCollidingCount = 0;
	if (m_CollidingArray) {
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
}
void ObjectRigidBody::AddCollisionWith(ObjectRigidBody* other)
{
	if (IsInCollidingArray(other)) {
		return;
	}

	ObjectRigidBody** pTemp;
	pTemp = new ObjectRigidBody*[m_uCollidingCount + 1];
	if (m_CollidingArray) {
		memcpy(pTemp, m_CollidingArray, sizeof(ObjectRigidBody*) * m_uCollidingCount);
		delete[] m_CollidingArray;
		m_CollidingArray = nullptr;
	}
	pTemp[m_uCollidingCount] = other;
	m_CollidingArray = pTemp;

	++m_uCollidingCount;
}
void ObjectRigidBody::RemoveCollisionWith(ObjectRigidBody* other)
{
	if (m_uCollidingCount == 0) {
		return;
	}

	for (uint i = 0; i < m_uCollidingCount; i++) {
		if (m_CollidingArray[i] == other) {

			// Remove the object rigid body from the array of colliding objects with this rigid body.
			std::swap(m_CollidingArray[i], m_CollidingArray[m_uCollidingCount - 1]);
			ObjectRigidBody** pTemp;
			pTemp = new ObjectRigidBody*[m_uCollidingCount - 1];
			if (m_CollidingArray)
			{
				memcpy(pTemp, m_CollidingArray, sizeof(uint) * (m_uCollidingCount - 1));
				delete[] m_CollidingArray;
				m_CollidingArray = nullptr;
			}
			m_CollidingArray = pTemp;

			--m_uCollidingCount;
			return;
		}
	}
}
bool ObjectRigidBody::IsColliding(ObjectRigidBody* const other)
{
	bool bColliding = true;
	//bColliding = (glm::distance(GetCenterGlobal(), other->GetCenterGlobal()) < m_fRadius + other->m_fRadius);
	//if they are check the Axis Aligned Bounding Box
	if (bColliding) //they are colliding with bounding sphere
	{
		// right or left
		if (this->m_v3MaxG.x < other->m_v3MinG.x) bColliding = false;
		if (this->m_v3MinG.x > other->m_v3MaxG.x) bColliding = false;

		// above or below
		if (this->m_v3MaxG.y < other->m_v3MinG.y) bColliding = false;
		if (this->m_v3MinG.y > other->m_v3MaxG.y) bColliding = false;

		// behind or in front
		if (this->m_v3MaxG.z < other->m_v3MinG.z) bColliding = false;
		if (this->m_v3MinG.z > other->m_v3MaxG.z) bColliding = false;

		if (bColliding) {
			this->AddCollisionWith(other);
			other->AddCollisionWith(this);
		} else  {
			this->RemoveCollisionWith(other);
			other->RemoveCollisionWith(this);
		}
	} else {
		this->RemoveCollisionWith(other);
		other->RemoveCollisionWith(this);
	}
	return bColliding;
}
ObjectRigidBody** ObjectRigidBody::GetColliderArray(void) { return m_CollidingArray; }
uint ObjectRigidBody::GetCollidingCount(void) { return m_uCollidingCount; }

bool ObjectRigidBody::IsInCollidingArray(ObjectRigidBody* other_rigid_body)
{
	for (uint i = 0; i < m_uCollidingCount; i++) {
		if (m_CollidingArray[i] == other_rigid_body)
			return true;
	}
	return false;
}
#pragma endregion

#pragma region Rendering
void ObjectRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS) {
		if (m_uCollidingCount > 0) mesh_manager->AddWireSphereToRenderList(glm::translate(world_matrix, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else mesh_manager->AddWireSphereToRenderList(glm::translate(world_matrix, m_v3CenterL) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	
	if (m_bVisibleOBB) {
		if (m_uCollidingCount > 0) mesh_manager->AddWireCubeToRenderList(glm::translate(world_matrix, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else mesh_manager->AddWireCubeToRenderList(glm::translate(world_matrix, m_v3CenterL) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	
	if (m_bVisibleARBB) {
		if (m_uCollidingCount > 0) mesh_manager->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else mesh_manager->AddWireCubeToRenderList(glm::translate(m_v3CenterG) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

matrix4 ObjectRigidBody::GetModelMatrix(void) { return world_matrix; }

void ObjectRigidBody::SetModelMatrix(matrix4 model_matrix)
{
	if (model_matrix == world_matrix) {
		return;
	}

	world_matrix = model_matrix;

	m_v3CenterG = vector3(world_matrix * vector4(m_v3CenterL, 1.0f));

	//Calculate the 8 corners of the cube
	vector3 v3Corner[8];
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex) {
		v3Corner[uIndex] = vector3(world_matrix * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
#pragma endregion