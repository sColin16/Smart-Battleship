//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_BOARD_H
#define SFML_TEMPLATE_BOARD_H

#include <vector>

#include "Ship.h"
#include "Fleet.h"

using namespace std;

struct ShotOutcome {
    bool hit;
    int sunkenIndex;
};

class Board {
public:
    Board(Fleet &fleet);

    ShotOutcome fireShotAt(int xPos, int yPos); // Returns true is shot is a hit, false if a miss (must check validity first)
    bool shipFits(Ship ship);
    void placeShip(Ship &ship); // Pas by reference to store reference to the ship so the board can sink it when appropropriate

    void markShot(int xPos, int yPos, ShotOutcome outcome);
    bool validGuess(int xPos, int yPos); // Returns true if guess is valid (in grid and not already guessed) called by tracking grids, primarily

    static const int GRID_SIZE = 10; // In theory, this could be changed to be a parameter, although renderer class probably couldnt handle that easily
    enum SquareState {BLANK, SHIP, HIT_MARKER, MISS_MARKER};
    friend class BoardRenderer;
    friend class IntelligentComputer;

private:
    Fleet *_fleet;
    vector<vector<SquareState>> _grid; // We intialize this in the constructor

    bool posInsideGrid(int xPos, int yPos); // Checks that a shot is within the bounds of the board
};


#endif //SFML_TEMPLATE_BOARD_H
