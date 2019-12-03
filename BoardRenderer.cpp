/* BoardRenderer.cpp
 *
 * Author: Colin Siles
 *
 * The BoardRenderer class is responsible for displaying a board onto an SFML window
 * It essentially wraps a Board object, and tracking the x and y position in order to draw it
 * The class also provides public methods to get information about the grid position of the mouse
*/

#include "BoardRenderer.h"

// Primary constructor
BoardRenderer::BoardRenderer(RenderWindow &window, Board &board, double dispX, double dispY, string label) {
    // Store pointer to the necessary objects
    _window = &window;
    _board = &board;

    // Copy other arguemnts
    _dispX = dispX;
    _dispY = dispY;

    _label = label;

    // Attempt to the load the font
    if(!_font.loadFromFile("data/arial.ttf")) {
        cerr << "Error loading font" << endl;
    }
}

// Draws the board, with its label onto an SFML Window object
void BoardRenderer::draw() {
    // Iterate over the 2D array of the board
    for(int i = 0; i < Board::GRID_SIZE; i++) {
        for(int j = 0; j < Board::GRID_SIZE; j++) {
            // Get the value of the given square
            Board::SquareState value = _board->_grid.at(i).at(j);

            Color color;

            // The square's background color is blue if blank or a miss
            if (value == Board::BLANK || value == Board::MISS_MARKER) {
                color = Color::Blue;

            // The square's background color is grey if a hit or a ship
            } else {
                color = Color(120, 120, 120);
            }

            // Draws the square onto the window, the the correct color
            RectangleShape square(Vector2f(50, 50));
            square.setPosition(_dispX + 50 * i, _dispY + 50 * j);
            square.setFillColor(color);
            square.setOutlineColor(Color::Black);
            square.setOutlineThickness(2);
            _window->draw(square);

            // If the square is a miss or a hit, draw a marker to display that
            if (value == Board::MISS_MARKER || value == Board::HIT_MARKER) {
                CircleShape marker(20, 20);
                marker.setPosition(_dispX + 5 + 50 * i, _dispY + 5 + 50 * j);

                // Color Determined by whether it was a hit or a miss
                marker.setFillColor(value == Board::MISS_MARKER ? Color::White : Color::Red);
                _window->draw(marker);
            }
        }
    }

    // Draw the label for the board (communicates whose board it is)
    Text boardLabel;
    boardLabel.setFont(_font);
    boardLabel.setString(_label);
    boardLabel.setFillColor(Color::White);
    boardLabel.setPosition(_dispX, _dispY + 25 + 50 * (Board::GRID_SIZE));

    _window->draw(boardLabel);
}

// Draws a little white square to show which square the user is currently selecting
void BoardRenderer::drawStatusSquare(double mouseX, double mouseY) {
    // Determine the position of the square the mouse is currently over
    pair<int, int> gridPos = getGridPos(mouseX, mouseY);

    int xPos = gridPos.first;
    int yPos = gridPos.second;

    // As long as the mouse is inside the grid, draw the status square
    if(xPos >=0 && xPos < 10 && yPos >=0 && yPos < 10) {
        RectangleShape selectionSquare(Vector2f(50, 50));
        selectionSquare.setPosition(xPos * 50 + _dispX, yPos * 50 + _dispY);

        // Drawing with some alpha makes the sqaure partially transparent
        selectionSquare.setFillColor(Color(255, 255, 255, 100));

        _window->draw(selectionSquare);
    }
}

pair<int, int> BoardRenderer::getGridPos(double xPos, double yPos) {
    // The position is simply the difference in the xPos and the mousePos, all divided by 50
    // (using integer division, to truncate any decimals)
    int rowPos = (xPos - _dispX) / 50;
    int columnPos = (yPos - _dispY) / 50;

    return make_pair(rowPos, columnPos);
}

// Various getters for the class
Board &BoardRenderer::getBoard() {
    return *_board;
}

double BoardRenderer::getDispX() const {
    return _dispX;
}

double BoardRenderer::getDispY() const {
    return _dispY;
}
