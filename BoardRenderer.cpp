//
// Created by Colin Siles on 2019-12-02.
//

#include "BoardRenderer.h"


BoardRenderer::BoardRenderer(RenderWindow &window, Board &board, double dispX, double dispY, string label) {
    _window = &window;
    _board = &board;
    _dispX = dispX;
    _dispY = dispY;

    _label = label;

    if(!_font.loadFromFile("data/arial.ttf")) {
        cerr << "Error loading font" << endl;
    }
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

    Text boardLabel;
    boardLabel.setFont(_font);
    boardLabel.setString(_label);
    boardLabel.setFillColor(Color::White);
    boardLabel.setPosition(_dispX, _dispY + 25 + 50 * (Board::GRID_SIZE));

    _window->draw(boardLabel);
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
