/* Board.h
 *
 * Author: Colin Siles
 *
 * The Board class is the data type that stores the state of a player's board (or their tracking board)
 * It can hold ships, have squares marked as hit or miss, and offers a variety of public member functions
 * to ensure that ships are placed only in valid locations, and guesses are also made in valid locations
*/

#ifndef SFML_TEMPLATE_BOARD_H
#define SFML_TEMPLATE_BOARD_H

#include <vector>

#include "Ship.h"
#include "Fleet.h"

using namespace std;

// Data type returned by the board to allow other objects to know if the shot was a hit, miss, sink, etc.
struct ShotOutcome {
    bool hit;        // Whether or not the move resulted in a hit (true) or miss (false)
    int sunkenIndex; // The index of the ship sunken, or -1 if no ship was sunken
};

class Board {
public:
    // Class is instantiated with a fleet object, so that it can mark them as hit/sunk when necessary
    Board(Fleet &fleet);

    // Returns the outcome based on a shot
    ShotOutcome fireShotAt(int xPos, int yPos);

    // Returns true if the provided ship fits at its location in this board
    bool shipFits(Ship ship);

    // Places the ship within the grid, marking the board's internal data structure as "ships" where appropriate
    // Pass by reference to mark this ship as placed
    void placeShip(Ship &ship);

    // Used for traacking boards. Marks a given location as a hit or a miss
    void markShot(int xPos, int yPos, ShotOutcome outcome);

    // Returns true if guess is valid (in grid and not already guessed) called by tracking grids, primarily
    bool validGuess(int xPos, int yPos);

    // Determines the size of a board
    static const int GRID_SIZE = 10; // In theory, this could be changed to be a parameter, although renderer class probably couldnt handle that easily

    // These are the values that fill up the board
    enum SquareState {BLANK, SHIP, HIT_MARKER, MISS_MARKER};

    // Marked as friend classes, since they benefit from access to the class's internal data structure
    // (Adding more public functions would just add a lot of extra code)
    friend class BoardRenderer;
    friend class IntelligentComputer;

private:
    // The fleet of ships associated with the baord
    Fleet *_fleet;

    // The 2D vector that stores the state of all the objects in the grid
    vector<vector<SquareState>> _grid; // We intialize this in the constructor

    // Checks that a shot is within the bounds of the board
    bool _posInsideGrid(int xPos, int yPos);
};


#endif //SFML_TEMPLATE_BOARD_H
