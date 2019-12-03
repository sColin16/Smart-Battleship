//
// Created by Colin Siles on 2019-12-02.
//

#include "Ship.h"


Ship::Ship(int length) {
    _sunk = false;
    _placed = false;
    _boardX = -1;
    _boardY = -1;
    _orientation = HORIZONTAL;

    _length = length;
}

void Ship::rotate() {
    _orientation = _orientation == HORIZONTAL ? VERTICAL : HORIZONTAL;
}

void Ship::setHorizontal() {
    _orientation = HORIZONTAL;
}

void Ship::setGridPos(int xPos, int yPos) {
    int xStep = _orientation == HORIZONTAL;
    int yStep = _orientation == VERTICAL;

    _boardX = xPos;
    _boardY = yPos;

    _squares.clear();

    for(int i = 0; i < _length; i++) {
        int newSquareX = xPos + xStep * i;
        int newSquareY = yPos + yStep * i;

        _squares.emplace(make_pair(newSquareX, newSquareY), false);
    }
}

void Ship::markAsHit(int xPos, int yPos) {
    // Find the square that was hit
    const auto &square = _squares.find(make_pair(xPos, yPos));

    // Change its value to true
    square->second = true;

    checkIfSunk();

}

bool Ship::isPlaced() {
    return _placed;
}

bool Ship::isSunk() {
    return _sunk;
}

void Ship::markAsSunk() {
    _sunk = true;
}

bool Ship::contains(int xPos, int yPos) {
    return _squares.count(make_pair(xPos, yPos));
}

void Ship::checkIfSunk() {
    _sunk = true;

    for(auto &square : _squares) {
        if(!square.second) {
            _sunk = false;
            break;
        }
    }
}

int Ship::getLength() {
    return _length;
}

map<pair<int, int>, bool> Ship::getSquares() {
    return _squares;
}
