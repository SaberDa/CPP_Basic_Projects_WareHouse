#include "speechManagement.h"

SpeechManager::SpeechManager() {

}

void SpeechManager::showMenu() {
    cout << "**************************************" << endl;
    cout << "****** Speech Contest Starts ! *******" << endl;
    cout << "************ 1. Start ****************" << endl;
    cout << "************ 2. View History *********" << endl;
    cout << "************ 3. Clear History ********" << endl;
    cout << "************ 0. Exit *****************" << endl;
    cout << "**************************************" << endl;
    cout << endl;
}

void SpeechManager::exitSystem() {
    cout << "EXIT SYSTEM" << endl;
    exit(0);
}

SpeechManager::~SpeechManager() {
    
}