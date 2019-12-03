//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_SHIP_H
#define SFML_TEMPLATE_SHIP_H

#include <map>
#include <iostream>
#include <utility>

using namespace std;

enum Orientation {HORIZONTAL, VERTICAL};

class Ship {
public:
    Ship(int length);

    void rotate();
    void setHorizontal();
    void setGridPos(int xPos, int yPos);
    void markAsHit(int xPos, int yPos);
    bool isPlaced();
    bool isSunk();
    void markAsSunk();
    bool contains(int xPos, int yPos);
    int getLength();
    map<pair<int, int>, bool> getSquares();

    friend class Board;

    friend class ShipRenderer;

private:
    void checkIfSunk();

    map<pair<int ,int>, bool> _squares;
    bool _sunk;
    bool _placed;
    int _boardX;
    int _boardY;
    int _length;
    Orientation _orientation;
};

#endif //SFML_TEMPLATE_SHIP_H
