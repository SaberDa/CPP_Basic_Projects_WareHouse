#include "workerManager.h"

WorkerManager::WorkerManager() {

}

// Print menu in command line
void WorkerManager::showMenu() {
    cout << "*********************************************" << endl;
    cout << "* Welcome to use employee management system *" << endl;
    cout << "******** 0. Exit the system *****************" << endl;
    cout << "******** 1. Add a new Employee **************" << endl;
    cout << "******** 2. Show an employee information ****" << endl;
    cout << "******** 3. Delete an employee **************" << endl;
    cout << "******** 4. Change an employee's information " << endl;
    cout << "******** 5. Query an employee's information *" << endl;
    cout << "******** 6. Sort all employees by ID ********" << endl;
    cout << "******** 7. Clear all documents *************" << endl;
    cout << "*********************************************" << endl;
    cout << endl;
}

WorkerManager::~WorkerManager() {

}