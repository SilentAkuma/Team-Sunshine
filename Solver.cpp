#include "Solver.h"

#include <stdint.h>
#include <vector>
//#include <iostream>

int8_t maze[16][16] = {0};
int8_t wallsx[16][15] = {0};
int8_t wallsy[15][16] = {0};
int8_t destCol = 7; //7
int8_t destRow = 8; //8
int8_t curRow = 0;
int8_t curCol = 0;
Heading curHd = North;
Vector directions[128];
bool firstTurn = true;
bool shouldGoHome = true;

Heading headingLeft(Heading hd) {
    switch(hd) {
      case North:
          return West;
      case South:
          return East;
      case East:
          return North;
      case West:
          return South;
    }
}

Heading headingRight(Heading hd) {
    switch(hd) {
      case North:
          return East;
      case South:
          return West;
      case East:
          return South;
      case West:
          return North;
    }
}

Heading headingDir(Dir dir, Heading hd) {
	Heading head;
	switch(hd) {
	case North:
		switch(dir) {
		case Left:
			head = West;
			break;
		case Right:
			head = East;
			break;
		case Forward:
			head = North;
			break;
		case Reverse:
			head = South;
			break;
		}
		break;
	case South:
		switch(dir) {
		case Left:
			head = East;
			break;
		case Right:
			head = West;
			break;
		case Forward:
			head = South;
			break;
		case Reverse:
			head = North;
			break;
		}
		break;
	case East:
		switch(dir) {
		case Left:
			head = North;
			break;
		case Right:
			head = South;
			break;
		case Forward:
			head = East;
			break;
		case Reverse:
			head = West;
			break;
		}
		break;
	case West:
		switch(dir) {
		case Left:
			head = South;
			break;
		case Right:
			head = North;
			break;
		case Forward:
			head = West;
			break;
		case Reverse:
			head = East;
			break;
		}
		break;
	}
	return head;
}

void insertWall(Heading hd, int8_t row, int8_t col) {
    switch(hd) {
      case North:
		  if(row != MAZEMAX-1)
			  if(wallsy[row][col] != 1)
				  wallsy[row][col] = 1;
		  break;
      case South:
		  if(row != 0)
			  if(wallsy[row-1][col] != 1)
				  wallsy[row-1][col] = 1;
		  break;
      case East:
		  if(col != MAZEMAX-1)
			  if(wallsx[row][col] != 1)
				  wallsx[row][col] = 1;
		  break;
      case West:
		  if(col != 0)
			  if(wallsx[row][col-1] != 1)
				  wallsx[row][col-1] = 1;
		  break;
    }
	return;
}

void updateWalls(bool left, bool right, bool front, int8_t row, int8_t col, Heading hd) {
    Heading cur;
    if(left) {
		cur = headingLeft(hd);
		insertWall(cur, row, col);
    }
	if(right) {
		cur = headingRight(hd);
		insertWall(cur, row, col);
	}
	if(front) {
		insertWall(hd, row, col);
	}
	return;
}

int8_t highestNeighborCell(int8_t row, int8_t col) {
	int8_t highest = -1;
	// CHECK SOUTH
	if(row != 0) {
		if(wallsy[row-1][col] != 1 && maze[row-1][col] > highest) {
			highest = maze[row-1][col];
		}
	}
	// CHECK NORTH
	if(row != MAZEMAX-1) {
		if(wallsy[row][col] != 1 && maze[row+1][col] > highest) {
			highest = maze[row+1][col];
		}
	}
	// CHECK WEST
	if(col != 0) {
		if(wallsx[row][col-1] != 1 && maze[row][col-1] > highest) {
			highest = maze[row][col-1];
		}
	}
	// CHECK EAST
	if(col != MAZEMAX-1) {
		if(wallsx[row][col] != 1 && maze[row][col+1] > highest) {
			highest = maze[row][col+1];
		}
	}
	return highest;
}

Point smallestAdjacent(int8_t row, int8_t col) {
	int8_t smallest = 100;
	Point r;
	// CHECK SOUTH
	if(row != 0) {
		if(maze[row-1][col] != -1 && maze[row-1][col] < smallest && wallsy[row-1][col] != 1) {
			smallest = maze[row-1][col];
			r.row = row-1;
			r.col = col;
		}
	}
	// CHECK NORTH
	if(row != MAZEMAX-1) {
		if(maze[row+1][col] != -1 && maze[row+1][col] < smallest && wallsy[row][col] != 1) {
			smallest = maze[row+1][col];
			r.row = row+1;
			r.col = col;
		}
	}
	// CHECK WEST
	if(col != 0) {
		if(maze[row][col-1] != -1 && maze[row][col-1] < smallest && wallsx[row][col-1] != 1) {
			smallest = maze[row][col-1];
			r.row = row;
			r.col = col-1;
		}
	}
	// CHECK EAST
	if(col != MAZEMAX-1) {
		if(maze[row][col+1] != -1 && maze[row][col+1] < smallest && wallsx[row][col] != 1) {
			smallest = maze[row][col+1];
			r.row = row;
			r.col = col+1;
		}
	}
	return r;
}

void floodfill() {
	int8_t curPathDistance = 1;
	int8_t row, col, neighbor;
	for(row = 0; row < MAZEMAX; row++) {
		for(col = 0; col < MAZEMAX; col++) {
			maze[row][col] = -1;
		}
	}

	maze[curRow][curCol] = 0;

	while(1) {
		for(row = 0; row < MAZEMAX; row++) {
			for(col = 0; col < MAZEMAX; col++) {
				if(maze[row][col] != -1)
					continue;
				neighbor = highestNeighborCell(row, col);
				if(neighbor != -1 && neighbor == curPathDistance-1)
					maze[row][col] = curPathDistance;
			}
		}
		if(maze[destRow][destCol] != -1)
			break;
		curPathDistance++;
	}
}

void handleFirstTurn(bool left, bool right, bool front) {
	//First turn is right, no change
	if(!right)
		firstTurn = false;
	//First turn is left, change current position and update walls
	if(!left) {
		firstTurn = false;
		curCol = MAZEMAX - 1; //Rightmost column is current column
		//Swap walls
		for(int8_t row = 0; row < MAZEMAX; row++) {
			if(wallsx[row][0] = 1) {
				wallsx[row][MAZEMAX-2] = 1;
				wallsx[row][0] = 0;
			}
		}
	}
}

Point findNextSpace() {
	Point nextSpace, nextCell;
	nextSpace.row = destRow;
	nextSpace.col = destCol;

	while(1) {
		nextCell = smallestAdjacent(nextSpace.row, nextSpace.col);
		if(nextCell.row == curRow && nextCell.col == curCol)
			break;
		nextSpace.row = nextCell.row;
		nextSpace.col = nextCell.col;
	}
	//std::cout << "CurRow: " << (int) curRow << std::endl;
	//std::cout << "CurCol: " << (int) curCol << std::endl;
	return nextSpace;
}

Vector moveNextSpace(Point nextSpace) {
	Vector nextMove;
	nextMove.mag = 1;
	nextMove.dir = Forward;
	if(curRow == nextSpace.row) {
		if(curCol+1 == nextSpace.col) {
			switch(curHd) {
			case North:
				nextMove.dir = Right;
				break;
			case South:
				nextMove.dir = Left;
				break;
			case East:
				nextMove.dir = Forward;
				break;
			case West:
				nextMove.dir = Reverse;
				break;
			}
		}
		if(curCol-1 == nextSpace.col) {
			switch(curHd) {
			case North:
				nextMove.dir = Left;
				break;
			case South:
				nextMove.dir = Right;
				break;
			case East:
				nextMove.dir = Reverse;
				break;
			case West:
				nextMove.dir = Forward;
				break;
			}
		}
	}

	if(curCol == nextSpace.col) {
		if(curRow+1 == nextSpace.row) {
			switch(curHd) {
			case North:
				nextMove.dir = Forward;
				break;
			case South:
				nextMove.dir = Reverse;
				break;
			case East:
				nextMove.dir = Left;
				break;
			case West:
				nextMove.dir = Right;
				break;
			}
		}
		if(curRow-1 == nextSpace.row) {
			switch(curHd) {
			case North:
				nextMove.dir = Reverse;
				break;
			case South:
				nextMove.dir = Forward;
				break;
			case East:
				nextMove.dir = Right;
				break;
			case West:
				nextMove.dir = Left;
				break;
			}
		}
	}
	curHd = headingDir(nextMove.dir, curHd);
	curRow = nextSpace.row;
	curCol = nextSpace.col;
	return nextMove;
}

uint16_t secondRun() {
	uint16_t index = 0;
	Point nextSpace;
	Vector nextDirection;
	Heading storeHd = curHd;

	destRow = MAZEGOALROW;
	destCol = MAZEGOALCOL;
	floodfill();

	nextSpace = findNextSpace();
	nextDirection = moveNextSpace(nextSpace);
	directions[index] = nextDirection;
	//curRow = nextSpace.row;
	//curCol = nextSpace.col;
	//curHd = headingDir(nextDirection.dir, curHd);
	while(1) {
		nextSpace = findNextSpace();
		nextDirection = moveNextSpace(nextSpace);
		if(nextDirection.dir == Forward)
			directions[index].mag++;
		else {
			index++;
			directions[index] = nextDirection;
		}
		//curRow = nextSpace.row;
		//curCol = nextSpace.col;
		//curHd = headingDir(nextDirection.dir, curHd);
		if(curRow == destRow && curCol == destCol) {
			curRow = 0;
			curCol = 0;
			curHd = storeHd;
			index++;
			break;
		}
	}
	return index;
}

/*
//THIS MAIN IS AN EXAMPLE OF INTENDED EXECUTION FLOW
int main() {
	Point nextSpace;
	Vector nextMove;
	uint16_t directions_size;
    wallsx[0][0] = 1;
    wallsx[0][1] = 1;
    wallsx[0][3] = 1;
    wallsx[0][5] = 1;
    wallsx[1][1] = 1;
    wallsx[1][2] = 1;
    wallsx[1][3] = 1;
    wallsx[1][4] = 1;
    wallsx[1][6] = 1;
    wallsx[2][1] = 1;
    wallsx[2][2] = 1;
    wallsx[2][3] = 1;
    wallsx[2][6] = 1;
    wallsx[3][1] = 1;
    wallsx[3][2] = 1;
    wallsx[3][3] = 1;
    wallsx[3][6] = 1;
    wallsx[4][1] = 1;
    wallsx[4][2] = 1;
    wallsx[4][3] = 1;
    wallsx[4][6] = 1;
    wallsx[5][1] = 1;
    wallsx[5][2] = 1;
    wallsx[5][6] = 1;
    wallsx[6][1] = 1;
    wallsx[6][5] = 1;
    wallsx[6][6] = 1;
    wallsx[7][1] = 1;
    wallsx[7][2] = 1;
    wallsx[7][3] = 1;
    wallsx[7][4] = 1;
    wallsx[7][5] = 1;
    wallsx[7][6] = 1;
    wallsx[8][5] = 1;
    wallsx[8][6] = 1;
    wallsx[9][0] = 1;
    wallsx[9][1] = 1;
    wallsx[9][2] = 1;
    wallsx[9][3] = 1;
    wallsx[9][4] = 1;
    wallsx[9][5] = 1;
    wallsx[9][6] = 1;
    wallsy[1][0] = 1;
    wallsy[1][4] = 1;
    wallsy[1][6] = 1;
    wallsy[2][0] = 1;
    wallsy[2][5] = 1;
    wallsy[2][6] = 1;
    wallsy[3][0] = 1;
    wallsy[3][4] = 1;
    wallsy[3][6] = 1;
    wallsy[4][1] = 1;
    wallsy[4][4] = 1;
    wallsy[4][5] = 1;
    wallsy[5][0] = 1;
    wallsy[5][3] = 1;
    wallsy[5][4] = 1;
    wallsy[5][5] = 1;
    wallsy[6][1] = 1;
    wallsy[6][2] = 1;
    wallsy[6][3] = 1;
    wallsy[6][4] = 1;
    wallsy[7][0] = 1;
    wallsy[8][6] = 1;
    wallsy[8][7] = 1;


	//std::cout << "Left: " << wallsx[1][0] << std::endl;
	//std::cout << "Right: " << wallsx[1][1] << std::endl;
	//std::cout << "Front: " << wallsy[1][1] << std:: endl;

	//Adjust position if it is first turn
	if(firstTurn) {
	//	handleFirstTurn(left, right, front);
	}
	//Update walls at current position
	//updateWalls(left, right, front, curRow, curCol, curHd);

	//Fill maze using flood fill algorithm
	floodfill();

	//Find the next space
	//nextSpace = findNextSpace();

	//Find the next move based on the next space
	//nextMove = moveNextSpace(nextSpace);


//	//If first arrival at goal, go back to begining
//	if(nextSpace.row == destRow && nextSpace.col == destCol && shouldGoHome) {
//		shouldGoHome = false;
//		destRow = 0;
//		destCol = 0;
//		//floodfill();
//	}
//
//
//	//If arrived back at home, do fast run and DONE
//	if(nextSpace.row == destRow && nextSpace.col == destCol && !shouldGoHome) {
//		directions_size = secondRun();
//		for(uint16_t i = 0; i < directions_size; i++) {
//			std::cout << "DIRECTIONS: " << directions[i].dir << ", " << (int) directions[i].mag << std::endl;
//		}
//		break;
//	}


	// THIS IS A SAMPLE TRAVERSAL EXAMPLE
	while(1) {

	nextSpace = findNextSpace();
	std::cout << "Row: " << (int) nextSpace.row << std::endl;
	std::cout << "Col: " << (int) nextSpace.col << std::endl;
	nextMove = moveNextSpace(nextSpace);
	std::cout << "Vector: " << (int) nextMove.dir << ", " << (int) nextMove.mag << std::endl;

	if(nextSpace.row == destRow && nextSpace.col == destCol && shouldGoHome) {
		shouldGoHome = false;
		destRow = 0;
		destCol = 0;
		floodfill();
	}

	if(nextSpace.row == destRow && nextSpace.col == destCol && !shouldGoHome) {
		directions_size = secondRun();
		for(uint16_t i = 0; i < directions_size; i++) {
			std::cout << "DIRECTIONS: " << directions[i].dir << ", " << (int) directions[i].mag << std::endl;
		}
		break;
	}
	}
	//

	//THIS PRINTS OUT THE DISTANCE ARRAY AT THE END
	for(int i = 0; i < MAZEMAX; i++) {
		for(int j = 0; j < MAZEMAX; j++) {
			std::cout << (int) maze[i][j] << ", ";
		}
		std::cout << std::endl;
	}


    return 1;
}
*/

