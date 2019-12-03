/* HumanSFMLPlayer.h
 *
 * Author: Colin Siles
 *
 * The HumanSFMLPlayer class is the subclass of player that allows for a human to play using a GUI with SFML
 * Utilizes the BoardRender, and FleetRenderer class to show the player the game
*/

#ifndef SFML_TEMPLATE_HUMANSFMLPLAYER_H
#define SFML_TEMPLATE_HUMANSFMLPLAYER_H

#include <SFML/Graphics.hpp>
using namespace sf;

#include "FleetRenderer.h"
#include "BoardRenderer.h"
#include "Player.h"

#include <string>
#include <vector>
#include <utility>

using namespace std;

class HumanSFMLPlayer : public Player {
public:
    HumanSFMLPlayer(string name, vector<int> shipLengths);

    // Override the three primary functions for a Player subclass
    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;

private:
    // Window object to draw on, that is passed to all the drawing data members
    RenderWindow _window;

    // Font to draw some instructions for the player
    Font _font;

    // Board Renderers for the player's grid, and tracking grid of their opponent's ships
    BoardRenderer _pBoardRenderer; // Primary Board Renderer
    BoardRenderer _tBoardRenderer; // Tracking Board Renderer

    // Fleet Renderers for their fleet, and their opponent's fleet
    FleetRenderer _pFleetRenderer; // Primary Fleet Renderer
    FleetRenderer _tFleetRenderer; // Tracking Fleet Renderer
};

#endif //SFML_TEMPLATE_HUMANSFMLPLAYER_H
