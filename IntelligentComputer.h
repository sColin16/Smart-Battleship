//
// Created by Colin Siles on 2019-12-02.
//

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
    using Player::Player;

    vector<pair<int, int>> hitList;
    pair<int, int> lastHit;

    void markShot(int xPos, int yPos, ShotOutcome outcome) override;
    void markAsSunk(int xPos, int yPos, int getLength);

    vector<vector<int>> newProbabilityGrid();

    void addToDensity(Ship ship, vector<vector<int>> &probabilityGrid);
    vector<vector<int>> findSearchProbability();
    vector<vector<int>> findDestroyProbability();
    pair<int, int> chooseFromProbability(vector<vector<int>> probabilityGrid);

    pair<int, int> getMove() override;
    void placeShips() override;
    void reportGameover(bool winner) override;
};


#endif //SFML_TEMPLATE_INTELLIGENTCOMPUTER_H
