/* FleetRenderer.cpp
 *
 * Author: Colin Siles
 *
 * The FleetRenderer class, wraps the ShipRenderer class to display multiple ships onto an SFML window.
 * It also provides numerous member functions to support the HumanSFMLPlayer class in placing ships
 * onto their board using the mouse
*/

#include "FleetRenderer.h"

// Primary Constructor
FleetRenderer::FleetRenderer(Fleet &fleet, BoardRenderer *boardRenderer, RenderWindow &window) {
    // Store the pointer to the boardRenderer, and reference to the window
    _boardRenderer = boardRenderer;
    _window = &window;

    // Provide defaults for these members
    _activeIndex = -1;
    _spaceCount = 0;

    // For each ship in the fleet
    for(int i = 0; i < fleet.size(); i++) {
        // Create a ship Renderer object
        ShipRenderer newShipRenderer(window, &fleet.ship(i), _boardRenderer->getDispX() + 525, _boardRenderer->getDispY() + 25 +i * 100);

        // Add it the vector of shipRenderer
        _shipRenderers.push_back(newShipRenderer);
    }
}

// Draws all of the ships
// The placingShips parameter will hide placed ships if ships are being placed (since they are already displayed by the
// BoardRenderer class)
void FleetRenderer::draw(bool placingShips) {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).draw(placingShips);
    }
}

// Resets the position of all the ships to be next to the board
void FleetRenderer::resetLocations() {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).setXY(_boardRenderer->getDispX()+ 525, _boardRenderer->getDispX() + 25 + i * 100);
    }
}

// Increments the spaceCount member to track how many frames the space bar has been held
void FleetRenderer::updateSpacePress() {
    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        _spaceCount += 1;
    } else {
        _spaceCount = 0;
    }
}

// If a ship is selected, this will update the ship's coordinates, snapping it the grid if the ship can be placed there
void FleetRenderer::updateShipPos(Vector2i mousePos) {
    // Ensure a ship has been selected
    if(_activeIndex != -1) {
        // Determine the default location to draw at (no snapping)
        double xPos = mousePos.x + _activeOffsetX;
        double yPos = mousePos.y + _activeOffsetY;

        // Get where in the grid the ship is attempting to be placed
        pair<int, int> gridPos = _boardRenderer->getGridPos(xPos, yPos);

        int rowPos = gridPos.first;
        int columnPos = gridPos.second;

        // Set the ship to that location
        _currentShip->setGridPos(rowPos, columnPos);

        // If the ship fits...
        if(_boardRenderer->getBoard().shipFits(*_currentShip)) {
            // Then reset the coordinates to snap it the grid of the board
            xPos = rowPos * 50 + _boardRenderer->getDispX();
            yPos = columnPos * 50 + _boardRenderer->getDispY();
        }

        // Reset the current ship's x and y position for drawing
        _shipRenderers.at(_activeIndex).setXY(xPos, yPos);
    }
}

// Selects a ship to be placed (only called when no ship has been selected, and a click occured)
void FleetRenderer::selectShip(Vector2i mousePos) {
    // Iterate over each possible ship
    for(int i = 0; i < _shipRenderers.size(); i++) {
        // If the mouse is inside the bounds of a ship that hasn't been placed
        if(_shipRenderers.at(i).contains(mousePos) && !_shipRenderers.at(i).getShip().isPlaced()) {
            // Set the activeIndex and currentShip
            _activeIndex = i;
            _currentShip = &_shipRenderers.at(_activeIndex).getShip();

            // Set the offset according to the difference in where the ship was lick on relative to the ship
            _activeOffsetX = -mousePos.x + _shipRenderers.at(i).getDispX();
            _activeOffsetY = -mousePos.y + _shipRenderers.at(i).getDispY();

            // Make the cursor invisible (so that the mouse isn't floating if the user rotates a ship)
            _window->setMouseCursorVisible(false);
        }
    }
}

// Places a ship, if it can be placed there (called only when a ship is selected, and there is a click)
void FleetRenderer::lockShip() {
    // If the ship fits
    if(_boardRenderer->getBoard().shipFits(*_currentShip)) {
        // Place the ship
        _boardRenderer->getBoard().placeShip(*_currentShip);

        // Reset the activeIndex to mark that no ship has been selected
        _activeIndex = -1;

        // Make the cursor visible again
        _window->setMouseCursorVisible(true);
    }
}

// Upper level function to determine if a ship should be selected or locked, based on if a ship is already selected
void FleetRenderer::handleClick(Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(mousePos);
    } else {
        lockShip();
    }
}

// If the space bar was just pressed, return true
bool FleetRenderer::spacePressed() {
    return _spaceCount == 1;
}

// As long as a ship has been selected, rotate the active ship
void FleetRenderer::rotateShip() {
    if(_activeIndex >= 0) {
        _currentShip->rotate();
    }
}