//
// Created by Colin Siles on 2019-11-17.
//

#include <iostream>                             // for standard input/output
using namespace std;                            // using the standard namespace

#include <SFML/Graphics.hpp>                    // include the SFML Graphics Library
using namespace sf;                             // using the sf namespace

#include <fstream>
#include <iomanip>
#include <string>
#include <utility>

enum Orientation {HORIZONTAL, VERTICAL};

template <typename BoardType>
class BoardRenderer;

class Ship {
public:
    Ship(int length);

    void rotate();
    void setGridPos(int xPos, int yPos);
    void markAsHit(int xPos, int yPos);
    bool isPlaced();
    bool isSunk();
    bool contains(int xPos, int yPos);
    int getLength();

    friend class Board;
    friend class PrimaryBoard;
    friend class TrackingBoard;

    friend class ShipRenderer;

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
    _placed = false;
    _boardX = -1;
    _boardY = -1;
    _orientation = HORIZONTAL;

    _length = length;
}

void Ship::rotate() {
    _orientation = _orientation == HORIZONTAL ? VERTICAL : HORIZONTAL;
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

class Fleet {
public:
    Fleet();
    Fleet(vector<int> lengths);

    bool markShipHit(int xPos, int yPos); // Return if the ship was hit or not

    int size(); // Returns the size of the fleet
    Ship &ship(int index);
    Ship lastSunken(); // What to return if no ship has been sunk yet?

    bool allPlaced();
    bool allSunk();

private:
    vector<Ship> _ships;
    int _lastSunkenIndex;
};

Fleet::Fleet() {
    _lastSunkenIndex = -1;
}

Fleet::Fleet(vector<int> lengths) : Fleet() {
    for(int i = 0; i < lengths.size(); i++) {
        Ship newShip(lengths.at(i));

        _ships.push_back(newShip);
    }
}

bool Fleet::markShipHit(int xPos, int yPos) {
    for(int i = 0; i < _ships.size(); i++) {
        if(_ships.at(i).contains(xPos, yPos)) {
            _ships.at(i).markAsHit(xPos, yPos);

            bool sunken = _ships.at(i).isSunk();

            if(sunken) {
                _lastSunkenIndex = i;
            }

            return sunken;
        }
    }
}

int Fleet::size() {
    return _ships.size();
};

Ship &Fleet::ship(int index) {
    return _ships.at(index);
}

Ship Fleet::lastSunken() {
    return _ships.at(_lastSunkenIndex);
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


class Board {
public:
    Board();

    static const int GRID_SIZE = 10; // In theory, this could be changed to be a parameter, although renderer class probably couldnt handle that easily

    enum SquareState {BLANK, SHIP, HIT_MARKER, MISS_MARKER};
    enum ShotOutcome {HIT, MISS, SUNK};

    friend class BoardRenderer<Board>;

protected:
    vector<vector<SquareState>> _grid; // We intialize this in the constructor

    bool posInGrid(int xPos, int yPos); // Checks that a shot is within the bounds of the board
};

Board::Board() {
    for(int i = 0; i < GRID_SIZE; i++) {
        vector<SquareState> newRow(GRID_SIZE, BLANK);
        _grid.push_back(newRow);
    }
}

bool Board::posInGrid(int xPos, int yPos){
    return xPos >=0 && xPos < GRID_SIZE && yPos >= 0 && yPos < GRID_SIZE;
}



class PrimaryBoard : Board {
public:
    PrimaryBoard(Fleet &fleet);
    using Board::Board; // Inheret the constructor of the Board class

    Board::ShotOutcome fireShotAt(int xPos, int yPos); // Returns true is shot is a hit, false if a miss (must check validity first)
    bool shipFits(Ship ship);
    void placeShip(Ship *ship); // Pas by reference to store reference to the ship so the board can sink it when appropropriate
    Board::ShotOutcome getOutcome(bool sunk, SquareState finalState);

    Fleet *_fleet;

    friend class BoardRenderer<PrimaryBoard>;

};

PrimaryBoard::PrimaryBoard(Fleet &fleet) : Board() {
    _fleet = &fleet;
}

Board::ShotOutcome PrimaryBoard::getOutcome(bool sunk, SquareState finalState) {
    if(sunk) {
        return SUNK;
    } else if(finalState == HIT_MARKER) {
        return HIT;
    }

    return MISS;
}


Board::ShotOutcome PrimaryBoard::fireShotAt(int xPos, int yPos) {
    _grid.at(xPos).at(yPos) = _grid.at(xPos).at(yPos) == SHIP ? HIT_MARKER : MISS_MARKER;

    bool sunk = _fleet->markShipHit(xPos, yPos);

    return getOutcome(sunk, _grid.at(xPos).at(yPos));
}

bool PrimaryBoard::shipFits(Ship ship) {
    if(ship._boardX < 0 || ship._boardY < 0 ){
        return false;
    }

    for(auto &square : ship._squares) {
        pair<int, int> squarePos = square.first;

        if(!posInGrid(squarePos.first, squarePos.second) || _grid.at(squarePos.first).at(squarePos.second) != BLANK) {
            return false;
        }
    }

    return true;
}

void PrimaryBoard::placeShip(Ship *ship) {
    for(auto &square : ship->_squares) {
        pair<int, int> squarePos = square.first;

        _grid.at(squarePos.first).at(squarePos.second) = SHIP;
    }

    ship->_placed = true;
}



class TrackingBoard : Board {
public:
    using Board::Board;
    void markShot(int xPos, int yPos, Board::ShotOutcome outcome);
    bool validShot(int xPos, int yPos); // Returns true if guess is valid (in grid and not already guessed) called by tracking grids, primarily

    friend class BoardRenderer<TrackingBoard>;
};

void TrackingBoard::markShot(int xPos, int yPos, Board::ShotOutcome outcome) {
    if(outcome == HIT || outcome == SUNK) {
        _grid.at(xPos).at(yPos) = HIT_MARKER;
    } else {
        _grid.at(xPos).at(yPos) = MISS_MARKER;
    }
}

bool TrackingBoard::validShot(int xPos, int yPos) {
    return posInGrid(xPos, yPos) && (_grid.at(xPos).at(yPos) == BLANK || _grid.at(xPos).at(yPos) == BLANK);
}


class Player {
public:
    Player(string name, vector<int> shipLengths);

    void bindTrackingFleets(Player *opponent);
    virtual void setTrackingFleet(Fleet *opponentFleet);

    PrimaryBoard primaryBoard;
    TrackingBoard trackingBoard;
    Fleet primaryFleet;
    Fleet *trackingFleet;
    string name;

    virtual pair<int, int> getMove() = 0;
    virtual void placeShips() = 0;

    Board::ShotOutcome fireShotAt(int xPos, int yPos);
    void markShot(int xPos, int yPos, Board::ShotOutcome outcome);

    bool allShipsPlaced();
    bool allShipsSunk();
};

Player::Player(string name, vector<int> shipLengths) : primaryFleet(shipLengths), primaryBoard(primaryFleet) {
    this->name = name;
}

void Player::bindTrackingFleets(Player *opponent) {
    setTrackingFleet(&opponent->primaryFleet);
    opponent->setTrackingFleet(&primaryFleet);
}

void Player::setTrackingFleet(Fleet *opponentFleet) {
    trackingFleet = opponentFleet;
}

Board::ShotOutcome Player::fireShotAt(int xPos, int yPos) {
    return primaryBoard.fireShotAt(xPos, yPos);
}

void Player::markShot(int xPos, int yPos, Board::ShotOutcome outcome) {
    trackingBoard.markShot(xPos, yPos, outcome);
}

bool Player::allShipsPlaced() {
    return primaryFleet.allPlaced();
}

bool Player::allShipsSunk() {
    return primaryFleet.allSunk();
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

        if(trackingBoard.validShot(randomRow, randomCol)) {
            return make_pair(randomRow, randomCol);
        }
    }
}

void ComputerRandomPlayer::placeShips() {
    for(int i = 0; i < primaryFleet.size(); i++) {
        while(true) {
            int randomX = rand() % 10;
            int randomY = rand() % 10;

            primaryFleet.ship(i).setGridPos(randomX, randomY);

            if(rand() % 2) {
                primaryFleet.ship(i).rotate();
            }

            if(primaryBoard.shipFits(primaryFleet.ship(i))) {
                primaryBoard.placeShip(&primaryFleet.ship(i));

                break;
            }
        }
    }
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

    friend class PrimaryBoardRenderer;

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

template <typename BoardType>
class FleetRenderer {
public:
    FleetRenderer();
    FleetRenderer(Fleet &fleet, BoardRenderer<BoardType> *boardRenderer, RenderWindow &window);
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
    BoardRenderer<BoardType> *_boardRenderer;
    vector<ShipRenderer> _shipRenderers;
    RenderWindow *_window;
    int _spaceCount;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;

    // TODO: maybe also store the active ship to clean up the code a bit?
};

template <typename BoardType>
FleetRenderer<BoardType>::FleetRenderer() {}

template <typename BoardType>
FleetRenderer<BoardType>::FleetRenderer(Fleet &fleet, BoardRenderer<BoardType> *boardRenderer, RenderWindow &window) {
    _boardRenderer = boardRenderer;
    _activeIndex = -1;
    _spaceCount = 0;

    _window = &window;

    for(int i = 0; i < fleet.size(); i++) {
        ShipRenderer newShipRenderer(window, &fleet.ship(i), _boardRenderer->getDispX() + 525, _boardRenderer->getDispY() + 25 +i * 100);

        _shipRenderers.push_back(newShipRenderer);
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::draw(bool placingShips) {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).draw(placingShips);
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::resetLocations() {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        _shipRenderers.at(i).setXY(_boardRenderer->getDispX()+ 525, _boardRenderer->getDispX() + 25 + i * 100);
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::updateSpacePress() {
    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        _spaceCount += 1;
    } else {
        _spaceCount = 0;
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::updateShipPos(Vector2i mousePos) {
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

template <typename BoardType>
void FleetRenderer<BoardType>::selectShip(Vector2i mousePos) {
    for(int i = 0; i < _shipRenderers.size(); i++) {
        if(_shipRenderers.at(i).contains(mousePos) && !_shipRenderers.at(i).getShip().isPlaced()) {
            _activeIndex = i;

            _activeOffsetX = -mousePos.x + _shipRenderers.at(i).getDispX();
            _activeOffsetY = -mousePos.y + _shipRenderers.at(i).getDispY();

            _window->setMouseCursorVisible(false);
        }
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::lockShip(Vector2i mousePos) {
    if(_boardRenderer->getBoard().shipFits((_shipRenderers.at(_activeIndex).getShip()))) { // TODO: this code sucks. Make it good
        _boardRenderer->getBoard().placeShip(&_shipRenderers.at(_activeIndex).getShip());

        _activeIndex = -1;

        _window->setMouseCursorVisible(true);
    }
}

template <typename BoardType>
void FleetRenderer<BoardType>::handleClick(Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(mousePos);
    } else {
        lockShip(mousePos);
    }
}

template <typename BoardType>
bool FleetRenderer<BoardType>::spacePressed() {
    return _spaceCount == 1;
}

template <typename BoardType>
void FleetRenderer<BoardType>::rotateShip() {
    _shipRenderers.at(_activeIndex).getShip().rotate();
}

// Responsible for rendering the grid and the fleet (depending on the context of if your being placed or playing)
template <typename BoardType>
class BoardRenderer {
public:
    BoardRenderer();
    BoardRenderer(RenderWindow &window, BoardType &board, Fleet &fleet, double dispX, double dispY);

    void draw();
    void drawStatusSquare(double mouseX, double mouseY);
    pair<int, int> getGridPos(double xPos, double yPos);

    double getDispX();
    double getDispY();

    BoardType &getBoard();

protected:
    RenderWindow *_window;
    BoardType *_board;

    double _dispX;
    double _dispY;
};

template <typename BoardType>
BoardRenderer<BoardType>::BoardRenderer() {} // Don't initialize any fields, and that's fine

template <typename BoardType>
BoardRenderer<BoardType>::BoardRenderer(RenderWindow &window, BoardType &board, Fleet &fleet, double dispX, double dispY) {
    _window = &window;
    _board = &board;
    _dispX = dispX;
    _dispY = dispY;
}

template <typename BoardType>
void BoardRenderer<BoardType>::draw() {
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

template <typename BoardType>
void BoardRenderer<BoardType>::drawStatusSquare(double mouseX, double mouseY) {
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

template <typename BoardType>
pair<int, int> BoardRenderer<BoardType>::getGridPos(double xPos, double yPos) {
    int rowPos = (xPos - _dispX) / 50;
    int columnPos = (yPos - _dispY) / 50;

    return make_pair(rowPos, columnPos);
}

template<typename BoardType>
BoardType &BoardRenderer<BoardType>::getBoard() {
    return *_board;
}

template<typename BoardType>
double BoardRenderer<BoardType>::getDispX() {
    return _dispX;
}

template<typename BoardType>
double BoardRenderer<BoardType>::getDispY() {
    return _dispY;
}


class PrimaryBoardRenderer : public BoardRenderer<PrimaryBoard> {
public:
    PrimaryBoardRenderer(RenderWindow &window, PrimaryBoard &board, Fleet &fleet, double dispX, double dispY);
    using BoardRenderer<PrimaryBoard>::BoardRenderer;

private:

};

PrimaryBoardRenderer::PrimaryBoardRenderer(RenderWindow &window, PrimaryBoard &board, Fleet &fleet, double dispX, double dispY):
        BoardRenderer<PrimaryBoard>(window, board, fleet, dispX, dispY) {}


class HumanSFMLPlayer : public Player {
public:
    HumanSFMLPlayer(string name, vector<int> shipLengths);

    RenderWindow window;
    PrimaryBoardRenderer pBoardRenderer;
    BoardRenderer<TrackingBoard> tBoardRenderer;
    FleetRenderer<PrimaryBoard> pFleetRenderer;
    FleetRenderer<TrackingBoard> tFleetRenderer;

    void setTrackingFleet(Fleet *otherFleet) override;

    pair<int, int> getMove() override;
    void placeShips() override;
};

HumanSFMLPlayer::HumanSFMLPlayer(string name, vector<int> shipLengths) : Player(name, shipLengths),
        window(VideoMode(1625, 700), "SFML Example Window"),
        pBoardRenderer(window, primaryBoard, primaryFleet, 25, 50),
        pFleetRenderer(primaryFleet, &pBoardRenderer, window){}

void HumanSFMLPlayer::setTrackingFleet(Fleet *otherFleet) {
    Player::setTrackingFleet(otherFleet);

    BoardRenderer<TrackingBoard> newTBoardRenderer(window, trackingBoard, *trackingFleet, 825, 50);
    tBoardRenderer = newTBoardRenderer;

    FleetRenderer<TrackingBoard> newTFleetRenderer(*trackingFleet, &tBoardRenderer, window);
    tFleetRenderer = newTFleetRenderer;
}

void HumanSFMLPlayer::placeShips() {
    while( window.isOpen() ) {
        window.clear( Color::Black );

        pBoardRenderer.draw();
        tBoardRenderer.draw();

        pFleetRenderer.draw(true);
        tFleetRenderer.draw(true);

        Vector2i mousePos = sf::Mouse::getPosition(window);
        pFleetRenderer.updateSpacePress();

        pFleetRenderer.updateShipPos(mousePos);

        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            pFleetRenderer.handleClick(mousePos);
        }

        if(pFleetRenderer.spacePressed()) {
            pFleetRenderer.rotateShip();
        }

        if(allShipsPlaced()) {
            pFleetRenderer.resetLocations(); // Reset the ships that were placed before to their original coordinates
            return;
        }

        window.display();

        Event event;
        while( window.pollEvent(event) ) {
            if( event.type == Event::Closed ) {
                window.close();
            }
        }

    }
}

pair<int, int> HumanSFMLPlayer::getMove() {
    while( window.isOpen() ) {
        window.clear( Color::Black );

        pBoardRenderer.draw();
        tBoardRenderer.draw();

        pFleetRenderer.draw(false);
        tFleetRenderer.draw(false);

        Vector2i mousePos = sf::Mouse::getPosition(window);

        tBoardRenderer.drawStatusSquare(mousePos.x, mousePos.y);

        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            pair<int, int> move = tBoardRenderer.getGridPos(mousePos.x, mousePos.y);

            if(trackingBoard.validShot(move.first, move.second)) {
                return move;
            }
        }

        window.display();

        Event event;
        while( window.pollEvent(event) ) {
            if( event.type == Event::Closed ) {
                window.close();
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
    string getOutcomeString(Board::ShotOutcome outcome);

    void writeSeparator();
    void writeName(string name);
    void writeMove(int xPos, int yPos, Board::ShotOutcome outcome);

    void recordMove(string name, int xPos, int yPos, Board::ShotOutcome outcome);
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

string Battlelog::getOutcomeString(Board::ShotOutcome outcome) {
    if(outcome == Board::SUNK) {
        return "SUNK";
    } else if(outcome == Board::HIT) {
        return "HIT";
    } else {
        return "MISS";
    }
}

void Battlelog::writeSeparator() {
    battlelogFile << "|" << setw(25) << setfill('=') << "" << "|" << endl;
}

void Battlelog::writeName(string name) {
    battlelogFile << "| " << setw(10) << setfill(' ') << left << name << " ";
}

void Battlelog::writeMove(int xPos, int yPos, Board::ShotOutcome outcome) {
    battlelogFile << " " << setw(5) << setfill(' ') << left << (convertYPos(yPos) + to_string(convertXPos(xPos)));
    battlelogFile << " " << setw(4) << setfill(' ') << right << getOutcomeString(outcome) << " |";
}

void Battlelog::recordMove(string name, int xPos, int yPos, Board::ShotOutcome outcome) {
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
    playerOne.bindTrackingFleets(&playerTwo);
}

template<typename p1Type, typename p2Type>
void Game<p1Type, p2Type>::runGame() {
    playerOne.placeShips();

    if(!playerOne.allShipsPlaced()) {
        cerr << playerOne.name <<  " failed to place all their ships" << endl;
        return;
    }

    playerTwo.placeShips();

    if(!playerTwo.allShipsPlaced()) {
        cerr << playerTwo.name << " failed to place all their ships" << endl;
        return;
    }

    while(true) {
        if(turn == 1) {
            pair<int, int> move = playerOne.getMove();

            if(move.first < 0) {
                cerr << playerOne.name << " has forfeited the match" << endl;
                return;
            }

            Board::ShotOutcome outcome = playerTwo.fireShotAt(move.first, move.second);
            playerOne.markShot(move.first, move.second, outcome);
            battlelog.recordMove(playerOne.name, move.first, move.second, outcome);

            if(playerTwo.allShipsSunk()) {
                battlelog.recordWinner(playerOne.name);
                cerr << playerOne.name << " wins" << endl;
                return;
            }

            turn = 2;
        } else if(turn == 2) {
            pair<int, int> move = playerTwo.getMove();

            if(move.first < 0) {
                cerr << playerTwo.name << " has forfeited the match" << endl;
                return;
            }

            Board::ShotOutcome outcome = playerOne.fireShotAt(move.first, move.second);
            playerTwo.markShot(move.first, move.second, outcome);
            battlelog.recordMove(playerTwo.name, move.first, move.second, outcome);

            if(playerOne.allShipsSunk()) {
                battlelog.recordWinner(playerTwo.name);
                cerr << playerTwo.name << " wins" << endl;
                return;
            }

            turn = 1;
        }
    }
}

int main() {
    vector<int> shipLengths = {1, 1, 1, 1, 1, 1};

    Game<HumanSFMLPlayer, ComputerRandomPlayer> game("Human", "Computer");

    game.runGame();
}