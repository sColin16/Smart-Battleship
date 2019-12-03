/* Battlelog.cpp
 *
 * Author: Colin Siles
 *
 * The Battlelog class writes the moves made by the players to a file for review later
*/

#include <iostream>
#include <iomanip>

#include "Battlelog.h"

Battlelog::Battlelog(string filename, string p1Name, string p2Name) {
    // Record the first player first
    _firstPlayer = true;

    // Attempt to open the battlelog file
    _battlelogFile.open(filename);
    if(!_battlelogFile) {
        cerr << "Failed to open the battlelog" << endl;
        return;
    }

    // Write the file header
    _writeSeparator();
    _writeName(p1Name);
    _writeName(p2Name);

    _battlelogFile << "|" << endl;
    _writeSeparator();
}

// Destructor to close the file
Battlelog::~Battlelog() {
    _battlelogFile.close();
}

// Writes a move to the file, based on the shot made, and its outcome
void Battlelog::recordMove(int xPos, int yPos, ShotOutcome outcome) {
    // Draw the line in front of the row if the first player
    if(_firstPlayer) {
        _battlelogFile << "|";
    }

    // Write the move to the file
    _writeMove(xPos, yPos, outcome);

    // End with a new line of the second player's move
    if(!_firstPlayer) {
        _battlelogFile << endl;
    }

    // Switch players to record the next player when the function is called again
    _firstPlayer = !_firstPlayer;
}

// Writes the winner of the game to the file
void Battlelog::recordWinner(string name) {
    // If the first player just won (since the player was toggled when writing the move)
    // Create a blank spot for the second player to complete the table
    if(!_firstPlayer) {
        _battlelogFile << setw(12) << setfill(' ') << "" << "|" << endl;
    }

    // Write the winner to the file
    _writeSeparator();
    _battlelogFile << endl;
    _battlelogFile << name << " wins!" << endl;
}

// Converts the horizontal position of a move made by a player to the corresponding character
char Battlelog::_convertYPos(int yPos) {
    return yPos + 'A';
}

// Converts the vertical position of a move made by a player to the corresponding integer
int Battlelog::_convertXPos(int xPos) {
    return xPos + 1;
}

// Returns a string (HIT, SUNK, or MISS) based on an outcome object
string Battlelog::_getOutcomeString(ShotOutcome outcome) {
    if(outcome.sunkenIndex >=0 ) {
        return "SUNK";
    }

    return outcome.hit ? "HIT" : "MISS";
}

// Write the Equal sign separator for the class
void Battlelog::_writeSeparator() {
    _battlelogFile << "|" << setw(25) << setfill('=') << "" << "|" << endl;
}

// Writes a name to the file, with proper padding
void Battlelog::_writeName(string name) {
    _battlelogFile << "| " << setw(10) << setfill(' ') << left << name << " ";
}

// Writes a move and its corresponding outcome tp the file, using other helper functions
void Battlelog::_writeMove(int xPos, int yPos, ShotOutcome outcome) {
    _battlelogFile << " " << setw(5) << setfill(' ') << left << (_convertYPos(yPos) + to_string(_convertXPos(xPos)));
    _battlelogFile << " " << setw(4) << setfill(' ') << right << _getOutcomeString(outcome) << " |";
}