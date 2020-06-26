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

using namespace std;

class SpeechManager {
public:

    SpeechManager();

    void showMenu();

    ~SpeechManager();

};

#endif