/* Board.cpp
 *
 * Author: Colin Siles
 *
 * The Board class is the data type that stores the state of a player's board (or their tracking board)
 * It can hold ships, have squares marked as hit or miss, and offers a variety of public member functions
 * to ensure that ships are placed only in valid locations, and guesses are also made in valid locations
*/

#include "Board.h"

// Function similar to "make_pair" that allows for quickly returning a ShotOutcome object
ShotOutcome makeOutcome(bool hit, int sunkenIndex) {
    ShotOutcome outcome;
    outcome.hit = hit;
    outcome.sunkenIndex = sunkenIndex;

    return outcome;
}

// Constructor
Board::Board(Fleet &fleet) {
    // Construct the 2D grid
    for(int i = 0; i < GRID_SIZE; i++) {
        vector<SquareState> newRow(GRID_SIZE, BLANK);
        _grid.push_back(newRow);
    }

    // Store the reference to thet fleet (not a copy so that the board can mark ships as sunk when necessary)
    _fleet = &fleet;
}

// Returns the outcome based on a shot
ShotOutcome Board::fireShotAt(int xPos, int yPos) {
    // Determine if a ship exists at the given location
    bool hit = _grid.at(xPos).at(yPos) == SHIP;

    // Mark the grid there appropriately
    _grid.at(xPos).at(yPos) = hit ? HIT_MARKER : MISS_MARKER;

    // Determine the index of the ship that was sunk, if one was sunk
    int sunkIndex = hit ? _fleet->markShipHit(xPos, yPos) : -1;

    // Return the outcome
    return makeOutcome(hit, sunkIndex);
}

// Returns true if the provided ship fits at its location in this board
bool Board::shipFits(Ship ship) {
    // Exit early if the ship's position is negative (likely because it hasn't been assigned a position yet)
    if(ship._boardX < 0 || ship._boardY < 0 ){
        return false;
    }

    // Iterate over each square in the ship
    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;
        int xPos = squarePos.first;
        int yPos = squarePos.second;

        // If any of the squares overlap with a ship or a miss marker (for the computer player)
        // The ship does not fit in the given location
        if(!_posInsideGrid(xPos, yPos) || _grid.at(xPos).at(yPos) == SHIP || _grid.at(xPos).at(yPos) == MISS_MARKER) {
            return false;
        }
    }

    // If none of the ship squares overlapped, then the ship does fit
    return true;
}

// Places the ship within the grid, marking the board's internal data structure as "ships" where appropriate
void Board::placeShip(Ship &ship) {
    // Iterate over each square
    for(auto &square : ship._squares) {
        // The square is a pair composed of the location, and whether that square has been hit. Get the first, the position
        pair<int, int> squarePos = square.first;

        // Mark the grid as having a square at the location
        _grid.at(squarePos.first).at(squarePos.second) = SHIP;
    }

    // Mark the ship as placed
    ship._placed = true;
}

// Used for tracking boards. Marks a given location as a hit or a miss
void Board::markShot(int xPos, int yPos, ShotOutcome outcome) {
    if(outcome.hit) { // Mark as a hit if it was a hit
        _grid.at(xPos).at(yPos) = HIT_MARKER;
    } else {          // Otherwise, it must have been a miss
        _grid.at(xPos).at(yPos) = MISS_MARKER;
    }

    // If a ship was sunken, mark the corresponding ship as sunken
    if(outcome.sunkenIndex >= 0) {
        _fleet->ship(outcome.sunkenIndex).markAsSunk();
    }
}

// Returns true if the given position is within the grid, and is currently blank (can't double guess)
bool Board::validGuess(int xPos, int yPos) {
    return _posInsideGrid(xPos, yPos) && _grid.at(xPos).at(yPos) == BLANK;
}

// Return true if the position is inside the grid (e.g. not a negative coordinate, or too large)
bool Board::_posInsideGrid(int xPos, int yPos){
    return xPos >=0 && xPos < GRID_SIZE && yPos >= 0 && yPos < GRID_SIZE;
}
