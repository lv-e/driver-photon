
#pragma once

typedef struct Size {
    unsigned short width;
    unsigned short height;
} Size;

typedef struct Point {
    unsigned short x;
    unsigned short y;
} Point;

typedef struct Region {
    Point origin;
    Size size;
    Region();
    Region(unsigned short width, unsigned short height);
} Region;