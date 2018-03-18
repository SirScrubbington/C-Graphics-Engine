#include "test.h"
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#elif __APPLE__
#include <unistd.h>
#elif __unix__
#include <unistd.h>
#endif

#define FRAME_WIDE	1000
#define FRAME_HIGH	600
#define COLOUR_MAX 255

#define ROUND(x) ((int)(x+0.5))
#define ABS(x) ((x)<0 ? -(x) : (x))
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

void testSetPixel(BYTE *screen) {
	for (int i = 0; i < 1000; i++) {
		unsigned int x, y, ex;
		unsigned char r, g, b;
		x = rand() % FRAME_WIDE;
		y = rand() % FRAME_HIGH;
		r = rand() % COLOUR_MAX;
		g = rand() % COLOUR_MAX;
		b = rand() % COLOUR_MAX;
		ex = 0;
		setPixel(x, y, r, g, b, screen);
	}
}

void testDrawLine(BYTE*screen) {
	unsigned int x1, x2;
	unsigned int y1, y2;
	unsigned char r, g, b;

	x1 = rand() % FRAME_WIDE;
	x2 = rand() % FRAME_WIDE;

	y1 = rand() % FRAME_HIGH;
	y2 = rand() % FRAME_HIGH;

	r = rand() % COLOUR_MAX;
	g = rand() % COLOUR_MAX;
	b = rand() % COLOUR_MAX;
	drawLine(x1, y1, x2, y2, r, g, b, screen);
}

void testDrawLineShaded(BYTE*screen) {
	unsigned int x1, x2;
	unsigned int y1, y2;
	unsigned char r1, r2, g1, g2, b1, b2;

	x1 = rand() % FRAME_WIDE;
	x2 = rand() % FRAME_WIDE;

	y1 = rand() % FRAME_HIGH;
	y2 = rand() % FRAME_HIGH;

	r1 = rand() % COLOUR_MAX;
	g1 = rand() % COLOUR_MAX;
	b1 = rand() % COLOUR_MAX;

	r2 = rand() % COLOUR_MAX;
	g2 = rand() % COLOUR_MAX;
	b2 = rand() % COLOUR_MAX;

	drawLineShaded(x1, y1, x2, y2, r1, g1, b1, r2, g2, b2, screen);
}

void testDrawTriangle(BYTE*screen) {
	int x1, y1, x2, y2, x3, y3;
	unsigned char r, g, b;

	x1 = rand() % FRAME_WIDE;
	x2 = rand() % FRAME_WIDE;
	x3 = rand() % FRAME_WIDE;

	y1 = rand() % FRAME_HIGH;
	y2 = rand() % FRAME_HIGH;
	y3 = rand() % FRAME_HIGH;

	r = rand() % COLOUR_MAX;
	g = rand() % COLOUR_MAX;
	b = rand() % COLOUR_MAX;

	drawTriangle(x1, y1, x2, y2, x3, y3, r, g, b, screen);

}

void testDrawTriangleShaded(BYTE*screen) {
	int x1, y1, x2, y2, x3, y3;
	unsigned char r1, g1, b1, r2, g2, b2;

	x1 = rand() % FRAME_WIDE;
	x2 = rand() % FRAME_WIDE;
	x3 = rand() % FRAME_WIDE;

	y1 = rand() % FRAME_HIGH;
	y2 = rand() % FRAME_HIGH;
	y3 = rand() % FRAME_HIGH;

	r1 = rand() % COLOUR_MAX;
	g1 = rand() % COLOUR_MAX;
	b1 = rand() % COLOUR_MAX;

	r2 = rand() % COLOUR_MAX;
	g2 = rand() % COLOUR_MAX;
	b2 = rand() % COLOUR_MAX;

	drawTriangleShaded(x1, y1, x2, y2, x3, y3, r1, g1, b1, r2, g2, b2, screen);

}