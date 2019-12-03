/* Ship.cpp
 *
 * Author: Colin Siles
 *
 * The ship class is the data structure for ships. It stores where on the grid all the parts of the ship are,
 * and provides numerous public member functions
*/

#include "Ship.h"

//Primary constructor. By default a ship hasn't been placed, or sunk, is horizontal, and has its coordinates set
// to negative numbers to mark that it hasn't been placed anywhere yet
Ship::Ship(int length) {
    _sunk = false;
    _placed = false;
    _boardX = -1;
    _boardY = -1;
    _orientation = HORIZONTAL;

    _length = length;
}

// Use the ternary operator to toggle the orientation
void Ship::rotate() {
    _orientation = _orientation == HORIZONTAL ? VERTICAL : HORIZONTAL;
}

void Ship::setHorizontal() {
    _orientation = HORIZONTAL;
}

// Sets where in the grid the ship is, updating the location of all of its squares
// Note: you must call this again after rotating a ship
void Ship::setGridPos(int xPos, int yPos) {
    // The amount that the loop will step in each direction, based off the orientation of the ship
    int xStep = _orientation == HORIZONTAL;
    int yStep = _orientation == VERTICAL;

    // Set the coordinate of the ship
    _boardX = xPos;
    _boardY = yPos;

    // Reset all the squares
    _squares.clear();

    // Recreate all the squares at the correct locations
    for(int i = 0; i < _length; i++) {
        int newSquareX = xPos + xStep * i;
        int newSquareY = yPos + yStep * i;

        _squares.emplace(make_pair(newSquareX, newSquareY), false);
    }
}

// marks the given coordinate of the ship as hit
void Ship::markAsHit(int xPos, int yPos) {
    // Find the square that was hit
    const auto &square = _squares.find(make_pair(xPos, yPos));

    // Change its value to true
    square->second = true;

    checkIfSunk();
}

// Returns true if this ship intersects with the given coordinate
bool Ship::contains(int xPos, int yPos) {
    return _squares.count(make_pair(xPos, yPos));
}

// Getters and setters for various properties of the class
bool Ship::isPlaced() {
    return _placed;
}

bool Ship::isSunk() {
    return _sunk;
}

void Ship::markAsSunk() {
    _sunk = true;
}

int Ship::getLength() {
    return _length;
}

map<pair<int, int>, bool> Ship::getSquares() {
    return _squares;
}

// Helper function called after each hit to determine if ship is sunk yet
void Ship::checkIfSunk() {
    _sunk = true; // By default, assume the ship has been sunk

    // If any of the squares in the ship haven't been hit, then the ship hasn't been sunk
    for(auto &square : _squares) {
        if(!square.second) {
            _sunk = false;
            break;
        }
    }
}