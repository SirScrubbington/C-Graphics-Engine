#include "vector.h"
#include <stdlib.h>
#include <math.h>

// A . B
double dot(vec a, vec b) {
	return a->x*b->x + a->y*b->y;
}

// A x B
double cross(vec a, vec b) {
	return a->x*b->x - a->y*b->y;
}

// A - B
vec vsub(vec a, vec b, vec res) {
	res->x = a->x - b->x;
	res->y = a->y - b->y;
	return res;
}

// A * B
vec vmul(vec a, vec b, vec res) {
	res->x = a->x*b->x;
	res->y = a->y*b->y;
	return res;
}

// A + B
vec vadd(vec a, vec b, vec res) {
	res->x = a->x + b->x;
	res->y = a->y + b->y;
	return res;
}

// A / B
vec vdiv(vec a, vec b, vec res) {
	res->x = a->x / b->x;
	res->y = a->y / b->y;
	return res;
}

vec vabs(vec a,vec res) {
	res->x = fabs(a->x);
	res->y = fabs(a->y);
	return res;
}

int left_of(vec a, vec b, vec c) {
	vec_t tmp1, tmp2;
	double x;
	vsub(b, a, &tmp1);
	vsub(c, b, &tmp2);
	x = cross(&tmp1, &tmp2);
	return x < 0 ? -1 : x > 0;
}

double sign(vec p1, vec p2, vec p3) {
	return (p1->x - p3->x)*(p2->y - p3->y) - (p2->x-p3->x)*(p1->y-p3->y);
}

int triangle_inside(vec p, vec a, vec b, vec c) {
	return (sign(p, a, b) < 0.0f) &&
		sign(p, b, c) < 0.0f &&
		sign(p, c, a) < 0.0f;

}

int line_sect(vec x0, vec x1, vec y0, vec y1, vec res) {
	vec_t dx, dy, d;
	vsub(x1, x0, &dx);
	vsub(y1, y0, &dy);
	vsub(x0, y0, &d);
	double dyx = cross(&dy, &dx);
	if (!dyx) return 0;
	dyx = cross(&d, &dx) / dyx;
	if (dyx <= 0 || dyx >= 1) return 0;
	res->x = y0->x + dyx * dy.x;
	res->y = y0->y + dyx * dy.y;
	return 1;
}

// a = vertex, b & c = sides to get angle for w.r.t a
double inner_angle(vec b, vec a,vec c) {
	double dx21, dx31, dy21, dy31, m12, m13, theta;
	dx21 = b->x - a->x;
	dx31 = c->x - a->x;
	dy21 = b->y - a->y;
	dy31 = c->y - a->y;
	m12 = sqrt(dx21*dx21 + dy21 * dy21);
	m13 = sqrt(dx31*dx31 + dy31 * dy31);
	return acos((dx21*dx31 + dy21 * dy31) / (m12*m13));
}

poly poly_new()
{
	return (poly)calloc(1, sizeof(poly_t));
}

void poly_free(poly p)
{
	free(p->v);
	free(p);
}

void poly_append(poly p, vec v)
{
	if (p->len >= p->alloc) {
		p->alloc *= 2;
		if (!p->alloc) p->alloc = 4;
		p->v = (vec)realloc(p->v, sizeof(vec_t) * p->alloc);
	}
	p->v[p->len++] = *v;
}

void poly_remove(poly p, int v) {
	for (int i = v; i < p->len-1; i++) {
		vec_t temp;
		p->v[i] = p->v[i + 1];
	}
	p->len--;

}

int poly_winding(poly p)
{
	return left_of(p->v, p->v + 1, p->v + 2);
}

void poly_edge_clip(poly sub, vec x0, vec x1, int left, poly res)
{
	int i, side0, side1;
	vec_t tmp;
	vec v0 = sub->v + sub->len - 1, v1;
	res->len = 0;

	side0 = left_of(x0, x1, v0);
	if (side0 != -left) poly_append(res, v0);

	for (i = 0; i < sub->len; i++) {
		v1 = sub->v + i;
		side1 = left_of(x0, x1, v1);
		if (side0 + side1 == 0 && side0)
			if (line_sect(x0, x1, v0, v1, &tmp))
				poly_append(res, &tmp);
		if (i == sub->len - 1) break;
		if (side1 != -left) poly_append(res, v1);
		v0 = v1;
		side0 = side1;
	}
}

poly poly_clip(poly sub, poly clip)
{
	int i;
	poly p1 = poly_new(), p2 = poly_new(), tmp;

	int dir = poly_winding(clip);
	poly_edge_clip(sub, clip->v + clip->len - 1, clip->v, dir, p2);
	for (i = 0; i < clip->len - 1; i++) {
		tmp = p2; p2 = p1; p1 = tmp;
		if (p1->len == 0) {
			p2->len = 0;
			break;
		}
		poly_edge_clip(p1, clip->v + i, clip->v + i + 1, dir, p2);
	}

	poly_free(p1);
	return p2;
}
