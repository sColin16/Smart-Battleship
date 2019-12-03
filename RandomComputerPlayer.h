//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H
#define SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H

#include "Player.h"

class RandomComputerPlayer : public Player {
public:
    using Player::Player;

    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;
};


#endif //SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H
