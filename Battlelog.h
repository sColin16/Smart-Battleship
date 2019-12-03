/* Battlelog.h
 *
 * Author: Colin Siles
 *
 * The Battlelog class writes the moves made by the players to a file for review later
*/

#ifndef SFML_TEMPLATE_BATTLELOG_H
#define SFML_TEMPLATE_BATTLELOG_H

#include <string>
#include <fstream>

#include "Board.h"

using namespace std;

class Battlelog {
public:
    Battlelog(string filename, string p1Name, string p2Name);
    ~Battlelog(); // Destructor, for closing the file that is written to

    // Public member functions to record both moves and the winner
    void recordMove(int xPos, int yPos, ShotOutcome outcome);
    void recordWinner(string name);

private:
    // File object
    ofstream _battlelogFile;

    // Tracks which player the battlelog will write the move for next
    bool _firstPlayer;

    // Helper functions for writing to the file
    void _writeSeparator();
    void _writeName(string name);
    void _writeMove(int xPos, int yPos, ShotOutcome outcome);

    // Helper functions for getting strings to be written to the file
    static char _convertYPos(int yPos);
    static int _convertXPos(int xPos);
    static string _getOutcomeString(ShotOutcome outcome);


};

#endif //SFML_TEMPLATE_BATTLELOG_H
