# Employee Management System
---

A small project for managing employees. 

---

## Main Classes

---


### Manager Class

#### Main Functions:

- Establish a communication menu interface based on command line with users.
- Realize the operation of adding, deleting, modifying and checking employees.
- Realize reading and writing interaction with files.

---

### Worker Class

#### Three types of all employees

- Normal employee
- Manager
- Boss

#### Employees' properties

- ID
- Name
- Department ID

#### Employees' actions

- Job responsibility information description
- Get job title

---

### Employee Class

Ordinary employee class inherits employee abstract class and rewrites virtual function in parent class (Worker class).

---

### Manager Class

Ordinary manager class inherits employee abstract class and rewrites virtual function in parent class (Worker class).

---

### Boss Class

Ordinary boss class inherits employee abstract class and rewrites virtual function in parent class (Worker class).

---

## Main Functions

---

### Add staff

- When users create in batches, they may create different kinds of employees
- If you want to put all different kinds of employees into an array, you can maintain the pointers of all employees into an array
- If you want to maintain this array of indefinite length in the program, you can create the array to the heap area and use the pointer maintenance of Worker**

---

### File interaction

- Read and write files
- Read the contents of the file into the program

#### There are three cases where the constructor initializes data

- For the first use, the file is not created
- The file exists, but the data is cleared by the user
- The file exists and saves all the data of the employees

---

### Show employees

- Display all current employee information

---

### Delete employee

- Delete employee according to employee number

---

### Modify employee

- Able to modify and save employee information according to employee number

---

### Find employees

#### Provide two ways to find employees

- By employee number
- By employee name

---

### Sort

- Sort by employee number, the order of sorting is specified by the user

---

### Clear files

- Clear the recorded data in the file