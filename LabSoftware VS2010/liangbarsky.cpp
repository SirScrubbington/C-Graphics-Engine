#include "liangbarsky.h"

int is_zero(double v) {
	return (v > -0.000001f && v < 0.000001f);
}

int point_inside(rect c, int x, int y) {
	return (x >= c->x1 && x <= c->x2 &&
		y >= c->y1 && y <= c->y2);
}

int clip_t(double num, double denom, double*tE, double*tL) {
	double t;
	if (is_zero(denom)) return num < 0.0;

	t = num / denom;

	if (denom > 0) {
		if (t > *tL)return 0;
		if (t > *tE)*tE = t;
	}
	else {
		if (t < *tE)return 0;
		if (t < *tL)*tL = t;
	}
	return 1;
}

int clip_line(rect c, int*x1, int*y1, int*x2, int*y2) {
	double dx, dy, tE, tL;
	dx = *x2 - *x1;
	dy = *y2 - *y1;

	if (is_zero(dx) && is_zero(dy) && point_inside(c, *x1, *y1)) return 1;

	tE = 0, tL = 1;

	if (clip_t(c->x1 - (double)*x1, dx, &tE, &tL) &&
		clip_t((double)*x1 - c->x2, -dx, &tE, &tL) &&
		clip_t(c->y1 - (double)*y1, dy, &tE, &tL) &&
		clip_t((double)*y1 - c->y2, -dy, &tE, &tL)) {
		if (tL < 1) {
			*x2 = (int)((double)*x1 + tL * dx);
			*y2 = (int)((double)*y1 + tL * dy);
		}
		if (tE > 0) {
			*x1 += tE * dx;
			*y1 += tE * dy;
		}
		return 1;
	}
	return 0;
}

int clip_line_copy(rect c, int x1, int y1, int x2, int y2, int *ox1, int*oy1, int*ox2, int*oy2) {
	*ox1 = x1;
	*oy1 = y1;
	*ox2 = x2;
	*oy2 = y2;
	return clip_line(c, ox1, ox2, oy1, oy2);
}
