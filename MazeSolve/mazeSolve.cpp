#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <ctime>
#include <termios.h>

using namespace std;

#define MAX_X 20
#define MAX_Y 30

bool flag = false;			// To create map, 'true' means create successfully
bool slow = false;			// To change game speed, 'true' means low speed
bool autogame = true;		// To change game mode, 'true' means paly automaticlly
int maze[MAX_X][MAX_Y];

// Route stack, to record the moving route
class stack_of_maze {
private:
	// Record maze position
	struct node {
		int x, y;
		char direction;
		node* next;
	};

	node* head;
public:
	
	stack_of_maze() {
		head = NULL;
	}

	~stack_of_maze() {
		node* p = head;
		while (head != NULL) {
			head = head->next;
			delete p;
			p = head;
		}
	}

	// Set a new node to push into stack
	// First record the information into new node, 
	// then put the new node on the top of stack
	// The function is add new information about moving
	void push(int xx, int yy, char ddirection) {
		node* new_node = new node;

		if (new_node != NULL) {
			new_node->x = xx;
			new_node->y = yy;
			new_node->direction = ddirection;
			new_node->next = NULL;

			if (head == NULL) head = new_node;
			else {
				new_node->next = head;
				head = new_node;
			}
		} else cout << "Allocate memory fail" << endl; 
	}

	// Pop the stack
	// To judge if the route is right
	// If the next node is NULL, which means the current route is wrong
	node* pop(int &xx, int &yy) {
		if (head != NULL) {
			node* p = head;
			head = head->next;
			xx = p->x;
			yy = p->y;
			delete p;
		}
		return head;
	}

	// Print route information
	void print() {
		if (head != NULL) {
			node* p = head;
			while (p != NULL) {
				cout << " " << p->x << " " << p->y << " " << p->direction << endl;
				p = p->next;
			}
		} else cout << "The stack is empty" << endl;
	}


	// ===============================================================================
	// Create the maze map
	void createMaze() {
		int maxway = MAX_X * MAX_Y;
		int x, y;

		// First fill the maze
		for (x = 0; x < MAX_X; x++)
			for (y = 0; y < MAX_Y; y++)
				maze[x][y] = 1;

		srand((unsigned)time(NULL));

		for (int i = 0; i < maxway; i++) {
			x = rand() % (MAX_X - 2) + 1;
			y = rand() % (MAX_Y - 2) + 1;
			maze[x][y] = 0;
		}

		maze[1][1] = 0;						// entry
		maze[MAX_X - 2][MAX_Y - 2] = 0;		// exit

		maze[0][1] = 3;
		maze[MAX_X - 1][MAX_Y - 2] = 0;
	}

	// Print maze map
	void printMaze() {
		int x, y;
		system("clear");

		for (x = 0; x < MAX_X; x++) {
			for (y = 0; y < MAX_Y; y++) {
				if (maze[x][y] == 0) { cout << "  "; continue; }
				if (maze[x][y] == 1) { cout << "■ "; continue; }
				if (maze[x][y] == 2) { cout << "x "; continue; }
				if (maze[x][y] == 3) { cout << "↓ "; continue; }        
				if (maze[x][y] == 4) { cout << "→ "; continue; }
				if (maze[x][y] == 5) { cout << "← "; continue; }
				if (maze[x][y] == 6) { cout << "↑ "; continue; }
				if (maze[x][y] == 7) { cout << "※ "; continue; }
			}
			cout << endl;
		}
		
		if (slow) sleep(1);
	}

	void check(stack_of_maze &s) {
		int temp[MAX_X][MAX_Y];
		for (int x = 0; x < MAX_X; x++) 
			for (int y = 0; y < MAX_Y; y++)
				temp[x][y] = maze[x][y];
		int x = 1, y = 1;
		while (true) {
			temp[x][y] = 2;

			// Down
			if (temp[x + 1][y] == 0) {
				s.push(x, y, 'D');
				temp[x][y] = 3;			// Make a "Down" flag at current poistion
				x++;
				temp[x][y] = 7;			// Current position
				// Judge we get the exit
				if ((x == MAX_X - 1) && (y == MAX_Y - 1)) {
					flag = true;
					return;
				} else {
					continue;
				}
			}

			// Right
			if (temp[x][y + 1] == 0) {
				s.push(x, y, 'R');
				temp[x][y] = 4;			// Make a "Right" flag at current position
				y++;
				temp[x][y] = 7;
				if ((x == MAX_X - 1) && (y == MAX_Y - 1)) {
					flag = true;
					return;
				} else {
					continue;
				}
			}

			// Up
			if (temp[x - 1][y] == 0) {
				s.push(x, y, 'U');
				temp[x][y] = 6;			// Make a "Up" flag at current position
				x--;
				temp[x][y] = 7;
				if ((x == MAX_X - 1) && y == MAX_Y - 1) {
					flag = true;
					return;
				} else {
					continue;
				}
			}

			// Left
			if (temp[x][y - 1] == 0) {
				s.push(x, y, 'L');
				temp[x][y] = 5;			// Make a "Left" flag at current position
				y--;
				temp[x][y] = 7;
				if ((x == MAX_X - 1) && (y == MAX_Y - 1)) {
					flag = true;
					return;
				} else {
					continue;
				}
			}

			// There is no way to move down, up, right or left
			// We go back to the start position
			if (s.pop(x, y) == NULL && temp[x - 1][y] != 0 && temp[x][y - 1] != 0 && temp[x + 1][y] != 0 && temp[x][y + 1] != 0) {
				temp[0][1] = 7;
				if (temp[1][1] != 1) temp[1][1] = 2;
				return;
			}
		}
	}

};

