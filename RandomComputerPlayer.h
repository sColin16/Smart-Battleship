/* RandomComputerPlayer.h
 *
 * Author: Colin Siles
 *
 * The RandomComputerPlayer Class is a subclass of Player that places its ships randomly, and fires randomly.
 * It's not a super fun opponent to play against, and it's awful.
*/

#ifndef SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H
#define SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H

#include "Player.h"

class RandomComputerPlayer : public Player {
public:
    using Player::Player; // Use the player constructor, no new fields to intialize

    // Override the three necessary functions
    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;
};


#endif //SFML_TEMPLATE_RANDOMCOMPUTERPLAYER_H
