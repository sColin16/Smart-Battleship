/* CSCI 261 Final Project: GUI Battleship
 *
 * Author: Colin Siles
 *
 * Play Battleship against an intelligent computer opponent
 *
 * A 2D vector is used to store the board
 * A file, called "battlelog.txt" is written to, which reports all the shots made during the game
 * A series of classes were created to implement Battleship
*/

#include <cstdlib>
#include <ctime>

#include "Game.h"
#include "HumanSFMLPlayer.h"
#include "IntelligentComputer.h"

int main() {
    // Seed random number generator for the computer player
    srand(time(0));

    // Uncomment these lines to customize the length of the ships you play with
    /*
    vector<int> shipLengths = {3, 3, 3, 3, 3, 3};
    Game<HumanSFMLPlayer, IntelligentComputer> game("Human", "Computer", shipLengths);
    */

    // Instantiate the game object with the given types and names
    Game<HumanSFMLPlayer, IntelligentComputer> game("Human", "Computer");

    // Run the game
    game.runGame();
}
