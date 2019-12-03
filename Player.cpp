//
// Created by Colin Siles on 2019-12-02.
//

#include "Player.h"


Player::Player(string name, vector<int> shipLengths) : _primaryFleet(shipLengths), _trackingFleet(shipLengths),
                                                       _primaryBoard(_primaryFleet), _trackingBoard(_trackingFleet) {
    _name = name;
}

void Player::placeShipsRandomly() {
    for(int i = 0; i < _primaryFleet.size(); i++) {
        while(true) {
            int randomX = rand() % 10;
            int randomY = rand() % 10;

            if(rand() % 2) {
                _primaryFleet.ship(i).rotate();
            }

            _primaryFleet.ship(i).setGridPos(randomX, randomY);

            if(_primaryBoard.shipFits(_primaryFleet.ship(i))) {
                _primaryBoard.placeShip(_primaryFleet.ship(i));

                break;
            }
        }
    }
}

ShotOutcome Player::fireShotAt(int xPos, int yPos) {
    return _primaryBoard.fireShotAt(xPos, yPos);
}

void Player::markShot(int xPos, int yPos, ShotOutcome outcome) {
    _trackingBoard.markShot(xPos, yPos, outcome);
}

string Player::getName() {
    return _name;
}

bool Player::allShipsPlaced() {
    return _primaryFleet.allPlaced();
}

bool Player::allShipsSunk() {
    return _primaryFleet.allSunk();
}
