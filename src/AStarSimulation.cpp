/*
* Implementation of the A Star algorithm to handle path finding between two entities. This file also contains
* the code to manage the simulation and make an interesting game state.
*/
#include "headers/AStarSimulation.h"

#pragma region Public Methods
Simplex::AStarSimulation* Simplex::AStarSimulation::GetInstance()
{
	if (instance == nullptr) {
		instance = new AStarSimulation(20, 20);
	}
	return instance;
}

void Simplex::AStarSimulation::ReleaseInstance()
{
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

void Simplex::AStarSimulation::Init()
{
	game_entity_manager = GameEntityManager::GetInstance();
	mesh_manager = MeshManager::GetInstance();
	system = SystemSingleton::GetInstance();
	recalculate_a_star = true;
	map_width = 0;
	map_height = 0;
	map_area = 0;
	floor_level = -1;
	tile_list = new Tile[map_area];

	allowed_hidden_tiles = 0;
	target_last_position = vector3();

	traveling_from = vector3();
	traveling_to = vector3();
}

void Simplex::AStarSimulation::Destroy()
{
	delete[] tile_list;
	tile_list = nullptr;
}

void Simplex::AStarSimulation::Update()
{
	static uint sClock = system->GenClock();
	float fDelta = system->GetDeltaTime(sClock);

	CheckIfWeNeedAStarUpdate();

	if (recalculate_a_star) {
		Tile* startTile = this->GetClosestTile(finder->GetPosition());
		Tile* endTile = this->GetClosestTile(target->GetPosition());
		if (ValidateAStarInput(startTile, endTile)) CalculateAStarBetweenTiles(startTile, endTile);
		recalculate_a_star = false;
	}

	if (WeHavePaths()) {
		MoveSeeker();
	}

	if (CheckCollisions()) {
		Collide(fDelta);
	}
}

void Simplex::AStarSimulation::AddLinesToRenderList()
{
	for (uint i = 1; i < paths.size(); i++)
	{
		mesh_manager->AddLineToRenderList(IDENTITY_M4, paths[i], paths[i - 1], C_BLUE, C_BLUE);
	}
}

void Simplex::AStarSimulation::AddObstacle()
{
	if (GetCurrentHidden() >= allowed_hidden_tiles) {
		return;
	}

	Tile* tile = RandomFreeTile();
	tile->EnableObstacle();
	game_entity_manager->HideModelRender(tile->GetEntityId());
	hidden_tile_list.push_back(tile);

	recalculate_a_star = true;
	OverrideAStarTileCheck();
}

void Simplex::AStarSimulation::RemoveObstacle()
{

	int currentHidden = GetCurrentHidden();
	if (currentHidden == 0) {
		return;
	}

	int randomIndex = std::rand() % currentHidden;

	Tile* tile = hidden_tile_list[randomIndex];
	tile->RemoveObstacle();
	game_entity_manager->ShowModelRender(tile->GetEntityId());

	hidden_tile_list.erase(hidden_tile_list.begin() + randomIndex);

	recalculate_a_star = true;
	OverrideAStarTileCheck();
}

int Simplex::AStarSimulation::GetTotalObstacleCount() { return allowed_hidden_tiles; }
int Simplex::AStarSimulation::GetCurrentHidden() { return hidden_tile_list.size();  }
# pragma endregion

# pragma region A Star Init
Simplex::AStarSimulation* Simplex::AStarSimulation::instance = nullptr;

Simplex::AStarSimulation::AStarSimulation(int width, int height) { 
	Init(); 

	map_width = width;
	map_height = height;
	map_area = width * height;
	tile_list = new Tile[map_area];

	allowed_hidden_tiles = (width * height) * 0.25f;

	std::srand(std::time(nullptr));

	// Generate the map tiles.
	for (uint i = 0; i < width * height; i++)
	{
		float x = (float)((int)(i / width));
		float z = i % height;
		vector3 tilePosition = vector3(x, floor_level, z);

		String entityId = "Cube_" + std::to_string(i);


		Tile tile(entityId, tilePosition, i);
		tile_list[i] = tile; // <= Don't know why that is yelling at me (for me it is underlined green)


		CreateEntity("Minecraft\\Cube.obj", entityId, tilePosition);
		game_entity_manager->DisableRigidBody(entityId);

	}

	// Create the creeper which will act as the finder.
	CreateEntity("Minecraft\\Creeper.obj", "Creeper", RandomFreeTile()->GetPosition());
	game_entity_manager->UsePhysics();
	finder = game_entity_manager->GetEntity();

	// Create Steve which will act as the target
	CreateEntity("Minecraft\\Steve.obj", "Steve", RandomFreeTile()->GetPosition());
	game_entity_manager->UsePhysics();
	target = game_entity_manager->GetEntity();
	
}

Simplex::AStarSimulation::AStarSimulation(AStarSimulation const& other) { }
Simplex::AStarSimulation& Simplex::AStarSimulation::operator=(AStarSimulation const& other) { return *this; }
Simplex::AStarSimulation::~AStarSimulation(void) { Destroy(); }

void Simplex::AStarSimulation::CreateEntity(String fileName, String entityId, vector3 position)
{
	// Refactor to custom block creation method.
	game_entity_manager->AddEntity(fileName, entityId);

	matrix4 m4Position = glm::translate(position);
	game_entity_manager->SetModelMatrix(m4Position);
}

Simplex::Tile* Simplex::AStarSimulation::RandomFreeTile()
{
	do {
		int randomIndex = std::rand() % map_area;

		Tile* tile = &tile_list[randomIndex];

		// Need to check to see if the tile we just removed is beneath the enemies
		if (tile->GetActive() && NoOneStandingOnTile(tile)) {
			return tile;
		}

	} while (true && hidden_tile_list.size() < map_area); // Keep going until we can't no more. Double check that we are not hidding more than we should!
}

bool Simplex::AStarSimulation::NoOneStandingOnTile(Tile* tile)
{
	if (target && PointInsideTile(target->GetPosition(), tile)) return false;
	if (finder && PointInsideTile(finder->GetPosition(), tile)) return false;
	return true;
}

Simplex::Tile* Simplex::AStarSimulation::GetClosestTile(vector3 position)
{
	for (uint i = 0; i < map_area; i++) {
		Tile* tile = &tile_list[i];

		if (PointInsideTile(position, tile)) {
			return tile;
		}
	}

	return nullptr;
}

bool Simplex::AStarSimulation::PointInsideTile(vector3 position, Tile* tile)
{
	ObjectRigidBody* tileRigidBody = game_entity_manager->GetRigidBody(tile->GetEntityId());

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
# pragma endregion

# pragma region A Star Validation
void Simplex::AStarSimulation::CheckIfWeNeedAStarUpdate()
{
	vector3 targetPosition = target->GetPosition();
	if (target_last_position.x != targetPosition.x ||
		target_last_position.z != targetPosition.z) {
		recalculate_a_star = true;
	}
		
}

bool Simplex::AStarSimulation::ValidateAStarInput(Tile* startTile, Tile* endTile)
{
	// Don't run AStar if we are not actively on a tile.
	if (endTile == nullptr ||
		startTile == nullptr)
	{
		// Update the last target position if we have moved inside the tile at all. 
		// Just cause we move the target does not mean we have to re-run the A Star Algorithm
		target_last_position = target->GetPosition();
		return false;
	}


	// Don't run a star if we are on the same tile as last time we ran it.
	if (endTile == target_last_tile) {
		target_last_position = target->GetPosition();
		return false;
	}


	// Don't run A Star if both tiles are inactive. (This is for floating Steve.)
	if (!endTile->GetActive() ||
		!startTile->GetActive()) {
		target_last_position = target->GetPosition();
		return false;
	}

	return true;
}

void Simplex::AStarSimulation::OverrideAStarTileCheck()
{
	target_last_position = vector3();
	target_last_tile = nullptr;
}
# pragma endregion

# pragma region A Star Algorithm
void Simplex::AStarSimulation::CalculateAStarBetweenTiles(Tile* startTile, Tile* endTile)
{
	AStarSetup(startTile, endTile);

	// If the A Star was succesful then get the path from the tiles
	if (AStarAlgo(startTile, endTile, 0)) {
		GetPathFromTiles(startTile, endTile);
	}

	target_last_tile = endTile;
}

// Setup the A Star Algo. Reset from last run, then calculate heuristic value.
void Simplex::AStarSimulation::AStarSetup(Tile* startTile, Tile* endTile)
{
	// Reset all cells, and then get the heuristic value for active cells.
	for (uint i = 0; i < map_area; i++) {
		Tile* tile = &tile_list[i];
		tile->ResetCell();

		vector3 tilePosition = tile->GetPosition();
		vector3 endTilePosition = endTile->GetPosition();
		float bigAppleDistance = map_area;

		if (tile->GetActive()) {
			bigAppleDistance = glm::abs(endTilePosition.x - tilePosition.x) + glm::abs(endTilePosition.z - tilePosition.z);
		}

		tile->SetH(bigAppleDistance);
	}


	// Reset the tiles.
	open_tile_list.clear();
	closed_tile_list.clear();
	paths.clear();


	// Prep the first tile.
	startTile->SetParent(startTile);
	startTile->SetG(0);
	open_tile_list.push_back(startTile);
}

// Get the backwards path from the end tile then reverse the path.
void Simplex::AStarSimulation::GetPathFromTiles(Tile* startTile, Tile* endTile)
{
	// Get the reverse path from the end tile. That is the path.
	Tile* currentTile = endTile;

	while (true) {
		vector3 position = currentTile->GetPosition();
		position.y = -floor_level;

		paths.push_back(position);

		if (currentTile == startTile) break;
		currentTile = currentTile->GetParent();
	}

	std::reverse(paths.begin(), paths.end());
}

/// Run the recursive A Star algorithm to find the best path possible.
bool Simplex::AStarSimulation::AStarAlgo(Tile* currentTile, Tile* endTile, int openIndex)
{
	open_tile_list.erase(open_tile_list.begin() + openIndex);
	closed_tile_list.push_back(currentTile);


	// End case
	if (currentTile == endTile) return true;


	// Get the x, and z vars from the index.
	int index = currentTile->GetIndex();

	int x = (int)(index / map_width);
	int z = index % map_height;


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
		if (checkX >= 0 && checkZ >= 0 && checkZ < map_height && checkX < map_width)
		{
			Tile* tile = &tile_list[checkZ + (checkX * map_width)];

			// Skip the inactive tiles. And the tiles which already have a parent
			if (!tile->GetActive()) continue;
			if (tile->GetParent()) continue;

			// Set the parent of the neighbhor tiles to the current cell.
			tile->SetParent(currentTile);
			tile->SetG(1.0);

			open_tile_list.push_back(tile);
		}

	}



	// Find the lowest F Value in the list.
	int lowestIndex = 0;
	for (int i = 0; i < open_tile_list.size(); i++)
	{
		if (open_tile_list[i]->GetF() < open_tile_list[lowestIndex]->GetF())
			lowestIndex = i;
	}

	// Other edge case where we ran out of open options. This can occur if the target is unreachable.
	if (open_tile_list.size() == 0) return false;

	return AStarAlgo(open_tile_list[lowestIndex], endTile, lowestIndex);
}
#pragma endregion

# pragma region Entity Movement
// Move the seeker on the path created from the a star algorithm
void Simplex::AStarSimulation::MoveSeeker()
{
	// Path Finding Behavior for the Creeper
	static float fTimer = 0;
	static uint uClock = system->GenClock();
	fTimer += system->GetDeltaTime(uClock);

	StartPathFindingIfNotMoving();

	//Lerp and store results in v3CurrentPos.
	vector3 v3CurrentPos;
	float fPercentage = static_cast<float>(MapValue(fTimer, 0.0f, 0.6f, 0.0f, 1.0f));
	v3CurrentPos = glm::lerp(traveling_from, traveling_to, fPercentage);

	// Made it to the next node. Lets remove the path.
	if (fPercentage >= 1.0)
	{
		GetNextPath();
		fTimer = system->GetDeltaTime(uClock);
	}


	// Move the finder!
	v3CurrentPos.y = 0.0; // This is important since we are a Creeper. Ideally we abstract this out a litle.
	matrix4 m4Finder = glm::translate(v3CurrentPos);
	m4Finder = glm::rotate(m4Finder, GetMovingDirctionRotation(), AXIS_Y);
	finder->SetModelMatrix(m4Finder);

}

// Get the moving direction rotation. Returns the angle in degrees for which way for the creeper to face.
float Simplex::AStarSimulation::GetMovingDirctionRotation()
{
	float rotationAngle = 0.0f;
	if (traveling_to.x > traveling_from.x) rotationAngle = 90.0f;
	if (traveling_to.z < traveling_from.z) rotationAngle = 180.0f;
	if (traveling_to.x < traveling_from.x) rotationAngle = 270.0f;
	return glm::radians(rotationAngle);
}

// Get the next path in the sequence of paths.
void Simplex::AStarSimulation::GetNextPath()
{
	paths.erase(paths.begin());
	traveling_from = traveling_to;

	// Bail out if we just removed the last path
	if (!WeHavePaths())
		return;

	if (traveling_to == paths[0] && paths.size() > 1) {
		traveling_to = paths[1];
	} else {
		traveling_to = paths[0];
	}
}

// Start up the pathfinding if we are not moving at the moment
void Simplex::AStarSimulation::StartPathFindingIfNotMoving()
{
	if (traveling_from == vector3() &&
		traveling_to == vector3() &&
		paths.size() > 1)
	{
		traveling_from = paths[0];
		traveling_to = paths[1];
	}
}

// Check to see if we want to move the seeker. Only move the seeker when we are actually following a path.
bool Simplex::AStarSimulation::WeHavePaths()
{
	return paths.size() != 0;
}

# pragma endregion

# pragma region Collision
/// Check if we have collision between the target, and the finder.
bool Simplex::AStarSimulation::CheckCollisions()
{
	return target->IsColliding(finder);
}

// Cause a collision between the two entities.
void Simplex::AStarSimulation::Collide(float fDelta)
{
	// Collide with Steve
	vector3 collisionVector = (traveling_to - traveling_from) * (fDelta * 20.0f);
	game_entity_manager->ApplyForce(collisionVector, "Steve");

	// Break Blocks
	ExploadGround();
	MoveCreeper();

	// Restart the A Star algorithm
	recalculate_a_star = true;
	RestartPathFinding();
	OverrideAStarTileCheck();
}

void Simplex::AStarSimulation::RestartPathFinding()
{
	traveling_from = vector3();
	traveling_to = vector3();
}

// Remove some tiles from the ground. Cause a EXPLOSION!!!!
void Simplex::AStarSimulation::ExploadGround()
{
	Tile* exploadTile = this->GetClosestTile(target->GetPosition());
	if (!exploadTile) return; // Bail out if we are floating

	int exploadAmount = 6;
	int exploadRadius = 3;
	int bailOutAmount = 20;


	// Remove all the exploaded blocks
	for (int i = 0; i < exploadAmount; i++)
	{
		// Get the x, and z vars from the index.
		int index = exploadTile->GetIndex();

		int x = (int)(index / map_width);
		int z = index % map_height;


		// Keep track of if we found a block, and if we have not bailed out yet.
		bool foundActiveBlock = false;
		int bailOutTracker = 0;

		do
		{
			int checkX = x + (-exploadRadius + (std::rand() % (exploadRadius * 2)));
			int checkZ = z + (-exploadRadius + (std::rand() % (exploadRadius * 2)));

			// Check the new block within radius
			if (checkX >= 0 && checkZ >= 0 && checkZ < map_height && checkX < map_width)
			{

				Tile* tile = &tile_list[checkZ + (checkX * map_width)];

				// If we are active hide the block
				if (tile->GetActive() && NoOneStandingOnTile(tile))
				{
					// Sacrifice another hole for this one.
					if (GetCurrentHidden() >= allowed_hidden_tiles) RemoveObstacle();

					tile->EnableObstacle();
					game_entity_manager->HideModelRender(tile->GetEntityId());

					hidden_tile_list.push_back(tile);
					foundActiveBlock = true;
				}

			}

			bailOutTracker++;

		} while (!foundActiveBlock && bailOutTracker <= bailOutAmount);

	}
}

// Move creeper to a random location.
void Simplex::AStarSimulation::MoveCreeper()
{	
	Tile* tile = RandomFreeTile();
	vector3 tilePosition = tile->GetPosition();
	tilePosition.y = 0.0;

	matrix4 m4Creeper = glm::translate(tilePosition);
	game_entity_manager->SetModelMatrix(m4Creeper, "Creeper");

}
#pragma endregion
