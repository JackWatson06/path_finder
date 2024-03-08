/*
* This file defines how a object works and interacts with other objects in our environment
*/
#include "headers/GameEntity.h"

using namespace Simplex;

std::map<String, GameEntity*> GameEntity::id_to_entity;

#pragma region Constructor / Destructor
Simplex::GameEntity::GameEntity(String model_file, String unique_id)
{
	Init();
	model = new Model();
	model->Load(model_file);

	if (model->GetName() != "") {
		GenUniqueID(unique_id);
		id = unique_id;
		id_to_entity[unique_id] = this;
		rigid_body = new ObjectRigidBody(model->GetVertexList());
		entity_model_loaded_in_memory = true;
	}

	physics = new ObjectPhysics();
}

Simplex::GameEntity::GameEntity(GameEntity const& other)
{
	entity_model_loaded_in_memory = other.entity_model_loaded_in_memory;
	model = other.model;
	is_rigid_body_disabled = other.is_rigid_body_disabled;
	is_model_rendering_disabled = other.is_model_rendering_disabled;
	rigid_body = new ObjectRigidBody(model->GetVertexList());
	world_matrix = other.world_matrix;
	mesh_manager = other.mesh_manager;
	id = other.id;
	should_render_axis = other.should_render_axis;
	exists_in_dimensions_count = other.exists_in_dimensions_count;
	dimensions_lives_in = other.dimensions_lives_in;
	physics = new ObjectPhysics(*other.physics);
}
GameEntity& Simplex::GameEntity::operator=(GameEntity const& other)
{
	if (this != &other) {
		Release();
		Init();
		GameEntity temp(other);
		Swap(temp);
	}
	return *this;
}

GameEntity::~GameEntity() { Release(); }

void Simplex::GameEntity::Swap(GameEntity& other)
{
	entity_model_loaded_in_memory = false;
	std::swap(model, other.model);
	std::swap(is_model_rendering_disabled, other.is_model_rendering_disabled);
	std::swap(is_rigid_body_disabled, other.is_rigid_body_disabled);
	std::swap(rigid_body, other.rigid_body);
	std::swap(world_matrix, other.world_matrix);
	std::swap(mesh_manager, other.mesh_manager);
	std::swap(entity_model_loaded_in_memory, other.entity_model_loaded_in_memory);
	std::swap(id, other.id);
	std::swap(should_render_axis, other.should_render_axis);
	std::swap(exists_in_dimensions_count, other.exists_in_dimensions_count);
	std::swap(dimensions_lives_in, other.dimensions_lives_in);
	std::swap(physics, other.physics);
}

bool Simplex::GameEntity::IsInitialized(void) { return entity_model_loaded_in_memory; }

void Simplex::GameEntity::Release(void)
{
	// The mesh manager will release the model of the entity.
	mesh_manager = nullptr;
	model = nullptr;
	if (dimensions_lives_in)
	{
		delete[] dimensions_lives_in;
		dimensions_lives_in = nullptr;
	}
	SafeDelete(rigid_body);
	SafeDelete(physics);
	id_to_entity.erase(id);
}

void Simplex::GameEntity::Init(void)
{
	mesh_manager = MeshManager::GetInstance();
	entity_model_loaded_in_memory = false;
	should_render_axis = false;
	is_model_rendering_disabled = false;
	is_rigid_body_disabled = false;
	model = nullptr;
	rigid_body = nullptr;
	dimensions_lives_in = nullptr;
	world_matrix = IDENTITY_M4;
	id = "";
	exists_in_dimensions_count = 0;
	should_use_physics = false;
	physics = nullptr;
}

#pragma endregion

#pragma region Game State
void Simplex::GameEntity::Update(void)
{
	if (should_use_physics) {
		physics->Update();
		SetModelMatrix(glm::translate(physics->GetPosition()) * glm::scale(physics->GetSize()));
	}
}

GameEntity* Simplex::GameEntity::GetEntity(String unique_id)
{
	//look the entity based on the unique id
	auto entity = id_to_entity.find(unique_id);
	//if not found return nullptr, if found return it
	return entity == id_to_entity.end() ? nullptr : entity->second;
}

void Simplex::GameEntity::GenUniqueID(String& unique_id)
{
	static uint index = 0;
	String sName = unique_id;
	GameEntity* pEntity = GetEntity(unique_id);

	while (pEntity) {
		unique_id = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(unique_id);
	}
	return;
}

String Simplex::GameEntity::GetUniqueID(void) { return id; }
#pragma endregion

#pragma region Collision
void Simplex::GameEntity::AddDimension(uint dimension)
{
	if (IsInDimension(dimension)) {
		return;
	}

	// Add the dimension into the dimensions_lives_in
	uint* pTemp;
	pTemp = new uint[exists_in_dimensions_count + 1];
	if (dimensions_lives_in) {
		memcpy(pTemp, dimensions_lives_in, sizeof(uint) * exists_in_dimensions_count);
		delete[] dimensions_lives_in;
		dimensions_lives_in = nullptr;
	}
	pTemp[exists_in_dimensions_count] = dimension;
	dimensions_lives_in = pTemp;

	++exists_in_dimensions_count;

	SortDimensions();
}

void Simplex::GameEntity::RemoveDimension(uint dimension)
{
	if (exists_in_dimensions_count == 0) {
		return;
	}

	for (uint i = 0; i < exists_in_dimensions_count; i++) {
		if (dimensions_lives_in[i] == dimension) {

			// Remove the dimension from the dimensions_lives_in
			std::swap(dimensions_lives_in[i], dimensions_lives_in[exists_in_dimensions_count - 1]);
			uint* pTemp;
			pTemp = new uint[exists_in_dimensions_count - 1];
			if (dimensions_lives_in) {
				memcpy(pTemp, dimensions_lives_in, sizeof(uint) * (exists_in_dimensions_count - 1));
				delete[] dimensions_lives_in;
				dimensions_lives_in = nullptr;
			}
			dimensions_lives_in = pTemp;

			--exists_in_dimensions_count;
			SortDimensions();
			return;
		}
	}
}

void Simplex::GameEntity::ClearDimensionSet(void)
{
	if (dimensions_lives_in) {
		delete[] dimensions_lives_in;
		dimensions_lives_in = nullptr;
	}
	exists_in_dimensions_count = 0;
}

bool Simplex::GameEntity::IsInDimension(uint dimension)
{
	for (uint i = 0; i < exists_in_dimensions_count; i++) {
		if (dimensions_lives_in[i] == dimension)
			return true;
	}
	return false;
}

bool Simplex::GameEntity::SharesDimension(GameEntity* const other_entity)
{
	if (0 == exists_in_dimensions_count) {
		if (0 == other_entity->exists_in_dimensions_count) {
			return true;
		}
	}

	for (uint i = 0; i < exists_in_dimensions_count; ++i) {
		for (uint j = 0; j < other_entity->exists_in_dimensions_count; j++) {
			if (dimensions_lives_in[i] == other_entity->dimensions_lives_in[j]) {
				return true;
			}
		}
	}

	return false;
}

void Simplex::GameEntity::SortDimensions(void)
{
	std::sort(dimensions_lives_in, dimensions_lives_in + exists_in_dimensions_count);
}

bool Simplex::GameEntity::IsColliding(GameEntity* const other)
{
	if (!entity_model_loaded_in_memory || !other->entity_model_loaded_in_memory) {
		return true;
	}


	if (is_rigid_body_disabled || other->is_rigid_body_disabled) {
		return false;
	}

	if (!SharesDimension(other)) {
		return false;
	}

	return rigid_body->IsColliding(other->GetRigidBody());
}

void Simplex::GameEntity::ResolveCollision(GameEntity* other_entity)
{
	if (should_use_physics) {
		physics->ResolveCollision(other_entity->GetSolver());
	}
}

Simplex::ObjectRigidBody::PRigidBody* Simplex::GameEntity::GetColliderArray(void) { return rigid_body->GetColliderArray(); }
uint Simplex::GameEntity::GetCollidingCount(void) { return rigid_body->GetCollidingCount(); }

void Simplex::GameEntity::ClearCollisionList(void)
{
	rigid_body->ClearCollidingList();
}
#pragma endregion

#pragma region Physics
void Simplex::GameEntity::ApplyForce(vector3 force)
{
	physics->ApplyForce(force);
}

void Simplex::GameEntity::SetPosition(vector3 position) { if (physics) physics->SetPosition(position); }

Simplex::vector3 Simplex::GameEntity::GetPosition(void)
{
	if (physics != nullptr)
		return physics->GetPosition();
	return vector3();
}

void Simplex::GameEntity::SetVelocity(vector3 velocity) { if (physics) physics->SetVelocity(velocity); }

Simplex::vector3 Simplex::GameEntity::GetVelocity(void)
{
	if (physics != nullptr)
		return physics->GetVelocity();
	return vector3();
}

void Simplex::GameEntity::SetMass(float mass) { if (physics) physics->SetMass(mass); }

float Simplex::GameEntity::GetMass(void)
{
	if (physics != nullptr)
		return physics->GetMass();
	return 1.0f;
}

Simplex::ObjectPhysics* Simplex::GameEntity::GetSolver(void) { return physics; }

void Simplex::GameEntity::UsePhysics(bool use_physics)
{
	should_use_physics = use_physics;
}

ObjectRigidBody* Simplex::GameEntity::GetRigidBody(void) { return rigid_body; }

void Simplex::GameEntity::DisableRigidBody()
{
	is_rigid_body_disabled = true;
}

bool Simplex::GameEntity::HasThisRigidBody(ObjectRigidBody* rigid_body) { return rigid_body == rigid_body; }
#pragma endregion

#pragma region Render
Model* Simplex::GameEntity::GetModel(void) { return model; }

matrix4 Simplex::GameEntity::GetModelMatrix(void) { return world_matrix; }

void Simplex::GameEntity::SetModelMatrix(matrix4 set_world_matrix)
{
	if (!entity_model_loaded_in_memory) {
		return;
	}

	world_matrix = set_world_matrix;
	model->SetModelMatrix(world_matrix);
	rigid_body->SetModelMatrix(world_matrix);

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(world_matrix, scale, rotation, translation, skew, perspective);

	physics->SetPosition(translation);
	physics->SetSize(scale);
}

void Simplex::GameEntity::AddToRenderList(bool draw_rigid_body)
{
	if (!entity_model_loaded_in_memory) {
		return;
	}

	if (!is_model_rendering_disabled) {
		model->AddToRenderList();
	}

	if (draw_rigid_body && !is_rigid_body_disabled) {
		rigid_body->AddToRenderList();
	}

	if (should_render_axis) {
		mesh_manager->AddAxisToRenderList(world_matrix);
	}
}

void Simplex::GameEntity::SetAxisVisible(bool set_axis_enabled) { should_render_axis = set_axis_enabled; }

void Simplex::GameEntity::HideModelRender()
{
	is_model_rendering_disabled = true;
}
void Simplex::GameEntity::ShowModelRender()
{
	is_model_rendering_disabled = false;
}
#pragma endregion
