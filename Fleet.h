/* Fleet.h
 *
 * Author: Colin Siles
 *
 * The Fleet class is essentially a container for multiple Ship objects, which also provides some functions
 * to mark ships as sunk, or check if all the ships in the fleet are sunk. An abstraction over a simple vector of Ships
*/

#ifndef SFML_TEMPLATE_FLEET_H
#define SFML_TEMPLATE_FLEET_H

#include <vector>

#include "Ship.h"

class Fleet {
public:
    Fleet(vector<int> lengths);

    // Returns the index of the ship sunk, or -1 if non sunk
    int markShipHit(int xPos, int yPos);

    // Returns the size of the fleet
    int size();

    // Returns the given ship at the index (similar to vector's at method)
    Ship &ship(int index);

    // Returns true if all the ships were placed (needed to verify players actually placed their ships)
    bool allPlaced();

    // Returns true if all the ships in teh fleet were sunk, and the player as lost
    bool allSunk();

private:
    // The class is just wrapping this singular vector of Ships with some member functions
    vector<Ship> _ships;
};


#endif //SFML_TEMPLATE_FLEET_H
