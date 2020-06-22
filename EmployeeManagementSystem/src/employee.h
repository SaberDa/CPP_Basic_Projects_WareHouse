/*
 * Employee Class: 
 * 
 * Ordinary employee class inherits employee abstract class and 
 * rewrites virtual function in parent class (Worker class).
*/

#ifndef __EMPLOYEE_H__
#define __EMPLOYEE_H__

#include <iostream>

using namespace std;

#include "worker.h"

class Employee : public Worker {
public:

    Employee(int id, string name, int deptId);

    // Show personal information
    virtual void showInfo();

    // Get job title
    virtual string getDeptName();
};

#endif