/* RandomComputerPlayer.cpp
 *
 * Author: Colin Siles
 *
 * The RandomComputerPlayer Class is a subclass of Player that places its ships randomly, and fires randomly.
 * It's not a super fun opponent to play against, and it's awful.
*/

#include "RandomComputerPlayer.h"

pair<int, int> RandomComputerPlayer::getMove() {
    // As long as a move hasn't been found
    while(true) {
        // Choose a random move
        int randomRow = rand() % 10;
        int randomCol = rand() % 10;

        // And return that move if it is valid
        if(_trackingBoard.validGuess(randomRow, randomCol)) {
            return make_pair(randomRow, randomCol);
        }
    }
}

// Random player just places ships randomly
void RandomComputerPlayer::placeShips() {
    placeShipsRandomly();
}

// Random player doesn't do anything upon gameover
void RandomComputerPlayer::reportGameover(bool winner) {
    return;
}
