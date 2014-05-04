#include "Solver.h"

uint8_t maze[16][16];
uint8_t wallsx[15][16];
uint8_t wallsy[16][15];

void initializeWalls()
{
    int i, j;
    for(i = 0; i < 16; i++)
        for(j = 0; j < 16; j++)
            maze[i][j] = 0;
    for(i = 0; i < 15; i++)
        for(j = 0; j < 16; j++)
            wallsx[i][j] = 0;
    for(i = 0; i < 16; i++)
        for(j = 0; j < 16; j++)
            wallsy[i][j] = 0;
}

Heading headingLeft(Heading hd)
{
    switch(hd)
    {
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

void insertWall(Heading hd, Point pos)
{
    uint8_t x = pos.x;
    uint8_t y = pos.y;
    switch(hd)
    {
    case North:
        wallsy[x][y] = 1;
    case South:
        wallsy[x][y-1] = 1;
    case East:
        wallsx[x][y] = 1;
    case West:
        wallsx[x-1][y] = 1;
    }
}

void updateWalls(bool left, bool right, bool front, Point pos, Heading hd)
{
    Heading cur;
    if(left)
    {
        if(pos.x == 0)
            return;
        cur = headingLeft(hd);
        insertWall(cur, pos);
    }
}
//
//int main()
//{
//    Point p;
//    p.x = 1;
//    p.y = 1;
//    updateWalls(true, false, false, p, North);
//    return 1;
//}


