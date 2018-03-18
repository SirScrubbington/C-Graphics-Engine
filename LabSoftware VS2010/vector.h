#pragma once

typedef struct { double x, y; }vec_t, *vec;

typedef struct { int len, alloc; vec v; }poly_t, *poly;

double dot(vec a, vec b);

double cross(vec a, vec b);

vec vsub(vec a, vec b, vec res);

vec vmul(vec a, vec b, vec res);

vec vadd(vec a, vec b, vec res);

vec vdiv(vec a, vec b, vec res);

int left_of(vec a, vec b, vec c);

double sign(vec p1, vec p2, vec p3);

int triangle_inside(vec p, vec a, vec b, vec c);

int line_sect(vec x0, vec x1, vec y0, vec y1, vec res);

double inner_angle(vec a, vec b,vec c);

poly poly_new();

void poly_free(poly p);

void poly_append(poly p, vec v);

void poly_remove(poly p, int v);

int poly_winding(poly p);

void poly_edge_clip(poly sub, vec xv0, vec x1, int left, poly res);

poly poly_clip(poly sub, poly clip);