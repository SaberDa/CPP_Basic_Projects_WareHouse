#include "workerManager.h"

WorkerManager::WorkerManager() {

    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    // The case that there is no file
    if (!ifs.is_open()) {
        cout << "There is no file named " << FILENAME << endl;
        this->m_EmpNum = 0;             // Init number
        this->m_FileIsEmpty = true;     // Init file flag
        this->m_EmpArray = NULL;        // Init array
        ifs.close();                    // Close the file
        return;
    }

    // The case that file exists
    char ch;
    ifs >> ch;
    if (ifs.eof()) {
        cout << "The file is empty!" << endl;
        this->m_EmpNum = 0;
        this->m_FileIsEmpty = true;
        this->m_EmpArray = NULL;
        ifs.close();
        return;
    }

    // Renew the number
    int num = getEmpNum();
    // cout << "The current employee number is " << num << endl;
    this->m_EmpNum = num;

    // Init the employee array based on employee number
    this->m_EmpArray = new Worker *[this->m_EmpNum];
    initEmp();

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
        this->m_FileIsEmpty = false;

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

int WorkerManager::getEmpNum() {
    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    int id, dId;
    string name;
    int num = 0;

    // Count employee number
    while (ifs >> id && ifs >> name && ifs >> dId) {
        num++;
    }

    ifs.close();
    return num;
}

void WorkerManager::initEmp() {
    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    int id, dId;
    string name;

    int index = 0;
    while (ifs >> id && ifs >> name && ifs >>dId) {
        Worker *worker = NULL;
        if(dId == 1) {
            worker = new Employee(id, name, dId);
        } else if (dId == 2) {
            worker = new Manager(id, name, dId);
        } else if (dId == 3) {
            worker = new Boss(id, name, dId);
        }
        this->m_EmpArray[index] = worker;
        index++;
        // delete[] worker;
    }
}

// Show employee information
void WorkerManager::showEmp() {
    if (this->m_FileIsEmpty) {
        cout << "The file is not exist or it is empty!" << endl;
    } else {
        for (int i = 0; i < m_EmpNum; i++) {
            this->m_EmpArray[i]->showInfo();
        }
    }
}

// Judge if this employee exists by its ID
// If exists, returns its index
// Or returns -1
int WorkerManager::isExist(int id) {
    int index = -1;

    for (int i = 0; i < this->m_EmpNum; i++) {
        if (this->m_EmpArray[i]->m_Id == id) {
            index = i;
            break;
        }
    }

    return index;
}

// Delete employee
void WorkerManager::delEmp() {
    if (this->m_FileIsEmpty) {
        cout << "The file is not exist or it is empty!" << endl;
    } else {
        // Input the ID
        cout << "Please input the employee ID you want to delete: " << endl;
        int id = 0;
        cin >> id;

        // Judge if the employee exists
        int index = this->isExist(id);
        if (index != -1) {
            for (int i = index; i < this->m_EmpNum; i++) {
                this->m_EmpArray[i] = this->m_EmpArray[i + 1];
            }
            // Renew the num
            this->m_EmpNum--;

            // Renew the data and save file
            this->saveFile();

            cout << "Delete successfully" << endl;
        } else {
            cout << "The employee is not exist" << endl;
        }
    }
}

// Modify employee
void WorkerManager::modEmp() {
    if (this->m_FileIsEmpty) {
        cout << "The file is not exist or it is empty!" << endl;
        return;
    } 

    // Input the ID
    cout << "Please input the employee ID you want to modify" << endl;
    int id;
    cin >> id;

    // Judge if the employee exists
    int index = this->isExist(id);
    if (index == -1) {
        cout << "The employee is not exist" << endl;
        return;
    }

    // Delete the old information
    delete this->m_EmpArray[index];

    // Input the new information
    int newId = 0, dSelect = 0;
    string newName = "";

    cout << "The origin ID is " << id << ", please input the new ID: " << endl;
    cin >> newId;
    cout << "Please input the new name:" << endl;
    cin >> newName;
    cout << "Please input the job title: "  
         << "1. Normal employee" 
         << "2. Manager" 
         << "3. Boss" << endl;
    cin >> dSelect;

    Worker *worker = NULL;
    switch (dSelect) {
        case 1:
            worker = new Employee(newId, newName, dSelect);
            break;
        case 2:
            worker = new Manager(newId, newName, dSelect);
            break;
        case 3:
            worker = new Boss(newId, newName, dSelect);
            break;
        default: break;
    }

    // Add the new information into its origin index in employee array
    this->m_EmpArray[index] = worker;
    cout << "Modify successfully" << endl;

    // Renew the data and save file 
    this->saveFile();
}

// Find employee
void WorkerManager::findEmp() {
    if (this->m_FileIsEmpty) {
        cout << "The file is not exist or it is empty!" << endl;
        return;
    } 

    cout << "Please input the number of finding method:"
         << "1. By Id"
         << "2. By name" << endl;
    int select = 0;
    cin >> select;

    if (select == 1) {
        // By Id
        int id;
        cout << "Please input the ID: " << endl;
        cin >> id;

        int index = this->isExist(id);
        if (index == -1) {
            cout << "The employee is not exist" << endl;
            return;
        } else {
            cout << "The employee you choose is " << endl;
            this->m_EmpArray[index]->showInfo();
        }
    } else if (select == 2) {
        // By name
        cout << "Please input the name: " << endl;
        string name;
        cin >> name;

        bool flag = false;
        for (int i = 0; i < m_EmpNum; i++) {
            if (m_EmpArray[i]->m_Name == name) {
                cout << "The employee you choose is " << endl;
                this->m_EmpArray[i]->showInfo();
                flag = true;
            }
        }

        if (flag == false) {
            cout << "The employee is not exist" << endl;
        }
    } else {
        cout << "Please input the correct number!" << endl;
    }
}

// Sort employee
void WorkerManager::sortEmp() {
    if (this->m_FileIsEmpty) {
        cout << "The file is not exist or it is empty!" << endl;
        return;
    } 

    cout << "Please choose a sorting method"
         << "1. Ascending order by employee number"
         << "2. Descending order by employee number" << endl;
    int select = 0;
    cin >> select;

    if (!(select == 1 || select == 2)) {
        cout << "Please input the right method number!" << endl;
        return;
    }

    for (int i = 0; i < m_EmpNum; i++) {
        int minOrMax = i;
        for (int j = i + 1; j < m_EmpNum; j++) {
            if (select == 1) {
                // Ascending
                if (m_EmpArray[minOrMax]->m_Id > m_EmpArray[j]->m_Id) 
                    minOrMax = j;
            } else if (select == 2) {
                // Descending
                if (m_EmpArray[minOrMax]->m_Id < m_EmpArray[j]->m_Id)
                    minOrMax = j;
            }
        }
        if (i != minOrMax) {
            Worker *temp = m_EmpArray[i];
            m_EmpArray[i] = m_EmpArray[minOrMax];
            m_EmpArray[minOrMax] = temp;
        }
    }

    cout << "Sort successfully" << endl;

    // Renew the data and save file 
    this->saveFile();
    // Show the results after sorting
    this->showEmp();
}

// Clean the file
void WorkerManager::cleanFile() {
    cout << "You are going to clear the file content, are you ready? "
         << "1. YES"
         << "2. NO" << endl;
    
    int select = 0;
    cin >> select;

    if (!(select == 1 || select == 2)) {
        cout << "Please input the right number!" << endl;
        return;
    }

    if (select == 2) return;

    // Use the ios::trunc to create a new file with the same name to replace the origin file
    ofstream ofs(FILENAME, ios::trunc);
    ofs.close();

    if (this->m_EmpArray != NULL) {
        for (int i = 0; i < m_EmpNum; i++) {
            if (this->m_EmpArray[i] != NULL) delete this->m_EmpArray[i];
        }
        this->m_EmpNum = 0;
        delete[] this->m_EmpArray;
        this->m_EmpArray = NULL;
        this->m_FileIsEmpty = true;
    }

    cout << "Clear successfully" << endl;
}   

WorkerManager::~WorkerManager() {
    if (this->m_EmpArray != NULL) delete[] this->m_EmpArray;
}