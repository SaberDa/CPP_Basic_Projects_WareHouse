#include "workerManager.h"

WorkerManager::WorkerManager() {

    // Init the number
    this->m_EmpNum = 0;

    // Init the pointer
    this->m_EmpArray = NULL;

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

void WorkerManager::ExitSystem() {
    cout << "-------------- EXIT SYSTEM -------------------" << endl;
    exit(0);
}

// Add new employees
void WorkerManager::addEmp() {
    cout << "Please input the number of added employees: " << endl;
    int addNum = 0;
    cin >> addNum;

    if (addNum > 0) {

        // Calculate the new space size
        int newSize = this->m_EmpNum + addNum;

        // Create the new space
        Worker **newSpace = new Worker*[newSize];

        if (this->m_EmpArray != NULL) {
            for (int i = 0; i < this->m_EmpNum; i++) {
                newSpace[i] = this->m_EmpArray[i];
            }
        }

        // Input new data
        for (int i = 0; i < addNum; i++) {
            int id;
            string name;
            int dSelect;

            cout << "Please input the " << i + 1 << "th employee iD: " << endl;
            cin >> id;
            cout << "Please input the " << i + 1 << "th employee name: " <<endl;
            cin >> name;

            cout << "Please input the new employee title" << endl;
            cout << "1. Normal Employee" << endl;
            cout << "2. Manager" << endl;
            cout << "3. Boss" << endl;
            cin >> dSelect;

            Worker *worker = NULL;
            switch (dSelect) {
                case 1: 
                    // Normal Employee
                    worker = new Employee(id, name, 1);
                    break;
                case 2:
                    // Manager
                    worker = new Manager(id, name, 2);
                    break;
                case 3:
                    // Boss
                    worker = new Boss(id, name, 3);
                    break;
                default: break;
            }

            newSpace[this->m_EmpNum + i] = worker;
        }

        // Release origin array
        delete[] this->m_EmpArray;

        // Renew the properties 
        this->m_EmpArray = newSpace;
        this->m_EmpNum = newSize;

        cout << "Add Successfully" << endl;

        this->saveFile();
    } else {
        cout << "Please input an integer which larger than 0" << endl;
    }
}

void WorkerManager::saveFile() {
    ofstream ofs;
    ofs.open(FILENAME, ios::out);

    // Write contents into file
    for (int i = 0; i < this->m_EmpNum; i++) {
        ofs << this->m_EmpArray[i]->m_Id << " "
            << this->m_EmpArray[i]->m_Name << " "
            << this->m_EmpArray[i]->m_DeptId << endl;
    }

    ofs.close();
}

WorkerManager::~WorkerManager() {
    if (this->m_EmpArray != NULL) delete[] this->m_EmpArray;
}