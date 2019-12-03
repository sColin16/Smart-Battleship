//
// Created by Colin Siles on 2019-12-02.
//

#include "Board.h"

ShotOutcome makeOutcome(bool hit, int sunkenIndex) {
    ShotOutcome outcome;
    outcome.hit = hit;
    outcome.sunkenIndex = sunkenIndex;

    return outcome;
}


Board::Board(Fleet &fleet) {
    for(int i = 0; i < GRID_SIZE; i++) {
        vector<SquareState> newRow(GRID_SIZE, BLANK);
        _grid.push_back(newRow);
    }

    _fleet = &fleet;
}

ShotOutcome Board::fireShotAt(int xPos, int yPos) {
    bool hit = _grid.at(xPos).at(yPos) == SHIP;
    _grid.at(xPos).at(yPos) = hit ? HIT_MARKER : MISS_MARKER;

    int sunkIndex = hit ? _fleet->markShipHit(xPos, yPos) : -1;

    return makeOutcome(hit, sunkIndex);
}

bool Board::shipFits(Ship ship) {
    if(ship._boardX < 0 || ship._boardY < 0 ){
        return false;
    }

    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;
        int xPos = squarePos.first;
        int yPos = squarePos.second;

        if(!posInsideGrid(xPos, yPos) || _grid.at(xPos).at(yPos) == SHIP || _grid.at(xPos).at(yPos) == MISS_MARKER) {
            return false;
        }
    }

    return true;
}

void Board::placeShip(Ship &ship) {
    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;

        _grid.at(squarePos.first).at(squarePos.second) = SHIP;
    }

    ship._placed = true;
}

void Board::markShot(int xPos, int yPos, ShotOutcome outcome) {
    if(outcome.hit) {
        _grid.at(xPos).at(yPos) = HIT_MARKER;
    } else {
        _grid.at(xPos).at(yPos) = MISS_MARKER;
    }

    if(outcome.sunkenIndex >= 0) {
        _fleet->ship(outcome.sunkenIndex).markAsSunk();
    }
}

bool Board::validGuess(int xPos, int yPos) {
    return posInsideGrid(xPos, yPos) && (_grid.at(xPos).at(yPos) == BLANK || _grid.at(xPos).at(yPos) == BLANK);
}

bool Board::posInsideGrid(int xPos, int yPos){
    return xPos >=0 && xPos < GRID_SIZE && yPos >= 0 && yPos < GRID_SIZE;
}
