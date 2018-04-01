#include <stdlib.h>			//- for exit()
#include <stdio.h>			//- for s//printf()
#include <string.h>			//- for memset()
#include <math.h>			//- for round()
#include <cmath>

#include "vector.h"
#include "queue.h"

#define M_PI acos(-1)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
	#include "libs/glut.h"
	#include <windows.h>
	#include <direct.h>
	#define GetCurrentDir _getcwd
	#pragma comment(lib, "winmm.lib")			//- not required but have it in anyway
	#pragma comment(lib, "libs/glut32.lib")
	
	void sleep(unsigned milliseconds) {
		Sleep(milliseconds);
	}
#elif __APPLE__
	#include <GLUT/glut.h>
	#include <unistd.h>
	#define GetCurrentDir getcwd

	void sleep(unsigned milliseconds)
	{
		usleep(milliseconds * 1000); // takes microseconds
	}
#elif __unix__		// all unices including  __linux__
	#include <GL/glut.h>
	#include <unistd.h>
	#define GetCurrentDir getcwd

	void sleep(unsigned milliseconds)
	{
		usleep(milliseconds * 1000); // takes microseconds
	}
#endif

//====== Macros and Defines =========

#define FRAME_WIDE	1000
#define FRAME_HIGH	600
#define COLOUR_MAX 255

#define DRAW_DISTANCE_CONSTANT 500

#define ROUND(x) ((int)(x+0.5))
#define ABS(x) ((x)<0 ? -(x) : (x))
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

// Vertex Join Sets
#define MaxNumObjs 20
#define MaxNumPts 600
#define MaxNumPolys 900
#define NumSidesPoly 4

// Converts degrees to radians.
#define D2R(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define R2D(angleRadians) (angleRadians * 180.0 / M_PI)
//====== Structs & typedefs =========

typedef unsigned char BYTE;
typedef struct POINT2D {int x, y;}p2d_t,*P2D;

// Vertex Join Set Stuff

typedef struct _Colour { unsigned char r, g, b; } colour_t, *colour;

typedef struct _Point_3D { int x, y, z; unsigned char r, g, b; }Point_3D, Points_3D[MaxNumPts];

typedef struct _Polygon { int Vertices[NumSidesPoly]; } Polygon_t, Polygons[MaxNumPolys];

typedef struct _ObjectAttribs { BYTE Colour; Point_3D Center, Scale; } ObjectAttribs, ObjAttrArray[MaxNumObjs];

typedef struct _Object { ObjectAttribs ObjProps; int NumPtsObj, NumPolysObj; Polygons ObjectPolys; Points_3D ObjectPoints; } Object, *obj;


//====== Global Variables ==========

char file_opened = 0;

int movespeed = 10;

char fname[FILENAME_MAX] = "test.vjs";

BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];

int	ZBuffer[FRAME_WIDE * FRAME_HIGH];

int		shade = 0;
POINT2D	xypos = {0,0};
int		stereo = 0;
int		eyes = 10;

//rect_t c = { 0,0,FRAME_HIGH,FRAME_WIDE };

poly bound = poly_new();

vec_t bound_p1 = { 0.0f,0.0f };
vec_t bound_p2 = { 0.0f,FRAME_WIDE };
vec_t bound_p3 = { FRAME_HIGH,FRAME_WIDE };
vec_t bound_p4 = { FRAME_HIGH,0.0f};

// 
Object o_t;
obj o = &o_t;

//===== Forward Declarations ========
void ClearScreen();
void DrawFrame();
void Interlace(BYTE* pL, BYTE* pR);
void PlaySoundEffect(char * filename);
void BuildFrame(BYTE *pFrame, int view);
void OnIdle(void);
void OnDisplay(void);
void reshape(int w, int h);
void OnMouse(int button, int state, int x, int y);
void OnKeypress(unsigned char key, int x, int y);
void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, BYTE*frame, int view);
void drawLine(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b, BYTE* screen, int view);
int sameSide(int x1, int y1, int x2, int y2, int l1x, int l1y, int l2x, int l2y);
int inside(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
void fillTriangle(vec A, vec B, vec C,BYTE*screen, int view);
void fillTriangleSort(vec A, vec B, vec C,BYTE*screen, int view);
void IdentifyConcave(poly start,BYTE*screen, int view);
vec xyztoij(int x, int y, int z, vec res);
void printObject(obj o);
void find_centre_3d(obj o);
void translate_obj(obj o, int dx, int dy, int dz);
void scale_obj(obj o, double xscale, double yscale, double zscale);
void rotate_obj(obj o, int dx, int dy, int dz);
void loadVJS(FILE * f, obj o);
void DrawVJS(obj o, BYTE*screen, int view);
void testDrawVJS(BYTE*screen, int view);
void move_to_origin(obj o);


////////////////////////////////////////////////////////
// Program Entry Poinr
////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	// Assign Boundary Polygon
	poly_append(bound, &bound_p1);
	poly_append(bound, &bound_p2);
	poly_append(bound, &bound_p3);
	poly_append(bound, &bound_p4);

	//-- setup GLUT --
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);	//GLUT_3_2_CORE_PROFILE |
	glutInitWindowSize(FRAME_WIDE, FRAME_HIGH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	//--- set openGL state --
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//-- register call back functions --
	glutIdleFunc(OnIdle);
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(OnKeypress);
	glutMouseFunc(OnMouse);

	//-- run the program
	glutMainLoop();
	return 0;
}

////////////////////////////////////////////////////////
// Event Handers
////////////////////////////////////////////////////////
      
void OnIdle(void)
{
	DrawFrame();
	glutPostRedisplay();
}


void OnDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(FRAME_WIDE, FRAME_HIGH, GL_RGB,GL_UNSIGNED_BYTE, (GLubyte*)pFrameR);
	glutSwapBuffers();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void OnMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		PlaySoundEffect("Laser.wav"); 
		if (++shade > 16) shade = 0;	
	}
}

void OnKeypress(unsigned char key, int x, int y)
{
	if (key == 'o') {
		FILE * f = fopen(fname, "r");
		if (f) {
			loadVJS(f, o);
		fclose(f);
		}
		else {
			perror("Error: ");
		}
		file_opened = 1;
	}
	else {
		switch (key)
		{
		case ' ': xypos.x = xypos.y = 0; break;
		case 'p': stereo ^= 1, eyes = 10; break;
		case 'w': translate_obj(o,0,movespeed,0); break;
		case 's': translate_obj(o,0,-movespeed,0); break;
		case 'a': translate_obj(o,-movespeed,0,0); break;
		case 'd': translate_obj(o,movespeed,0,0); break;
		case 'q': translate_obj(o,0,0,-movespeed); break;
		case 'e': translate_obj(o,0,0,movespeed); break;
		case 'x': move_to_origin(o); break;
		case 'r': scale_obj(o,1.1,1.1,1.1); break;
		case 'f': scale_obj(o,0.9,0.9,0.9); break;
		case '=': movespeed += 10; break;
		case '-': movespeed -= 10; break;
		case '\\': movespeed = 10; break;
		case ']': eyes++;	break;
		case '[': eyes--;	break;
		case 27: exit(0);
		}
		//PlaySoundEffect("Whoosh.wav");
	}
	
}

////////////////////////////////////////////////////////
// Utility Functions
////////////////////////////////////////////////////////


void ClearScreen()
{
	memset(pFrameL, 0, FRAME_WIDE * FRAME_HIGH * 3);
	memset(pFrameR, 0, FRAME_WIDE * FRAME_HIGH * 3);
}


void Interlace(BYTE* pL, BYTE* pR)
{
	int rowlen = 3 * FRAME_WIDE;
	for (int y = 0; y < FRAME_HIGH; y+=2)
	{
		for (int x = 0; x < rowlen; x++) *pR++ = *pL++;
		pL += rowlen;
		pR += rowlen;
	}
}


void DrawFrame()
{
	ClearScreen();
	
	if (!stereo) BuildFrame(pFrameR, 0);
	else {
		BuildFrame(pFrameL, -eyes);
		BuildFrame(pFrameR, +eyes);
		Interlace((BYTE*)pFrameL, (BYTE*)pFrameR);
	}
}

void	PlaySoundEffect(char * filename)		
{
#ifdef _WIN32
	PlaySound(filename, NULL, SND_ASYNC | SND_FILENAME ); 
#else
	char command[80];
	#ifdef __APPLE__
		s//printf(command, "afplay %s &", filename);
	#else
		s//printf(command, "play %s &", filename);
	#endif	
	system(command);
#endif
}

////////////////////////////////////////////////////////
// Drawing Function
////////////////////////////////////////////////////////

int checkZBuf(int x, int y, int z, int view) {
	if (ZBuffer[x + view + (y*FRAME_WIDE)] < z) {
		ZBuffer[x + view + (y*FRAME_WIDE)] = z;
		return 1;
	}
	else return 0;
}

// lazy collision detection
void setPixel(int x,int y, unsigned char r, unsigned char g, unsigned char b, BYTE* screen, int view){
	int channels = 3;
	int colour = 0;
	if (x > 0 && x < FRAME_WIDE && y > 0 && y < FRAME_HIGH) {
		screen[channels*((x + view + (y * FRAME_WIDE))) + 0] = r;
		screen[channels*((x + view + (y * FRAME_WIDE))) + 1] = g;
		screen[channels*((x + view + (y * FRAME_WIDE))) + 2] = b;
	}
}

void drawLine(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen, int view) {
	
	int nx1=x1, ny1=y1, nx2=x2, ny2=y2;
	
	//if(clip_line(&c, &nx1, &ny1, &nx2, &ny2)){
		int dx = nx2 - nx1;
		int dy = ny2 - ny1;
		int steps;
	
		if (abs(dx) > abs(dy)) steps = abs(dx);
		else steps = abs(dy);

		double x_inc = dx / (double)steps;
		double y_inc = dy / (double)steps;
	
		double x = x1;
		double y = y1;

		setPixel(ROUND(x), ROUND(y),r,g,b,screen, view);
		for (int i = 0;i < steps;i++, x += x_inc, y += y_inc) setPixel(ROUND((int)x),ROUND((int)y), r, g, b,screen,view);
	//}
}

// SameSide(x1,y1,x2,y2,l1x,l1y,l2x,l2y): int
// l1x, l1y and l2x, l2y are the end points of the line
// Returns 1 if point (x1,y1) and point (x2,y2) are on the same side of the line
int sameSide(int x1, int y1, int x2, int y2, int l1x, int l1y, int l2x, int l2y) {
	int apt = (x1 - l1x)*(l2y - l1y) - (l2x - l1x)*(y1 - l1y);
	int bpt = (x2 - l1x)*(l2y - l1y) - (l2x - l1x)*(y2 - l1y);
	return ((apt*bpt) > 0);
}

// Inside (x1,y1,x2,y2,x3,y3,x4,y4): int
// Generalised Triangle Inside Test, checks if a given point (x1,y1) is inside a triangle with points (x2,y2),(x3,y3),(x4,y4)
// Returns 1 if it is inside, 0 if it is outside
int inside(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
	return sameSide(x1, y1, x2, y2, x3, y3, x4, y4)
		&& sameSide(x1, y1, x3, y3, x2, y2, x4, y4)
		&& sameSide(x1, y1, x4, y4, x2, y2, x3, y3);
}

void fillTriangle(vec A, vec B, vec C, BYTE*screen, int view) {

	printf("%f,%f,%f",A->y,B->y,C->y);

	double dx1, dx2, dx3;
	double dr, dg, db;
	double dr1, dr2, dr3, dg1, dg2, dg3, db1, db2, db3;

	if (B->y - A->y > 0) {
		dx1 = (B->x - A->x) / (B->y - A->y);
		dr1 = (B->r - A->r) / (B->y - A->y);
		dg1 = (B->g - A->g) / (B->y - A->y);
		db1 = (B->b - A->b) / (B->y - A->y);
	}else dx1=dr1=dg1=db1= 0;
	
	if (C->y - A->y > 0) {
		dx2 = (C->x - A->x) / (C->y - A->y);
		dr2 = (C->r - A->r) / (C->y - A->y);
		dg2 = (C->g - A->g) / (C->y - A->y);
		db2 = (C->b - A->b) / (C->y - A->y);
	}else dx2=dr2=dg2=db2 = 0;
	
	if (C->y - B->y > 0) {
		dx3 = (C->x - B->x)	/ (C->y - B->y);
		dr3 = (C->r - B->r) / (C->y - B->y);
		dg3 = (C->g - B->g) / (C->y - B->y);
		db3 = (C->b - B->b) / (C->y - B->y);
	} else dx3=dr3=dg3=db3 = 0;
	
	vec_t S = {A->x,A->y,A->r,A->g,A->b}, 
		  E = {A->x,A->y,A->r,A->g,A->b};

	if (dx1 > dx2) {
		printf("dx1 > dx2\n");
		for (; S.y <= B->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (E.r - S.r) / (E.x - S.x);
				dg = (E.g - S.g) / (E.x - S.x);
				db = (E.b - S.b) / (E.x - S.x);
			}else dr = dg = db = 0;
			
			vec_t P = {S.x,S.y,S.r,S.g,S.b};
			
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, P.r, P.g, P.b, screen, view);
				P.r += dr; P.g += dg; P.b += db;
			}
			S.x += dx2; S.r += dr2; S.g += dg2; S.b += db2;
			E.x += dx1; E.r += dr1; E.g += dg1; E.b += db1;
		}
		
		E.x = B->x;
		E.y = B->y;
		E.r = B->r;
		E.g = B->g;
		E.b = B->b;
		
		for (; S.y <= C->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (E.r - S.r) / (E.x - S.x);
				dg = (E.g - S.g) / (E.x - S.x);
				db = (E.b - S.b) / (E.x - S.x);
			}else dr = dg = db = 0;
			
			vec_t P = { S.x,S.y,S.r,S.g,S.b };
			
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, P.r, P.g, P.b, screen, view);
				P.r += dr; P.g += dg; P.b += db;
			}
			S.x += dx2; S.r += dr2; S.g += dg2; S.b += db2;
			E.x += dx3; E.r += dr3; E.g += dg3; E.b += db3;
		}
	}
	else {
		printf("dx1 < dx2\n");
		for (; S.y <= B->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (E.r - S.r) / (E.x - S.x);
				dg = (E.g - S.g) / (E.x - S.x);
				db = (E.b - S.b) / (E.x - S.x);
			}else dr = dg = db = 0;
			
			vec_t P = { S.x,S.y,S.r,S.g,S.b };
			
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, P.r, P.g, P.b, screen, view);
				P.r += dr; P.g += dg; P.b += db;
			}
			
			S.x += dx1; S.r += dr1; S.g += dg1; S.b += db1;
			E.x += dx2; E.r += dr2; E.g += dg2; E.b += db2;
		}
		S.x = B->x;
		S.y = B->y;
		S.r = B->r;
		S.g = B->g;
		S.b = B->b;
		for (; S.y <= C->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (E.r - S.r) / (E.x - S.x);
				dg = (E.g - S.g) / (E.x - S.x);
				db = (E.b - S.b) / (E.x - S.x);
			}else dr = dg = db = 0;
			
			vec_t P = { S.x,S.y,S.r,S.g,S.b };
			
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, P.r, P.g, P.b, screen, view);
				P.r += dr; P.g += dg; P.b += db;
			}
			
			S.x += dx3; S.r += dr3; S.g += dg3; S.b += db3;
			E.x += dx2; E.r += dr2; E.g += dg2; E.b += db2;
		}

	}

}

void fillTriangleSort(vec A, vec B, vec C,BYTE*screen, int view) {
	vec a = A, b = B, c = C;
	
	if (a->y > c->y) {
		a = C;
		c = A;
	}
	if (a->y > b->y) {
		a = B;
		b = A;
	}
	if (b->y > c->y) {
		b = C;
		c = B;
	}

	fillTriangle(a, b, c,screen, view);
}

// DrawPoly(): Void
// Accepts an array 'points' of size 'n' and draws the resulting polygon using triangle decomposition.
void DrawPoly(poly p,BYTE*screen, int view) {
	//printf("%i\n", p->len);

	while (p->len > 3) {
		// l = left point, c = centre point, r = right point, off = offset
		int lp = 0, cp = 1, rp = 2, off = 0;
		for (int h = 0; h < p->len; h++) {
			int valid = 0;
			for (int i = 0; i < p->len; i++) {
				//printf("%i,%i,%i,%i\n", lp + off, cp + off, rp + off, i);
				if (i != lp && i != cp && i != rp) {
					if (triangle_inside(&p->v[i], &p->v[lp + off], &p->v[cp + off], &p->v[rp + off])) {
						//printf("Point inside triangle\n");
						continue;
					}
					else {
						//fillTriangleSort(&p->v[lp + off], &p->v[cp + off], &p->v[rp + off],screen,view);
						printf("Drawing Triangle at (%f,%f),(%f,%f),(%f,%f)\n", p->v[lp + off].x, p->v[lp + off].y, p->v[cp + off].x, p->v[cp + off].y, p->v[rp + off].x, p->v[rp + off].y);
						drawLine(p->v[lp + off].x, p->v[lp + off].y, p->v[cp + off].x, p->v[cp + off].y, 0, 255, 255, screen, view);
						drawLine(p->v[cp + off].x, p->v[cp + off].y, p->v[rp + off].x, p->v[rp + off].y, 255, 0, 255, screen, view);
						drawLine(p->v[rp + off].x, p->v[rp + off].y, p->v[lp + off].x, p->v[lp + off].y, 255, 255, 0, screen, view);
						poly_remove(p, cp + off);
						valid = 1;
						break;
					}
				}
			}
			off++;
			if (lp > p->len)lp -= p->len;
			if (cp > p->len)cp -= p->len;
			if (rp > p->len)rp -= p->len;
			if (valid) break;
		}
	}
	//fillTriangleSort(&p->v[0], &p->v[1], &p->v[2],screen, view);
	printf("Drawing Triangle at (%f,%f),(%f,%f),(%f,%f)\n", p->v[0].x, p->v[0].y, p->v[1].x, p->v[1].y, p->v[2].x, p->v[2].y);
	drawLine(p->v[0].x,p->v[0].y,p->v[1].x,p->v[1].y,0,255,255,screen, view);
	drawLine(p->v[1].x,p->v[1].y,p->v[2].x,p->v[2].y,255,0,255,screen, view);
	drawLine(p->v[2].x,p->v[2].y,p->v[0].x,p->v[0].y,255,255,0,screen, view);
	printf("\n");
	return;
}

void IdentifyConcave(poly start,byte*screen, int view) {
	
	poly p_arr[1024];
	Queue in,out;
	
	initialise_queue(1024,&in);
	initialise_queue(1024,&out);
	
	double dx1, dy1, dx2, dy2, cross;

	int pos = 0, neg = 0;

	int convex=0, index = 0;

	push(index, &in);
	p_arr[index++] = start;

	while (in.itemc > 0) {

		for (int i = 0; i < in.itemc; i++) {
			//printf("%i,",in.items[i]);
		}
		//printf("\n");
		poly p = p_arr[pop(&in)];

		int * crossarr;
		crossarr = (int*)malloc(p->len * sizeof(crossarr));

		for (int i = 0; i < p->len; i++) {
			if (i == 0) {
				dx1 = p->v[i].x - p->v[p->len - 1].x;
				dy1 = p->v[i].y - p->v[p->len - 1].y;
				dx2 = p->v[i + 1].x - p->v[i].x;
				dy2 = p->v[i + 1].y - p->v[i].y;
			}
			else if (i == p->len - 1) {
				dx1 = p->v[i].x - p->v[i - 1].x;
				dy1 = p->v[i].y - p->v[i - 1].y;
				dx2 = p->v[0].x - p->v[i].x;
				dy2 = p->v[0].y - p->v[i].y;
			}
			else {
				dx1 = p->v[i].x - p->v[i - 1].x;
				dy1 = p->v[i].y - p->v[i - 1].y;
				dx2 = p->v[i + 1].x - p->v[i].x;
				dy2 = p->v[i + 1].y - p->v[i].y;
			}
			cross = dx1 * dy2 - dy1 * dx2;
			//printf("%f\n", cross);
			//(cross < 0) ? neg++ : pos++;
			if (cross < 0) {
				neg++;
				crossarr[i] = -1;
			}
			else {
				pos++;
				crossarr[i] = 1;
			}
		}
		//printf("pos: %i neg: %i len: %i\n", pos, neg, p->len);
		if (pos == p->len || neg == p->len) {
			//printf("Drawing Polygon!\n");
			DrawPoly(p,screen,view);
		}

		else {
			for (int i = 0; i < p->len; i++) {
				if (pos > neg) {
					if (crossarr[i] < 0) {
						convex = i;
						break;
					}
				}
				else {
					if (crossarr[i] > 0) {
						convex = i;
						break;
					}
				}
			}
			//printf("Convex Point: %i\n", convex);
			// grab vertex (v1) where angle is found
			vec v1 = &p->v[convex], v2;
			int v1_i = convex, v2_i;
			// pick other vertex (v2) which does not intersect with other line (line_sect)
			for (int j = 0; j < p->len; j++) {
				if (j == convex || j == convex - 1 || j == convex + 1)continue;
				if ((convex - 1 < 0 && j == p->len - 1) || (convex + 1 > p->len && j == 0))continue;
				v2 = &p->v[j];
				v2_i = j;
				int valid = 1;
				//printf("Attempting (%f,%f) to (%f,%f)\n", v1->x, v1->y, v2->x, v2->y);
				// make sure it doesnt cross over any lines
				for (int k = 0; k < p->len; k++) {
					if (k == convex || k == convex - 1 || k == convex + 1)continue;
					if ((convex - 1 < 0 && k == p->len - 1) || (convex + 1 > p->len && j == 0))continue;
					if (k == j || k == j - 1 || k == j + 1)continue;
					if ((j - 1 < 0 && k == p->len - 1) || (j + 1 > p->len && j == 0))continue;
					if (k == p->len - 1) {
						//printf("Checking for intersection between (%f,%f),(%f,%f) and (%f,%f),(%f,%f)\n", v1->x, v1->y, v2->x, v2->y, p->v[k].x, p->v[k].y, p->v[0].x, p->v[0].y);
						if (line_sect_ignore_edge(v1, v2, &p->v[k], &p->v[0])) {
							valid = 0;
							//drawLine(v1->x, v1->y, v2->x, v2->y, 255, 255, 0, screen);
							//drawLine(p->v[k].x, p->v[k].y, p->v[0].x, p->v[0].y, 0, 255, 255, screen);
							break;
						}
					}
					else {
						if (line_sect_ignore_edge(v1, v2, &p->v[k], &p->v[k + 1])) {
							valid = 0;
							break;
						}
					}
				}
				if (valid) {
					poly p1 = poly_new(), p2 = poly_new();		
					int i = v1_i;
					while (i != v2_i) {
						if (i >= p->len)i = 0;
						if (i == v2_i) break;
						poly_append(p1, &p->v[i]);
						i++;
					}
					poly_append(p1, v2);

					i = v2_i;
					while (i != v1_i) {
						if (i >= p->len)i = 0;
						if (i == v1_i)break;
						poly_append(p2, &p->v[i]);
						i++;
					}
					poly_append(p2, v1);

					push(index, &in);
					p_arr[index++] = p1;
					
					push(index, &in);
					p_arr[index++] = p2;
					
					break;
				}
			}
		}
		free(crossarr);
	}

	for (int i = 0; i < index; i++) {
		poly_free(p_arr[i]);
	}
	
}

vec xyztoij(int x, int y, int z,vec res) {
	res->x = (DRAW_DISTANCE_CONSTANT*(x-(FRAME_WIDE)/2) / (z + DRAW_DISTANCE_CONSTANT));
	res->y = (DRAW_DISTANCE_CONSTANT*(y-(FRAME_HIGH)/2) / (z + DRAW_DISTANCE_CONSTANT));
	res->x += (FRAME_WIDE / 2);
	res->y += (FRAME_HIGH / 2);
	return res;
}

void printObject(obj o) {
	printf("%d,%d\n", o->NumPtsObj, o->NumPolysObj);
	for (int i = 0; i < o->NumPtsObj;i++) {
		printf("(%d,%d,%d,%d,%d,%d)\n", o->ObjectPoints[i].x, o->ObjectPoints[i].y, o->ObjectPoints[i].z, o->ObjectPoints[i].r, o->ObjectPoints[i].g, o->ObjectPoints[i].b);
	}
	for (int i = 0;i < o->NumPolysObj;i++) {
		printf("(%d,%d,%d,%d)\n", o->ObjectPolys[i].Vertices[0], o->ObjectPolys[i].Vertices[1], o->ObjectPolys[i].Vertices[2], o->ObjectPolys[i].Vertices[3]);
	}
}

void find_centre_3d(obj o) {
	int cx=0, cy=0, cz=0;
	for (int i = 0; i < o->NumPtsObj; i++) {
		cx += o->ObjectPoints[i].x;
		cy += o->ObjectPoints[i].y;
		cz += o->ObjectPoints[i].z;
	}
	cx /= o->NumPtsObj;
	cy /= o->NumPtsObj;
	cz /= o->NumPtsObj;
	o->ObjProps.Center = {cx,cy,cz,0,0,0};
}

void translate_obj(obj o, int dx, int dy, int dz) {
	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x += dx;
		o->ObjectPoints[i].y += dy;
		o->ObjectPoints[i].z += dz;
	}
	o->ObjProps.Center.x += dx;
	o->ObjProps.Center.y += dy;
	o->ObjProps.Center.z += dz;
}

void move_to_origin(obj o) {
	translate_obj(o, +o->ObjProps.Center.x, +o->ObjProps.Center.y, +o->ObjProps.Center.z);
}

void scale_obj(obj o, double xscale, double yscale, double zscale) {
	double tx, ty, tz;
	translate_obj(o, -o->ObjProps.Center.x, -o->ObjProps.Center.y, -o->ObjProps.Center.z);

	for (int i = 0; i < o->NumPtsObj; i++) {
		o->ObjectPoints[i].x *= xscale;
		o->ObjectPoints[i].y *= yscale;
		o->ObjectPoints[i].z *= zscale;
	}

	translate_obj(o,o->ObjProps.Center.x,o->ObjProps.Center.y,o->ObjProps.Center.z);
}

void rotate_obj(obj o, int dx, int dy, int dz) {

}

void loadVJS(FILE * f, obj o) {
	
	if (f) {
		int stage = 0;

		int pt = 0, polys = 0;

		char line[256];
		char line_dup[256];
		char *tok;
		while (fgets(line, sizeof(line), f)) {
			strcpy(line_dup, line);
			if (line[0]=='/' && line[1]=='/') {
				stage++;
			}
			else if (line[0] == ' ' && line[1] == ' ') {

			}
			else {
				if (stage == 1) {
					tok = strtok(line_dup,",\n");
					//printf("%s\n",tok);
					if(tok) o->NumPtsObj = atoi(tok);
					tok = strtok(NULL,",\n");
					//printf("%s\n", tok);
					if(tok) o->NumPolysObj = atoi(tok);
					//printf("%d,%d", o.NumPtsObj, o.NumPolysObj);
				}
				if (stage == 2) {
					if (pt < o->NumPtsObj){
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
					int sides=0;
					tok = strtok(line_dup, " \n");
					if (tok) {
						sides = atoi(tok);
					}
					tok = strtok(NULL, " \n");
					for (int i = 0;i < sides;i++) {
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
		find_centre_3d(o);
	}
	else return;
}

void DrawVJS(obj o,BYTE*screen,int view) {

	vec_t ij;

	for (int i = 0; i < o->NumPolysObj; i++) {
		
		Polygon_t * obj = &o->ObjectPolys[i];
		
		poly p = poly_new();

		for (int j = 0; j < NumSidesPoly; j++) {
			xyztoij(o->ObjectPoints[obj->Vertices[j]].x, o->ObjectPoints[obj->Vertices[j]].y, o->ObjectPoints[obj->Vertices[j]].z, &ij);
			ij.r = o->ObjectPoints[obj->Vertices[j]].r;
			ij.g = o->ObjectPoints[obj->Vertices[j]].g;
			ij.b = o->ObjectPoints[obj->Vertices[j]].b;
			poly_append(p,&ij);
		}
		poly_remove_duplicates(p);
		IdentifyConcave(p,screen,view);
	}
}

void testDrawVJS(BYTE*screen,int view) {
	Object o;
	o.NumPolysObj = 6;
	o.NumPtsObj = 8;
	o.ObjectPolys[0] = {{3,2,1,0}};
	o.ObjectPolys[1] = {{0,7,4,3}};
	o.ObjectPolys[2] = {{5,4,3,2}};
	o.ObjectPolys[3] = {{1,2,5,6}};
	o.ObjectPolys[4] = {{0,1,6,7}};
	o.ObjectPolys[5] = {{4,5,6,7}};
	o.ObjectPoints[0] = {100,100,100,255,0,128};
	o.ObjectPoints[1] = {100,100,200,128,0,0};
	o.ObjectPoints[2] = {200,100,200,255,255,255};
	o.ObjectPoints[3] = {200,100,100,0,0,255};
	o.ObjectPoints[4] = {200,200,100,255,255,0};
	o.ObjectPoints[5] = {200,200,200,128,128,0};
	o.ObjectPoints[6] = {100,200,200,255,0,128};
	o.ObjectPoints[7] = {100,200,100,0,255,255};
	DrawVJS(&o, screen, view);
}

void BuildFrame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety
	
	if (file_opened) {
		DrawVJS(o, screen, view);
		//printObject(o);
	}
	//getchar();
	
	vec_t res;
	xyztoij(o->ObjProps.Center.x, o->ObjProps.Center.y, o->ObjProps.Center.z,&res);
	setPixel(res.x, res.y,255, 255, 255, screen, view);
	
	printf("Next Frame From Here \n");
	

}