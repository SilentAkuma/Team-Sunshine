typedef struct Point {
    uint8_t x;
    uint8_t y;
} Point;

enum Heading {
    North,
    South,
    East,
    West
};

updateWalls(bool left, bool right, bool front, Point pos, Heading hd);

