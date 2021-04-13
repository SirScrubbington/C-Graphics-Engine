#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "vector.h"
#include "math.h"

//====== Macros and Defines =========

#define MAX_OBJECTS 20

// Cohen Sutherland Line Clipping Constants

#define INSIDE 0
#define LEFT 1
#define RIGHT 2
#define BOTTOM 4
#define TOP 8

// Frame Manipulation Constants

#define FRAME_WIDE	1000
#define FRAME_HIGH	600
#define COLOUR_MAX 255
#define DRAW_DISTANCE_CONSTANT 500

// Vertex Join Sets
#define MaxNumObjs 20
#define MaxNumPts 600
#define MaxNumPolys 900
#define NumSidesPoly 4

// Game Constants
#define START_LIVES 3

// Macros

// ROUND(x): int
// Returns the value of 'x' as an integer rounded to the nearest whole number
#define ROUND(x) ((int)(x+0.5))

// ABS(x): (same as x)
// Returns the value of 'x' as an absolute value
#define ABS(x) ((x)<0 ? -(x) : (x))

// SWAP(x,y,T): void
// Swaps the values of x and y, given they are both of data type T
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

// MAX(x,y): (same as x,y)
// Returns the maximum values of 'x' and 'y'
#define MAX(x,y) ((x) >= (y)) ? (x) : (y)

// MAX(x,y): (same as x,y)
// Returns the minimum values of 'x' and 'y'
#define MIN(x,y) ((x) <= (y)) ? (x) : (y)

//====== Structs & typedefs =========

typedef unsigned char BYTE;
typedef struct POINT2D { int x, y; }p2d_t, *P2D;

// Vertex Join Set Stuff

typedef struct coordinate { int x, y; char code[4]; }ch_PT;
typedef struct _Colour { unsigned char r, g, b; } colour_t, *colour;
typedef struct _Point_3D { int x, y, z; unsigned char r, g, b; }Point_3D, Points_3D[MaxNumPts], *P3D;
typedef struct _Point_3D_D { double x, y, z; unsigned char r, g, b; }Point_3D_D, Points_3D_D[MaxNumPts], *P3D_D;
typedef struct _Polygon { int Vertices[NumSidesPoly]; Point_3D_D normals; int can_draw; } Polygon_t, Polygons[MaxNumPolys];
typedef struct _ObjectAttribs { BYTE Colour; Point_3D Center, Scale, Rotation; } ObjectAttribs, ObjAttrArray[MaxNumObjs];
typedef struct _Object { ObjectAttribs ObjProps; int NumPtsObj, NumPolysObj; Polygons ObjectPolys; Points_3D ObjectPoints; } Object, *obj;

static const Object Blank_Object;

// Object Functions

void load_vjs(FILE * f, obj o);
vec xyztoij(int x, int y, int z, vec res);
void print_object(obj o);
void find_centre_3d(obj o);
void translate_obj(obj o, double dx, double dy, double dz);
void move_to_origin(obj o);
void move_to_point(obj o, double x, double y, double z);
void scale_obj(obj o, double xscale, double yscale, double zscale);
void calculate_normals(obj o);
void backface_culling(obj o);
void rotate_x(obj o, double theta);
void rotate_y(obj o, double theta);
void rotate_z(obj o, double theta);