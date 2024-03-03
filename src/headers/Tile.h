#ifndef STUDENT_H
#define STUDENT_H

# include "MyEntityManager.h"

namespace Simplex
{
    class Tile {


        protected: 

            MyEntityManager* m_entityManager; // Get a referance to the Entity Manager so we can just pull out the entity in this class.

            String m_entityId; // Store the entity id associated with this tile.
            vector3 m_position; // Store the posiiton of the tile in world coords
            int m_mapIndex; // Store the index in which this tile is on the map array.
            bool m_active; // Store weather or not this tile should be active.


            // Varaibles which are used for calculating the best path.
            // These arguably could go in a seperate cell associated class, but I think their fine here for this assignment.
            Tile* m_parent; // Store the parent for A star (Used for path finding ability)
            float m_g; // G value associated with A Star
            float m_h; // H value associated with A Star
            float m_f; // F value associated with A Star

        public: 

            /*
            Usage: Default Constructor
            Arguments: ---
            Output: ---
            */
            Tile();
            /*
            Usage: Constructor
            Arguments: Entity associated with tile, position of tile, index of tile in map
            Output: ---
            */
            Tile(String entityId, vector3 position, int index);
            /*
            Usage: Copy Constructor
            Arguments: Other tile
            Output: ---
            */
			Tile(Tile const& other);
            /*
            Usage: = Operator
            Arguments: Set tile equal to this one
            Output: ---
            */
			Tile& operator=(Tile const& other);
            /*
            Usage: Destructor
            Arguments: ---
            Output: ---
            */
			~Tile(void);
            /*
            Usage: Swap variables
            Arguments: Other Tile to swap
            Output: ---
            */
            void Swap(Tile& other);
            /*
            Usage: Initialize varaibles
            Arguments: ---
            Output: ---
            */
            void Init();
            /*
            Usage: Release objects memoery
            Arguments: ---
            Output: ---
            */
            void Release(void);
            /*
            Usage: Reset this objects to a default state
            Arguments: ---
            Output: ---
            */
            void ResetCell(void);
            

            // Getters for the tile class.
            float GetF(void);
            float GetG(void);
            float GetH(void);
            int GetIndex(void);
            bool GetActive(void);
            String GetEntityId(void);
            Tile* GetParent(void);
            vector3 GetPosition(void);

            // Setters for the tile class.
            void EnableObstacle(void);
            void RemoveObstacle(void);
            void SetH(float value);
            void SetG(float value);
            void SetParent(Tile* value);



    };
}

#endif