#pragma once

#include "vector.h"

#include <math.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <unistd.h>
#elif __unix__		// all unices including  __linux__
#include <unistd.h>
#endif

void fillFlatTopTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen);

void fillFlatBottomTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen);

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen);

void fillTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen);

void drawTriangleShaded(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2, BYTE*screen);