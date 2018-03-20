#include "sutherlandhodgeman.h"
#include "liangbarsky.h"
#include "vector.h"

// Accepts a bounding box, input polygon ptr and ptr to empty output polygon
void clip_poly(rect bound, poly in) {
	
	poly out = poly_copy(in);
	
	for(int edge=0;edge<4;edge++){
		vec_t e1, e2;
		if (edge == 0) {
			e1=
		}
		if (edge == 1) {
			
		}
		if (edge == 2) {
			
		}
		if (edge == 3) {
			
		}
		poly_free(in);
		in = poly_copy(out);
		for (int i = 0;i < out->len;i++) {
			poly_remove(out, 0);
		}
		vec S = &in->v[in->len - 1];
		for (int i = 0;i < in->len;i++) {
			vec E = &in->v[i];
			
			// if E inside ClipEdge
			if (left_of())

			else {

			}
		}
	}
}