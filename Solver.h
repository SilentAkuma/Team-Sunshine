#ifndef SOLVER
#define SOLVER


#include <stdint.h>
#include <vector>

#define MAZEMAX 16
#define MAZEGOALROW 8 //7
#define MAZEGOALCOL 7 //7

enum Dir {
	Left,
	Right,
	Forward,
	Reverse
};

enum Heading {
    North,
    South,
    East,
    West
};

typedef struct Point {
    int8_t row;
    int8_t col;
} Point;

typedef struct Vector {
	Dir dir;
	uint8_t mag;
} Vector;

extern int8_t maze[16][16];
extern int8_t wallsx[16][15];
extern int8_t wallsy[15][16];
extern int8_t destCol; //7
extern int8_t destRow; //8
extern int8_t curRow;
extern int8_t curCol;
extern Heading curHd;
extern Vector directions[128];
extern bool firstTurn;
extern bool shouldGoHome;

//This must be run until first turn is executed.  Expects the walls detected at first cell where
//turn is necessary.
void handleFirstTurn(bool left, bool right, bool front);

//Update walls at row, col facing hd(north, west, east, south)
void updateWalls(bool left, bool right, bool front, int8_t row, int8_t col, Heading hd);

//Bellman-Ford algorithm on the maze
void floodfill();

//Returns the row and column of the next space
Point findNextSpace();

//Returns a Vector(direction, magnitude) to the next space (distance 1)
//Also assigns nextSpace to curRow and curCol and updates curHd
Vector moveNextSpace(Point nextSpace);

//Returns size of directions array for second run
//Fills directions array with directions to get to center
uint16_t secondRun();

#endif
