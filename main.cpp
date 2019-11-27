//
// Created by Colin Siles on 2019-11-17.
//

#include <iostream>                             // for standard input/output
using namespace std;                            // using the standard namespace

#include <SFML/Graphics.hpp>                    // include the SFML Graphics Library
using namespace sf;                             // using the sf namespace

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <string>
#include <utility>

enum Orientation {HORIZONTAL, VERTICAL};
// Make A vector of squares instead of map, and yeah, that should be good
//struct Coordinate {
//    int x;
//    int y;
//};
//
//Coordinate makeCoordinate(int x, int y) {
//    Coordinate coordinate;
//    coordinate.x = x;
//    coordinate.y = y;
//
//    return coordinate;
//}
//
//struct Square {
//    Coordinate location;
//    bool hit;
//};
//
//Square makeSquare(Coordinate location, bool hit) {
//    Square square;
//    square.location = location;
//    square.hit = hit;
//
//    return square;
//}

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

    //TOOD: remove this after done testing
    friend class IntelligentComputer;

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

Ship::Ship(int length) {
    _sunk = false;
    _placed = false;
    _boardX = -1;
    _boardY = -1;
    _orientation = HORIZONTAL;

    _length = length;
}

void Ship::rotate() {
    _orientation = _orientation == HORIZONTAL ? VERTICAL : HORIZONTAL;
}

void Ship::setHorizontal() {
    _orientation = HORIZONTAL;
}

void Ship::setGridPos(int xPos, int yPos) {
    int xStep = _orientation == HORIZONTAL;
    int yStep = _orientation == VERTICAL;

    _boardX = xPos;
    _boardY = yPos;

    _squares.clear();

    for(int i = 0; i < _length; i++) {
        int newSquareX = xPos + xStep * i;
        int newSquareY = yPos + yStep * i;

        _squares.emplace(make_pair(newSquareX, newSquareY), false);
    }
}

void Ship::markAsHit(int xPos, int yPos) {
    // Find the square that was hit
    const auto &square = _squares.find(make_pair(xPos, yPos));

    // Change its value to true
    square->second = true;

    checkIfSunk();

}

bool Ship::isPlaced() {
    return _placed;
}

bool Ship::isSunk() {
    return _sunk;
}

void Ship::markAsSunk() {
    _sunk = true;
}

bool Ship::contains(int xPos, int yPos) {
    return _squares.count(make_pair(xPos, yPos));
}

void Ship::checkIfSunk() {
    _sunk = true;

    for(auto &square : _squares) {
        if(!square.second) {
            _sunk = false;
            break;
        }
    }
}

int Ship::getLength() {
    return _length;
}

map<pair<int, int>, bool> Ship::getSquares() {
    return _squares;
}

class Fleet {
public:
    Fleet(vector<int> lengths);

    int markShipHit(int xPos, int yPos); // Returns the index of the ship sunk, or -1 if non sunk

    int size(); // Returns the size of the fleet
    Ship &ship(int index);

    bool allPlaced();
    bool allSunk();

private:
    vector<Ship> _ships;
};

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


struct ShotOutcome {
    bool hit;
    int sunkenIndex;
};

ShotOutcome makeOutcome(bool hit, int sunkenIndex) {
    ShotOutcome outcome;
    outcome.hit = hit;
    outcome.sunkenIndex = sunkenIndex;

    return outcome;
}

class Board {
public:
    Board(Fleet &fleet);

    ShotOutcome fireShotAt(int xPos, int yPos); // Returns true is shot is a hit, false if a miss (must check validity first)
    bool shipFits(Ship ship);
    void placeShip(Ship &ship); // Pas by reference to store reference to the ship so the board can sink it when appropropriate

    void markShot(int xPos, int yPos, ShotOutcome outcome);
    bool validGuess(int xPos, int yPos); // Returns true if guess is valid (in grid and not already guessed) called by tracking grids, primarily

    static const int GRID_SIZE = 10; // In theory, this could be changed to be a parameter, although renderer class probably couldnt handle that easily
    enum SquareState {BLANK, SHIP, HIT_MARKER, MISS_MARKER};
    friend class BoardRenderer;
    friend class IntelligentComputer;

private:
    Fleet *_fleet;
    vector<vector<SquareState>> _grid; // We intialize this in the constructor

    bool posInsideGrid(int xPos, int yPos); // Checks that a shot is within the bounds of the board
};

Board::Board(Fleet &fleet) {
    for(int i = 0; i < GRID_SIZE; i++) {
        vector<SquareState> newRow(GRID_SIZE, BLANK);
        _grid.push_back(newRow);
    }

    _fleet = &fleet;
}

ShotOutcome Board::fireShotAt(int xPos, int yPos) {
    bool hit = _grid.at(xPos).at(yPos) == SHIP;
    _grid.at(xPos).at(yPos) = hit ? HIT_MARKER : MISS_MARKER;

    int sunkIndex = hit ? _fleet->markShipHit(xPos, yPos) : -1;

    return makeOutcome(hit, sunkIndex);
}

bool Board::shipFits(Ship ship) {
    if(ship._boardX < 0 || ship._boardY < 0 ){
        return false;
    }

    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;
        int xPos = squarePos.first;
        int yPos = squarePos.second;

        if(!posInsideGrid(xPos, yPos) || _grid.at(xPos).at(yPos) == SHIP || _grid.at(xPos).at(yPos) == MISS_MARKER) {
            return false;
        }
    }

    return true;
}

void Board::placeShip(Ship &ship) {
    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;

        _grid.at(squarePos.first).at(squarePos.second) = SHIP;
    }

    ship._placed = true;
}

void Board::markShot(int xPos, int yPos, ShotOutcome outcome) {
    if(outcome.hit) {
        _grid.at(xPos).at(yPos) = HIT_MARKER;
    } else {
        _grid.at(xPos).at(yPos) = MISS_MARKER;
    }

    if(outcome.sunkenIndex >= 0) {
        _fleet->ship(outcome.sunkenIndex).markAsSunk();
    }
}

bool Board::validGuess(int xPos, int yPos) {
    return posInsideGrid(xPos, yPos) && (_grid.at(xPos).at(yPos) == BLANK || _grid.at(xPos).at(yPos) == BLANK);
}

bool Board::posInsideGrid(int xPos, int yPos){
    return xPos >=0 && xPos < GRID_SIZE && yPos >= 0 && yPos < GRID_SIZE;
}

class Player {
public:
    Player(string name, vector<int> shipLengths);

    virtual pair<int, int> getMove() = 0;
    virtual void placeShips() = 0;
    void placeShipsRandomly();

    ShotOutcome fireShotAt(int xPos, int yPos);

    virtual void markShot(int xPos, int yPos, ShotOutcome outcome);

    string getName();

    bool allShipsPlaced();
    bool allShipsSunk();

protected:
    Board _primaryBoard;
    Board _trackingBoard;

    Fleet _primaryFleet;
    Fleet _trackingFleet;
    string _name;
};

Player::Player(string name, vector<int> shipLengths) : _primaryFleet(shipLengths), _trackingFleet(shipLengths),
        _primaryBoard(_primaryFleet), _trackingBoard(_trackingFleet) {
    _name = name;
}

void Player::placeShipsRandomly() {
    for(int i = 0; i < _primaryFleet.size(); i++) {
        while(true) {
            int randomX = rand() % 10;
            int randomY = rand() % 10;

            if(rand() % 2) {
                _primaryFleet.ship(i).rotate();
            }

            _primaryFleet.ship(i).setGridPos(randomX, randomY);

            if(_primaryBoard.shipFits(_primaryFleet.ship(i))) {
                _primaryBoard.placeShip(_primaryFleet.ship(i));

                break;
            }
        }
    }
}

ShotOutcome Player::fireShotAt(int xPos, int yPos) {
    return _primaryBoard.fireShotAt(xPos, yPos);
}

void Player::markShot(int xPos, int yPos, ShotOutcome outcome) {
    _trackingBoard.markShot(xPos, yPos, outcome);
}

string Player::getName() {
    return _name;
}

bool Player::allShipsPlaced() {
    return _primaryFleet.allPlaced();
}

bool Player::allShipsSunk() {
    return _primaryFleet.allSunk();
}


class ComputerRandomPlayer : public Player {
public:
    using Player::Player;

    pair<int, int> getMove() override;
    void placeShips() override;
};

pair<int, int> ComputerRandomPlayer::getMove() {
    while(true) {
        int randomRow = rand() % 10;
        int randomCol = rand() % 10;

        if(_trackingBoard.validGuess(randomRow, randomCol)) {
            return make_pair(randomRow, randomCol);
        }
    }
}

void ComputerRandomPlayer::placeShips() {
    placeShipsRandomly();
}

class IntelligentComputer : public Player {
public:
    using Player::Player;

    vector<pair<int, int>> hitList;
    pair<int, int> lastHit;

    void markShot(int xPos, int yPos, ShotOutcome outcome) override;
    void markAsSunk(int xPos, int yPos, int getLength);

    vector<vector<int>> newProbabilityGrid();

    void addToDensity(Ship ship, vector<vector<int>> &probabilityGrid);
    vector<vector<int>> findSearchProbability();
    vector<vector<int>> findDestroyProbability();
    pair<int, int> chooseFromProbability(vector<vector<int>> probabilityGrid);

    pair<int, int> getMove() override;
    void placeShips() override;
};

void IntelligentComputer::markShot(int xPos, int yPos, ShotOutcome outcome) {
    Player::markShot(xPos, yPos, outcome);

    if(outcome.hit) {
        hitList.push_back(make_pair(xPos, yPos));
    }

    if(outcome.sunkenIndex >= 0) {
        markAsSunk(xPos, yPos, _trackingFleet.ship(outcome.sunkenIndex).getLength());
    } else if(outcome.hit){
        lastHit = make_pair(xPos, yPos);
    }
}

void IntelligentComputer::markAsSunk(int xPos, int yPos, int length) {
    pair<int, int> direction;

    if(lastHit.first - xPos == 0) {
        direction = make_pair(0, 1);
    } else {
        direction = make_pair(1, 0);
    }

    pair<int, int> start = make_pair(-1, -1);
    int consecutiveHits = 0;

    for(int i = -length + 1; i < length; i++) {
        int xMark = xPos + direction.first * i;
        int yMark = yPos + direction.second * i;

        if(_trackingBoard.posInsideGrid(xMark, yMark) && _trackingBoard._grid.at(xMark).at(yMark) == Board::HIT_MARKER) {
            consecutiveHits++;

            if(start.first < 0) {
                start = make_pair(xMark, yMark);
            }
        }
    }

    if(consecutiveHits >= length) {
        if(consecutiveHits > length) {
            int diff = consecutiveHits - length;
            //cerr << "Ambiguous Case!! " << diff * 2 << " of " << length <<  "Unknown State squares" << endl;
            length -= diff;

            start.first += direction.first * diff;
            start.second += direction.second * diff;
        }

        for(int i = 0; i < length; i++) {
            int xMark = start.first + direction.first * i;
            int yMark = start.second + direction.second * i;

            _trackingBoard._grid.at(xMark).at(yMark) = Board::SHIP;

            for (int j = 0; j < hitList.size(); j++) {
                if (hitList.at(j).first == xMark && hitList.at(j).second == yMark) {
                    hitList.erase(hitList.begin() + j);
                    break;
                }
            }
        }
    } else {
        if(direction.first == 0) {
            direction = make_pair(1, 0);
        } else {
            direction = make_pair(0, 1);
        }

        start = make_pair(-1, -1);
        consecutiveHits = 0;

        for(int i = -length + 1; i < length; i++) {
            int xMark = xPos + direction.first * i;
            int yMark = yPos + direction.second * i;

            if(_trackingBoard.posInsideGrid(xMark, yMark) && _trackingBoard._grid.at(xMark).at(yMark) == Board::HIT_MARKER) {
                consecutiveHits++;

                if(start.first < 0) {
                    start = make_pair(xMark, yMark);
                }
            }
        }

        if(consecutiveHits >= length) {
            for(int i = 0; i < length; i++) {
                int xMark = start.first + direction.first * i;
                int yMark = start.second + direction.second * i;

                _trackingBoard._grid.at(xMark).at(yMark) = Board::SHIP;

                for (int j = 0; j < hitList.size(); j++) {
                    if (hitList.at(j).first == xMark && hitList.at(j).second == yMark) {
                        hitList.erase(hitList.begin() + j);
                        break;
                    }
                }
            }
        } else {
            //cerr << "Something went wrong determing where the sunken ship was located" << endl;
        }
    }
}

vector<vector<int>> IntelligentComputer::newProbabilityGrid() {
    vector<vector<int>> output;

    for(int i = 0; i < 10; i++) {
        vector<int> newRow(10, 0);
        output.push_back(newRow);
    }

    return output;
}

void IntelligentComputer::addToDensity(Ship ship, vector<vector<int>> &probabilityGrid) {
    for(auto &square : ship.getSquares()) {
        pair<int, int> squarePos = square.first;

        probabilityGrid.at(squarePos.first).at(squarePos.second) += 1;
    }
}

vector<vector<int>> IntelligentComputer::findSearchProbability() {
    vector<vector<int>> probabilityGrid = newProbabilityGrid();

    for(int i = 0; i < _trackingFleet.size(); i++) {
        Ship currentShip = _trackingFleet.ship(i);

        if(currentShip.isSunk()) {
            continue;
        }

        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < 10; k++) {
                currentShip.setHorizontal();
                currentShip.setGridPos(j, k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }

                currentShip.rotate();
                currentShip.setGridPos(j, k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }
            }
        }
    }

    return probabilityGrid;
}

vector<vector<int>> IntelligentComputer::findDestroyProbability() {
    vector<vector<int>> probabilityGrid = newProbabilityGrid();

    for(int i = 0; i < hitList.size(); i++) {
        for(int j = 0; j < _trackingFleet.size(); j++) {
            Ship currentShip = _trackingFleet.ship(j);

            if(currentShip.isSunk()) {
                continue;
            }

            for(int k = 0; k < currentShip.getLength(); k++) {
                currentShip.setHorizontal();
                currentShip.setGridPos(hitList.at(i).first - k, hitList.at(i).second);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }

                currentShip.rotate();
                currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second - k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }
            }

            currentShip.setHorizontal();
            currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second);
            if(_trackingBoard.shipFits(currentShip)) {
                addToDensity(currentShip, probabilityGrid);
            }

            currentShip.rotate();
            currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second);
            if(_trackingBoard.shipFits(currentShip)) {
                addToDensity(currentShip, probabilityGrid);
            }
        }
    }

    return probabilityGrid;
}

pair<int, int> IntelligentComputer::chooseFromProbability(vector<vector<int>> probabilityGrid) {
    int maxVal = 0;
    vector<pair<int, int>> maxCoords;

    for(int i = 0; i < probabilityGrid.size(); i++) {
        for(int j = 0; j < probabilityGrid.at(i).size(); j++) {
            if(probabilityGrid.at(i).at(j) > maxVal && _trackingBoard.validGuess(i, j)) {
                maxVal = probabilityGrid.at(i).at(j);

                maxCoords.clear();
                maxCoords.push_back(make_pair(i, j));

            } else if(probabilityGrid.at(i).at(j) == maxVal && _trackingBoard.validGuess(i, j)) {
                maxCoords.push_back(make_pair(i, j));
            }
        }
    }

    // This section is necessary in case all of the probabilities were 0

    if(maxCoords.size() > 0) {
        int randIndex = rand() % maxCoords.size();
        return maxCoords.at(randIndex);
    } else {
        return make_pair(0, 0);
    }
}

pair<int, int> IntelligentComputer::getMove() {
    vector<vector<int>> probabilityGrid;

    if(hitList.empty()) {
        probabilityGrid = findSearchProbability();
    } else {
        probabilityGrid = findDestroyProbability();
        pair<int, int> maxPos = chooseFromProbability(probabilityGrid);

        if(probabilityGrid.at(maxPos.first).at(maxPos.second) == 0) {
            //cerr << "SOMETHING WENT WRONG. NOW RESETING" << endl;

            hitList.clear();
            probabilityGrid = findSearchProbability();
        } else {
            return maxPos;
        }
    }

    return chooseFromProbability(probabilityGrid);
}

void IntelligentComputer::placeShips() {
    placeShipsRandomly();
}

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


// Responsible for rendering the grid and the fleet (depending on the context of if your being placed or playing)
class BoardRenderer {
public:
    BoardRenderer(RenderWindow &window, Board &board, Fleet &fleet, double dispX, double dispY);

    void draw();
    void drawStatusSquare(double mouseX, double mouseY);
    pair<int, int> getGridPos(double xPos, double yPos);

    double getDispX();
    double getDispY();

    Board &getBoard();

protected:
    RenderWindow *_window;
    Board *_board;

    double _dispX;
    double _dispY;
};


class FleetRenderer {
public:
    FleetRenderer(Fleet &fleet, BoardRenderer *boardRenderer, RenderWindow &window);
    void draw(bool placingShips);
    void resetLocations();

    void updateSpacePress();
    void updateShipPos(Vector2i mousePos);
    void selectShip(Vector2i mousePos);
    void lockShip(Vector2i mousePos);
    void handleClick(Vector2i mousePos);
    bool spacePressed();
    void rotateShip();

private:
    BoardRenderer *_boardRenderer;
    vector<ShipRenderer> _shipRenderers;
    RenderWindow *_window;
    int _spaceCount;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;

    // TODO: maybe also store the active ship to clean up the code a bit?
};

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

        _shipRenderers.at(_activeIndex).getShip().setGridPos(rowPos, columnPos);

        if(_boardRenderer->getBoard().shipFits(_shipRenderers.at(_activeIndex).getShip())) {
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

            _activeOffsetX = -mousePos.x + _shipRenderers.at(i).getDispX();
            _activeOffsetY = -mousePos.y + _shipRenderers.at(i).getDispY();

            _window->setMouseCursorVisible(false);
        }
    }
}

void FleetRenderer::lockShip(Vector2i mousePos) {
    if(_boardRenderer->getBoard().shipFits((_shipRenderers.at(_activeIndex).getShip()))) { // TODO: this code sucks. Make it good
        _boardRenderer->getBoard().placeShip(_shipRenderers.at(_activeIndex).getShip());

        _activeIndex = -1;

        _window->setMouseCursorVisible(true);
    }
}

void FleetRenderer::handleClick(Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(mousePos);
    } else {
        lockShip(mousePos);
    }
}

bool FleetRenderer::spacePressed() {
    return _spaceCount == 1;
}

void FleetRenderer::rotateShip() {
    _shipRenderers.at(_activeIndex).getShip().rotate();
}

BoardRenderer::BoardRenderer(RenderWindow &window, Board &board, Fleet &fleet, double dispX, double dispY) {
    _window = &window;
    _board = &board;
    _dispX = dispX;
    _dispY = dispY;
}

void BoardRenderer::draw() {
    for(int i = 0; i < Board::GRID_SIZE; i++) {
        for(int j = 0; j < 10; j++) {
            Board::SquareState value = _board->_grid.at(i).at(j);

            Color color;

            if (value == Board::BLANK || value == Board::MISS_MARKER) {
                color = Color::Blue;
            } else {
                color = Color(120, 120, 120);
            }

            RectangleShape square(Vector2f(50, 50));
            square.setPosition(_dispX + 50 * i, _dispY + 50 * j);
            square.setFillColor(color);
            square.setOutlineColor(Color::Black);
            square.setOutlineThickness(2);
            _window->draw(square);

            if (value == Board::MISS_MARKER || value == Board::HIT_MARKER) {
                CircleShape marker(20, 20);
                marker.setPosition(_dispX + 5 + 50 * i, _dispY + 5 + 50 * j);
                marker.setFillColor(value == Board::MISS_MARKER ? Color::White : Color::Red);
                _window->draw(marker);
            }
        }
    }
}

void BoardRenderer::drawStatusSquare(double mouseX, double mouseY) {
    pair<int, int> gridPos = getGridPos(mouseX, mouseY);

    int xPos = gridPos.first;
    int yPos = gridPos.second;

    if(xPos >=0 && xPos < 10 && yPos >=0 && yPos < 10) {
        RectangleShape selectionSquare(Vector2f(50, 50));
        selectionSquare.setPosition(xPos * 50 + _dispX, yPos * 50 + _dispY);
        selectionSquare.setFillColor(Color(255, 255, 255, 100));

        _window->draw(selectionSquare);
    }
}

pair<int, int> BoardRenderer::getGridPos(double xPos, double yPos) {
    int rowPos = (xPos - _dispX) / 50;
    int columnPos = (yPos - _dispY) / 50;

    return make_pair(rowPos, columnPos);
}

Board &BoardRenderer::getBoard() {
    return *_board;
}

double BoardRenderer::getDispX() {
    return _dispX;
}

double BoardRenderer::getDispY() {
    return _dispY;
}



class HumanSFMLPlayer : public Player {
public:
    HumanSFMLPlayer(string name, vector<int> shipLengths);

    pair<int, int> getMove() override;
    void placeShips() override;

private:
    RenderWindow _window;

    BoardRenderer _pBoardRenderer;
    BoardRenderer _tBoardRenderer;

    FleetRenderer _pFleetRenderer;
    FleetRenderer _tFleetRenderer;
};

HumanSFMLPlayer::HumanSFMLPlayer(string name, vector<int> shipLengths) : Player(name, shipLengths),
        _window(VideoMode(1625, 700), "SFML Example Window"),
        _pBoardRenderer(_window, _primaryBoard, _primaryFleet, 25, 50),
        _tBoardRenderer(_window, _trackingBoard, _trackingFleet, 825, 50),
        _tFleetRenderer(_trackingFleet, &_tBoardRenderer, _window),
        _pFleetRenderer(_primaryFleet, &_pBoardRenderer, _window){}

void HumanSFMLPlayer::placeShips() {
    while(_window.isOpen()) {
        _window.clear( Color::Black );

        _pBoardRenderer.draw();
        _tBoardRenderer.draw();

        _pFleetRenderer.draw(true);
        _tFleetRenderer.draw(true);

        Vector2i mousePos = sf::Mouse::getPosition(_window);
        _pFleetRenderer.updateSpacePress();

        _pFleetRenderer.updateShipPos(mousePos);

        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            _pFleetRenderer.handleClick(mousePos);
        }

        if(_pFleetRenderer.spacePressed()) {
            _pFleetRenderer.rotateShip();
        }

        if(allShipsPlaced()) {
            _pFleetRenderer.resetLocations(); // Reset the ships that were placed before to their original coordinates
            return;
        }

        _window.display();

        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
            }
        }

    }
}

pair<int, int> HumanSFMLPlayer::getMove() {
    while(_window.isOpen()) {
        _window.clear( Color::Black );

        _pBoardRenderer.draw();
        _tBoardRenderer.draw();

        _pFleetRenderer.draw(false);
        _tFleetRenderer.draw(false);

        Vector2i mousePos = sf::Mouse::getPosition(_window);

        _tBoardRenderer.drawStatusSquare(mousePos.x, mousePos.y);

        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            pair<int, int> move = _tBoardRenderer.getGridPos(mousePos.x, mousePos.y);

            if(_trackingBoard.validGuess(move.first, move.second)) {
                return move;
            }
        }

        _window.display();

        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
                return make_pair(-1, -1);
            }
        }
    }
}

// Handles doing the file input/output stuff that is required for the project
class Battlelog {
public:
    Battlelog(string filename, string p1Name, string p2Nmae);
    ~Battlelog();

    ofstream battlelogFile;

    bool firstPlayer;

    char convertYPos(int yPos);
    int convertXPos(int xPos);
    string getOutcomeString(ShotOutcome outcome);

    void writeSeparator();
    void writeName(string name);
    void writeMove(int xPos, int yPos, ShotOutcome outcome);

    void recordMove(string name, int xPos, int yPos, ShotOutcome outcome);
    void recordWinner(string name);
};

Battlelog::Battlelog(string filename, string p1Name, string p2Name) {
    battlelogFile.open(filename);

    firstPlayer = true;

    if(!battlelogFile) {
        cerr << "Failed to open the battlelog" << endl;
        return;
    }

    writeSeparator();
    writeName(p1Name);
    writeName(p2Name);

    battlelogFile << "|" << endl;

    writeSeparator();
}

Battlelog::~Battlelog() {
    battlelogFile.close();
}

char Battlelog::convertYPos(int yPos) {
    return yPos + 'A';
}

int Battlelog::convertXPos(int xPos) {
    return xPos + 1;
}

string Battlelog::getOutcomeString(ShotOutcome outcome) {
    if(outcome.sunkenIndex >=0 ) {
        return "SUNK";
    }

    return outcome.hit ? "HIT" : "MISS";
}

void Battlelog::writeSeparator() {
    battlelogFile << "|" << setw(25) << setfill('=') << "" << "|" << endl;
}

void Battlelog::writeName(string name) {
    battlelogFile << "| " << setw(10) << setfill(' ') << left << name << " ";
}

void Battlelog::writeMove(int xPos, int yPos, ShotOutcome outcome) {
    battlelogFile << " " << setw(5) << setfill(' ') << left << (convertYPos(yPos) + to_string(convertXPos(xPos)));
    battlelogFile << " " << setw(4) << setfill(' ') << right << getOutcomeString(outcome) << " |";
}

void Battlelog::recordMove(string name, int xPos, int yPos, ShotOutcome outcome) {
    if(firstPlayer) {
        battlelogFile << "|";
    }

    writeMove(xPos, yPos, outcome);

    if(!firstPlayer) {
        battlelogFile << endl;
    }

    firstPlayer = !firstPlayer;
}

void Battlelog::recordWinner(string name) {
    if(!firstPlayer) { // This really means if the first player just won
        battlelogFile << setw(12) << setfill(' ') << "" << "|" << endl;
    }

    writeSeparator();
    battlelogFile << endl;
    battlelogFile << name << " wins!" << endl;
}

template<typename p1Type, typename p2Type>
class Game {
public:
    Game(string p1name, string p2name, vector<int> shipLengths = DEFAULT_LENGTHS, string battlelogName = "battelog.txt");

    p1Type playerOne;
    p2Type playerTwo;
    int turn;

    Battlelog battlelog;

    void runGame();

    static const vector<int> DEFAULT_LENGTHS;
};

template<typename p1Type, typename p2Type>
const vector<int> Game<p1Type, p2Type>::DEFAULT_LENGTHS = {5, 4, 4, 3, 2};

template<typename p1Type, typename p2Type>
Game<p1Type, p2Type>::Game(string p1Name, string p2Name, vector<int> shipLengths, string battlelogName) : playerOne(p1Name, shipLengths),
        playerTwo(p2Name, shipLengths), battlelog(battlelogName, p1Name, p2Name) {
    turn = 1;
}

template<typename p1Type, typename p2Type>
void Game<p1Type, p2Type>::runGame() {
    //playerOne.placeShips();
    playerOne.placeShipsRandomly();

    if(!playerOne.allShipsPlaced()) {
        cerr << playerOne.getName() <<  " failed to place all their ships" << endl;
        return;
    }

    playerTwo.placeShips();

    if(!playerTwo.allShipsPlaced()) {
        cerr << playerTwo.getName() << " failed to place all their ships" << endl;
        return;
    }

    int turns = 0;

    while(true) {
        if(turn == 1) {
            turns++;
            pair<int, int> move = playerOne.getMove();

            if(move.first < 0) {
                cerr << playerOne.getName() << " has forfeited the match" << endl;
                return;
            }

            ShotOutcome outcome = playerTwo.fireShotAt(move.first, move.second);
            playerOne.markShot(move.first, move.second, outcome);
            battlelog.recordMove(playerOne.getName(), move.first, move.second, outcome);

            if(playerTwo.allShipsSunk()) {
                battlelog.recordWinner(playerOne.getName());
                cerr << playerOne.getName() << " wins" << endl;
                cout << turns << endl;
                return;
            }

            turn = 2;
        } else if(turn == 2) {
            pair<int, int> move = playerTwo.getMove();

            if(move.first < 0) {
                cerr << playerTwo.getName() << " has forfeited the match" << endl;
                return;
            }

            ShotOutcome outcome = playerOne.fireShotAt(move.first, move.second);
            playerTwo.markShot(move.first, move.second, outcome);
            battlelog.recordMove(playerTwo.getName(), move.first, move.second, outcome);

            if(playerOne.allShipsSunk()) {
                battlelog.recordWinner(playerTwo.getName());
                cerr << playerTwo.getName() << " wins" << endl;
                cout << turns << endl;
                return;
            }

            turn = 1;
        }
    }
}

int main() {
    srand(time(0));

    vector<int> shipLengths = {3, 3, 3, 3, 3, 3};

    Game<HumanSFMLPlayer, IntelligentComputer> game("Human", "Computer");

    game.runGame();
}