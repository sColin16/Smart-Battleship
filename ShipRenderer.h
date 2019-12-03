/* ShipRenderer.h
 *
 * Author: Colin Siles
 *
 * Similar to the BoardRenderer class, the ShipRenderer class wraps a ship in order to draw it on an SFML window
 * It also provides some helper functions to determine if the mouse is hover over the ship or not
*/

#ifndef SFML_TEMPLATE_SHIPRENDERER_H
#define SFML_TEMPLATE_SHIPRENDERER_H

#include <SFML/Graphics.hpp>
using namespace sf;

#include "Ship.h"

class ShipRenderer {
public:
    ShipRenderer(RenderWindow &window, Ship *ship, double dispX, double dispY);

    // Draws the ship, unless ships are being placed, and this one was placed
    void draw(bool placingShips);

    // Sets the coordinates of teh ship within the window
    void setXY(double xPos, double yPos);

    // Returns a reference to the ship for modifictation by the FleetRenderer and other classes
    Ship &getShip();

    // Getters for the coordinates of the ship
    double getDispX();
    double getDispY();

    // Returns true if the mousePos is within the ship's drawing boundaries
    bool contains(Vector2i mousePos);

private:
    RenderWindow *_window;
    Ship *_ship; // Store a pointer to the ship, so that it can be modified
    double _dispX;
    double _dispY;
};

#endif //SFML_TEMPLATE_SHIPRENDERER_H
