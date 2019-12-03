//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_FLEET_H
#define SFML_TEMPLATE_FLEET_H

#include <vector>

#include "Ship.h"

class Fleet {
public:
    Fleet(vector<int> lengths);

    int markShipHit(int xPos, int yPos); // Returns the index of the ship sunk, or -1 if non sunk

    int size(); // Returns the size of the fleet
    Ship &ship(int index);

    bool allPlaced();
    bool allSunk();

private:
    vector<Ship> _ships;
};


#endif //SFML_TEMPLATE_FLEET_H
