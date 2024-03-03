/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2017/06
----------------------------------------------*/
#ifndef __MYASTAR_H_
#define __MYASTAR_H_

#include "MyEntityManager.h"
#include "Tile.h"

namespace Simplex
{

	//System Class
	class MyAStar
	{

	static MyAStar* m_instance; // Singleton pointer
	protected:
		

		MyEntityManager* m_entityManager = nullptr; // List of all entities currently in the map.
		MeshManager* m_meshManager = nullptr;
		SystemSingleton* m_system = nullptr; //Singleton of the system

		MyEntity* m_target; //Entity which we are targeting.
		MyEntity* m_finder;  //Entity which is searching for the other entities.
		vector3 m_targetLastPosition; // Hold the targets last position to check to see if we actually moved. If we did not
										// then do not calc the A Star path.
		Tile* m_targetLastTile;
		std::vector<Tile*> m_openList; // Tiles which are on the open list... used for the a star algo.
		std::vector<Tile*> m_closedList; // Tiles which are on the closed list... used for the a star algo.
		std::vector<vector3> m_paths;

		Tile* m_tileList = nullptr; // Array of the tiles in the game world

		bool m_calcAStar = true; // Recalc A Star

		int m_mapWidth; // Width of the map
		int m_mapHeight; // Height of the map
		int m_mapSize; // The total size of tiles in the 
		int m_floorLevel;

		int m_totalHidden; // Number of hidden tiles which are allowed
		int m_currentHidden; // Current number of tiles we have hidden
		std::vector<Tile*> m_hiddenTileList;

		vector3 m_currentTravelTo; // Point we are traveling to
		vector3 m_currentTravelFrom; // Point we are coming from


	public:

		/*
		Usage: Gets the singleton pointer
		Arguments: ---
		Output: singleton pointer
		*/
		static MyAStar* GetInstance();
		/*
		Usage: Releases the content of the singleton
		Arguments: ---
		Output: ---
		*/
		static void ReleaseInstance(void);
		/*
		Usage: Initialize member variables.
		Arguments: ---
		Output: ---
		*/
		void Init(void);
		/*
		Usage: Destroy this class (free memory)
		Arguments: ---
		Output: ---
		*/
		void Destroy(void);
		/*
		Usage: Update the map
		Arguments: ---
		Output: ---
		*/
		void Update(void);
		/*
		Usage: Render the paths
		Arguments: ---
		Output: ---
		*/
		void AddLinesToRenderList(void);
		/*
		Usage: Add an obstacle
		Arguments: ---
		Output: ---
		*/
		void AddObstacle(void);
		/*
		Usage: Remove an obstacle
		Arguments: ---
		Output: ---
		*/
		void RemoveObstacle(void);
		/*
		Usage: Get current hidden obstacle count
		Arguments: ---
		Output: Number of current obsticles
		*/
		int GetCurrentHidden(void);
		/*
		Usage: Get total number of obstacles
		Arguments: ---
		Output: The number of obstacles allowed in the map
		*/
		int GetTotalObstacleCount(void);

	private:
		/*
		Usage: Constructor
		Arguments: std::vector<vector3> a_pointList -> list of points to make the Rigid Body for
		Output: class object instance
		*/
		MyAStar(int width, int height);
		/*
		Usage: Copy Constructor
		Arguments: class object to copy
		Output: class object instance
		*/
		MyAStar(MyAStar const& other);
		/*
		Usage: Copy Assignment Operator
		Arguments: class object to copy
		Output: ---
		*/
		MyAStar& operator=(MyAStar const& other);
		/*
		Usage: Destructor
		Arguments: ---
		Output: ---
		*/
		~MyAStar(void);
		/*
		Usage: Create a new entity
		Arguments: Filname, the entity id, and the position of the entity
		Output: ---
		*/
		void CreateEntity(String fileName, String entityId, vector3 position);
		/*
		Usage: Find a random free tile
		Arguments: ---
		Output: A Free tile
		*/
		Tile* RandomFreeTile(void);
		/*
		Usage: Test to see if their are any entities on the tile
		Arguments: Tile to test
		Output: True, or false if their are any entities
		*/
		bool NoOneStandingOnTile(Tile* tile);
		/*
		Usage: Validate the input to a star
		Arguments: The starting tile for A Star, and the ending Tile for A Star
		Output: True or false if input is valid
		*/
		bool ValidateAStarInput(Tile* startTile, Tile* endTile);
		/*
		Usage: Get the closest Tile to the position
		Arguments: Vector3 position to test
		Output: Tile pointer to the closest tile
		*/
		Tile* GetClosestTile(vector3 position);
		/*
		Usage: Test if the point is inside the bounds of a tile
		Arguments: vector3 position of the tile, Tile to test
		Output: True or False if the point is inside the tile.
		*/
		bool PointInsideTile(vector3 position, Tile* tile);
		/*
		Usage: Check if we need to do an A Star Update
		Arguments: ---
		Output: ---
		*/
		void CheckIfWeNeedAStarUpdate(void);
		/*
		Usage: Get the A Star path between two tiles.
		Arguments: The start tile, and the end tile.
		Output: ---
		*/
		void CalculateAStarBetweenTiles(Tile* startTile, Tile* endTile);
		/*
		Usage: Setup A Star
		Arguments: The start tile, and the end tile.
		Output: ---
		*/
		void AStarSetup(Tile* startTile, Tile* endTile); // Setup A Star for running
		/*
		Usage: Run the A Star Algorithm
		Arguments: Current Tile cell, end goal, index on open list
		Output: True if successful
		*/
		bool AStarAlgo(Tile* currentCell, Tile* endCell, int openIndex);
		/*
		Usage: Manually run A Star
		Arguments: ---
		Output: ---
		*/
		void OverrideAStarTileCheck(void);
		/*
		Usage: Restart entity path finding
		Arguments: ---
		Output: ---
		*/
		void RestartPathFinding(void);
		/*
		Usage: Get the path from the end tile
		Arguments: The start tile, and the end tile.
		Output: ---
		*/
		void GetPathFromTiles(Tile* startTile, Tile* endTile);
		/*
		Usage: Test if we have any paths right now
		Arguments: ---
		Output: True if we have paths in arra
		*/
		bool WeHavePaths(void);
		/*
		Usage: Move the seeker to the target
		Arguments: ---
		Output: ---
		*/
		void MoveSeeker(void);
		/*
		Usage: Get the direction seeker is facing
		Arguments: ---
		Output: Degrees for seeker
		*/
		float GetMovingDirctionRotation(void);
		/*
		Usage: Get the next path for seeker
		Arguments: ---
		Output: ---
		*/
		void GetNextPath(void);
		/*
		Usage: Start the path finding if we are not
		Arguments: ---
		Output: ---
		*/
		void StartPathFindingIfNotMoving(void);
		/*
		Usage: Check the collisions between the seeker, and target.
		Arguments: ---
		Output: True if colliding
		*/
		bool CheckCollisions(void);
		/*
		Usage: Collide the two entities
		Arguments: Delta Time
		Output: ---
		*/
		void Collide(float fDelta);
		/*
		Usage: Expload an area around Steve
		Arguments: ---
		Output: ---
		*/
		void ExploadGround(void);
		/*
		Usage: Move the creeper
		Arguments: ---
		Output: ---
		*/
		void MoveCreeper(void);


	};//class

} //namespace Simplex

#endif //__MYRIGIDBODY_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/