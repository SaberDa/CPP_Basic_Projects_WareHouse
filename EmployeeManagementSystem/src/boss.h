/*
 * Boss Class
 * 
 * Ordinary boss class inherits employee abstract class and 
 * rewrites virtual function in parent class (Worker class).
*/

#ifndef __BOSS_H__
#define __BOSS_H__

#include <iostream>

using namespace std;

#include "worker.h"

class Boss : public Worker {
public:

    Boss(int id, string name, int deptId);

    // Show personal information
    virtual void showInfo();

    // Get job title
    virtual string getDeptName();

};

#endif