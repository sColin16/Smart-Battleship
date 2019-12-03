//
// Created by Colin Siles on 2019-12-02.
//

#include "RandomComputerPlayer.h"

pair<int, int> RandomComputerPlayer::getMove() {
    while(true) {
        int randomRow = rand() % 10;
        int randomCol = rand() % 10;

        if(_trackingBoard.validGuess(randomRow, randomCol)) {
            return make_pair(randomRow, randomCol);
        }
    }
}

void RandomComputerPlayer::placeShips() {
    placeShipsRandomly();
}

void RandomComputerPlayer::reportGameover(bool winner) {
    return;
}
