/* Ship.h
 *
 * Author: Colin Siles
 *
 * The ship class is the data structure for ships. It stores where on the grid all the parts of the ship are,
 * and provides numerous public member functions
*/

#ifndef SFML_TEMPLATE_SHIP_H
#define SFML_TEMPLATE_SHIP_H

#include <map>
#include <iostream>
#include <utility>

using namespace std;

enum Orientation {HORIZONTAL, VERTICAL};

class Ship {
public:
    // A ship is create based purely on how long it is
    Ship(int length);

    // Rotates the ship from horizontal to vertical, or vice versa
    void rotate();

    // Sets the ship's rotation to horizontal
    void setHorizontal();

    // Sets where in the grid the ship is, updating the location of all of its squares
    void setGridPos(int xPos, int yPos);

    // marks the given coordinate of the ship as hit
    void markAsHit(int xPos, int yPos);

    // Returns true if this ship intersects with the given coordinate
    bool contains(int xPos, int yPos);

    // Getter for the placed field
    bool isPlaced();

    // Getter for the sunk field
    bool isSunk();

    // Setter for the sunk field (called for tracking fleets)
    void markAsSunk();

    // Getter for the length field
    int getLength();

    // Returns all of the square s that make up the ship (but just a copy)
    map<pair<int, int>, bool> getSquares();

    // Friend classes to prevent a lot of extra getters and setters for this class
    friend class Board;
    friend class ShipRenderer;

private:
    void checkIfSunk(); // Helper function called after each hit to determine if ship is sunk yet

    map<pair<int ,int>, bool> _squares; // All the squares that make up the ship, and if those have been hit yet
    bool _sunk;
    bool _placed;
    int _boardX; // The coordinates of the upper left most square
    int _boardY;
    int _length;
    Orientation _orientation;
};

#endif //SFML_TEMPLATE_SHIP_H
