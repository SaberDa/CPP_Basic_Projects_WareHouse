#include <iostream>

using namespace std;

#include "speechManagement.h"

int main() {
    
    srand((unsigned int)time(NULL));

    SpeechManager sm;

    int choice = 0;

    while (true) {
        sm.showMenu();

        cout << "Please input your choice" << endl;
        cin >> choice;

        switch (choice) {
            case 1:
                sm.startSpeech();
                break;
            case 2:
                sm.showRecord();
                break;
            case 3:
                break;
            case 0:
                sm.exitSystem();
                break;
            default: break;
        }
    }
    
    return 0;
}