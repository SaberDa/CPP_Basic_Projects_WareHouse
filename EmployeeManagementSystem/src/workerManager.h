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

using namespace std;

#include "worker.h"
#include "employee.h"
#include "manager.h"
#include "boss.h"

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

    ~WorkerManager();
};

#endif