/* Implement Manager Class */

#include "manager.h"

Manager::Manager(int id, string name, int deptId) {
    this->m_Id = id;
    this->m_Name = name;
    this->m_DeptId = deptId;
}

void Manager::showInfo() {
	cout << "Employee ID" << this->m_Id
		<< " \tEmployee Name " << this->m_Name
		<< " \tEmployee Job" << this->getDeptName() << endl;
}

string Manager::getDeptName() {
    return string("manager");
}