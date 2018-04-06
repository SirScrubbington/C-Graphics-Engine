#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "vector.h"
#include "math.h"

//====== Macros and Defines =========

#define FRAME_WIDE	1000
#define FRAME_HIGH	600
#define COLOUR_MAX 255

#define DRAW_DISTANCE_CONSTANT 500

#define ROUND(x) ((int)(x+0.5))
#define ABS(x) ((x)<0 ? -(x) : (x))
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

#define MAX(x,y) ((x) >= (y)) ? (x) : (y)
#define MIN(x,y) ((x) <= (y)) ? (x) : (y)

// Vertex Join Sets
#define MaxNumObjs 20
#define MaxNumPts 600
#define MaxNumPolys 900
#define NumSidesPoly 4

#define MAX_BULLETS 100
#define MAX_ENEMIES 20
#define START_LIVES 3

#define PLAYER_MOV_SPD 10
#define ENEMY_MOV_SPD 10
#define BLT_MOV_SPD 50

// Converts degrees to radians.
#define D2R(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define R2D(angleRadians) (angleRadians * 180.0 / M_PI)

#define M_PI acos(-1)

//====== Structs & typedefs =========

typedef unsigned char BYTE;
typedef struct POINT2D { int x, y; }p2d_t, *P2D;

// Vertex Join Set Stuff

typedef struct _Colour { unsigned char r, g, b; } colour_t, *colour;
typedef struct _Point_3D { int x, y, z; unsigned char r, g, b; }Point_3D, Points_3D[MaxNumPts], *P3D;
typedef struct _Point_3D_D { double x, y, z; unsigned char r, g, b; }Point_3D_D, Points_3D_D[MaxNumPts], *P3D_D;
typedef struct _Polygon { int Vertices[NumSidesPoly]; Point_3D_D normals; int can_draw; } Polygon_t, Polygons[MaxNumPolys];
typedef struct _ObjectAttribs { BYTE Colour; Point_3D Center, Scale; } ObjectAttribs, ObjAttrArray[MaxNumObjs];
typedef struct _Object { ObjectAttribs ObjProps; int NumPtsObj, NumPolysObj; Polygons ObjectPolys; Points_3D ObjectPoints; } Object, *obj;

void loadVJS(FILE * f, obj o);
vec xyztoij(int x, int y, int z, vec res);
void printObject(obj o);
void find_centre_3d(obj o);
void translate_obj(obj o, double dx, double dy, double dz);
void move_to_origin(obj o);
void move_to_point(obj o, double x, double y, double z);
void scale_obj(obj o, double xscale, double yscale, double zscale);
void rotate_obj(obj o, double xrot, double yrot, double zrot);
void SortObjByZ(obj o);
void calculatenormals(obj o);
void backfaceculling(obj o);