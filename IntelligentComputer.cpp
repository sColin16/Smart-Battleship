//
// Created by Colin Siles on 2019-12-02.
//

#include "IntelligentComputer.h"


void IntelligentComputer::markShot(int xPos, int yPos, ShotOutcome outcome) {
    Player::markShot(xPos, yPos, outcome);

    if(outcome.hit) {
        hitList.push_back(make_pair(xPos, yPos));
    }

    if(outcome.sunkenIndex >= 0) {
        markAsSunk(xPos, yPos, _trackingFleet.ship(outcome.sunkenIndex).getLength());
    } else if(outcome.hit){
        lastHit = make_pair(xPos, yPos);
    }
}

void IntelligentComputer::markAsSunk(int xPos, int yPos, int length) {
    pair<int, int> direction;

    if(lastHit.first - xPos == 0) {
        direction = make_pair(0, 1);
    } else {
        direction = make_pair(1, 0);
    }

    pair<int, int> start = make_pair(-1, -1);
    int consecutiveHits = 0;

    for(int i = -length + 1; i < length; i++) {
        int xMark = xPos + direction.first * i;
        int yMark = yPos + direction.second * i;

        if(_trackingBoard.posInsideGrid(xMark, yMark) && _trackingBoard._grid.at(xMark).at(yMark) == Board::HIT_MARKER) {
            consecutiveHits++;

            if(start.first < 0) {
                start = make_pair(xMark, yMark);
            }
        }
    }

    if(consecutiveHits >= length) {
        if(consecutiveHits > length) {
            int diff = consecutiveHits - length;
            //cerr << "Ambiguous Case!! " << diff * 2 << " of " << length <<  "Unknown State squares" << endl;
            length -= diff;

            start.first += direction.first * diff;
            start.second += direction.second * diff;
        }

        for(int i = 0; i < length; i++) {
            int xMark = start.first + direction.first * i;
            int yMark = start.second + direction.second * i;

            _trackingBoard._grid.at(xMark).at(yMark) = Board::SHIP;

            for (int j = 0; j < hitList.size(); j++) {
                if (hitList.at(j).first == xMark && hitList.at(j).second == yMark) {
                    hitList.erase(hitList.begin() + j);
                    break;
                }
            }
        }
    } else {
        if(direction.first == 0) {
            direction = make_pair(1, 0);
        } else {
            direction = make_pair(0, 1);
        }

        start = make_pair(-1, -1);
        consecutiveHits = 0;

        for(int i = -length + 1; i < length; i++) {
            int xMark = xPos + direction.first * i;
            int yMark = yPos + direction.second * i;

            if(_trackingBoard.posInsideGrid(xMark, yMark) && _trackingBoard._grid.at(xMark).at(yMark) == Board::HIT_MARKER) {
                consecutiveHits++;

                if(start.first < 0) {
                    start = make_pair(xMark, yMark);
                }
            }
        }

        if(consecutiveHits >= length) {
            for(int i = 0; i < length; i++) {
                int xMark = start.first + direction.first * i;
                int yMark = start.second + direction.second * i;

                _trackingBoard._grid.at(xMark).at(yMark) = Board::SHIP;

                for (int j = 0; j < hitList.size(); j++) {
                    if (hitList.at(j).first == xMark && hitList.at(j).second == yMark) {
                        hitList.erase(hitList.begin() + j);
                        break;
                    }
                }
            }
        } else {
            //cerr << "Something went wrong determing where the sunken ship was located" << endl;
        }
    }
}

vector<vector<int>> IntelligentComputer::newProbabilityGrid() {
    vector<vector<int>> output;

    for(int i = 0; i < 10; i++) {
        vector<int> newRow(10, 0);
        output.push_back(newRow);
    }

    return output;
}

void IntelligentComputer::addToDensity(Ship ship, vector<vector<int>> &probabilityGrid) {
    for(auto &square : ship.getSquares()) {
        pair<int, int> squarePos = square.first;

        probabilityGrid.at(squarePos.first).at(squarePos.second) += 1;
    }
}

vector<vector<int>> IntelligentComputer::findSearchProbability() {
    vector<vector<int>> probabilityGrid = newProbabilityGrid();

    for(int i = 0; i < _trackingFleet.size(); i++) {
        Ship currentShip = _trackingFleet.ship(i);

        if(currentShip.isSunk()) {
            continue;
        }

        for(int j = 0; j < 10; j++) {
            for(int k = 0; k < 10; k++) {
                currentShip.setHorizontal();
                currentShip.setGridPos(j, k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }

                currentShip.rotate();
                currentShip.setGridPos(j, k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }
            }
        }
    }

    return probabilityGrid;
}

vector<vector<int>> IntelligentComputer::findDestroyProbability() {
    vector<vector<int>> probabilityGrid = newProbabilityGrid();

    for(int i = 0; i < hitList.size(); i++) {
        for(int j = 0; j < _trackingFleet.size(); j++) {
            Ship currentShip = _trackingFleet.ship(j);

            if(currentShip.isSunk()) {
                continue;
            }

            for(int k = 0; k < currentShip.getLength(); k++) {
                currentShip.setHorizontal();
                currentShip.setGridPos(hitList.at(i).first - k, hitList.at(i).second);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }

                currentShip.rotate();
                currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second - k);
                if(_trackingBoard.shipFits(currentShip)) {
                    addToDensity(currentShip, probabilityGrid);
                }
            }

            currentShip.setHorizontal();
            currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second);
            if(_trackingBoard.shipFits(currentShip)) {
                addToDensity(currentShip, probabilityGrid);
            }

            currentShip.rotate();
            currentShip.setGridPos(hitList.at(i).first, hitList.at(i).second);
            if(_trackingBoard.shipFits(currentShip)) {
                addToDensity(currentShip, probabilityGrid);
            }
        }
    }

    return probabilityGrid;
}

pair<int, int> IntelligentComputer::chooseFromProbability(vector<vector<int>> probabilityGrid) {
    int maxVal = 0;
    vector<pair<int, int>> maxCoords;

    for(int i = 0; i < probabilityGrid.size(); i++) {
        for(int j = 0; j < probabilityGrid.at(i).size(); j++) {
            if(probabilityGrid.at(i).at(j) > maxVal && _trackingBoard.validGuess(i, j)) {
                maxVal = probabilityGrid.at(i).at(j);

                maxCoords.clear();
                maxCoords.push_back(make_pair(i, j));

            } else if(probabilityGrid.at(i).at(j) == maxVal && _trackingBoard.validGuess(i, j)) {
                maxCoords.push_back(make_pair(i, j));
            }
        }
    }

    // This section is necessary in case all of the probabilities were 0

    if(maxCoords.size() > 0) {
        int randIndex = rand() % maxCoords.size();
        return maxCoords.at(randIndex);
    } else {
        return make_pair(0, 0);
    }
}

pair<int, int> IntelligentComputer::getMove() {
    vector<vector<int>> probabilityGrid;

    if(hitList.empty()) {
        probabilityGrid = findSearchProbability();
    } else {
        probabilityGrid = findDestroyProbability();
        pair<int, int> maxPos = chooseFromProbability(probabilityGrid);

        if(probabilityGrid.at(maxPos.first).at(maxPos.second) == 0) {
            hitList.clear();
            probabilityGrid = findSearchProbability();
        } else {
            return maxPos;
        }
    }

    return chooseFromProbability(probabilityGrid);
}

void IntelligentComputer::placeShips() {
    placeShipsRandomly();
}

void IntelligentComputer::reportGameover(bool winner) {
    return;
}