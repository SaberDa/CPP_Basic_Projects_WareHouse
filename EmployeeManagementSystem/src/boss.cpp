/* Implement Boss Class */

#include "boss.h"

Boss::Boss(int id, string name, int deptId) {
    this->m_Id = id;
    this->m_Name = name;
    this->m_DeptId = deptId;
}

void Boss::showInfo() {
	cout << "Employee ID" << this->m_Id
		<< " \tEmployee Name " << this->m_Name
		<< " \tEmployee Job" << this->getDeptName() << endl;
}

string Boss::getDeptName() {
    return string("boss");
}