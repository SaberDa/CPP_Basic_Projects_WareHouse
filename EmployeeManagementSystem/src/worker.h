/*
 * Worker Class:
 * 
 * Three types of all employees:
 *      1. Normal employee
 *      2. Manager
 *      3. Boss
 * 
 * Employees' properties
 *      1. ID
 *      2. Name
 *      3. Department ID
 * 
 * Employees' actions:
 *      1. Job responsibility information description
 *      2. Get job title
 * 
*/

#ifndef __WORKER_H__
#define __WORKER_H__

#include <iostream>
#include <string>

using namespace std;

class Worker {
public:

    // Show personal information
    virtual void showInfo() = 0;

    // Get job title
    virtual string getDeptName() = 0;

    int m_Id;           // Employee number
    string m_Name;      // Name of employee
    int m_DeptId;       // Name of the department 

};

#endif