/*
 * Manager Class
 * 
 * Main Functions:
 * 1. Establish a communication menu interface based on command line with
 *    users.
 * 2. Realize the operation of adding, deleting, modifying and checking 
 *    employees.
 * 3. Realize reading and writing interaction with files
*/

#ifndef __WORKERMANAGER_H__
#define __WORKERMANAGER_H__

#include <iostream>
#include <fstream>

using namespace std;

#include "worker.h"
#include "employee.h"
#include "manager.h"
#include "boss.h"

#define FILENAME "empFile.txt"

class WorkerManager {
public:

    WorkerManager();

    // Show system menu
    void showMenu();

    // Exit the system
    void ExitSystem();

    // Count the number of employees in file
    int m_EmpNum;

    // The pointer to employee array
    Worker **m_EmpArray;

    // Add employee
    void addEmp();

    // Save file
    void saveFile();

    // Judge if the file is empty
    bool m_FileIsEmpty;

    // Get employee number
    int getEmpNum();

    // Init employee
    void initEmp();

    // Show employee information
    void showEmp();

    // Delete employee
    void delEmp();

    // Judge employee
    int isExist(int id);

    // Modify employee
    void modEmp();

    // Find employee
    void findEmp();

    // Sort employee
    void sortEmp();

    // Clean the file
    void cleanFile();

    ~WorkerManager();
};

#endif