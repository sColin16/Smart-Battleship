/* Player.cpp
 *
 * Author: Colin Siles
 *
 * The Player class is an abstract base class, that provides basic functionality for all players (such as marking shots
 * and reporting the outcome of a shot). The class has three pure virtual functions for getting a move, placing ships,
 * and handling game over.
*/


#include "Player.h"

// Use initializer lists to instantiate some of the member fields
Player::Player(string name, vector<int> shipLengths) : _primaryFleet(shipLengths), _trackingFleet(shipLengths),
        _primaryBoard(_primaryFleet), _trackingBoard(_trackingFleet) {
    _name = name;
}

// Function that sub classes can use to place their ships randomly on the board
void Player::placeShipsRandomly() {
    // Iterate over each ship
    for(int i = 0; i < _primaryFleet.size(); i++) {
        // As long as the ship hasn't been placed succesfully
        while(true) {
            // Pick a random palce for the ship
            int randomX = rand() % 10;
            int randomY = rand() % 10;

            // Randomly rotate it
            if(rand() % 2) {
                _primaryFleet.ship(i).rotate();
            }

            // Then if the ship fits there, break, and move onto the next ship
            _primaryFleet.ship(i).setGridPos(randomX, randomY);

            if(_primaryBoard.shipFits(_primaryFleet.ship(i))) {
                _primaryBoard.placeShip(_primaryFleet.ship(i));

                break;
            }
        }
    }
}

// Wrappers for the Board and Fleet classes
ShotOutcome Player::fireShotAt(int xPos, int yPos) {
    return _primaryBoard.fireShotAt(xPos, yPos);
}

void Player::markShot(int xPos, int yPos, ShotOutcome outcome) {
    _trackingBoard.markShot(xPos, yPos, outcome);
}

bool Player::allShipsPlaced() {
    return _primaryFleet.allPlaced();
}

bool Player::allShipsSunk() {
    return _primaryFleet.allSunk();
}

// Getter for the name property
string Player::getName() {
    return _name;
}