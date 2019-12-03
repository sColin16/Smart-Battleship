/* IntelligentComputer.h
 *
 * Author: Colin Siles
 *
 * The IntelligentComputer class is a Player subclass that uses probability to determine where to make its next move
 * This player is really good, and can win a game in an average of about 40 shots
*/

#ifndef SFML_TEMPLATE_INTELLIGENTCOMPUTER_H
#define SFML_TEMPLATE_INTELLIGENTCOMPUTER_H

#include <vector>
#include <utility>

#include "Board.h"
#include "Player.h"
#include "Ship.h"

using namespace std;

class IntelligentComputer : public Player {
public:
    using Player::Player;  // Just use the Player constructor, there are no extra data members to initialize

    // Overrid the three main methods of the player class
    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;

    // Also override the mark shot function to perform extra analysis on which ships were sunken
    void markShot(int xPos, int yPos, ShotOutcome outcome) override;

private:
    // Vector of hits that haven't led to sunken ships yet
    vector<pair<int, int>> _hitList;

    // The vector to store the last hit (to determine the direction of a sunken ship is likely in)
    pair<int, int> _lastHit;

    // Marks a series of hits a ship, once its confirmed the ship was sunk
    void _markAsSunk(int xPos, int yPos, int getLength);

    // Helper function for marking a ships positino as sunk
    // Determines the number of consecutive hits in a direction
    void _findConsecutiveShots(int xPos, int yPos, int length, bool horizontal, pair<int, int> &start, int &consecutiveHits);
    void _markSquaresAsSunk(pair<int, int> start, pair<int, int> direction, int length, int consecutiveHits);

    // Returns a blank grid in which its possible to store the probability of a ship being in each location
    vector<vector<int>> _newProbabilityGrid();

    // Similar to "placeShip", but adds to the probabiltiy density grid
    void _addToDensity(Ship ship, vector<vector<int>> &probabilityGrid);

    // Returns a probability grid in "search" mode, when no hits have been identified
    vector<vector<int>> _findSearchProbability();

    // Returns a probability grid in "destory" mode, when a hit has been found
    vector<vector<int>> _findDestroyProbability();

    // Selects a move from a probaility grid (random move with maximum probability)
    pair<int, int> _chooseFromProbability(vector<vector<int>> probabilityGrid);
};


#endif //SFML_TEMPLATE_INTELLIGENTCOMPUTER_H
