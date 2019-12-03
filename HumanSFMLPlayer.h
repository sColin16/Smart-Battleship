//
// Created by Colin Siles on 2019-12-02.
//

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

    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;

private:
    RenderWindow _window;
    Font _font;

    BoardRenderer _pBoardRenderer;
    BoardRenderer _tBoardRenderer;

    FleetRenderer _pFleetRenderer;
    FleetRenderer _tFleetRenderer;
};

#endif //SFML_TEMPLATE_HUMANSFMLPLAYER_H
