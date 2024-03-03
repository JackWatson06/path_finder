#include "MyAStar.h"


Simplex::MyAStar* Simplex::MyAStar::m_instance = nullptr;
/// <summary>
/// Get an instance of the A Star Map
/// </summary>
/// <returns></returns>
Simplex::MyAStar* Simplex::MyAStar::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new MyAStar(20, 20);
	}
	return m_instance;
}



/// <summary>
/// Release memoery for this singleton
/// </summary>
void Simplex::MyAStar::ReleaseInstance()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
	}
}

/// <summary>
/// Constuctor
/// </summary>
/// <param name="width">Width of map</param>
/// <param name="height">Height of map</param>
Simplex::MyAStar::MyAStar(int width, int height) { 
	Init(); 

	m_mapWidth = width;
	m_mapHeight = height;
	m_mapSize = width * height;
	m_tileList = new Tile[m_mapSize];

	m_totalHidden = (width * height) * 0.25f;

	std::srand(std::time(nullptr));

	// Generate the map tiles.
	for (uint i = 0; i < width * height; i++)
	{
		float x = (float)((int)(i / width));
		float z = i % height;
		vector3 tilePosition = vector3(x, m_floorLevel, z);

		String entityId = "Cube_" + std::to_string(i);


		Tile tile(entityId, tilePosition, i);
		m_tileList[i] = tile; // <= Don't know why that is yelling at me (for me it is underlined green)


		CreateEntity("Minecraft\\Cube.obj", entityId, tilePosition);
		m_entityManager->DisableRigidBody(entityId);

	}

	// Create the creeper which will act as the finder.
	CreateEntity("Minecraft\\Creeper.obj", "Creeper", RandomFreeTile()->GetPosition());
	m_entityManager->UsePhysicsSolver();
	m_finder = m_entityManager->GetEntity();

	// Create Steve which will act as the target
	CreateEntity("Minecraft\\Steve.obj", "Steve", RandomFreeTile()->GetPosition());
	m_entityManager->UsePhysicsSolver();
	m_target = m_entityManager->GetEntity();
	
}



/// <summary>
/// Create a new entity
/// </summary>
/// <param name="fileName">File we are creating entity from</param>
/// <param name="entityId">The Entity Id we are using</param>
/// <param name="position">Position of the entity</param>
void Simplex::MyAStar::CreateEntity(String fileName, String entityId, vector3 position)
{
	// Refactor to custom block creation method.
	m_entityManager->AddEntity(fileName, entityId);

	matrix4 m4Position = glm::translate(position);
	m_entityManager->SetModelMatrix(m4Position);
}




/// <summary>
/// Get a random free tile on the map.
/// </summary>
/// <returns>Pointer to a random free tile.</returns>
Simplex::Tile* Simplex::MyAStar::RandomFreeTile()
{
	do
	{
		int randomIndex = std::rand() % m_mapSize;

		Tile* tile = &m_tileList[randomIndex];

		// Need to check to see if the tile we just removed is beneath the enemies
		if (tile->GetActive() && NoOneStandingOnTile(tile))
		{
			return tile;
		}

	} while (true && m_hiddenTileList.size() < m_mapSize); // Keep going until we can't no more. Double check that we are not hidding more than we should!
}



/// <summary>
/// Test to see if anyone is standing on the tile inputed
/// </summary>
/// <param name="tile">Tile to test</param>
/// <returns>True or false</returns>
bool Simplex::MyAStar::NoOneStandingOnTile(Tile* tile)
{
	if (m_target && PointInsideTile(m_target->GetPosition(), tile)) return false;
	if (m_finder && PointInsideTile(m_finder->GetPosition(), tile)) return false;
	return true;
}


// The Big Three
Simplex::MyAStar::MyAStar(MyAStar const& other){ }
Simplex::MyAStar& Simplex::MyAStar::operator=(MyAStar const& other){ return *this; }
Simplex::MyAStar::~MyAStar(void) { Destroy(); }


/// <summary>
/// Get the total obstacle count
/// </summary>
/// <returns>Total hidden obstacles</returns>
int Simplex::MyAStar::GetTotalObstacleCount(){ return m_totalHidden; }



/// <summary>
/// Initilize Member Variables
/// </summary>
void Simplex::MyAStar::Init()
{
	m_entityManager = MyEntityManager::GetInstance();
	m_meshManager = MeshManager::GetInstance();
	m_system = SystemSingleton::GetInstance();
	m_calcAStar = true;
	m_mapWidth = 0;
	m_mapHeight = 0;
	m_mapSize = 0;
	m_floorLevel = -1;
	m_tileList = new Tile[m_mapSize];

	m_totalHidden = 0;
	m_targetLastPosition = vector3();

	m_currentTravelFrom = vector3();
	m_currentTravelTo = vector3();
}



/// <summary>
/// Tear down the class.
/// </summary>
void Simplex::MyAStar::Destroy()
{
	delete[] m_tileList;
	m_tileList = nullptr;
}



/// <summary>
/// Add an hole to the map. Then we want to rerun the A Star algorithm from this
/// </summary>
void Simplex::MyAStar::AddObstacle()
{
	if (GetCurrentHidden() >= m_totalHidden)
		return;

	Tile* tile = RandomFreeTile();
	tile->EnableObstacle();
	m_entityManager->HideModelRender(tile->GetEntityId());
	m_hiddenTileList.push_back(tile);
		
	m_calcAStar = true;
	OverrideAStarTileCheck();
}



/// <summary>
/// Remove an hole in the ground in the map.
/// </summary>
void Simplex::MyAStar::RemoveObstacle()
{

	int currentHidden = GetCurrentHidden();
	if (currentHidden == 0)
		return;

	int randomIndex = std::rand() % currentHidden;

	Tile* tile = m_hiddenTileList[randomIndex];
	tile->RemoveObstacle();
	m_entityManager->ShowModelRender(tile->GetEntityId());

	m_hiddenTileList.erase(m_hiddenTileList.begin() + randomIndex);

	m_calcAStar = true;
	OverrideAStarTileCheck();
}



/// <summary>
/// Update the map
/// </summary>
void Simplex::MyAStar::Update()
{

	static uint sClock = m_system->GenClock();
	float fDelta = m_system->GetDeltaTime(sClock);

	CheckIfWeNeedAStarUpdate();

	if (m_calcAStar)
	{
		Tile* startTile = this->GetClosestTile(m_finder->GetPosition());
		Tile* endTile = this->GetClosestTile(m_target->GetPosition());
		if(ValidateAStarInput(startTile, endTile)) CalculateAStarBetweenTiles(startTile, endTile);
		m_calcAStar = false;
	}

	if (WeHavePaths())
	{
		MoveSeeker();
	}

	if (CheckCollisions())
	{
		Collide(fDelta);
	}

}




/// <summary>
/// Render method for the class. We use this to render all of the paths currently available to us.
/// </summary>
void Simplex::MyAStar::AddLinesToRenderList()
{
	for (uint i = 1; i < m_paths.size(); i++)
	{
		m_meshManager->AddLineToRenderList(IDENTITY_M4, m_paths[i], m_paths[i - 1], C_BLUE, C_BLUE);
	}
}



/// <summary>
/// Check if we moved the target at all. If we have then flag that we want the A Star Algo to try to run
/// </summary>
void Simplex::MyAStar::CheckIfWeNeedAStarUpdate()
{
	vector3 targetPosition = m_target->GetPosition();
	if (m_targetLastPosition.x != targetPosition.x ||
		m_targetLastPosition.z != targetPosition.z)
		m_calcAStar = true;
}



/// <summary>
/// Get the tile that contains the x, z point passed in (this is because we are on a 2D grid)
/// </summary>
/// <param name="position">Position we are testing to get the closest tile.</param>
/// <returns>Tile</returns>
Simplex::Tile* Simplex::MyAStar::GetClosestTile(vector3 position)
{

	for (uint i = 0; i < m_mapSize; i++)
	{
		Tile* tile = &m_tileList[i];

		if(PointInsideTile(position, tile))
			return tile;

	}

	return nullptr;
}



/// <summary>
/// Check to see if this point is inside the tile.
/// </summary>
/// <param name="position">Point. We will only beusing x, and z</param>
/// <param name="tile">The tile we are checking</param>
/// <returns>True or false</returns>
bool Simplex::MyAStar::PointInsideTile(vector3 position, Tile* tile)
{
	MyRigidBody* tileRigidBody = m_entityManager->GetRigidBody(tile->GetEntityId());

	vector3 blockCenter = tileRigidBody->GetCenterGlobal();
	vector3 halfWidth = tileRigidBody->GetHalfWidth();

	if (position.x <= blockCenter.x + halfWidth.x &&
		position.x >= blockCenter.x - halfWidth.x &&
		position.z <= blockCenter.z + halfWidth.z &&
		position.z >= blockCenter.z - halfWidth.z)
	{
		return true;
	}

	return false;

}




/// <summary>
/// Determine if the tiles both the start, and end tiles are valid
/// </summary>
/// <param name="startTile">Start tile which we will be seeking from</param>
/// <param name="endTile">End tile which we are seeking</param>
/// <returns>True or false</returns>
bool Simplex::MyAStar::ValidateAStarInput(Tile* startTile, Tile* endTile)
{
	// Don't run AStar if we are not actively on a tile.
	if (endTile == nullptr ||
		startTile == nullptr)
	{
		// Update the last target position if we have moved inside the tile at all. 
		// Just cause we move the target does not mean we have to re-run the A Star Algorithm
		m_targetLastPosition = m_target->GetPosition();
		return false;
	}


	// Don't run a star if we are on the same tile as last time we ran it.
	if (endTile == m_targetLastTile)
	{
		m_targetLastPosition = m_target->GetPosition();
		return false;
	}


	// Don't run A Star if both tiles are inactive. (This is for floating Steve.)
	if (!endTile->GetActive() ||
		!startTile->GetActive())
	{
		m_targetLastPosition = m_target->GetPosition();
		return false;
	}

	return true;

}



/// <summary>
/// Override the last target tile so that we can get past the tile check in the A Star Validate
/// (yes there is a better way to do this ... for another time)
/// </summary>
void Simplex::MyAStar::OverrideAStarTileCheck()
{
	m_targetLastPosition = vector3();
	m_targetLastTile = nullptr;
}



/// <summary>
/// Restart the from, and to travel vectso to their default so we can restart path finding from a new path.
/// </summary>
void Simplex::MyAStar::RestartPathFinding()
{
	m_currentTravelFrom = vector3();
	m_currentTravelTo = vector3();
}



/// <summary>
/// Calculate the A Star Path between two start, and end tiles.
/// </summary>
/// <param name="startTile">Seeker Tile</param>
/// <param name="endTile">Target Tile</param>
void Simplex::MyAStar::CalculateAStarBetweenTiles(Tile* startTile, Tile* endTile)
{
	AStarSetup(startTile, endTile);

	// If the A Star was succesful then get the path from the tiles
	if (AStarAlgo(startTile, endTile, 0))
	{
		GetPathFromTiles(startTile, endTile);
	}

	m_targetLastTile = endTile;
}



/// <summary>
/// Setup the A Star Algo. Reset from last run, then calculate heuristic value.
/// </summary>
/// <param name="startTile">Seeker Tile</param>
/// <param name="endTile">Target Tile</param>
void Simplex::MyAStar::AStarSetup(Tile* startTile, Tile* endTile)
{

	// Reset all cells, and then get the heuristic value for active cells.
	for (uint i = 0; i < m_mapSize; i++)
	{
		Tile* tile = &m_tileList[i];
		tile->ResetCell();

		vector3 tilePosition = tile->GetPosition();
		vector3 endTilePosition = endTile->GetPosition();
		float bigAppleDistance = m_mapSize;

		if (tile->GetActive())
			bigAppleDistance = glm::abs(endTilePosition.x - tilePosition.x) + glm::abs(endTilePosition.z - tilePosition.z);

		tile->SetH(bigAppleDistance);
	}


	// Reset the tiles.
	m_openList.clear();
	m_closedList.clear();
	m_paths.clear();


	// Prep the first tile.
	startTile->SetParent(startTile);
	startTile->SetG(0);
	m_openList.push_back(startTile);

}




/// <summary>
/// Get the backwards path from the end tile then reverse the path.
/// </summary>
/// <param name="startTile">Seeker Tile</param>
/// <param name="endTile">Target Tile</param>
void Simplex::MyAStar::GetPathFromTiles(Tile* startTile, Tile* endTile)
{
	// Step Four. Get the reverse path from the end tile. That is the path.
	Tile* currentTile = endTile;

	while (true)
	{
		vector3 position = currentTile->GetPosition();
		position.y = -m_floorLevel;

		m_paths.push_back(position);

		if (currentTile == startTile) break;
		currentTile = currentTile->GetParent();
	}

	std::reverse(m_paths.begin(), m_paths.end());
}




/// <summary>
/// Run the recursive A Star algorithm to find the best path possible.
/// </summary>
/// <param name="currentTile">The curren tile we are on</param>
/// <param name="endTile">The target tile</param>
/// <param name="openIndex">Which open list index is the current tile. (This is so that we can remove it from the list)</param>
bool Simplex::MyAStar::AStarAlgo(Tile* currentTile, Tile* endTile, int openIndex)
{
	m_openList.erase(m_openList.begin() + openIndex);
	m_closedList.push_back(currentTile);


	// End case
	if (currentTile == endTile) return true;


	// Get the x, and z vars from the index.
	int index = currentTile->GetIndex();

	int x = (int)(index / m_mapWidth);
	int z = index % m_mapHeight;


	// Check the tiles around the list.
	for (uint i = 0; i < 4; i++)
	{
		int checkX = x;
		int checkZ = z;

		if (i == 0) checkX--;
		else if (i == 2) checkX++;
		else if (i == 1) checkZ--;
		else if (i == 3) checkZ++;

		// We found a valid cell in the map
		if (checkX >= 0 && checkZ >= 0 && checkZ < m_mapHeight && checkX < m_mapWidth)
		{
			Tile* tile = &m_tileList[checkZ + (checkX * m_mapWidth)];

			// Skip the inactive tiles. And the tiles which already have a parent
			if (!tile->GetActive()) continue;
			if (tile->GetParent()) continue;

			// Set the parent of the neighbhor tiles to the current cell.
			tile->SetParent(currentTile);
			tile->SetG(1.0);

			m_openList.push_back(tile);
		}

	}



	// Find the lowest F Value in the list!
	int lowestIndex = 0;
	for (int i = 0; i < m_openList.size(); i++)
	{
		if (m_openList[i]->GetF() < m_openList[lowestIndex]->GetF())
			lowestIndex = i;
	}

	// Other edge case where we ran out of open options. This can occur if the target is unreachable.
	if (m_openList.size() == 0) return false;

	return AStarAlgo(m_openList[lowestIndex], endTile, lowestIndex);
}



/// <summary>
/// Move the seeker on the path created from the a star algorithm
/// </summary>
void Simplex::MyAStar::MoveSeeker()
{
	// Path Finding Behavior for the Creeper
	static float fTimer = 0;
	static uint uClock = m_system->GenClock();
	fTimer += m_system->GetDeltaTime(uClock);

	StartPathFindingIfNotMoving();

	//Lerp and store results in v3CurrentPos.
	vector3 v3CurrentPos;
	float fPercentage = static_cast<float>(MapValue(fTimer, 0.0f, 0.6f, 0.0f, 1.0f));
	v3CurrentPos = glm::lerp(m_currentTravelFrom, m_currentTravelTo, fPercentage);

	// Made it to the next node. Lets remove the path.
	if (fPercentage >= 1.0)
	{
		GetNextPath();
		fTimer = m_system->GetDeltaTime(uClock);
	}


	// Move the finder!
	v3CurrentPos.y = 0.0; // This is important since we are a Creeper. Ideally we abstract this out a litle.
	matrix4 m4Finder = glm::translate(v3CurrentPos);
	m4Finder = glm::rotate(m4Finder, GetMovingDirctionRotation(), AXIS_Y);
	m_finder->SetModelMatrix(m4Finder);

}




/// <summary>
/// Get the moving direction rotation
/// </summary>
/// <returns>Angle in degrees for which way for the creeper to face</returns>
float Simplex::MyAStar::GetMovingDirctionRotation()
{
	float rotationAngle = 0.0f;
	if (m_currentTravelTo.x > m_currentTravelFrom.x) rotationAngle = 90.0f;
	if (m_currentTravelTo.z < m_currentTravelFrom.z) rotationAngle = 180.0f;
	if (m_currentTravelTo.x < m_currentTravelFrom.x) rotationAngle = 270.0f;
	return glm::radians(rotationAngle);
}




/// <summary>
/// Get the next path in the sequence of paths.
/// </summary>
void Simplex::MyAStar::GetNextPath()
{
	m_paths.erase(m_paths.begin());
	m_currentTravelFrom = m_currentTravelTo;

	// Bail out if we just removed the last path
	if (!WeHavePaths())
		return;

	if (m_currentTravelTo == m_paths[0] && m_paths.size() > 1)
	{
		m_currentTravelTo = m_paths[1];
	}
	else
	{
		m_currentTravelTo = m_paths[0];
	}
}



/// <summary>
/// Start up the pathfinding if we are not moving at the moment
/// </summary>
void Simplex::MyAStar::StartPathFindingIfNotMoving()
{
	if (m_currentTravelFrom == vector3() &&
		m_currentTravelTo == vector3() &&
		m_paths.size() > 1)
	{
		m_currentTravelFrom = m_paths[0];
		m_currentTravelTo = m_paths[1];
	}
}




/// <summary>
/// Check if we have collision between the targer, and the finder.
/// </summary>
/// <returns>True or false</returns>
bool Simplex::MyAStar::CheckCollisions()
{
	return m_target->IsColliding(m_finder);
}



/// <summary>
/// Cause a collision between the two entities
/// </summary>
/// <param name="fDelta"></param>
void Simplex::MyAStar::Collide(float fDelta)
{

	// Collide with Steve
	vector3 collisionVector = (m_currentTravelTo - m_currentTravelFrom) * (fDelta * 20.0f);
	m_entityManager->ApplyForce(collisionVector, "Steve");

	// Break Blocks
	ExploadGround();

	MoveCreeper();


	// Restart the A Star algorithm
	m_calcAStar = true;
	RestartPathFinding();
	OverrideAStarTileCheck();

}



/// <summary>
/// Remove some tiles from the ground. Cause a EXPLOSION!!!!
/// </summary>
void Simplex::MyAStar::ExploadGround()
{
	Tile* exploadTile = this->GetClosestTile(m_target->GetPosition());
	if (!exploadTile) return; // Bail out if we are floating

	int exploadAmount = 6;
	int exploadRadius = 3;
	int bailOutAmount = 20;


	// Remove all the exploaded blocks
	for (int i = 0; i < exploadAmount; i++)
	{
		// Get the x, and z vars from the index.
		int index = exploadTile->GetIndex();

		int x = (int)(index / m_mapWidth);
		int z = index % m_mapHeight;


		// Keep track of if we fuound a block, and if we have not bailed out yet.
		bool foundActiveBlock = false;
		int bailOutTracker = 0;

		do
		{
			int checkX = x + (-exploadRadius + (std::rand() % (exploadRadius * 2)));
			int checkZ = z + (-exploadRadius + (std::rand() % (exploadRadius * 2)));

			// Check the new block within radius
			if (checkX >= 0 && checkZ >= 0 && checkZ < m_mapHeight && checkX < m_mapWidth)
			{

				Tile* tile = &m_tileList[checkZ + (checkX * m_mapWidth)];

				// If we are active hide the block
				if (tile->GetActive() && NoOneStandingOnTile(tile))
				{
					// Sacrifice another hole for this one.
					if (GetCurrentHidden() >= m_totalHidden) RemoveObstacle();

					tile->EnableObstacle();
					m_entityManager->HideModelRender(tile->GetEntityId());

					m_hiddenTileList.push_back(tile);
					foundActiveBlock = true;
				}

			}

			bailOutTracker++;

		} while (!foundActiveBlock && bailOutTracker <= bailOutAmount);

	}
}



/// <summary>
/// Move the Creeper to a new random location
/// </summary>
void Simplex::MyAStar::MoveCreeper()
{
	
	Tile* tile = RandomFreeTile();
	vector3 tilePosition = tile->GetPosition();
	tilePosition.y = 0.0;

	matrix4 m4Creeper = glm::translate(tilePosition);
	m_entityManager->SetModelMatrix(m4Creeper, "Creeper");

}


/// <summary>
/// Check to see if we want to move the seeker. Only move the seeker when we are actually following a path.
/// </summary>
/// <returns></returns>
bool Simplex::MyAStar::WeHavePaths()
{
	return m_paths.size() != 0;
}



/// <summary>
/// Get the current size of the hidden tile list.
/// </summary>
/// <returns></returns>
int Simplex::MyAStar::GetCurrentHidden()
{
	return m_hiddenTileList.size();
}
