/* Player.h
 *
 * Author: Colin Siles
 *
 * The Player class is an abstract base class, that provides basic functionality for all players (such as marking shots
 * and reporting the outcome of a shot). The class has three pure virtual functions for getting a move, placing ships,
 * and handling game over.
*/

#ifndef SFML_TEMPLATE_PLAYER_H
#define SFML_TEMPLATE_PLAYER_H

#include <cstdlib>
#include <string>
#include <vector>
#include <utility>

#include "Board.h"

using namespace std;

class Player {
public:
    Player(string name, vector<int> shipLengths);

    // Pure virtual functions that all subclasses must overrid
    virtual pair<int, int> getMove() = 0;
    virtual void placeShips() = 0;
    virtual void reportGameover(bool winner) = 0;

    // A player can use this function to place their ships randomly
    void placeShipsRandomly();

    // Wrapper for the primaryBoard's fireShotAt function
    ShotOutcome fireShotAt(int xPos, int yPos);

    // Wrapper for board's markShot function. Virtual so that players can add functionality to track sunken ships
    virtual void markShot(int xPos, int yPos, ShotOutcome outcome);

    // Wrapper for the primaryFleet's functions
    bool allShipsPlaced();
    bool allShipsSunk();

    // Getter for the name property
    string getName();

protected:
    Board _primaryBoard; // The player's board where all their ships are
    Board _trackingBoard; // The tracking board, where a player marks hits and misses

    Fleet _primaryFleet; // The player's fleet, which has been palced on the board
    Fleet _trackingFleet; // The tracking fleet, where a player can know which of the opponents ships have been sunk

    // Store a name for the battelog and/or printing to terminal
    string _name;
};

#endif //SFML_TEMPLATE_PLAYER_H
