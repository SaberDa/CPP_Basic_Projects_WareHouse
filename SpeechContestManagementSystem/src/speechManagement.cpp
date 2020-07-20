#include <algorithm>
#include <random>
#include <deque>
#include <numeric>
#include <string>
#include <fstream>

using namespace std;

#include "speechManagement.h"

SpeechManager::SpeechManager() {

    this->initSpeech();

    // Add speakers
    this->createSpeaker();

    // Get previous results
    this->loadRecord();

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
    this->m_Record.clear();
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

void SpeechManager::startSpeech() {

    // The first round
    // Draw
    speechDraw();
    // Start contest
    speechContest();
    // Show the results
    showScore();

    // The second round
    this->m_Index++;
    // Draw
    speechDraw();
    // Start contest
    speechContest();
    // show the final results
    showScore();
    // Store scores
    saveRecord();

    // Reset contest
    this->initSpeech();
    this->createSpeaker();
    this->loadRecord();

    cout << "The speech contest is finished" << endl;
}

void SpeechManager::speechDraw() {
    cout << "The " << this->m_Index << "round starts drawing" << endl;
    cout << "=========================" << endl;
    cout << "Here are the results" << endl;

    if (this->m_Index == 1) {
        random_device rd;
        mt19937 g(rd());
        shuffle(vStudents.begin(), vStudents.end(), g);
        for (auto it : vStudents) cout << it << endl;
        cout << endl;
    } else {
        random_device rd;
        mt19937 g(rd());
        shuffle(vFirstRoundWinner.begin(), vFirstRoundWinner.end(), g);
        for (auto it : vFirstRoundWinner) cout << it << endl;
        cout << endl;
    }
    cout << "=========================" << endl;
    cout << endl;
}

void SpeechManager::speechContest() {
    cout << "-------------- The " << this->m_Index << "round will start -------------------" << endl;
    multimap<double, int, greater<int>> groupScores;        // To store key:score value:ID
    int num = 0;        // To record the number of people, 6 person each group

    vector<int> v_Src;      // To store the contest people
    if (this->m_Index == 1) 
        v_Src = vStudents;
    else 
        v_Src = vFirstRoundWinner; 

    for (auto it : v_Src) {
        num++;

        deque<double> d;
        for (int i = 0; i < 10; i++) {
            double score = (rand() % 401 + 600) / 10.f;     // The range of score is 600 ~ 1000
            // cout << "The score is: " << score << endl;
            d.push_back(score);
        }

        sort(d.begin(), d.end(), greater<double>());
        d.pop_front();
        d.pop_back();

        double sum = accumulate(d.begin(), d.end(), 0);
        double avg = sum / (double)d.size();

        this->m_Speaker[it].m_Score[this->m_Index - 1] = avg;

        groupScores.insert(make_pair(avg, it));

        if (num % 6 == 0) {
            cout << "The " << num / 6 << " group results: " << endl;
            // for (auto x : groupScores) 
            //     cout << "The ID is: " << x.second << ", name is: " 
            //          << this->m_Speaker[x.second].m_Name <<
            //     ", score is: " << this->m_Speaker[x.second].m_Score[this->m_Index - 1] << endl;
            int count = 3;
            for (auto x : groupScores) {
                if (this->m_Index == 1) {
                    vFirstRoundWinner.push_back(x.second);
                } else {
                    vVictory.push_back(x.second);
                }
            }
            groupScores.clear();
            cout << endl;
        }
    }
    cout << "-------------- The " << this->m_Index << "round finished -------------------" << endl;
}

void SpeechManager::showScore() {
    vector<int> v;

    if (this->m_Index == 1) {
        v = vFirstRoundWinner;
    } else {
        v = vVictory;
    }

    for (vector<int>::iterator it  = v.begin(); it != v.end(); it++) {
        std::cout << *it << " " << m_Speaker[*it].m_Name << " " << m_Speaker[*it].m_Score << std::endl;
    }

    this->showMenu();
}

void SpeechManager::saveRecord() {
    ofstream ofs;

    /* Use output to open and write file */
    ofs.open("speech.csv", ios::out | ios::app);

    for (vector<int>::iterator it = vVictory.begin(); it != vVictory.end(); it++) {
        ofs << *it << "," << m_Speaker[*it].m_Score[1] << ",";
    }
    
    ofs << endl;

    ofs.close();

    cout << "Results are stored" << endl;

    this->fileIsEmpty = false;
}

void SpeechManager::loadRecord() {
    ifstream ifs("speech.csv", ios::in);

    if (!ifs.is_open()) {
        this->fileIsEmpty = true;
        cout << "Cannot find the file" << endl;
        ifs.close();
        return;
    }

    char ch;
    ifs >> ch;
    if (ifs.eof()) {
        cout << "The file is empty" << endl;
        this->fileIsEmpty = true;
        ifs.close();
        return;
    }

    this->fileIsEmpty = false;

    ifs.putback(ch);

    string data;
    int index = 0;
    while (ifs >> data) {
        cout << data << endl;
        vector<string> s;
        int pos = -1, start = 0;

        while (true) {
            pos = data.find(",", start);
            if (pos == -1) break;
            string temp = data.substr(start, pos - start);
            s.push_back(temp);
            start = pos + 1;
        }

        this->m_Record.insert({index, s});
        index++;
    }

    ifs.close();
}

void SpeechManager::showRecord() {
    if (this->fileIsEmpty) {
        cout << "The file doesn't exist" << endl;
        return;
    } 
    for (int i = 0; i < this->m_Record.size(); i++) {
        cout << "Results:" << endl;
    }
}

void SpeechManager::clearRecord() {
    cout << "Are you going to clear all results?" << endl;
    cout << "Y/n" << endl;

    char select;
    cin >> select;

    if (select == 'Y') {
        ofstream ofs("speech.csv", ios::trunc);
        ofs.close();
        this->initSpeech();
        this->createSpeaker();
        this->loadRecord();
        cout << "All clear" << endl;
    } else {
        return;
    }
}

SpeechManager::~SpeechManager() {
    
}