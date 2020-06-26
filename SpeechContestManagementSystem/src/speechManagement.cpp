#include "speechManagement.h"

SpeechManager::SpeechManager() {

    this->initSpeech();

    // Add speakers
    this->createSpeaker();

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

void SpeechManager::createSpeaker() {
    string nameSeed = "ABCDEFGHIJKL";
    for (int i = 0; i < nameSeed.size(); i++) {
        string name = "Student ";
        name += nameSeed[i];

        // init speaker's properties
        Speaker sp;
        sp.m_Name = name;
        for (int i = 0; i < 2; i++) sp.m_Score[i] = 0;

        this->vStudents.push_back(i + 10001);

        this->m_Speaker.insert(make_pair(i + 10001, sp));
    }
}

SpeechManager::~SpeechManager() {
    
}