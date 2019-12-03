//
// Created by Colin Siles on 2019-12-02.
//

#ifndef SFML_TEMPLATE_BATTLELOG_H
#define SFML_TEMPLATE_BATTLELOG_H

#include <string>
#include <fstream>

#include "Board.h"

using namespace std;

class Battlelog {
public:
    Battlelog(string filename, string p1Name, string p2Name);
    ~Battlelog();

    void recordMove(string name, int xPos, int yPos, ShotOutcome outcome);
    void recordWinner(string name);

private:
    ofstream _battlelogFile;

    bool _firstPlayer;

    static char _convertYPos(int yPos);
    static int _convertXPos(int xPos);
    static string _getOutcomeString(ShotOutcome outcome);

    void _writeSeparator();
    void _writeName(string name);
    void _writeMove(int xPos, int yPos, ShotOutcome outcome);
};

#endif //SFML_TEMPLATE_BATTLELOG_H
