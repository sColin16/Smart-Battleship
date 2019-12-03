//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_SHIPRENDERER_H
#define SFML_TEMPLATE_SHIPRENDERER_H

#include <SFML/Graphics.hpp>
using namespace sf;

#include "Ship.h"

// Responsible for rendering a single ship
class ShipRenderer {
public:
    ShipRenderer(RenderWindow &window, Ship *ship, double dispX, double dispY);
    void draw(bool showPlaced);

    void setXY(double xPos, double yPos);

    Ship &getShip();
    double getDispX();
    double getDispY();

    bool contains(Vector2i mousePos);

private:
    RenderWindow *_window;
    Ship *_ship;
    double _dispX;
    double _dispY;
};


#endif //SFML_TEMPLATE_SHIPRENDERER_H
