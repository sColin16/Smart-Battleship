/* BoardRenderer.h
 *
 * Author: Colin Siles
 *
 * The BoardRenderer class is responsible for displaying a board onto an SFML window
 * It essentially wraps a Board object, and tracking the x and y position in order to draw it
 * The class also provides public methods to get information about the grid position of the mouse
*/

#ifndef SFML_TEMPLATE_BOARDRENDERER_H
#define SFML_TEMPLATE_BOARDRENDERER_H

#include <SFML/Graphics.hpp>
using namespace sf;

#include "Board.h"

class BoardRenderer {
public:
    BoardRenderer(RenderWindow &window, Board &board, double dispX, double dispY, string label);

    // Draws the full board
    void draw();

    // Draws a little white square to show which square the user is currently selecting
    void drawStatusSquare(double mouseX, double mouseY);

    // Returns the position that the mouse is at, so ships can be placed according to where the user's mouse is
    pair<int, int> getGridPos(double xPos, double yPos);

    // Getters for the Board's x and y position, and the board
    double getDispX() const;
    double getDispY()const ;

    Board &getBoard();

protected:
    // Store a pointer to the window, and the board to manipulate/access them when necessary
    RenderWindow *_window;
    Board *_board;

    // Store the upper left-hand corner form which the board is drawn
    double _dispX;
    double _dispY;

    // Other members for text
    Font _font;
    string _label;
};

#endif //SFML_TEMPLATE_BOARDRENDERER_H
