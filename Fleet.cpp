//
// Created by Colin Siles on 2019-12-02.
//

#include "Fleet.h"


Fleet::Fleet(vector<int> lengths) {
    for(int i = 0; i < lengths.size(); i++) {
        Ship newShip(lengths.at(i));

        _ships.push_back(newShip);
    }
}

int Fleet::markShipHit(int xPos, int yPos) {
    for(int i = 0; i < _ships.size(); i++) {
        if(_ships.at(i).contains(xPos, yPos)) {
            _ships.at(i).markAsHit(xPos, yPos);

            bool sunken = _ships.at(i).isSunk();

            return sunken ? i : -1;
        }
    }

    return -1;
}

int Fleet::size() {
    return _ships.size();
};

Ship &Fleet::ship(int index) {
    return _ships.at(index);
}

bool Fleet::allPlaced() {
    for(int i = 0; i < _ships.size(); i++) {
        if(!_ships.at(i).isPlaced()) {
            return false;
        }
    }

    return true;
}

bool Fleet::allSunk() {
    for(int i = 0; i < _ships.size(); i++) {
        if(!_ships.at(i).isSunk()) {
            return false;
        }
    }

    return true;
}