#pragma once

typedef struct {
	vec_t x1, y1, x2, y2;
}rect_t,*rect;

int clip_line(rect c, int*x1, int*y1, int*x2, int*y2);

int clip_line_copy(rect c, int x1, int y1, int x2, int y2, int *ox1, int*oy1, int*ox2, int*oy2);