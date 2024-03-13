/*
* Implementation of our Tile class which helps model a board for the A Star algorithm to follow.
*/
#include "headers/Tile.h"

#pragma region Constructors / Destructors
Simplex::Tile::Tile() { Init(); }
Simplex::Tile::Tile(String entityId, vector3 entityPosition, int index) { 
	Init(); 
	entity_id = entityId;
	position = entityPosition;
	map_index_location = index;
}

Simplex::Tile::Tile(Tile const& other)
{
	game_entity_manager = GameEntityManager::GetInstance();
	entity_id = other.entity_id;
	active = other.active;
	position = other.position;
	a_star_parent = other.a_star_parent;
	map_index_location = other.map_index_location;
	a_star_g_value = other.a_star_g_value;
	a_star_h_value = other.a_star_h_value;
	a_star_f_value = other.a_star_f_value;
}

Simplex::Tile& Simplex::Tile::operator=(Tile const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Tile temp(other);
		Swap(temp);
	}
	return *this;
}

Simplex::Tile::~Tile(void) { Release(); }

void Simplex::Tile::Release() { }

void Simplex::Tile::Init()
{
	game_entity_manager = GameEntityManager::GetInstance();

	a_star_parent = nullptr;
	active = true;
	entity_id = "None";
	map_index_location = 0;
	position = vector3();

	a_star_h_value = 0;
	a_star_g_value = 0;
	a_star_f_value = 0;
}

void Simplex::Tile::Swap(Tile& other)
{
	std::swap(game_entity_manager, other.game_entity_manager);
	std::swap(entity_id, other.entity_id);
	std::swap(active, other.active);
	std::swap(position, other.position);
	std::swap(a_star_parent, other.a_star_parent);
	std::swap(map_index_location, other.map_index_location);
	std::swap(a_star_g_value, other.a_star_g_value);
	std::swap(a_star_h_value, other.a_star_h_value);
	std::swap(a_star_f_value, other.a_star_f_value);
}


void Simplex::Tile::ResetCell()
{
	a_star_parent = nullptr;
	a_star_h_value = 0;
	a_star_g_value = 0;
	a_star_f_value = 0;
}
#pragma endregion

#pragma region Getters
bool Simplex::Tile::GetActive(void) { return active; }
float Simplex::Tile::GetF(void) { return GetG() + a_star_h_value; }
float Simplex::Tile::GetG(void) { return a_star_g_value; }
float Simplex::Tile::GetH(void) { return a_star_h_value; }
int Simplex::Tile::GetIndex(void) { return map_index_location; }
Simplex::String Simplex::Tile::GetEntityId(void) { return entity_id; }
Simplex::vector3 Simplex::Tile::GetPosition(void) { return position + vector3(0.5, 0.0, 0.5); } // Add on to get center.
Simplex::Tile* Simplex::Tile::GetParent(void) { return a_star_parent; }
#pragma endregion

#pragma region Setters
void Simplex::Tile::EnableObstacle(void) { active = false; }
void Simplex::Tile::RemoveObstacle(void) { active = true; }
void Simplex::Tile::SetH(float value) { a_star_h_value = value; }
void Simplex::Tile::SetG(float value) { a_star_g_value = a_star_parent->GetG() + value; } // G is the value + the parents.
void Simplex::Tile::SetParent(Tile* value) { if (!a_star_parent) a_star_parent = value; }
#pragma endregion