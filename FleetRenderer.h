/* FleetRenderer.h
 *
 * Author: Colin Siles
 *
 * The FleetRenderer class, wraps the ShipRenderer class to display multiple ships onto an SFML window.
 * It also provides numerous member functions to support the HumanSFMLPlayer class in placing ships
 * onto their board using the mouse
*/

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

    // Draws all the ships in the fleet
    void draw(bool placingShips);

    // Resets the entire fleet's location to default (used after the user has placed all their ships)
    void resetLocations();

    // Called by the HumanSFML player to determine if the space bar has been pressed
    void updateSpacePress();

    // Allows the fleet renderer to know where to draw the ship
    void updateShipPos(Vector2i mousePos);

    // Attempts to select the ship at the given location
    void selectShip(Vector2i mousePos);

    // Places the ship, if it fits
    void lockShip();

    // General function that will either select of lock a ship, depending on if a ship is selected
    void handleClick(Vector2i mousePos);

    // Returns true if the space bar was pressed (but false if its held for multiple frames)
    bool spacePressed();

    // Rotates the currently selected ship from horizontal to vertical, or vice versa
    void rotateShip();

private:
    // Pointer to the corresponding board renderer, to know where the ship is located
    // (Used to locking to grid and placing ships)
    BoardRenderer *_boardRenderer;

    // The Fleet Renderer just wraps the shipRenderer class in a vector
    vector<ShipRenderer> _shipRenderers;

    // Pointer to the window to draw on
    RenderWindow *_window;

    // Pointer to the current ship being placed
    Ship *_currentShip;

    // Number of consecutive frames the space bar has been held
    int _spaceCount;

    // The index of the currently selected ship
    int _activeIndex;

    // The offset that the ship was click on from the upper left-hand corner, to draw it at the correct position
    // relative to the mouse
    double _activeOffsetX;
    double _activeOffsetY;
};


#endif //SFML_TEMPLATE_FLEETRENDERER_H
