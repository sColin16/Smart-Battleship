//
// Created by Colin Siles on 2019-12-02.
//

#include "ShipRenderer.h"


ShipRenderer::ShipRenderer(RenderWindow &window, Ship *ship, double dispX, double dispY) {
    _window = &window;
    _ship = ship;
    _dispX = dispX;
    _dispY = dispY;
}

void ShipRenderer::draw(bool placingShips) {
    if(!_ship->_placed || !placingShips) {
        Orientation drawOrientation = _ship->_orientation;

        if(!placingShips) {
            drawOrientation = HORIZONTAL;
        }

        int xStep = drawOrientation == HORIZONTAL;
        int yStep = drawOrientation == VERTICAL;

        for(int i = 0; i < _ship->getLength(); i++) {
            RectangleShape shipPart(Vector2f(50, 50));
            shipPart.setPosition(_dispX + i * xStep * 50, _dispY + i * yStep * 50);
            shipPart.setOutlineColor(Color::Black);
            shipPart.setOutlineThickness(2);

            shipPart.setFillColor(_ship->isSunk() ? Color::Red : Color(125, 125, 125));

            _window->draw(shipPart);
        }
    }
}

void ShipRenderer::setXY(double xPos, double yPos) {
    _dispX = xPos;
    _dispY = yPos;
}

Ship &ShipRenderer::getShip() {
    return *_ship;
};

double ShipRenderer::getDispX() {
    return _dispX;
}

double ShipRenderer::getDispY() {
    return _dispY;
}

bool ShipRenderer::contains(Vector2i mousePos) {
    int x = mousePos.x;
    int y = mousePos.y;

    int width =  50 * (_ship->_orientation == HORIZONTAL ? _ship->_length : 1);
    int height = 50 * (_ship->_orientation == VERTICAL ? 1 : _ship->_length);

    return x >= _dispX && x <= _dispX + width && y >= _dispY && y <= _dispY + height;
}
