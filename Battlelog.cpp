//
// Created by Colin Siles on 2019-12-02.
//

#include <iostream>
#include <iomanip>

#include "Battlelog.h"

Battlelog::Battlelog(string filename, string p1Name, string p2Name) {
    _battlelogFile.open(filename);

    _firstPlayer = true;

    if(!_battlelogFile) {
        cerr << "Failed to open the battlelog" << endl;
        return;
    }

    _writeSeparator();
    _writeName(p1Name);
    _writeName(p2Name);

    _battlelogFile << "|" << endl;

    _writeSeparator();
}

Battlelog::~Battlelog() {
    _battlelogFile.close();
}

void Battlelog::recordMove(string name, int xPos, int yPos, ShotOutcome outcome) {
    if(_firstPlayer) {
        _battlelogFile << "|";
    }

    _writeMove(xPos, yPos, outcome);

    if(!_firstPlayer) {
        _battlelogFile << endl;
    }

    _firstPlayer = !_firstPlayer;
}

void Battlelog::recordWinner(string name) {
    if(!_firstPlayer) { // This really means if the first player just won
        _battlelogFile << setw(12) << setfill(' ') << "" << "|" << endl;
    }

    _writeSeparator();
    _battlelogFile << endl;
    _battlelogFile << name << " wins!" << endl;
}

char Battlelog::_convertYPos(int yPos) {
    return yPos + 'A';
}

int Battlelog::_convertXPos(int xPos) {
    return xPos + 1;
}

string Battlelog::_getOutcomeString(ShotOutcome outcome) {
    if(outcome.sunkenIndex >=0 ) {
        return "SUNK";
    }

    return outcome.hit ? "HIT" : "MISS";
}

void Battlelog::_writeSeparator() {
    _battlelogFile << "|" << setw(25) << setfill('=') << "" << "|" << endl;
}

void Battlelog::_writeName(string name) {
    _battlelogFile << "| " << setw(10) << setfill(' ') << left << name << " ";
}

void Battlelog::_writeMove(int xPos, int yPos, ShotOutcome outcome) {
    _battlelogFile << " " << setw(5) << setfill(' ') << left << (_convertYPos(yPos) + to_string(_convertXPos(xPos)));
    _battlelogFile << " " << setw(4) << setfill(' ') << right << _getOutcomeString(outcome) << " |";
}