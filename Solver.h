#ifndef SOLVER_H
#define SOLVER_H

#include <stdint.h>

typedef struct
{
    uint8_t x;
    uint8_t y;
} Point;

typedef enum
{
    North,
    South,
    East,
    West
} Heading;

void updateWalls(bool left, bool right, bool front, Point pos, Heading hd);

#endif // SOLVER_H
