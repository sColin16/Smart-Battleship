//
// Created by Colin Siles on 2019-12-02.
//

#include "HumanSFMLPlayer.h"


HumanSFMLPlayer::HumanSFMLPlayer(string name, vector<int> shipLengths) : Player(name, shipLengths),
                                                                         _window(VideoMode(1625, 700), "SFML Example Window"),
                                                                         _pBoardRenderer(_window, _primaryBoard, 25, 25, "Your Board"),
                                                                         _tBoardRenderer(_window, _trackingBoard, 825, 25, "Opponent's Board"),
                                                                         _tFleetRenderer(_trackingFleet, &_tBoardRenderer, _window),
                                                                         _pFleetRenderer(_primaryFleet, &_pBoardRenderer, _window){
    if(!_font.loadFromFile("data/arial.ttf")) {
        cerr << "Error loading font" << endl;
    }
}

void HumanSFMLPlayer::placeShips() {
    while(_window.isOpen()) {
        _window.clear( Color::Black );

        _pBoardRenderer.draw();
        _pFleetRenderer.draw(true);

        Text instructions;

        instructions.setFont(_font);
        instructions.setPosition(850, 150);
        instructions.setFillColor(Color::White);

        instructions.setString("Instructions:\n\n\t1. Click (don\'t drag) to select a ship\n\
\t2. Move the ship to the desired location\n\t3. Press Space to rotate the ship\n\t4. Click to lock the ship\n\
\t5. Click on your opponents grid to make guesses\n\t6. Sink your opponent's ships first win!");

        _window.draw(instructions);

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

    return make_pair(-1, -1);
}

void HumanSFMLPlayer::reportGameover(bool winner) {
    while(_window.isOpen()) {
        _window.clear( Color::Black );

        _pBoardRenderer.draw();
        _tBoardRenderer.draw();

        _pFleetRenderer.draw(false);
        _tFleetRenderer.draw(false);

        Text gameStatus;
        gameStatus.setFont(_font);

        gameStatus.setPosition(500, 575);
        gameStatus.setCharacterSize(100);

        if(winner) {
            gameStatus.setFillColor(Color::Green);
            gameStatus.setString("You win!");
        } else {
            gameStatus.setFillColor(Color::Red);
            gameStatus.setString("You lose!");
        }

        _window.draw(gameStatus);

        _window.display();

        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
            }
        }
    }
}
