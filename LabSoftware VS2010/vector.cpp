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

int line_sect_ignore_edge(vec start1, vec end1, vec start2, vec end2) {
	double denom = ((end1->x - start1->x)*(end2->y - start2->y)) - ((end1->y - start1->y)*(end2->x - start2->y));
	if (denom == 0) return 0;
	double numer = ((start1->y - start2->y)*(end2->x - start2->x)) - ((start1->x-start2->x)*(end1->y-start1->y));

	double r = numer / denom;

	double numer2 = ((start1->y - start2->y)*(end2->x - start2->x)) - ((start1->x - start2->x)*(end1->y - start1->y));

	double s = numer2 / denom;

	if ((r <= 0 || r >= 1) || (s <= 0 || s >= 1)) return false;
	return true;
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

poly poly_copy(poly p1) {
	poly p2 = poly_new();
	for (int i = 0; i < p1->len;i++) {
		poly_append(p2,&p1->v[i]);
	}
	return p2;
}

vec rotate_point_around_point(vec p1, vec origin, double theta, vec res) {
	double angle = theta * (acos(-1) / 180);
	vec_t temp;
	temp.x = (p1->x - origin->x)*cos(angle) - (p1->y - origin->y) * sin(angle) + origin->x;
	temp.y = (p1->y - origin->y)*cos(angle) + (p1->y - origin->y) * sin(angle) + origin->y;
	res->x = temp.x;
	res->y = temp.y;
	return res;
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

void poly_remove_duplicates(poly p) {
	int removed = 1;
	if(removed){
		removed = 0;
		for (int i = 0; i < p->len; i++) {
			for (int j = i+1; j < p->len; j++) {
				if (p->v[i].x == p->v[j].x && p->v[i].y == p->v[j].y) {
					removed = 1;
					poly_remove(p, i);
					break;
				}
			}
			if (removed)break;
		}
	}
}

void poly_remove(poly p, int v) {
	for (int i = v; i < p->len-1; i++) {
		vec_t temp;
		p->v[i] = p->v[i + 1];
	}
	p->len--;
}