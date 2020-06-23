#include <iostream>

using namespace std;

#include "workerManager.h"
#include "worker.h"
#include "employee.h"
#include "manager.h"
#include "boss.h"

int main() {

	/* ================ Test Polymorphism ================*/ 
	Worker *worker = NULL;

	// Test for Employee
	worker = new Employee(1, "Bob", 1);
	worker->showInfo();
	delete worker;

	// Test for Manager
	worker = new Manager(2, "Alice", 2);
	worker->showInfo();
	delete worker;

	// Test for Boss
	worker = new Boss(3, "Kate", 3);
	worker->showInfo();
	delete worker;

	WorkerManager wm;
	int choice = 0;
	while (true)
	{
		
		wm.showMenu();
		cout << "INPUT YOUR CHOICE:" << endl;
		cin >> choice;

		switch (choice)
		{
		case 0: 
			wm.ExitSystem();
			break;
		case 1: 
			wm.addEmp();
			break;
		case 2:
			wm.showEmp();
			break;
		case 3: 
			wm.delEmp();
			break;
		case 4: 
			wm.modEmp();
			break;
		case 5: 
			wm.findEmp();
			break;
		case 6: 
			wm.sortEmp();
			break;
		case 7: 
			wm.cleanFile();
			break;
		default:
			break;
		}
	}

	system("pause");
	return 0;
}