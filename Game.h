/* Game.h (includes both the header and the implementation file in one, since this class is templated)
 *
 * Author: Colin Siles
 *
 * The Game class serves as a mediator between two players, getting a move from them when its their turn, and alerting
 * the other player the outcome of the shot. It also will report to the players when the game is over, and uses the
 * Battlelog class to record the moves made by both players. It is templated so that the class can be used with various
 * types of players
*/

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

    // The only function that needs to be run: manages the entire game
    void runGame();

private:
    // Store players in a vector to prevent duplicate code
    vector<Player *> _players;

    // Also store players here, so all you need to do is pass the player types to the template
    p1Type _playerOne;
    p2Type _playerTwo;

    // Tracks whose turn it is. 0 is for the first player, 1 is for the second player
    int _turn;

    // Battlelog objects
    Battlelog _battlelog;

    // Static object to store the default lengths for ships in Battleship
    static const vector<int> DEFAULT_LENGTHS;
};

// Define the default lengths according to the original game of Battleship
template<typename p1Type, typename p2Type>
const vector<int> Game<p1Type, p2Type>::DEFAULT_LENGTHS = {5, 4, 4, 3, 2};


// Use intiizlier lists to initizilize both players
template<typename p1Type, typename p2Type>
Game<p1Type, p2Type>::Game(string p1Name, string p2Name, vector<int> shipLengths, string battlelogName) : _playerOne(p1Name, shipLengths),
        _playerTwo(p2Name, shipLengths), _battlelog(battlelogName, p1Name, p2Name) {

    // Set the vector of players to store pointer to the two players
    _players = {&_playerOne, &_playerTwo};

    // Deafult for turn is 0 (first player, obviously)
    _turn = 0;
}

template<typename p1Type, typename p2Type>
void Game<p1Type, p2Type>::runGame() {
    // For both of the players
    for(int i = 0; i < 2; i++) {
        // Request that they place their ships
        _players.at(i)->placeShips();

        // But return an error and exit if the player doesn't palce all their ships
        if(!_players.at(i)->allShipsPlaced()) {
            cerr << _players.at(i)->getName() <<  " failed to place all their ships" << endl;
            return;
        }
    }

    // Continue running until the game is over
    while(true) {
        // Get the move from the player
        pair<int, int> move = _players.at(_turn)->getMove();

        // Ensure that its valid, end the game if it's not
        // (HumanSFMLPlayer returns -1 if the player closes the window)
        if(move.first < 0) {
            cerr << _players.at(_turn)->getName() << " has forfeited the match" << endl;
            return;
        }

        // Capture the outcome of the shot at the opponent
        ShotOutcome outcome = _players.at(!_turn)->fireShotAt(move.first, move.second);

        // Allow the player to mark the outcome of their shot
        _players.at(_turn)->markShot(move.first, move.second, outcome);

        // Record the move in the battlelog
        _battlelog.recordMove(move.first, move.second, outcome);

        // If all the opponents ships were sunk...
        if(_players.at(!_turn)->allShipsSunk()) {
            // Record the winner in the battlelog
            _battlelog.recordWinner(_players.at(_turn)->getName());

            // Report that the game ended, and who won to the players
            _players.at(_turn)->reportGameover(true);
            _players.at(!_turn)->reportGameover(false);

            // Exit the game
            return;
        }

        // Toggle the turn
        _turn = !_turn;
    }
}

#endif //SFML_TEMPLATE_GAME_H
