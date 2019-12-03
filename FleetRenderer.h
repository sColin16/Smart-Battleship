//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_FLEETRENDERER_H
#define SFML_TEMPLATE_FLEETRENDERER_H

#include "BoardRenderer.h"
#include "Fleet.h"
#include "Ship.h"
#include "ShipRenderer.h"

#include <vector>

using namespace std;

class FleetRenderer {
public:
    FleetRenderer(Fleet &fleet, BoardRenderer *boardRenderer, RenderWindow &window);
    void draw(bool placingShips);
    void resetLocations();

    void updateSpacePress();
    void updateShipPos(Vector2i mousePos);
    void selectShip(Vector2i mousePos);
    void lockShip();
    void handleClick(Vector2i mousePos);
    bool spacePressed();
    void rotateShip();

private:
    BoardRenderer *_boardRenderer;
    vector<ShipRenderer> _shipRenderers;
    RenderWindow *_window;
    Ship *_currentShip;
    int _spaceCount;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;

    // TODO: maybe also store the active ship to clean up the code a bit?
};


#endif //SFML_TEMPLATE_FLEETRENDERER_H
