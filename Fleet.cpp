/* Fleet.cpp
 *
 * Author: Colin Siles
 *
 * The Fleet class is essentially a container for multiple Ship objects, which also provides some functions
 * to mark ships as sunk, or check if all the ships in the fleet are sunk. An abstraction over a simple vector of Ships
*/

#include "Fleet.h"

// Constructor, simply creates ships that in the internally stroed vector of ships
Fleet::Fleet(vector<int> lengths) {
    // For each length passed to the functions
    for(int i = 0; i < lengths.size(); i++) {
        // Create a new ship with the given length
        Ship newShip(lengths.at(i));

        // Add the ship to the vector of ships
        _ships.push_back(newShip);
    }
}

// Called by the board, to mark the ship as hit, and determine which whip, if any, was sunk
int Fleet::markShipHit(int xPos, int yPos) {
    // Iterate over each ship
    for(int i = 0; i < _ships.size(); i++) {
        // If the ship has a square at the provided location...
        if(_ships.at(i).contains(xPos, yPos)) {
            // Then nark that ship as hit
            _ships.at(i).markAsHit(xPos, yPos);

            // If that ship was sunken
            bool sunken = _ships.at(i).isSunk();

            // Return it's index, or return -1 (no ship sunk)
            return sunken ? i : -1;
        }
    }

    // If something went wrong (e.g. this function called when no hit was made) just return -1
    return -1;
}

// Simple getter for number of ships in the fleet
int Fleet::size() {
    return _ships.size();
};

// Simple getter for the ship at the given index
// Return a reference to that ship, so that other objects can modify the ship if necessary (e.g. ShipRenderer)
Ship &Fleet::ship(int index) {
    return _ships.at(index);
}

// Returns true if all the ships were placed (needed to verify players actually placed their ships)
bool Fleet::allPlaced() {
    // Iterate over each ship
    for(int i = 0; i < _ships.size(); i++) {
        // If any ship wasn't placed...
        if(!_ships.at(i).isPlaced()) {
            // Then return false
            return false;
        }
    }

    // If all the ships were placed, return true
    return true;
}

// Returns true if all the ships in teh fleet were sunk, and the player as lost
// Uses a similar construction to the function above
bool Fleet::allSunk() {
    for(int i = 0; i < _ships.size(); i++) {
        if(!_ships.at(i).isSunk()) {
            return false;
        }
    }

    return true;
}