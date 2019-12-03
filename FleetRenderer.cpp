//
// Created by Colin Siles on 2019-12-02.
//

#include "FleetRenderer.h"

FleetRenderer::FleetRenderer(Fleet &fleet, BoardRenderer *boardRenderer, RenderWindow &window) {
    _boardRenderer = boardRenderer;
    _activeIndex = -1;
    _spaceCount = 0;

    _window = &window;

    for(int i = 0; i < fleet.size(); i++) {
        ShipRenderer newShipRenderer(window, &fleet.ship(i), _boardRenderer->getDispX() + 525, _boardRenderer->getDispY() + 25 +i * 100);

        _shipRenderers.push_back(newShipRenderer);
    }
}

void FleetRenderer::draw(bool placingShips) {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).draw(placingShips);
    }
}

void FleetRenderer::resetLocations() {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).setXY(_boardRenderer->getDispX()+ 525, _boardRenderer->getDispX() + 25 + i * 100);
    }
}

void FleetRenderer::updateSpacePress() {
    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        _spaceCount += 1;
    } else {
        _spaceCount = 0;
    }
}

void FleetRenderer::updateShipPos(Vector2i mousePos) {
    if(_activeIndex != -1) {
        double xPos = mousePos.x + _activeOffsetX ;
        double yPos = mousePos.y + _activeOffsetY;

        pair<int, int> gridPos = _boardRenderer->getGridPos(xPos, yPos);

        int rowPos = gridPos.first;
        int columnPos = gridPos.second;

        _currentShip->setGridPos(rowPos, columnPos);

        if(_boardRenderer->getBoard().shipFits(*_currentShip)) {
            xPos = rowPos * 50 + _boardRenderer->getDispX();
            yPos = columnPos * 50 + _boardRenderer->getDispY();
        }

        _shipRenderers.at(_activeIndex).setXY(xPos, yPos);
    }
}

void FleetRenderer::selectShip(Vector2i mousePos) {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        if(_shipRenderers.at(i).contains(mousePos) && !_shipRenderers.at(i).getShip().isPlaced()) {
            _activeIndex = i;
            _currentShip = &_shipRenderers.at(_activeIndex).getShip();

            _activeOffsetX = -mousePos.x + _shipRenderers.at(i).getDispX();
            _activeOffsetY = -mousePos.y + _shipRenderers.at(i).getDispY();

            _window->setMouseCursorVisible(false);
        }
    }
}

void FleetRenderer::lockShip() {
    if(_boardRenderer->getBoard().shipFits(*_currentShip)) {
        _boardRenderer->getBoard().placeShip(*_currentShip);

        _activeIndex = -1;

        _window->setMouseCursorVisible(true);
    }
}

void FleetRenderer::handleClick(Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(mousePos);
    } else {
        lockShip();
    }
}

bool FleetRenderer::spacePressed() {
    return _spaceCount == 1;
}

void FleetRenderer::rotateShip() {
    if(_activeIndex >= 0) {
        _currentShip->rotate();
    }
}