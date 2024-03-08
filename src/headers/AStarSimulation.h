/*
* This header file outlines an A Star simulation environment where we have a seeker and a target. The seeker
* pursues the target until they explode upon collision. We do this all through the A Star algorithm for path
* finding.
*/
#pragma once

#include "GameEntityManager.h"
#include "Tile.h"
#include <vector>

namespace Simplex
{
	class AStarSimulation
	{
		public:
		#pragma region Public Methods
			static AStarSimulation* GetInstance();
			static void ReleaseInstance(void);
			void Init(void);
			void Destroy(void);

			void Update(void);
			void AddLinesToRenderList(void);
			void AddObstacle(void);
			void RemoveObstacle(void);
			int GetCurrentHidden(void);
			int GetTotalObstacleCount(void);
		# pragma endregion

		private:
			static AStarSimulation* instance;
			bool recalculate_a_star = true;

		#pragma region Game State
			GameEntityManager* game_entity_manager = nullptr;
			MeshManager* mesh_manager = nullptr;
			SystemSingleton* system = nullptr;
		#pragma endregion

		#pragma region Map Data
			int map_width;
			int map_height;
			int map_area;
			int floor_level;
		#pragma endregion

		#pragma region Entity State
			GameEntity* target;
			GameEntity* finder;
			vector3 target_last_position;
			vector3 traveling_to;
			vector3 traveling_from;
		#pragma endregion

		#pragma region Tile Data
			Tile* tile_list = nullptr;
			Tile* target_last_tile;
			std::vector<Tile*> open_tile_list;
			std::vector<Tile*> closed_tile_list;
			std::vector<vector3> paths;
			int allowed_hidden_tiles;
			int number_tiles_hidden;
			std::vector<Tile*> hidden_tile_list;
		#pragma endregion

		#pragma region A Star Init
			AStarSimulation(int width, int height);
			AStarSimulation(AStarSimulation const& other);
			AStarSimulation& operator=(AStarSimulation const& other);
			~AStarSimulation(void);
			void CreateEntity(String fileName, String entityId, vector3 position);
			Tile* RandomFreeTile(void);
			bool NoOneStandingOnTile(Tile* tile); // Pick Locaiton / Simulation Restart
			Tile* GetClosestTile(vector3 position);
			bool PointInsideTile(vector3 position, Tile* tile);
		#pragma endregion

		#pragma region A Star Validation
			void CheckIfWeNeedAStarUpdate(void);
			bool ValidateAStarInput(Tile* startTile, Tile* endTile); // Init
			void OverrideAStarTileCheck(void);
		#pragma endregion

		# pragma region A Star Algorithm
			void CalculateAStarBetweenTiles(Tile* startTile, Tile* endTile);
			void AStarSetup(Tile* startTile, Tile* endTile);
			void GetPathFromTiles(Tile* startTile, Tile* endTile);
			bool AStarAlgo(Tile* currentCell, Tile* endCell, int openIndex);
		#pragma endregion

		#pragma region Entity Movement
			void MoveSeeker(void);
			float GetMovingDirctionRotation(void);
			void GetNextPath(void);
			void StartPathFindingIfNotMoving(void);
			bool WeHavePaths(void);
		#pragma endregion

		#pragma region Collision
			bool CheckCollisions(void);
			void Collide(float fDelta);
			void RestartPathFinding(void);
			void ExploadGround(void);
			void MoveCreeper(void);
		#pragma endregion
	};
}