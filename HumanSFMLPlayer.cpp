/* HumanSFMLPlayer.cpp
 *
 * Author: Colin Siles
 *
 * The HumanSFMLPlayer class is the subclass of player that allows for a human to play using a GUI with SFML
 * Utilizes the BoardRender, and FleetRenderer class to show the player the game
*/

#include "HumanSFMLPlayer.h"

// Use intializer lists to initialize all of the data members
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

// Lets the human player place all their ships onto the grid
void HumanSFMLPlayer::placeShips() {
    // Repeatedly render the window, as long as it is open
    while(_window.isOpen()) {
        // Set the bakgroudn to black
        _window.clear( Color::Black );

        // Draw the player's baord and fleet
        _pBoardRenderer.draw();
        _pFleetRenderer.draw(true);

        // Draw some unstrctions for the player
        Text instructions;

        instructions.setFont(_font);
        instructions.setPosition(850, 150);
        instructions.setFillColor(Color::White);

        instructions.setString("Instructions:\n\n\t1. Click (don\'t drag) to select a ship\n\
\t2. Move the ship to the desired location\n\t3. Press Space to rotate the ship\n\t4. Click to lock the ship\n\
\t5. Click on your opponents grid to make guesses\n\t6. Sink your opponent's ships first win!");

        _window.draw(instructions);

        // Update the fleetRenderer on the mousePos and space press status
        Vector2i mousePos = sf::Mouse::getPosition(_window);
        _pFleetRenderer.updateSpacePress();

        _pFleetRenderer.updateShipPos(mousePos);

        // Handle a click if one occurs
        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            _pFleetRenderer.handleClick(mousePos);
        }

        // Rotate the ship if the space key is pressed
        if(_pFleetRenderer.spacePressed()) {
            _pFleetRenderer.rotateShip();
        }

        // Exit once the player has placed all their ships
        if(allShipsPlaced()) {
            _pFleetRenderer.resetLocations(); // Reset the ships that were placed before to their original coordinates
            return;
        }

        // Render the window
        _window.display();

        // Event checking loop to let the player close the window if they want
        // (Will result in the game class stopping the game, if not all ships were placed)
        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
            }
        }

    }
}

// Allows the player to make a guess against their opponent
pair<int, int> HumanSFMLPlayer::getMove() {
    while(_window.isOpen()) {
        // Draw the black background
        _window.clear( Color::Black );

        // Draw both board and both fleets
        _pBoardRenderer.draw();
        _tBoardRenderer.draw();

        _pFleetRenderer.draw(false);
        _tFleetRenderer.draw(false);

        // Get the mouse position
        Vector2i mousePos = sf::Mouse::getPosition(_window);

        // Draw the selection square on the tracking board
        _tBoardRenderer.drawStatusSquare(mousePos.x, mousePos.y);

        // If the mouse is pressed
        if(Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Get the board position of the guess
            pair<int, int> move = _tBoardRenderer.getGridPos(mousePos.x, mousePos.y);

            // And make that move if the move is valid (inside the grid, nota double guess, etc.)
            if(_trackingBoard.validGuess(move.first, move.second)) {
                return move;
            }
        }

        // Render the window
        _window.display();

        // Event loop to let the player close the window if they want
        // Return an invalid move to let the game object know the player has forfeited
        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
                return make_pair(-1, -1);
            }
        }
    }

    // If something went wrong, jsut return an invalid move
    return make_pair(-1, -1);
}

// Shows the player who won the game, on the screen
void HumanSFMLPlayer::reportGameover(bool winner) {
    while(_window.isOpen()) {
        // Draw the background, and the fleets
        _window.clear( Color::Black );

        _pBoardRenderer.draw();
        _tBoardRenderer.draw();

        _pFleetRenderer.draw(false);
        _tFleetRenderer.draw(false);

        // Draw the label for if the player won or lost
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

        // Again, even loop to let the player close the window, and stop the game
        Event event;
        while(_window.pollEvent(event)) {
            if( event.type == Event::Closed ) {
                _window.close();
            }
        }
    }
}
