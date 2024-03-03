#include "Tile.h"



/// <summary>
/// Default Constructor
/// </summary>
Simplex::Tile::Tile() { Init(); }



/// <summary>
/// Constructor
/// </summary>
/// <param name="entityId">Entity associated with tile</param>
/// <param name="position">Position of tile</param>
/// <param name="index">Index of tile in world map</param>
Simplex::Tile::Tile(String entityId, vector3 position, int index) { 
	Init(); 
	m_entityId = entityId;
	m_position = position;
	m_mapIndex = index;
}



/// <summary>
/// Copy asignment
/// </summary>
/// <param name="other"></param>
Simplex::Tile::Tile(Tile const& other)
{
	m_entityManager = MyEntityManager::GetInstance();
	m_entityId = other.m_entityId;
	m_active = other.m_active;
	m_position = other.m_position;
	m_parent = other.m_parent;
	m_mapIndex = other.m_mapIndex;
	m_g = other.m_g;
	m_h = other.m_h;
	m_f = other.m_f;
}


/// <summary>
/// = sign operator
/// </summary>
/// <param name="other">Other tile</param>
/// <returns></returns>
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


/// <summary>
/// Destructor
/// </summary>
/// <param name=""></param>
Simplex::Tile::~Tile(void) { Release(); }




/// <summary>
/// Swap out the variables when passing in another tile.
/// </summary>
/// <param name="other"></param>
void Simplex::Tile::Swap(Tile& other)
{
	std::swap(m_entityManager, other.m_entityManager);
	std::swap(m_entityId, other.m_entityId);
	std::swap(m_active, other.m_active);
	std::swap(m_position, other.m_position);
	std::swap(m_parent, other.m_parent);
	std::swap(m_mapIndex, other.m_mapIndex);
	std::swap(m_g, other.m_g);
	std::swap(m_h, other.m_h);
	std::swap(m_f, other.m_f);
}



/// <summary>
/// Initialize class member variables
/// </summary>
void Simplex::Tile::Init()
{
	m_entityManager = MyEntityManager::GetInstance();

	m_parent = nullptr;
	m_active = true;
	m_entityId = "None";
	m_mapIndex = 0;
	m_position = vector3();

	m_h = 0;
	m_g = 0;
	m_f = 0;
}



/// <summary>
/// Release all the tiles memory (nothing to be release. All on stack)
/// </summary>
void Simplex::Tile::Release(){ }



/// <summary>
/// Reset the cell to it's default values.
/// </summary>
void Simplex::Tile::ResetCell()
{
	m_parent = nullptr;
	m_h = 0;
	m_g = 0;
	m_f = 0;
}



// Getters for class member variables.
bool Simplex::Tile::GetActive(void) { return m_active; }
float Simplex::Tile::GetF(void) { return GetG() + m_h; }
float Simplex::Tile::GetG(void) { return m_g; }
float Simplex::Tile::GetH(void) { return m_h; }
int Simplex::Tile::GetIndex(void) { return m_mapIndex; }
Simplex::String Simplex::Tile::GetEntityId(void) { return m_entityId; }
Simplex::vector3 Simplex::Tile::GetPosition(void) { return m_position + vector3(0.5, 0.0, 0.5); } // Add on to get center.
Simplex::Tile* Simplex::Tile::GetParent(void) { return m_parent; }

/// Setters for the tiles variables
void Simplex::Tile::EnableObstacle(void) { m_active = false; }
void Simplex::Tile::RemoveObstacle(void) { m_active = true; }
void Simplex::Tile::SetH(float value) { m_h = value; }
void Simplex::Tile::SetG(float value) { m_g = m_parent->GetG() + value; } // G is the value + the parents.
void Simplex::Tile::SetParent(Tile* value) { if (!m_parent) m_parent = value; }