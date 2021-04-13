#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// dot(a,b): double
// Returns the dot product of two vectors a and b.
double dot(vec a, vec b) {
	return a->x*b->x + a->y*b->y;
}

// cross(a,b): double
// Returns the cross product of two vectors a and b.
double cross(vec a, vec b) {
	return a->x*b->x - a->y*b->y;
}

// copy(a,res): vec
// Copies all of the values from vector 'a' to vector 'res'
vec copy(vec a, vec res) {
	res->x = a->x;
	res->y = a->y;
	res->r = a->r;
	res->g = a->g;
	res->b = a->b;
	res->z = a->z;
	return res;
}

// vsub(a,b): vec
// Subtracts vector b from vector 'a' and stores it in vector 'res', returning a pointer to 'res'
vec vsub(vec a, vec b, vec res) {
	res->x = a->x - b->x;
	res->y = a->y - b->y;
	return res;
}

// vabs(a,res): vec
// Assigns 'res' to the absolute value of vector 'a', returning a pointer to 'res'
vec vabs(vec a,vec res) {
	res->x = fabs(a->x);
	res->y = fabs(a->y);
	return res;
}

// left_of(a,b,c): int
// Calculates if point c is to the left of the line created by points a and b. Returns 0 if true, -1 if false.
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

// triangle_inside(p,a,b,c): int
// Calculates if point 'p' is inside the triangle created by 'a','b','c'. Returns 1 if true, 0 if false.
int triangle_inside(vec p, vec a, vec b, vec c) {
	return (sign(p, a, b) < 0.0f) &&
		sign(p, b, c) < 0.0f &&
		sign(p, c, a) < 0.0f;

}

// line_sect_ignore_edge(start1,end1,start2,end2): int
// Calculates the intersection between lines start1,end1 and start2, end2. Returns 1 if intersection is found, 0 otherwise
int line_sect_ignore_edge(vec start1, vec end1, vec start2, vec end2) {
	double denom = ((end1->x - start1->x)*(end2->y - start2->y)) - ((end1->y - start1->y)*(end2->x - start2->y));
	if (denom == 0) return 0;
	double numer = ((start1->y - start2->y)*(end2->x - start2->x)) - ((start1->x-start2->x)*(end1->y-start1->y));

	double r = numer / denom;

	double numer2 = ((start1->y - start2->y)*(end2->x - start2->x)) - ((start1->x - start2->x)*(end1->y - start1->y));

	double s = numer2 / denom;

	if ((r <= 0 || r >= 1) || (s <= 0 || s >= 1)) return 0;
	return 1;
}

// inner_angle(a,b,c): double
// Calculates the inner angle between the points where (a) is the leftmost point, (b) is the centre point and (c) is the rightmost point
double inner_angle(vec b, vec a,vec c) {
	double dx21, dx31, dy21, dy31, m12, m13;
	dx21 = b->x - a->x;
	dx31 = c->x - a->x;
	dy21 = b->y - a->y;
	dy31 = c->y - a->y;
	m12 = sqrt(dx21*dx21 + dy21 * dy21);
	m13 = sqrt(dx31*dx31 + dy31 * dy31);
	return acos((dx21*dx31 + dy21 * dy31) / (m12*m13));
}

// poly_new(): poly
// Allocates a new polygon structure and returns a pointer to it.
poly poly_new()
{
	return (poly)calloc(1, sizeof(poly_t));
}

// poly_copy(p1): poly
// Allocates a new polygon structure, copies an existing polygon into it and returns a pointer to the new structure.
poly poly_copy(poly p1) {
	poly p2 = poly_new();
	for (int i = 0; i < p1->len;i++) {
		poly_append(p2,&p1->v[i]);
	}
	return p2;
}

// rotate_point_around_point(p1,origin,theta,res): vec
// Rotates a given vec p1 around a given origin point 'origin' by angle theta, and puts the result in vector 'res'.
vec rotate_point_around_point(vec p1, vec origin, double theta, vec res) {
	D2R(theta);
	vec_t temp;
	temp.x = ((p1->x - origin->x)*cos(theta) - (p1->y - origin->y)*sin(theta)) + origin->x;
	temp.y = ((p1->x - origin->x)*sin(theta) + (p1->y - origin->y)*cos(theta)) + origin->y;
	res->x = temp.x;
	res->y = temp.y;
	return res;
}

// poly_free(p): void
// Frees the memory allocated to a polygon so it can be reused
void poly_free(poly p)
{
	free(p->v);
	free(p);
}

// poly_append(p,v): void
// Appends vector 'v' to the end of given polygon 'p'
void poly_append(poly p, vec v)
{
	if (p->len >= p->alloc) {
		p->alloc *= 2;
		if (!p->alloc) p->alloc = 4;
		p->v = (vec)realloc(p->v, sizeof(vec_t) * p->alloc);
	}
	p->v[p->len++] = *v;
}

// poly_remove_duplicates(p): void
// Removes all duplicate points from 'p', should they occur
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

// poly_remove(p,v): void
// Removes the vector from 'p' at index 'v'
void poly_remove(poly p, int v) {
	for (int i = v; i < p->len-1; i++) {
		vec_t temp;
		p->v[i] = p->v[i + 1];
	}
	p->len--;
}