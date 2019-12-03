/* IntelligentComputer.cpp
 *
 * Author: Colin Siles
 *
 * The IntelligentComputer class is a Player subclass that uses probability to determine where to make its next move
 * This player is really good, and can win a game in an average of about 40 shots
*/

#include "IntelligentComputer.h"

// Overrode function to track what happens when a ship is sunk
void IntelligentComputer::markShot(int xPos, int yPos, ShotOutcome outcome) {
    // Call the superclass markShot function, to mark the gird
    Player::markShot(xPos, yPos, outcome);

    // Add the move to the hit list if it was a hit
    if(outcome.hit) {
        _hitList.push_back(make_pair(xPos, yPos));
    }

    // If the shot resulted in a hit, mark the correct squares as sunk, so they aren't considered anymore
    if(outcome.sunkenIndex >= 0) {
        _markAsSunk(xPos, yPos, _trackingFleet.ship(outcome.sunkenIndex).getLength());

    // Otherwise, just reset the lastHit variable
    } else if(outcome.hit){
        _lastHit = make_pair(xPos, yPos);
    }
}

void IntelligentComputer::_markAsSunk(int xPos, int yPos, int length) {
    // "Start spots" for guesses where a ship begins in each direction
    pair<int, int> horizontalStart = make_pair(-1, -1);
    pair<int, int> verticalStart = make_pair(-1, -1);

    // Number of consecutive hits in each direction from this spot
    int horizontalConsecutiveHits = 0;
    int verticalConsecutiveHits = 0;

    // Find the number of consecutive shots in each direction
    _findConsecutiveShots(xPos, yPos, length, true, horizontalStart, horizontalConsecutiveHits);
    _findConsecutiveShots(xPos, yPos, length, false, verticalStart, verticalConsecutiveHits);

    // If sufficient hits in both directions were found to mark a ship as sunk, we don't know what to mark
    // So just mark the last shot as sunk, and we'll let it work itself out
    if(horizontalConsecutiveHits >= length && verticalConsecutiveHits >=length) {
        _trackingBoard._grid.at(xPos).at(yPos) = Board::SHIP;

    // We found enough hits in the horizontal direction to mark a ship as sunk, so do so
    } else if(horizontalConsecutiveHits >= length) {
        pair<int, int> direction = make_pair(1, 0);

        _markSquaresAsSunk(horizontalStart, direction, length, horizontalConsecutiveHits);

    // We found enough hits in the vertical direction to mark a ship as sunk, so do so
    } else if(verticalConsecutiveHits >=length) {
        pair<int, int> direction = make_pair(0, 1);

        _markSquaresAsSunk(verticalStart, direction, length, verticalConsecutiveHits);

    // We didn't find enough consecutive hits in any direction to mark a ship as sunk
    // This probably means somethign went wrong above, so just mark that spot as sunk
    } else {
        _trackingBoard._grid.at(xPos).at(yPos) = Board::SHIP;
    }
}

// Finds the maximum number of consecutive shots
void IntelligentComputer::_findConsecutiveShots(int xPos, int yPos, int length, bool horizontal, pair<int, int> &start, int &consecutiveHits) {
    // Determine the direction to search in based on if finding shots in the horizontal direction or not
    pair<int, int> direction = horizontal ? make_pair(1, 0) : make_pair(0, 1);

    // Track the maximum number of consecutive hits, and the start of that ship
    // We need to track maximums, in case their are pockets of consecutive hits
    int maxConsecutiveHits = 0;
    pair<int, int> maxStart = make_pair(-1, -1);

    // Iterate over each possiblesquare in either direction
    for(int i = -length + 1; i < length; i++) {
        // Determine the square to search based on the direction
        int xMark = xPos + direction.first * i;
        int yMark = yPos + direction.second * i;

        // If the guess is inside the grid, and a hit, we found a hit
        if(_trackingBoard._posInsideGrid(xMark, yMark) && _trackingBoard._grid.at(xMark).at(yMark) == Board::HIT_MARKER) {
            // If the start variable has negatives, it hasn't been set yet, so set this square as a the start
            if(start.first < 0) {
                start = make_pair(xMark, yMark);
            }

            // Increment the number of consecutive hits found
            consecutiveHits++;

            // If the current number of consecutive hits is at a max, store that number, and the start
            if(consecutiveHits > maxConsecutiveHits) {
                maxConsecutiveHits = consecutiveHits;
                maxStart = start;
            }

        // If we didn't find a hit, but found hits before, reset all the variables
        } else if(consecutiveHits > 0) {
            consecutiveHits = 0;
            start = make_pair(-1, -1);
        }
    }

    // Set the variables to the maximum that were found, which is most likely what we're looking for
    start = maxStart;
    consecutiveHits = maxConsecutiveHits;
}

// Marks a series of squares from a starting point, in a given direction as sunk.
void IntelligentComputer::_markSquaresAsSunk(pair<int, int> start, pair<int, int> direction, int length, int consecutiveHits) {
    // If more consecutive hits were found than ships, we can't mark them all as sunk, just a subset
    if(consecutiveHits > length) {
        int diff = consecutiveHits - length;

        // modify the length that we mark as hit
        // (e.g. if we found found conseutive hits for length 3, we can only mark two squares as sunk)
        length -= diff;

        // Change the start position as well to account for this difference
        // "We'll mark the middle, where we can be sure the ships were indeed sunk
        start.first += direction.first * diff;
        start.second += direction.second * diff;
    }

    // Iterate over every square to be marked as sunk
    for(int i = 0; i < length; i++) {
        int xMark = start.first + direction.first * i;
        int yMark = start.second + direction.second * i;

        // Mark the position as a ship. This is an easy way to mark as sunk, that leverages existing functionality
        // (i.e. don't need to create a sunk marker or an entirely new board class to handle this)
        _trackingBoard._grid.at(xMark).at(yMark) = Board::SHIP;

        // Remove the coordinate in the hit list; that coordinate was sunk
        for (int j = 0; j < _hitList.size(); j++) {
            if (_hitList.at(j).first == xMark && _hitList.at(j).second == yMark) {
                _hitList.erase(_hitList.begin() + j);
                break;
            }
        }
    }
}

// Returns a blank grid in which its possible to store the probability of a ship being in each location
vector<vector<int>> IntelligentComputer::_newProbabilityGrid() {
    vector<vector<int>> output;

    // Create a 10 by 10 grid, which matches the size of the board
    for(int i = 0; i < 10; i++) {
        vector<int> newRow(10, 0);
        output.push_back(newRow);
    }

    return output;
}

// Similar to "placeShip", but adds to the probabiltiy density grid
void IntelligentComputer::_addToDensity(Ship ship, vector<vector<int>> &probabilityGrid) {
    // Iterate over each square in the ship
    for(auto &square : ship.getSquares()) {
        pair<int, int> squarePos = square.first;

        // Add one to the corresponding position, since there's a new way for a ship to be there
        probabilityGrid.at(squarePos.first).at(squarePos.second) += 1;
    }
}

// Returns a probability grid in "search" mode, when no hits have been identified
vector<vector<int>> IntelligentComputer::_findSearchProbability() {
    vector<vector<int>> probabilityGrid = _newProbabilityGrid();

    // Iterate over every possible ship
    for(int i = 0; i < _trackingFleet.size(); i++) {
        Ship currentShip = _trackingFleet.ship(i);

        // Exit if the ship has been sunk; it cannot be located anywhere now
        if(currentShip.isSunk()) {
            continue;
        }

        // Iterate through the entire board, every possible place to fit the ship
        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < 10; k++) {
                // Make the ship horizontal at the position
                currentShip.setHorizontal();
                currentShip.setGridPos(j, k);

                // If the ship fits there, add it to the density
                if(_trackingBoard.shipFits(currentShip)) {
                    _addToDensity(currentShip, probabilityGrid);
                }

                // Now make the ship vertical, at the position, add to density if it fits again
                currentShip.rotate();
                currentShip.setGridPos(j, k);
                if(_trackingBoard.shipFits(currentShip)) {
                    _addToDensity(currentShip, probabilityGrid);
                }
            }
        }
    }

    // Return the resulting grid
    return probabilityGrid;
}

// Returns a probability grid in "destory" mode, when a hit has been found
vector<vector<int>> IntelligentComputer::_findDestroyProbability() {
    vector<vector<int>> probabilityGrid = _newProbabilityGrid();

    // Iterate over each hit in the hit list
    for(int i = 0; i < _hitList.size(); i++) {
        // Iterate over each ship
        for(int j = 0; j < _trackingFleet.size(); j++) {
            Ship currentShip = _trackingFleet.ship(j);

            // Continue to the next ship if this ship was already sunk
            if(currentShip.isSunk()) {
                continue;
            }

            // Iterate over each possible position the ship could be placed intersecting with this hit
            for(int k = 0; k < currentShip.getLength(); k++) {
                // As with search mode, place the ship in the given locations, check if they fit
                // and add to the probability density if it does fit

                currentShip.setHorizontal();
                currentShip.setGridPos(_hitList.at(i).first - k, _hitList.at(i).second);
                if(_trackingBoard.shipFits(currentShip)) {
                    _addToDensity(currentShip, probabilityGrid);
                }

                currentShip.rotate();
                currentShip.setGridPos(_hitList.at(i).first, _hitList.at(i).second - k);
                if(_trackingBoard.shipFits(currentShip)) {
                    _addToDensity(currentShip, probabilityGrid);
                }
            }
        }
    }

    // Return the resulting probability distribution
    return probabilityGrid;
}

// Selects a move from a probaility grid (random move with maximum probability)
pair<int, int> IntelligentComputer::_chooseFromProbability(vector<vector<int>> probabilityGrid) {
    int maxVal = 0; // Tracks the maximum value in the grid
    vector<pair<int, int>> maxCoords; // Tracks the location of those maximums

    // Iterate throughout the entire probaility grid
    for(int i = 0; i < probabilityGrid.size(); i++) {
        for(int j = 0; j < probabilityGrid.at(i).size(); j++) {
            // If a new maximum is found
            if(probabilityGrid.at(i).at(j) > maxVal && _trackingBoard.validGuess(i, j)) {
                // Record that maximum
                maxVal = probabilityGrid.at(i).at(j);

                // Reset the maxCoords vector to store only this coordinate
                maxCoords.clear();
                maxCoords.push_back(make_pair(i, j));

            // If a value matching the maximum was found, ad that coordinate to the maxCoords vector
            } else if(probabilityGrid.at(i).at(j) == maxVal && _trackingBoard.validGuess(i, j)) {
                maxCoords.push_back(make_pair(i, j));
            }
        }
    }

    // This section is necessary in case all of the probabilities were 0, and then modulus doesn't work
    // because something mod 0 is undefined
    if(maxCoords.size() > 0) {
        int randIndex = rand() % maxCoords.size();
        return maxCoords.at(randIndex);
    }

    // If no max value greater than 0 was found, jstu return 0, 0, b/c its all the same
    return make_pair(0, 0);
}

// Allows the computer player to intelligent return a move
pair<int, int> IntelligentComputer::getMove() {
    vector<vector<int>> probabilityGrid;

    // If the hit list is empty, we're in "search mode"
    if(_hitList.empty()) {
        probabilityGrid = _findSearchProbability();

    // Otherwise, we're in destory mode
    } else {
        probabilityGrid = _findDestroyProbability();
        pair<int, int> maxPos = _chooseFromProbability(probabilityGrid);

        // If it turns out the max value was 0, then something went wrong. Probably an ambiguous case when trying
        // to determine which ship was sunk, which left some hits in the hit list which chould have been marked as sunk
        if(probabilityGrid.at(maxPos.first).at(maxPos.second) == 0) {
            // To accomodate, reset the hit list, and return to search mode
            // If this isn't done, the computer starts guessing randomly
            _hitList.clear();
            probabilityGrid = _findSearchProbability();
        }
    }

    // Return a move with one of the maximum mprobabilities for a hit
    return _chooseFromProbability(probabilityGrid);
}

// Intellignet player places them randomly, there doesn't seem to be a better strategy
void IntelligentComputer::placeShips() {
    placeShipsRandomly();
}

// Intellignet player curently doesn't do anything upon game over
void IntelligentComputer::reportGameover(bool winner) {
    return;
}