//
// Created by Colin Siles on 2019-12-02.
//

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

    virtual pair<int, int> getMove() = 0;
    virtual void placeShips() = 0;
    virtual void reportGameover(bool winner) = 0;
    void placeShipsRandomly();

    ShotOutcome fireShotAt(int xPos, int yPos);

    virtual void markShot(int xPos, int yPos, ShotOutcome outcome);

    string getName();

    bool allShipsPlaced();
    bool allShipsSunk();

protected:
    Board _primaryBoard;
    Board _trackingBoard;

    Fleet _primaryFleet;
    Fleet _trackingFleet;
    string _name;
};


#endif //SFML_TEMPLATE_PLAYER_H
