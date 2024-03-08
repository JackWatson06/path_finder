/*
* Implementation of our game entity manager which wraps an array of entities loaded in memory. We wrap much
* of the functionality found in the GameEntity class.
*/
#include "headers/GameEntityManager.h"

using namespace Simplex;

Simplex::GameEntityManager* Simplex::GameEntityManager::instance = nullptr;

#pragma region Constructor / Destructor
Simplex::GameEntityManager::GameEntityManager() { Init(); }
Simplex::GameEntityManager::GameEntityManager(GameEntityManager const& other) { }
Simplex::GameEntityManager& Simplex::GameEntityManager::operator=(GameEntityManager const& other) { return *this; }
Simplex::GameEntityManager::~GameEntityManager() { Release(); };

void Simplex::GameEntityManager::Release(void)
{
	for (uint uEntity = 0; uEntity < entity_count; ++uEntity)
	{
		GameEntity* pEntity = entities[uEntity];
		SafeDelete(pEntity);
	}
	entity_count = 0;
	entities = nullptr;
}

void Simplex::GameEntityManager::Init(void)
{
	entity_count = 0;
	entities = nullptr;
}

Simplex::GameEntityManager* Simplex::GameEntityManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new GameEntityManager();
	}
	return instance;
}
void Simplex::GameEntityManager::ReleaseInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}
#pragma endregion

#pragma region Game State
int Simplex::GameEntityManager::GetEntityIndex(String entity_id)
{
	for (uint uIndex = 0; uIndex < entity_count; ++uIndex) {
		if (entity_id == entities[uIndex]->GetUniqueID()) {
			return uIndex;
		}
	}

	return -1;
}

void Simplex::GameEntityManager::AddEntity(String model_file_name, String entity_id)
{
	GameEntity* pTemp = new GameEntity(model_file_name, entity_id);
	
	if (pTemp->IsInitialized()) {
		GameEntity** tempArray = new GameEntity*[entity_count + 1];
		
		uint uCount = 0;
		for (uint i = 0; i < entity_count; ++i) {
			tempArray[uCount] = entities[i];
			++uCount;
		}
		tempArray[uCount] = pTemp;
		
		if (entities) {
			delete[] entities;
		}
		
		entities = tempArray;
		++entity_count;
	}
}

void Simplex::GameEntityManager::RemoveEntity(uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	if (entity_index != entity_count - 1) {
		std::swap(entities[entity_index], entities[entity_count - 1]);
	}

	GameEntity** tempArray = new GameEntity*[entity_count - 1];
	
	for (uint i = 0; i < entity_count - 1; ++i) {
		tempArray[i] = entities[i];
	}
	
	if (entities) {
		delete[] entities;
	}

	entities = tempArray;
	--entity_count;
}

void Simplex::GameEntityManager::RemoveEntity(String entity_id)
{
	int nIndex = GetEntityIndex(entity_id);
	RemoveEntity((uint)nIndex);
}

Simplex::String Simplex::GameEntityManager::GetUniqueID(uint entity_index)
{
	if (entity_count == 0) {
		return "";
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->GetUniqueID();
}

Simplex::uint Simplex::GameEntityManager::GetEntityCount(void) { return entity_count; }

Simplex::GameEntity* Simplex::GameEntityManager::GetEntity(uint entity_index)
{
	if (entity_count == 0) {
		return nullptr;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index];
}

void Simplex::GameEntityManager::Update(void)
{
	for (uint i = 0; i < entity_count; i++) {
		entities[i]->ClearCollisionList();
	}

	for (uint i = 0; i < entity_count; i++) {
		entities[i]->Update();
	}
}
#pragma endregion

#pragma region Collision
void Simplex::GameEntityManager::AddDimension(uint entity_index, uint dimension)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->AddDimension(dimension);
}

void Simplex::GameEntityManager::AddDimension(String entity_id, uint dimension)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	if (pTemp) {
		pTemp->AddDimension(dimension);
	}
}

void Simplex::GameEntityManager::RemoveDimension(uint entity_index, uint dimension)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->RemoveDimension(dimension);
}

void Simplex::GameEntityManager::RemoveDimension(String entity_id, uint dimension)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	if (pTemp) {
		pTemp->RemoveDimension(dimension);
	}
}

void Simplex::GameEntityManager::ClearDimensionSetAll(void)
{
	for (uint i = 0; i < entity_count; ++i) {
		ClearDimensionSet(i);
	}
}

void Simplex::GameEntityManager::ClearDimensionSet(uint entity_index)
{	
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->ClearDimensionSet();
}

void Simplex::GameEntityManager::ClearDimensionSet(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->ClearDimensionSet();
	}
}

bool Simplex::GameEntityManager::IsInDimension(uint entity_index, uint dimension)
{
	if (entity_count == 0) {
		return false;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->IsInDimension(dimension);
}

bool Simplex::GameEntityManager::IsInDimension(String entity_id, uint dimension)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		return pTemp->IsInDimension(dimension);
	}
	return false;
}

bool Simplex::GameEntityManager::SharesDimension(uint entity_index, GameEntity* const other_entity)
{
	if (entity_count == 0) {
		return false;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->SharesDimension(other_entity);
}

bool Simplex::GameEntityManager::SharesDimension(String entity_id, GameEntity* const other_entity)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		return pTemp->SharesDimension(other_entity);
	}
	return false;
}

void Simplex::GameEntityManager::DisableRigidBody(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	if (pTemp) {
		pTemp->DisableRigidBody();
	}

}

Simplex::ObjectRigidBody* Simplex::GameEntityManager::GetRigidBody(uint entity_index)
{
	if (entity_count == 0) {
		return nullptr;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->GetRigidBody();
}

Simplex::ObjectRigidBody* Simplex::GameEntityManager::GetRigidBody(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		return pTemp->GetRigidBody();
	}
	return nullptr;
}
#pragma endregion

#pragma region Physics
void Simplex::GameEntityManager::ApplyForce(vector3 force, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->ApplyForce(force);
	}
	return;
}

void Simplex::GameEntityManager::ApplyForce(vector3 force, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->ApplyForce(force);
}

void Simplex::GameEntityManager::SetPosition(vector3 position, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->SetPosition(position);
	}
	return;
}

void Simplex::GameEntityManager::SetPosition(vector3 position, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	entities[entity_index]->SetPosition(position);

	return;
}

void Simplex::GameEntityManager::SetMass(float mass, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->SetMass(mass);
	}
	return;
}

void Simplex::GameEntityManager::SetMass(float mass, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	entities[entity_index]->SetMass(mass);

	return;
}

void Simplex::GameEntityManager::UsePhysics(bool a_bUse, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->UsePhysics(a_bUse);
	}
	return;
}

void Simplex::GameEntityManager::UsePhysics(bool a_bUse, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->UsePhysics(a_bUse);
}
#pragma endregion

#pragma region Rendering
Simplex::Model* Simplex::GameEntityManager::GetModel(uint entity_index)
{
	if (entity_count == 0) {
		return nullptr;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->GetModel();
}
Simplex::Model* Simplex::GameEntityManager::GetModel(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	
	if (pTemp) {
		return pTemp->GetModel();
	}
	return nullptr;
}

Simplex::matrix4 Simplex::GameEntityManager::GetModelMatrix(uint entity_index)
{
	if (entity_count == 0) {
		return matrix4();
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->GetModelMatrix();
}
Simplex::matrix4 Simplex::GameEntityManager::GetModelMatrix(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		return pTemp->GetModelMatrix();
	}
	return IDENTITY_M4;
}

void Simplex::GameEntityManager::SetModelMatrix(matrix4 world_matrix, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	entities[entity_index]->SetModelMatrix(world_matrix);
}

void Simplex::GameEntityManager::SetModelMatrix(matrix4 world_matrix, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->SetModelMatrix(world_matrix);
	}
}


void Simplex::GameEntityManager::HideModelRender(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->HideModelRender();
	}
}

void Simplex::GameEntityManager::ShowModelRender(String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	if (pTemp) {
		pTemp->ShowModelRender();
	}

}

void Simplex::GameEntityManager::SetAxisVisibility(bool axis_visibility, uint entity_index)
{
	if (entity_count == 0) {
		return;
	}

	if (entity_index >= entity_count) {
		entity_index = entity_count - 1;
	}

	return entities[entity_index]->SetAxisVisible(axis_visibility);
}

void Simplex::GameEntityManager::SetAxisVisibility(bool axis_visibility, String entity_id)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);
	
	if (pTemp) {
		pTemp->SetAxisVisible(axis_visibility);
	}
}

void Simplex::GameEntityManager::AddEntityToRenderList(uint entity_index, bool rigid_body)
{
	if (entity_index >= entity_count) {
		for (entity_index = 0; entity_index < entity_count; ++entity_index) {
			entities[entity_index]->AddToRenderList(rigid_body);
		}
	} else {
		entities[entity_index]->AddToRenderList(rigid_body);
	}
}

void Simplex::GameEntityManager::AddEntityToRenderList(String entity_id, bool rigid_body)
{
	GameEntity* pTemp = GameEntity::GetEntity(entity_id);

	if (pTemp) {
		pTemp->AddToRenderList(rigid_body);
	}
}
#pragma endregion
