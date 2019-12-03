//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_GAME_H
#define SFML_TEMPLATE_GAME_H

#include <string>
#include <vector>

#include "Battlelog.h"
#include "Player.h"

using namespace std;

template<typename p1Type, typename p2Type>
class Game {
public:
    Game(string p1Name, string p2Name, vector<int> shipLengths = DEFAULT_LENGTHS, string battlelogName = "battlelog.txt");

    void runGame();

private:
    vector<Player *> _players;
    p1Type _playerOne;
    p2Type _playerTwo;
    int _turn;

    Battlelog _battlelog;

    static const vector<int> DEFAULT_LENGTHS;
};

template<typename p1Type, typename p2Type>
const vector<int> Game<p1Type, p2Type>::DEFAULT_LENGTHS = {5, 4, 4, 3, 2};


template<typename p1Type, typename p2Type>
Game<p1Type, p2Type>::Game(string p1Name, string p2Name, vector<int> shipLengths, string battlelogName) : _playerOne(p1Name, shipLengths),
                                                                                                          _playerTwo(p2Name, shipLengths), _battlelog(battlelogName, p1Name, p2Name) {
    _players = {&_playerOne, &_playerTwo};
    _turn = 0;
}

template<typename p1Type, typename p2Type>
void Game<p1Type, p2Type>::runGame() {
    for(int i = 0; i < 2; i++) {
        _players.at(i)->placeShips();

        if(!_players.at(i)->allShipsPlaced()) {
            cerr << _players.at(i)->getName() <<  " failed to place all their ships" << endl;
            return;
        }
    }

    while(true) {
        pair<int, int> move = _players.at(_turn)->getMove();

        if(move.first < 0) {
            cerr << _players.at(_turn)->getName() << " has forfeited the match" << endl;
            return;
        }

        ShotOutcome outcome = _players.at(!_turn)->fireShotAt(move.first, move.second);
        _players.at(_turn)->markShot(move.first, move.second, outcome);
        _battlelog.recordMove(_players.at(_turn)->getName(), move.first, move.second, outcome);

        if(_players.at(!_turn)->allShipsSunk()) {
            _battlelog.recordWinner(_players.at(_turn)->getName());

            _players.at(_turn)->reportGameover(true);
            _players.at(!_turn)->reportGameover(false);

            return;
        }

        _turn = !_turn;
    }
}

#endif //SFML_TEMPLATE_GAME_H
