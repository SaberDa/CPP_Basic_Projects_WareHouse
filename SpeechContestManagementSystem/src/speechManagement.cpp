#include "speechManagement.h"

SpeechManager::SpeechManager() {

    this->initSpeech();

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

void SpeechManager::initSpeech() {
    
    this->vStudents.clear();
    this->vFirstRoundWinner.clear();
    this->vVictory.clear();
    this->m_Speaker.clear();

    this->m_Index = 1;

}

SpeechManager::~SpeechManager() {
    
}