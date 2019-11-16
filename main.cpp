#include <iostream>                             // for standard input/output
using namespace std;                            // using the standard namespace

#include <SFML/Graphics.hpp>                    // include the SFML Graphics Library
using namespace sf;                             // using the sf namespace

//#include "BattleshipBoard.h"

#include <utility>
#include <string>
#include <set>

class Ship {
public:
    Ship(double x, double y, int length, bool horizontal);
    void draw(RenderWindow &window);
    double getX();
    double getY();
    int getLength();
    bool isHorizontal();

protected:
    double _x;
    double _y;
    int _length;
    bool _horizontal;
    Color _color;
};

Ship::Ship(double x, double y, int length ,bool horizontal) {
    this->_x = x;
    this->_y = y;
    this->_length = length;
    this->_horizontal = true;
    _color = Color(125, 125, 125);
}

void Ship::draw(RenderWindow &window) {
    int xOffset = _horizontal ? 1 : 0;
    int yOffset = _horizontal ? 0 : 1;

    for(int i = 0; i < _length; i++) {
        RectangleShape shipPart(Vector2f(50, 50));
        shipPart.setPosition(_x + i * xOffset * 50, _y + i * yOffset * 50);
        shipPart.setOutlineColor(Color::Black);
        shipPart.setOutlineThickness(2);
        shipPart.setFillColor(_color);

        window.draw(shipPart);
    }
}

double Ship::getX() {
    return _x;
}

double Ship::getY() {
    return _y;
}

int Ship::getLength() {
    return _length;
}

bool Ship::isHorizontal() {
    return _horizontal;
}

// Class for a ship that can be clicked, moved around, and placed onto the board
class PlaceableShip : public Ship{
public:
    using Ship::Ship; // Inherit the constructor of the ship class
    bool contains(Vector2i loc); // Checks if the ship contians the given coordinate for clicking purposes
    void updatePos(double x, double y);
    void rotate();
};

bool PlaceableShip::contains(Vector2i loc) {
    int x = loc.x;
    int y = loc.y;

    int width =  50 * (_horizontal ? _length : 1);
    int height = 50 * (_horizontal ? 1 : _length);

    return x >= this->_x && x <= this->_x + width && y >= this->_y && y <= this->_y + height;
}

void PlaceableShip::updatePos(double x, double y) {
    this->_x = x;
    this->_y = y;
}

void PlaceableShip::rotate() {
    _horizontal = !_horizontal; // Toggle the horizontal position for the object
}


// Class for ships that display whether or not certain ships have been sunk or not
class StatusShip : public Ship {
public:
    using Ship::Ship; // Inherit constructor of the ship class
    void sink();
    bool isSunk();

    bool _sunk;
};

void StatusShip::sink() {
    _sunk = true;
    this->_color = Color::Red;
}

bool StatusShip::isSunk() {
    return _sunk;
}


class BattleshipBoard {
public:
    BattleshipBoard();
    int getSquare(int row, int column, bool shipsHidden);
    int fireShot(int row, int column);
    bool shipFits(PlaceableShip ship, int rowPos, int colPos);
    void placeShip(PlaceableShip ship, int rowPos, int colPos); // Always make sure the ship fits first
    bool shipSunk(int index); // Return if a given ship was sunk or not

    int getNumShips();

    vector<vector<int>> *getAddress();

    vector<vector<int>> _grid;
    vector<set<pair<int, int>>> _ships;

    static const int _BLANK = 0;
    static const int _SHIP  = 1;
    static const int _MISS  = 2;
    static const int _HIT   = 3;
};

BattleshipBoard::BattleshipBoard() {
    // Initialize the board as a grid of blank squares
    for(int i = 0; i < 10; i++) {
        vector<int> newRow(10, 0);
        _grid.push_back(newRow);
    }
}

int BattleshipBoard::getSquare(int row, int column, bool shipsHidden = false) {
    if(row < 0 || row >= 10 || column < 0 || column >= 10) {
        return -1;
    }

    int value = _grid.at(row).at(column);

    if(shipsHidden && value == _SHIP) {
        value = _BLANK;
    }

    return value;
}

int BattleshipBoard::fireShot(int row, int column) {
    int value = getSquare(row, column);

    if(value == -1 || value == _HIT || value == _MISS) {
        return -1;

    } else if(value == _BLANK) {
        _grid.at(row).at(column) = _MISS;
        return 0;

    } else {
        _grid.at(row).at(column) = _HIT;

        for(int i = 0; i < _ships.size(); i++) {
            if(_ships.at(i).count(make_pair(row, column))) {
                _ships.at(i).erase(make_pair(row, column));
            }
        }

        return 1;
    }
}

bool BattleshipBoard::shipFits(PlaceableShip ship, int rowPos, int colPos) {
    int xOffset = ship.isHorizontal() ? 1 : 0;
    int yOffset = ship.isHorizontal() ? 0 : 1;

    for(int i = 0; i < ship.getLength(); i++) {
        if(getSquare(rowPos + xOffset * i, colPos + yOffset * i) != _BLANK) {
            return false;
        }
    }

    return true;
}

void BattleshipBoard::placeShip(PlaceableShip ship, int rowPos, int colPos) {
    int xOffset = ship.isHorizontal() ? 1 : 0;
    int yOffset = ship.isHorizontal() ? 0 : 1;

    set<pair<int, int>> newShip;

    for(int i = 0; i < ship.getLength(); i++) {
        int finalRowPos = rowPos + xOffset * i;
        int finalColPos = colPos + yOffset * i;

        _grid.at(finalRowPos).at(finalColPos) = _SHIP;
        newShip.insert(make_pair(finalRowPos, finalColPos));
    }

    _ships.push_back(newShip);
}

bool BattleshipBoard::shipSunk(int index) {
    return _ships.empty();
}

int BattleshipBoard::getNumShips() {
    return _ships.size();
}

vector<vector<int>> * BattleshipBoard::getAddress() {
    return &_grid;
}

class BoardRenderer {
public:
    BoardRenderer(BattleshipBoard &board, int x, int y);
    int x;
    int y;

    BattleshipBoard *_battleShipBoard;
    void drawBoard(RenderWindow &window, bool shipsHidden);
    bool contains(int xPoint, int yPoint);

    pair<int, int> getGridPos(double xPos, double yPos);

    // Fires a shot on the board
    bool handleClick(RenderWindow &window, double mouseX, double mouseY); // Returns true if valid guess was made

    void updateMousePos(RenderWindow &window, double mouseX, double mouseY); // Draws a rectangle to show the current selection
};

BoardRenderer::BoardRenderer(BattleshipBoard &board, int x, int y){
    this->_battleShipBoard = &board;

    this->x = x;
    this->y = y;
}

void BoardRenderer::drawBoard(RenderWindow &window, bool shipsHidden) {
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            int value = _battleShipBoard->_grid.at(i).at(j);

            if(shipsHidden && value == BattleshipBoard::_SHIP) {
                value = BattleshipBoard::_BLANK;
            }

            Color color;

            if (value == BattleshipBoard::_BLANK || value == BattleshipBoard::_MISS) {
                color = Color::Blue;
            } else {
                color = Color(120, 120, 120);
            }

            RectangleShape square(Vector2f(50, 50));
            square.setPosition(x + 50 * i, y + 50 * j);
            square.setFillColor(color);
            square.setOutlineColor(Color::Black);
            square.setOutlineThickness(2);
            window.draw(square);

            if (value == BattleshipBoard::_MISS || value == BattleshipBoard::_HIT) {
                CircleShape marker(20, 20);
                marker.setPosition(x + 5 + 50 * i, y + 5 + 50 * j);
                marker.setFillColor(value == BattleshipBoard::_MISS ? Color::White : Color::Red);
                window.draw(marker);
            }
        }
    }
}

bool BoardRenderer::contains(int xPoint, int yPoint) {
    int width = 500;
    int height = 500;

    return xPoint >= this->x && xPoint <= this->x + width && yPoint >= this->y && yPoint <= this->y + height;
}

pair<int, int> BoardRenderer::getGridPos(double xPos, double yPos) {
    int rowPos = (xPos - this->x) / 50;
    int columnPos = (yPos - this->y) / 50;

    return make_pair(rowPos, columnPos);
}

bool BoardRenderer::handleClick(RenderWindow &window, double mouseX, double mouseY) {
    pair<int, int> gridPos = getGridPos(mouseX, mouseY);

    int rowPos = gridPos.first;
    int columnPos = gridPos.second;

    if(rowPos >=0 && rowPos < 10 && columnPos >=0 && columnPos < 10) {
        int status = _battleShipBoard->fireShot(rowPos, columnPos);

        if(status != -1) {
            return true;
        }
    }

    return false;
}

void BoardRenderer::updateMousePos(RenderWindow &window, double mouseX, double mouseY) {
    pair<int, int> gridPos = getGridPos(mouseX, mouseY);

    int rowPos = gridPos.first;
    int columnPos = gridPos.second;

    if(rowPos >=0 && rowPos < 10 && columnPos >=0 && columnPos < 10) {
        RectangleShape selectionSquare(Vector2f(50, 50));
        selectionSquare.setPosition(rowPos * 50 + x, columnPos * 50 + y);
        selectionSquare.setFillColor(Color(255, 255, 255, 100));

        window.draw(selectionSquare);
    }
}



template<typename T>
class ShipCollection {
public:
    ShipCollection(bool player);
    virtual void drawShips(RenderWindow &window);
protected:
    vector<T> _ships;
};

template<typename T>
ShipCollection<T>::ShipCollection(bool player) {
    int defaultXPos = player ? 550 : 1350;
    vector<int> lengths = {5, 4, 4, 3, 2};

    for(int i = 0; i < lengths.size(); i++) {
        T nextShip(defaultXPos, 75 + 100 * i, lengths.at(i), true);

        _ships.push_back(nextShip);
    }
}

template<typename T>
void ShipCollection<T>::drawShips(RenderWindow &window) {
    for(int i = 0; i < _ships.size(); i++) {
        _ships.at(i).draw(window);
    }
}

class StatusShipCollection : public ShipCollection<StatusShip> {
public:
    StatusShipCollection(bool player, BattleshipBoard &battleshipBoard);
    using ShipCollection<StatusShip>::ShipCollection;
    void drawShips(RenderWindow &window) override;

    BattleshipBoard *_battleshipBoard;
};

StatusShipCollection::StatusShipCollection(bool player, BattleshipBoard &battleshipBoard)
        : ShipCollection<StatusShip>(player) {
    _battleshipBoard = &battleshipBoard;
}

void StatusShipCollection::drawShips(RenderWindow &window) {
    for(int i = 0; i < _battleshipBoard->getNumShips(); i++) {
        if(!_ships.at(i).isSunk() && _battleshipBoard->shipSunk(i)) {
            _ships.at(i).sink();
        }
    }

    ShipCollection<StatusShip>::drawShips(window);
}

class PlaceableShipCollection : public ShipCollection<PlaceableShip> {
public:
    PlaceableShipCollection(bool player, BoardRenderer &boardRenderer, BattleshipBoard &battleshipBoard);
    //void handleClick(RenderWindow &window, double mouseX, double mouseY);
    //void selectShip(RenderWindow &window, double mouseX, double mouseY);
    //void lockShip(RenderWindow &window, double mouseX, double mouseY);

    //void updateMousePos(double mouseX, double mouseY);
    //void rotateActiveShip();

    bool allPlaced();

    PlaceableShip &getShip(int index);
    int getShipNum();

    void removeShip(int index);

private:
    BoardRenderer *_boardRenderer;
    BattleshipBoard *_battleshipBoard;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;
};

// This strange syntax should, in theory, run the parent class constructor with player, then our custom ones
PlaceableShipCollection::PlaceableShipCollection(bool player, BoardRenderer &boardRenderer,
        BattleshipBoard &battleshipBoard) : ShipCollection<PlaceableShip>(player) {
    this->_boardRenderer = &boardRenderer;
    this->_battleshipBoard = &battleshipBoard;
    this->_activeIndex = -1;
    this->_activeOffsetX = 0.0;
    this->_activeOffsetY = 0.0;
}
//
//void PlaceableShipCollection::handleClick(RenderWindow &window, double mouseX, double mouseY) {
//    if(_activeIndex == -1) {
//        selectShip(window, mouseX, mouseY);
//    } else {
//        lockShip(window, mouseX, mouseY);
//    }
//}
//
//void PlaceableShipCollection::selectShip(RenderWindow &window, double mouseX, double mouseY) {
//    for(int i = 0; i < _ships.size(); i++) {
//        if(_ships.at(i).contains(mouseX, mouseY)) {
//            _activeIndex = i;
//
//            _activeOffsetX = -mouseX + _ships.at(i).getX();
//            _activeOffsetY = -mouseY + _ships.at(i).getY();
//
//            window.setMouseCursorVisible(false);
//        }
//    }
//}
//
//void PlaceableShipCollection::lockShip(RenderWindow &window, double mouseX, double mouseY) {
//    pair<int, int> gridPos = _boardRenderer->getGridPos(mouseX + _activeOffsetX, mouseY + _activeOffsetY);
//
//    int rowPos = gridPos.first;
//    int columnPos = gridPos.second;
//
//    if(_battleshipBoard->shipFits(_ships.at(_activeIndex), rowPos, columnPos)) {
//        _battleshipBoard->placeShip(_ships.at(_activeIndex), rowPos, columnPos);
//        _ships.erase(_ships.begin() + _activeIndex);
//        _activeIndex = -1;
//
//        window.setMouseCursorVisible(true);
//    }
//}
//
//void PlaceableShipCollection::updateMousePos(double mouseX, double mouseY) {
//    if(_activeIndex != -1) {
//        double xPos = mouseX + _activeOffsetX ;
//        double yPos = mouseY + _activeOffsetY;
//
//        pair<int, int> gridPos = _boardRenderer->getGridPos(xPos, yPos);
//
//        int rowPos = gridPos.first;
//        int columnPos = gridPos.second;
//
//        if(_battleshipBoard->shipFits(_ships.at(_activeIndex), rowPos, columnPos)) {
//            xPos = rowPos * 50 + _boardRenderer->x;
//            yPos = columnPos * 50 + _boardRenderer->y;
//        }
//
//        this->_ships.at(_activeIndex).updatePos(xPos, yPos);
//    }
//}
//
//void PlaceableShipCollection::rotateActiveShip() {
//    this->_ships.at(_activeIndex).rotate();
//}

bool PlaceableShipCollection::allPlaced() {
    return _ships.empty();
}

PlaceableShip &PlaceableShipCollection::getShip(int index) {
    return _ships.at(index);
}

int PlaceableShipCollection::getShipNum() {
    return _ships.size();
}

void PlaceableShipCollection::removeShip(int index) {
    _ships.erase(_ships.begin() + index);
}

class GameRenderer {
public:
    GameRenderer(bool human, BattleshipBoard &board);
    BoardRenderer _boardRenderer;
    PlaceableShipCollection _placeableShipCollection;
    StatusShipCollection _statusShipCollection;
    bool _human;
    int _spaceCount;

    void drawGame(RenderWindow &window); // Draws the board and the status ships
    void drawSetup(RenderWindow &window); // Draws the board and the placeable ships

    bool setup(RenderWindow &window); // Returns true is set up was finished
};

//GameRenderer::GameRenderer(bool human, BattleshipBoard &board): _boardRenderer(board, human ? 25 : 825, 50),
//        _placeableShipCollection(human, _boardRenderer, board), _statusShipCollection(human, board){
//
//    _human = human;
//    _spaceCount = 0;
//}
//
//void GameRenderer::drawGame(RenderWindow &window) {
//    _boardRenderer.drawBoard(window, !_human);
//    _statusShipCollection.drawShips(window);
//}
//
//void GameRenderer::drawSetup(RenderWindow &window) {
//    _boardRenderer.drawBoard(window, !_human);
//    _placeableShipCollection.drawShips(window);
//};
//
//bool GameRenderer::setup(RenderWindow &window) {
//    Vector2i mousePos = sf::Mouse::getPosition(window);
//    int mouseX = mousePos.x;
//    int mouseY = mousePos.y;
//
//    if(Mouse::isButtonPressed(sf::Mouse::Left)) {
//        _placeableShipCollection.handleClick(window, mouseX, mouseY);
//    }
//
//    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
//        _spaceCount += 1;
//    } else {
//        _spaceCount = 0;
//    }
//
//    if(_spaceCount == 1) {
//        _placeableShipCollection.rotateActiveShip();
//    }
//
//    _placeableShipCollection.updateMousePos(mouseX, mouseY);
//
//    return _placeableShipCollection.allPlaced();
//}

// Holds the board, collections of ships, and renderer for a player
class Player {
public:
    Player(bool human);

    virtual void placeShips();
    virtual void getMove();

protected:
    BattleshipBoard _board;

    BoardRenderer _boardRenderer;
    PlaceableShipCollection _placeableShipCollection;
    StatusShipCollection _statusShipCollection;

    static const int HUMAN_BOARD_X_POS = 25;
    static const int COMPUTER_BOARD_X_POS = 825;
};

Player::Player(bool human) : _boardRenderer(_board, human ? HUMAN_BOARD_X_POS : COMPUTER_BOARD_X_POS, 50),
        _placeableShipCollection(human, _boardRenderer, _board), _statusShipCollection(human, _board) {}

void Player::placeShips() {
    cout << "Derived classes must define the placeShips method" << endl;
}

void Player::getMove() {
    cout << "Derived classes must define the getMove method" << endl;
}


class HumanPlayer : public Player {
public:
    HumanPlayer();

    bool placeShips(RenderWindow &window);
    void getMove() override;

    void updateSpacePress();
    bool spacePressed();

    void handleClick(RenderWindow &window, Vector2i mousePos);
    void selectShip(RenderWindow &window, Vector2i mousePos);
    void lockShip(RenderWindow &window, Vector2i mousePos);
    void updateShipPos(Vector2i mousePos);
    void rotateShip();

    int _spaceCount;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;
};

HumanPlayer::HumanPlayer() : Player(true) {
    _activeIndex = -1;
    _spaceCount = 0;
}

void HumanPlayer::getMove() {
    // Add code for detecting and drawing on the board here
}

void HumanPlayer::updateSpacePress() {
    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        _spaceCount += 1;
    } else {
        _spaceCount = 0;
    }
}

bool HumanPlayer::spacePressed() {
    return _spaceCount == 1;
}

bool HumanPlayer::placeShips(RenderWindow &window) {
    _boardRenderer.drawBoard(window, false);
    _placeableShipCollection.drawShips(window);

    Vector2i mousePos = sf::Mouse::getPosition(window);
    updateSpacePress();

    updateShipPos(mousePos);

    if(Mouse::isButtonPressed(sf::Mouse::Left)) {
        handleClick(window, mousePos);
    }

    if(spacePressed()) {
        rotateShip();
    }

    return _placeableShipCollection.allPlaced();
}

void HumanPlayer::handleClick(RenderWindow &window, Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(window, mousePos);
    } else {
        lockShip(window, mousePos);
    }
}

void HumanPlayer::selectShip(RenderWindow &window, Vector2i mousePos) {
    for(int i = 0; i < _placeableShipCollection.getShipNum(); i++) {
        PlaceableShip currentShip = _placeableShipCollection.getShip(i);

        if(currentShip.contains(mousePos)) {
            _activeIndex = i;

            _activeOffsetX = -mousePos.x + currentShip.getX();
            _activeOffsetY = -mousePos.y + currentShip.getY();

            window.setMouseCursorVisible(false);
        }
    }
}

void HumanPlayer::lockShip(RenderWindow &window, Vector2i mousePos) {
    PlaceableShip currentShip = _placeableShipCollection.getShip(_activeIndex);

    pair<int, int> gridPos = _boardRenderer.getGridPos(mousePos.x + _activeOffsetX, mousePos.y + _activeOffsetY);

    int rowPos = gridPos.first;
    int columnPos = gridPos.second;

    if(_board.shipFits(currentShip, rowPos, columnPos)) {
        _board.placeShip(currentShip, rowPos, columnPos);
        _placeableShipCollection.removeShip(_activeIndex);

        _activeIndex = -1;

        window.setMouseCursorVisible(true);
    }
}

void HumanPlayer::updateShipPos(Vector2i mousePos) {
    if(_activeIndex != -1) {
        cout << "Running this code" << endl;
        double xPos = mousePos.x + _activeOffsetX ;
        double yPos = mousePos.y + _activeOffsetY;

        pair<int, int> gridPos = _boardRenderer.getGridPos(xPos, yPos);

        int rowPos = gridPos.first;
        int columnPos = gridPos.second;

        if(_board.shipFits(_placeableShipCollection.getShip(_activeIndex), rowPos, columnPos)) {
            xPos = rowPos * 50 + _boardRenderer.x;
            yPos = columnPos * 50 + _boardRenderer.y;
        }

        cout << xPos << ", " << yPos << endl;

        _placeableShipCollection.getShip(_activeIndex).updatePos(xPos, yPos);
    }
}

void HumanPlayer::rotateShip() {
    _placeableShipCollection.getShip(_activeIndex).rotate();
}

class ComputerPlayer : public Player {
public:
    ComputerPlayer();

    void placeShips();
    void getMove();
};

ComputerPlayer::ComputerPlayer() : Player(false) {}

void ComputerPlayer::placeShips() {
    for(int i = 0; i < _placeableShipCollection.getShipNum(); i++) {
        PlaceableShip currentShip = _placeableShipCollection.getShip(i);

        while(true) {
            int randomRow = rand() % 10;
            int randomCol = rand() % 10;

            if(rand() % 2) {
                currentShip.rotate();
            }

            if(_board.shipFits(currentShip, randomRow, randomCol)) {
                _board.placeShip(currentShip, randomRow, randomCol);

                break;
            }
        }
    }
}

void ComputerPlayer::getMove() {

}


int main() {
    // create a RenderWindow object
    // specify the size to be 640x640
    // set the title to be "SFML Example Window"
    RenderWindow window( VideoMode(1625, 700), "SFML Example Window" );

    HumanPlayer humanPlayer;
    ComputerPlayer computerPlayer;

    //BattleshipBoard myBoard;
    BattleshipBoard opponentBoard;

    //GameRenderer myRenderer(true, myBoard);
    //GameRenderer opponentRenderer(false, opponentBoard);

    enum STATE {SETUP, COMPUTER_SETUP, PLAYER_TURN, COMPUTER_TURN};

    STATE state = SETUP;

    while( window.isOpen() ) {
        window.clear( Color::Black );

        window.setFramerateLimit(0);

        if(state == SETUP) {
            bool allShipsPlaced = humanPlayer.placeShips(window);

            if(allShipsPlaced) {
                state = COMPUTER_SETUP;
            }
        } else if(state == COMPUTER_SETUP) {
            computerPlayer.placeShips();

            state = PLAYER_TURN;
        } //else if (state == PLAYER_TURN){
//            myBoardRenderer.drawBoard(window, false);
//            opponentBoardRenderer.drawBoard(window, true);
//
//            myStatusShips.drawShips(window);
//            opponentStatusShips.drawShips(window);
//
//            Vector2i mousePos = sf::Mouse::getPosition(window);
//            int mouseX = mousePos.x;
//            int mouseY = mousePos.y;
//
//            if(Mouse::isButtonPressed(sf::Mouse::Left)) {
//                if(opponentBoardRenderer.handleClick(window, mouseX, mouseY)) {
//                    state = COMPUTER_TURN;
//                };
//            }
//
//            opponentBoardRenderer.updateMousePos(window, mouseX, mouseY);
//        } else if(state == COMPUTER_TURN) {
//            while(true) {
//                int randomRow = rand() % 10;
//                int randomCol = rand() % 10;
//
//                if(myBoard.fireShot(randomRow, randomCol) >= 0) {
//                    state = PLAYER_TURN;
//                    break;
//                }
//            }
//        }

        window.display();

        Event event;
        while( window.pollEvent(event) ) {      // ask the window if any events occurred
            if( event.type == Event::Closed ) { // if event type is a closed event
                // i.e. they clicked the X on the window
                window.close();                 // then close our window
            }
        }
    }

    return EXIT_SUCCESS;                        // report our program exited successfully
}