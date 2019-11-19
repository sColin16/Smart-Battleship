//
// Created by Colin Siles on 2019-11-17.
//

#include <iostream>                             // for standard input/output
using namespace std;                            // using the standard namespace

#include <SFML/Graphics.hpp>                    // include the SFML Graphics Library
using namespace sf;                             // using the sf namespace

#include <fstream>
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

    friend class Board;
    friend class PrimaryBoard;
    friend class TrackingBoard;

    friend class ShipRenderer;

private:
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

    // Update the ship's sunk status
    _sunk = isSunk();
}

bool Ship::isPlaced() {
    return _placed;
}

bool Ship::isSunk() {
    if(_squares.empty()) {
        return false;
    }

    for(auto &square : _squares) {
        if(!square.second) {
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
    vector<Ship *> _fleet;               // We leave this empty until ships are placed on the grid

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
    using Board::Board; // Inheret the constructor of the Board class

    Board::ShotOutcome fireShotAt(int xPos, int yPos); // Returns true is shot is a hit, false if a miss (must check validity first)
    bool shipFits(Ship ship);
    void placeShip(Ship *ship); // Pas by reference to store reference to the ship so the board can sink it when appropropriate

    friend class BoardRenderer<PrimaryBoard>;
};


Board::ShotOutcome PrimaryBoard::fireShotAt(int xPos, int yPos) {
    _grid.at(xPos).at(yPos) = _grid.at(xPos).at(yPos) == SHIP ? HIT_MARKER : MISS_MARKER;

    bool sunk = false;

    for(int i = 0; i < _fleet.size(); i++) {
        if(_fleet.at(i)->_squares.count(make_pair(xPos, yPos))) {
            _fleet.at(i)->markAsHit(xPos, yPos);

            if(_fleet.at(i)->_sunk) {
                sunk = true;
            }
        }
    }

    ShotOutcome outcome;

    if(sunk) {
        outcome = SUNK;
    } else if(_grid.at(xPos).at(yPos) == HIT_MARKER) {
        outcome = HIT;
    } else {
        outcome = MISS;
    };

    return outcome;
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
    _fleet.push_back(ship);
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
    Player(vector<int> shipLengths);

    void bindTrackingFleets(Player *opponent);
    virtual void setTrackingFleet(vector<Ship> *opponentFleet);

    PrimaryBoard primaryBoard;
    TrackingBoard trackingBoard;
    vector<Ship> primaryFleet;
    vector<Ship> *trackingFleet;

    virtual pair<int, int> getMove();
    virtual void placeShips();

    Board::ShotOutcome fireShotAt(int xPos, int yPos);
    void markShot(int xPos, int yPos, Board::ShotOutcome outcome);

    bool allShipsPlaced();
    bool allShipsSunk();
};

Player::Player(vector<int> shipLengths) {
    for(int i = 0; i < shipLengths.size(); i++) {
        Ship newShip(shipLengths.at(i));

        primaryFleet.push_back(newShip);
    }


}

void Player::bindTrackingFleets(Player *opponent) {
    opponent->setTrackingFleet(trackingFleet);
    setTrackingFleet(&opponent->primaryFleet);
}

void Player::setTrackingFleet(vector<Ship> *opponentFleet) {
    trackingFleet = opponentFleet;
}

pair<int, int> Player::getMove(){}
void Player::placeShips(){}

Board::ShotOutcome Player::fireShotAt(int xPos, int yPos) {
    return primaryBoard.fireShotAt(xPos, yPos);
}

void Player::markShot(int xPos, int yPos, Board::ShotOutcome outcome) {
    trackingBoard.markShot(xPos, yPos, outcome);
}

bool Player::allShipsPlaced() {
    for(int i = 0; i < primaryFleet.size(); i++) {
        if(!primaryFleet.at(i).isPlaced()) {
            return false;
        }
    }

    return true;
}

bool Player::allShipsSunk() {
    for(int i = 0; i < primaryFleet.size(); i++) {
        if(!primaryFleet.at(i).isSunk()) {
            return false;
        }
    }

    return true;
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

            primaryFleet.at(i).setGridPos(randomX, randomY);

            if(rand() % 2) {
                primaryFleet.at(i).rotate();
            }

            if(primaryBoard.shipFits(primaryFleet.at(i))) {
                primaryBoard.placeShip(&primaryFleet.at(i));

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

        for(int i = 0; i < _ship->_length; i++) {
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

bool ShipRenderer::contains(Vector2i mousePos) {
    int x = mousePos.x;
    int y = mousePos.y;

    int width =  50 * (_ship->_orientation == HORIZONTAL ? _ship->_length : 1);
    int height = 50 * (_ship->_orientation == VERTICAL ? 1 : _ship->_length);

    return x >= _dispX && x <= _dispX + width && y >= _dispY && y <= _dispY + height;
}

// Responsible for rendering the grid and the fleet (depending on the context of if your being placed or playing)
template <typename BoardType>
class BoardRenderer {
public:
    BoardRenderer();
    BoardRenderer(RenderWindow &window, BoardType &board, vector<Ship> &fleet, double dispX, double dispY);

    void draw(bool showPlaced);
    void drawStatusSquare(double mouseX, double mouseY);
    pair<int, int> getGridPos(double xPos, double yPos);
    void resetShipLocation();

protected:
    RenderWindow *_window;
    BoardType *_board;
    vector<Ship> *_fleet;
    vector<ShipRenderer> shipRenderers;
    double _dispX;
    double _dispY;
};

template <typename BoardType>
BoardRenderer<BoardType>::BoardRenderer() {} // Don't initialize any fields, and that's fine

template <typename BoardType>
BoardRenderer<BoardType>::BoardRenderer(RenderWindow &window, BoardType &board, vector<Ship> &fleet, double dispX, double dispY) {
    _window = &window;
    _board = &board;
    _fleet = &fleet;
    _dispX = dispX;
    _dispY = dispY;

    for(int i = 0; i < fleet.size(); i++) {
        ShipRenderer newShipRenderer(window, &fleet.at(i), dispX + 525, dispY + 25 +i * 100);

        shipRenderers.push_back(newShipRenderer);
    }
}

template <typename BoardType>
void BoardRenderer<BoardType>::draw(bool placingShips) {
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

    for(int i = 0; i < shipRenderers.size(); i++) {
        shipRenderers.at(i).draw(placingShips);
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

template <typename BoardType>
void BoardRenderer<BoardType>::resetShipLocation() {
    for(int i = 0; i < _board->_fleet.size(); i++) {
        shipRenderers.at(i).setXY(_dispX + 525, _dispY + 25 + i * 100);
    }
}

class PrimaryBoardRenderer : public BoardRenderer<PrimaryBoard> {
public:
    PrimaryBoardRenderer(RenderWindow &window, PrimaryBoard &board, vector<Ship> &fleet, double dispX, double dispY);
    using BoardRenderer<PrimaryBoard>::BoardRenderer;

    void updateSpacePress();
    void updateShipPos(Vector2i mousePos);
    void selectShip(Vector2i mousePos);
    void lockShip(Vector2i mousePos);
    void handleClick(Vector2i mousePos);
    bool spacePressed();
    void rotateShip();
private:
    int _spaceCount;
    int _activeIndex;
    double _activeOffsetX;
    double _activeOffsetY;
};

PrimaryBoardRenderer::PrimaryBoardRenderer(RenderWindow &window, PrimaryBoard &board, vector<Ship> &fleet, double dispX, double dispY):
        BoardRenderer<PrimaryBoard>(window, board, fleet, dispX, dispY) {
    _activeIndex = -1;
    _spaceCount = 0;
}

void PrimaryBoardRenderer::updateSpacePress() {
    if(Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        _spaceCount += 1;
    } else {
        _spaceCount = 0;
    }
}

void PrimaryBoardRenderer::updateShipPos(Vector2i mousePos) {
    if(_activeIndex != -1) {
        double xPos = mousePos.x + _activeOffsetX ;
        double yPos = mousePos.y + _activeOffsetY;

        pair<int, int> gridPos = getGridPos(xPos, yPos);

        int rowPos = gridPos.first;
        int columnPos = gridPos.second;

        shipRenderers.at(_activeIndex)._ship->setGridPos(rowPos, columnPos);

        if(_board->shipFits(*shipRenderers.at(_activeIndex)._ship)) {
            xPos = rowPos * 50 + _dispX;
            yPos = columnPos * 50 + _dispY;
        }

        shipRenderers.at(_activeIndex).setXY(xPos, yPos);
    }
}

void PrimaryBoardRenderer::selectShip(Vector2i mousePos) {
    for(int i = 0; i < shipRenderers.size(); i++) {
        if(shipRenderers.at(i).contains(mousePos) && !shipRenderers.at(i)._ship->isPlaced()) {
            _activeIndex = i;

            _activeOffsetX = -mousePos.x + shipRenderers.at(i)._dispX;
            _activeOffsetY = -mousePos.y + shipRenderers.at(i)._dispY;

            _window->setMouseCursorVisible(false);
        }
    }
}

void PrimaryBoardRenderer::lockShip(Vector2i mousePos) {
    if(_board->shipFits(*shipRenderers.at(_activeIndex)._ship)) {
        _board->placeShip(shipRenderers.at(_activeIndex)._ship);

        _activeIndex = -1;

        _window->setMouseCursorVisible(true);
    }
}

void PrimaryBoardRenderer::handleClick(Vector2i mousePos) {
    if(_activeIndex == -1) {
        selectShip(mousePos);
    } else {
        lockShip(mousePos);
    }
}

bool PrimaryBoardRenderer::spacePressed() {
    return _spaceCount == 1;
}

void PrimaryBoardRenderer::rotateShip() {
    shipRenderers.at(_activeIndex)._ship->rotate();
}

class HumanSFMLPlayer : public Player {
public:
    HumanSFMLPlayer(vector<int> shipLengths);

    RenderWindow window;
    PrimaryBoardRenderer pBoardRenderer;
    BoardRenderer<TrackingBoard> tBoardRenderer;

    void setTrackingFleet(vector<Ship> *otherFleet) override;

    pair<int, int> getMove() override;
    void placeShips() override;
};

HumanSFMLPlayer::HumanSFMLPlayer(vector<int> shipLengths) : Player(shipLengths),
                                                            pBoardRenderer(window, primaryBoard, primaryFleet, 25, 50),
                                                            window(VideoMode(1625, 700), "SFML Example Window"){}

void HumanSFMLPlayer::setTrackingFleet(vector<Ship> *otherFleet) {
    Player::setTrackingFleet(otherFleet);

    BoardRenderer<TrackingBoard> newTBoardRenderer(window, trackingBoard, *trackingFleet, 825, 50);
    tBoardRenderer = newTBoardRenderer;
}

void HumanSFMLPlayer::placeShips() {
    while( window.isOpen() ) {
        window.clear( Color::Black );

        pBoardRenderer.draw(true);
        tBoardRenderer.draw(true);

        Vector2i mousePos = sf::Mouse::getPosition(window);
        pBoardRenderer.updateSpacePress();

        pBoardRenderer.updateShipPos(mousePos);

        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            pBoardRenderer.handleClick(mousePos);
        }

        if(pBoardRenderer.spacePressed()) {
            pBoardRenderer.rotateShip();
        }

        if(allShipsPlaced()) {
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
    pBoardRenderer.resetShipLocation(); // Reset the ships that were placed before to their original coordinates

    while( window.isOpen() ) {
        window.clear( Color::Black );

        pBoardRenderer.draw(false);
        tBoardRenderer.draw(false);

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
    Battlelog(string filename);
    ~Battlelog();

    ofstream battlelogFile;

    char convertYPos(int yPos);
    int convertXPos(int xPos);
    string getOutcomeString(Board::ShotOutcome outcome);
    void recordMove(string name, int xPos, int yPos, Board::ShotOutcome outcome);
    void recordWinner(string name);
};

Battlelog::Battlelog(string filename) {
    battlelogFile.open(filename);

    if(!battlelogFile) {
        cerr << "Failed to open the battelog" << endl;
        return;
    }
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

void Battlelog::recordMove(string name, int xPos, int yPos, Board::ShotOutcome outcome) {
    battlelogFile << name << " called " << convertYPos(yPos) << convertXPos(xPos) << ": " << getOutcomeString(outcome) << endl;
}

void Battlelog::recordWinner(string name) {
    battlelogFile << name << " wins!" << endl;
}

template<typename p1Type, typename p2Type>
class Game {
public:
    Game(vector<int> shipLengths = DEFAULT_LENGTHS, string battlelogName = "battelog.txt");

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
Game<p1Type, p2Type>::Game(vector<int> shipLengths, string battlelogName) : playerOne(shipLengths), playerTwo(shipLengths), battlelog(battlelogName) {
    turn = 1;
    playerOne.bindTrackingFleets(&playerTwo);
}

template<typename p1Type, typename p2Type>
void Game<p1Type, p2Type>::runGame() {
    playerOne.placeShips();

    if(!playerOne.allShipsPlaced()) {
        cerr << "Player One Failed to place all their ships" << endl;
        return;
    }

    playerTwo.placeShips();

    if(!playerTwo.allShipsPlaced()) {
        cerr << "Player Two failed to place all their ships" << endl;
        return;
    }

    while(true) {
        if(turn == 1) {
            pair<int, int> move = playerOne.getMove();

            if(move.first < 0) {
                cerr << "Player one has forfeited the match" << endl;
                return;
            }

            Board::ShotOutcome outcome = playerTwo.fireShotAt(move.first, move.second);
            playerOne.markShot(move.first, move.second, outcome);
            battlelog.recordMove("Player One", move.first, move.second, outcome);

            if(playerTwo.allShipsSunk()) {
                battlelog.recordWinner("Player One");
                cerr << "Player One wins" << endl;
                return;
            }

            turn = 2;
        } else if(turn == 2) {
            pair<int, int> move = playerTwo.getMove();

            if(move.first < 0) {
                cerr << "Player two has forfeited the match" << endl;
                return;
            }

            Board::ShotOutcome outcome = playerOne.fireShotAt(move.first, move.second);
            playerTwo.markShot(move.first, move.second, outcome);
            battlelog.recordMove("Player Two", move.first, move.second, outcome);

            if(playerOne.allShipsSunk()) {
                battlelog.recordWinner("Player Two");
                cerr << "Player Two wins" << endl;
                return;
            }

            turn = 1;
        }
    }
}

int main() {
    vector<int> shipLengths = {1, 1, 1, 1, 1, 1};

    Game<HumanSFMLPlayer, ComputerRandomPlayer> game;

    game.runGame();
}