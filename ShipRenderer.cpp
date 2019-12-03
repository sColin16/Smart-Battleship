/* ShipRenderer.cpp
 *
 * Author: Colin Siles
 *
 * Similar to the BoardRenderer class, the ShipRenderer class wraps a ship in order to draw it on an SFML window
 * It also provides some helper functions to determine if the mouse is hover over the ship or not
*/

#include "ShipRenderer.h"

// Primary constructor: simple copy the arguments
ShipRenderer::ShipRenderer(RenderWindow &window, Ship *ship, double dispX, double dispY) {
    _window = &window;
    _ship = ship;
    _dispX = dispX;
    _dispY = dispY;
}

void ShipRenderer::draw(bool placingShips) {
    // Only draw if this ship hasn't been placed, or ships aren't being placed
    // The reason is that the board will draw placed ships, so this avoid duplicates being drawn
    if(!_ship->_placed || !placingShips) {
        Orientation drawOrientation = _ship->_orientation;

        // Draw horizontally if not placing ships, because they are always drawn that way on the side
        if(!placingShips) {
            drawOrientation = HORIZONTAL;
        }

        int xStep = drawOrientation == HORIZONTAL;
        int yStep = drawOrientation == VERTICAL;

        // Iterate through each square and draw the rectangle for the ship
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

// Returns true if the mousePos is within the ship's drawing boundaries
bool ShipRenderer::contains(Vector2i mousePos) {
    int x = mousePos.x;
    int y = mousePos.y;

    int width =  50 * (_ship->_orientation == HORIZONTAL ? _ship->_length : 1);
    int height = 50 * (_ship->_orientation == VERTICAL ? 1 : _ship->_length);

    return x >= _dispX && x <= _dispX + width && y >= _dispY && y <= _dispY + height;
}

// Various getters and setters for the class
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
