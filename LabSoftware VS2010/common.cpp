#include "common.h"

void loadVJS(FILE * f, obj o) {

	if (f) {
		int stage = 0;

		int pt = 0, polys = 0;

		char line[256];
		char line_dup[256];
		char *tok;
		while (fgets(line, sizeof(line), f)) {
			strcpy(line_dup, line);
			if (line[0] == '/' && line[1] == '/') {
				stage++;
			}
			else if (line[0] == ' ' && line[1] == ' ') {

			}
			else {
				if (stage == 1) {
					tok = strtok(line_dup, ",\n");
					//printf("%s\n",tok);
					if (tok) o->NumPtsObj = atoi(tok);
					tok = strtok(NULL, ",\n");
					//printf("%s\n", tok);
					if (tok) o->NumPolysObj = atoi(tok);
					//printf("%d,%d", o.NumPtsObj, o.NumPolysObj);
				}
				if (stage == 2) {
					if (pt < o->NumPtsObj) {
						//o->ObjectPoints[pt] = {0.0f,0.0f,0.0f,0,0,0};

						tok = strtok(line_dup, " \n");
						if (tok) {
							o->ObjectPoints[pt].x = atoi(tok);
							//printf("%d ", o.ObjectPoints[pt].x);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].y = atoi(tok);
							//printf("%d ", o.ObjectPoints[pt].y);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].z = atoi(tok);
							//printf("%d\n", o.ObjectPoints[pt].z);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].r = atoi(tok);
							//printf("%d ", o.ObjectPoints[pt].r);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].g = atoi(tok);
							//printf("%d ", o.ObjectPoints[pt].g);
						}
						tok = strtok(NULL, " \n");
						if (tok) {
							o->ObjectPoints[pt].b = atoi(tok);
							//printf("%d\n", o.ObjectPoints[pt].b);
						}

						pt++;
					}
				}
				if (stage == 3) {
					int sides = 0;
					tok = strtok(line_dup, " \n");
					if (tok) {
						sides = atoi(tok);
					}
					tok = strtok(NULL, " \n");
					for (int i = 0; i < sides; i++) {
						if (tok) {
							o->ObjectPolys[polys].Vertices[i] = atoi(tok);
							//printf("%i ",atoi(tok));
						}
						tok = strtok(NULL, " \n");
					}
					polys++;
					//printf("\n");
				}
			}
		}
		calculatenormals(o);
		find_centre_3d(o);
	}
	else return;
}

vec xyztoij(int x, int y, int z, vec res) {
	res->x = (DRAW_DISTANCE_CONSTANT*(x - (FRAME_WIDE) / 2) / (z + DRAW_DISTANCE_CONSTANT));
	res->y = (DRAW_DISTANCE_CONSTANT*(y - (FRAME_HIGH) / 2) / (z + DRAW_DISTANCE_CONSTANT));
	res->x += (FRAME_WIDE / 2);
	res->y += (FRAME_HIGH / 2);
	return res;
}

void printObject(obj o) {
	printf("%d,%d\n", o->NumPtsObj, o->NumPolysObj);
	for (int i = 0; i < o->NumPtsObj; i++) {
		printf("(%d,%d,%d,%d,%d,%d)\n", o->ObjectPoints[i].x, o->ObjectPoints[i].y, o->ObjectPoints[i].z, o->ObjectPoints[i].r, o->ObjectPoints[i].g, o->ObjectPoints[i].b);
	}
	for (int i = 0; i < o->NumPolysObj; i++) {
		printf("(%d,%d,%d,%d)\n", o->ObjectPolys[i].Vertices[0], o->ObjectPolys[i].Vertices[1], o->ObjectPolys[i].Vertices[2], o->ObjectPolys[i].Vertices[3]);
	}
}

void find_centre_3d(obj o) {
	int cx = 0, cy = 0, cz = 0;
	for (int i = 0; i < o->NumPtsObj; i++) {
		cx += o->ObjectPoints[i].x;
		cy += o->ObjectPoints[i].y;
		cz += o->ObjectPoints[i].z;
	}
	cx /= o->NumPtsObj;
	cy /= o->NumPtsObj;
	cz /= o->NumPtsObj;
	o->ObjProps.Center = { cx,cy,cz,0,0,0 };
}

void translate_obj(obj o, double dx, double dy, double dz) {
	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x += dx;
		o->ObjectPoints[i].y += dy;
		o->ObjectPoints[i].z += dz;
	}
	o->ObjProps.Center.x += dx;
	o->ObjProps.Center.y += dy;
	o->ObjProps.Center.z += dz;
}

void move_to_point(obj o, double x, double y, double z){
	move_to_origin(o);
	translate_obj(o, x, y, z);
}

void move_to_origin(obj o) {
	translate_obj(o, +o->ObjProps.Center.x, +o->ObjProps.Center.y, +o->ObjProps.Center.z);
}

void check_collision(obj o1, obj o2) {

}

void scale_obj(obj o, double xscale, double yscale, double zscale) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;

	translate_obj(o, -tx, -ty, -tz);

	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x *= xscale;
		o->ObjectPoints[i].y *= yscale;
		o->ObjectPoints[i].z *= zscale;
	}

	translate_obj(o, tx, ty, tz);
}

void rotate_obj(obj o, double xrot, double yrot, double zrot) {
	double tx = o->ObjProps.Center.x, ty = o->ObjProps.Center.y, tz = o->ObjProps.Center.z;
	translate_obj(o, -tx, -ty, -tz);
	for (int i = 0; i < o->NumPtsObj; i++) {
		if (xrot != 0) {
			o->ObjectPoints[i].y;// = o->ObjectPoints[i].y*cos(xrot) - o->ObjectPoints[i].z*sin(xrot);
			o->ObjectPoints[i].z;// = o->ObjectPoints[i].y*sin(xrot) + o->ObjectPoints[i].z*cos(xrot);
		}
		if (yrot != 0) {
			o->ObjectPoints[i].x;// = o->ObjectPoints[i].z*cos(yrot) - o->ObjectPoints[i].x*sin(yrot);
			o->ObjectPoints[i].z;// = o->ObjectPoints[i].z*sin(yrot) + o->ObjectPoints[i].x*cos(yrot);
		}
		if (zrot != 0) {
			o->ObjectPoints[i].x;// = o->ObjectPoints[i].x*cos(zrot) - o->ObjectPoints[i].y*sin(zrot);
			o->ObjectPoints[i].y;// = o->ObjectPoints[i].x*sin(zrot) + o->ObjectPoints[i].y*cos(zrot);
		}
	}
	translate_obj(o, tx, ty, tz);
}

void SortObjByZ(obj o) {
	int sorted = 0;
	while (!sorted) {
		sorted = 1;
		for (int i = 0; i < o->NumPolysObj; i++) {

		}
	}
}

void calculatenormals(obj o) {
	printf("Calculating normals...\n");
	for (int i = 0; i < o->NumPolysObj; i++) {
		Point_3D_D t = { 0,0,0 };
		for (int j = 0; j < NumSidesPoly; j++) {
			printf("(%i,%i,%i) & (%i,%i,%i)\n", o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].x, o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].y, o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].z, o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].x, o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].y, o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].z);
			t.x = t.x + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].y) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].y))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].z) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].z));
			t.y = t.y + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].z) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].z))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].x) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].x));
			t.z = t.z + ((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].x) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].x))*((double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[j]].y) - (double)(o->ObjectPoints[o->ObjectPolys[i].Vertices[(j + 1) % NumSidesPoly]].y));
			printf("%f,%f,%f\n", t.x, t.y, t.z);
		}
		o->ObjectPolys[i].normals = { t.x,t.y,t.z };
	}
	//getchar();
}

void backfaceculling(obj o) {
	Point_3D_D view = { FRAME_WIDE / 2,FRAME_HIGH / 2,0 };
	double dp;
	for (int i = 0; i < o->NumPolysObj; i++) {
		//Point_3D_D 
		dp = (view.x * o->ObjectPolys[i].normals.x) + (view.y * o->ObjectPolys[i].normals.y) + (view.z * o->ObjectPolys[i].normals.z);
		printf("Normal: %f,%f,%f \nDot Product: %f\n", o->ObjectPolys[i].normals.x, o->ObjectPolys[i].normals.y, o->ObjectPolys[i].normals.z, dp);
		if (dp >= 0) o->ObjectPolys[i].can_draw = 1;
		else o->ObjectPolys[i].can_draw = 0;
	}
}