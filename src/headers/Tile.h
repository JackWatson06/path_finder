/*
* Model a tile which is one block in our game world. We use this for our A Star algorithm as each 'Node'
*/
#pragma once

# include "GameEntityManager.h"

namespace Simplex
{
    class Tile 
    {
        private: 
        #pragma region Game State Data
            GameEntityManager* game_entity_manager;
            String entity_id;
            vector3 position;
            int map_index_location;
            bool active;
        #pragma endregion

        #pragma region A Star Data
            Tile* a_star_parent;
            float a_star_g_value;
            float a_star_h_value;
            float a_star_f_value;
        #pragma endregion

        public: 
        #pragma region Constructors / Destructors
            Tile();
            Tile(String entityId, vector3 position, int index);
			Tile(Tile const& other);
			Tile& operator=(Tile const& other);
			~Tile(void);
            void Release(void);
            void Init();
            void Swap(Tile& other);
            void ResetCell(void);
        #pragma endregion
            
        #pragma region Getters
            float GetF(void);
            float GetG(void);
            float GetH(void);
            int GetIndex(void);
            bool GetActive(void);
            String GetEntityId(void);
            Tile* GetParent(void);
            vector3 GetPosition(void);
        #pragma endregion

        #pragma region Setters
            void EnableObstacle(void);
            void RemoveObstacle(void);
            void SetH(float value);
            void SetG(float value);
            void SetParent(Tile* value);
        #pragma endregion
    };
}
