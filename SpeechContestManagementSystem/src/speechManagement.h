/*
 * Speech Management Class
 * 
 * Main Function:
 * 1. Establish a communication menu interface based on command line with
 *    users.
 * 2. Control the speech contest process
 * 3. Read and write interactions with files
*/

#ifndef __SPEECHMANAGEMENT_H__
#define __SPEECHMANAGEMENT_H__

#include <iostream>
#include <vector>
#include <map>

using namespace std;

#include "speaker.h"

class SpeechManager {
public:

    // Contest students, size : 12
    vector<int> vStudents;

    // The first round winner, size : 6
    vector<int> vFirstRoundWinner;

    // Contest winner, size : 3
    vector<int> vVictory;

    // To store students' ID
    map<int, Speaker> m_Speaker;

    // Contest Round
    int m_Index;

    // The flag to judge file is empty or not
    bool fileIsEmpty;

    // Previous record
    map<int, vector<string>> m_Record;

    SpeechManager();

    void showMenu();

    // Exit system
    void exitSystem();

    // Init property
    void initSpeech();

    // Init students
    void createSpeaker();

    // Start speech
    void startSpeech();

    // Draw
    void speechDraw();

    // Contest
    void speechContest();

    // Show results
    void showScore();

    // Store results
    void saveRecord();

    // Read results
    void loadRecord();

    // Show previous results
    void showRecord();

    ~SpeechManager();

};

#endif