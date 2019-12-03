//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_BOARDRENDERER_H
#define SFML_TEMPLATE_BOARDRENDERER_H

#include <SFML/Graphics.hpp>
using namespace sf;

#include "Board.h"

// Responsible for rendering the grid and the fleet (depending on the context of if your being placed or playing)
class BoardRenderer {
public:
    BoardRenderer(RenderWindow &window, Board &board, double dispX, double dispY, string label);

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

    Font _font;
    string _label;
};


#endif //SFML_TEMPLATE_BOARDRENDERER_H
