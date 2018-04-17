#pragma once

/* -- * -- * -- * -- Preprocessor Calls -- * -- * -- * -- */

// Converts degrees to radians.
#define D2R(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define R2D(angleRadians) (angleRadians * 180.0 / M_PI)

#define M_PI (double)acos((double)-1)

/* -- * -- * -- * -- Stuctures -- * -- * -- * -- */

typedef struct { double x, y; double r, g, b; double z; }vec_t, *vec;
typedef struct { int len, alloc; vec v; }poly_t, *poly;

/* -- * -- * -- * -- Vector Functions -- * -- * -- * -- */

double dot(vec a, vec b);
double cross(vec a, vec b);
vec copy(vec a, vec res);
int left_of(vec a, vec b, vec c);
double sign(vec p1, vec p2, vec p3);
int triangle_inside(vec p, vec a, vec b, vec c);
int line_sect_ignore_edge(vec x0, vec x1, vec y0, vec y1);
double inner_angle(vec a, vec b,vec c);
vec rotate_point_around_point(vec p1, vec origin, double theta, vec res);

/* -- * -- * -- * -- Polygon Functions -- * -- * -- * -- */

poly poly_new();
poly poly_copy(poly p1);
void poly_free(poly p);
void poly_remove_duplicates(poly p);
void poly_append(poly p, vec v);
void poly_remove(poly p, int v);