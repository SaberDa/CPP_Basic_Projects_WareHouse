/*
 * Manager Class:
 * 
 * Ordinary manager class inherits employee abstract class and 
 * rewrites virtual function in parent class (Worker class).
*/

#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <iostream>

using namespace std;

#include "worker.h"

class Manager : public Worker {
public:

    Manager(int id, string name, int deptId);

    // Show personal information
    virtual void showInfo();

    // Get job title
    virtual string getDeptName();
};

#endif