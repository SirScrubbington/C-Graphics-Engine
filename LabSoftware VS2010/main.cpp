#include <stdlib.h>			//- for exit()
#include <stdio.h>			//- for s//printf()
#include <string.h>			//- for memset()
#include <math.h>			//- for round()
#include <cmath>
#include "vector.h"
#include "queue.h"

//#include "liangbarsky.h"
//#include "sutherlandhodgeman.h"

#define M_PI acos(-1)

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

#define DRAW_DISTANCE_CONSTANT 800

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

typedef struct _Object { ObjectAttribs ObjProps; int NumPtsObj, NumPolysObj; Polygons ObjectPolys; Points_3D ObjectPoints; } Object;


//====== Global Variables ==========
BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];
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

/*
#ifdef WIN32
	//- eliminate flashing --
	typedef void (APIENTRY * PFNWGLEXTSWAPCONTROLPROC) (int i);
	PFNWGLEXTSWAPCONTROLPROC wglSwapControl = NULL;
	wglSwapControl = (PFNWGLEXTSWAPCONTROLPROC) wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapControl != NULL) wglSwapControl(1); 
#endif
*/

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
	switch (key) 
	{ 
	case ' ': xypos.x = xypos.y = 0; break;
	case 's': stereo ^= 1, eyes = 10;break;
	case ']': eyes++;	break;
	case '[': eyes--;	break;
	case 27 : exit(0);
	}
	PlaySoundEffect("Whoosh.wav"); 
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

// lazy collision detection
void setPixel(int x,int y, unsigned char r, unsigned char g, unsigned char b, BYTE* screen){
	int channels = 3;
	int colour = 0;
	if (x > 0 && x < FRAME_WIDE && y > 0 && y < FRAME_HIGH) {
		screen[channels*((x + (y * FRAME_WIDE))) + 0] = r;
		screen[channels*((x + (y * FRAME_WIDE))) + 1] = g;
		screen[channels*((x + (y * FRAME_WIDE))) + 2] = b;
	}
	
}

void drawLine(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen) {
	
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

		setPixel(ROUND(x), ROUND(y),r,g,b,screen);
		for (int i = 0;i < steps;i++, x += x_inc, y += y_inc) setPixel(ROUND((int)x),ROUND((int)y), r, g, b,screen);
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

void fillTriangle(vec A, vec B, vec C, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	// sort triangle

	double dx1, dx2, dx3;

	if (B->y - A->y > 0) dx1 = (B->x - A->x) / (B->y - A->y); else dx1 = 0;
	if (C->y - A->y > 0) dx2 = (C->x - A->x) / (C->y - A->y); else dx2 = 0;
	if (C->y - B->y > 0) dx3 = (C->x - B->x) / (C->y - B->y); else dx3 = 0;

	vec_t S = {A->x,A->y}, E = {A->x,A->y};

	if (dx1 > dx2) {
		for (; S.y <= B->y; S.y++, E.y++, S.x += dx2, E.x += dx1) {
			drawLine(S.x, S.y, E.x, E.y, r, g, b, screen);
		}
		E.x = B->x;
		E.y = B->y;
		for (; S.y <= C->y; S.y++, E.y++, S.x += dx2, E.x += dx3) {
			drawLine(S.x, S.y, E.x, E.y, r, g, b, screen);
		}
	}
	else {
		for (; S.y <= B->y; S.y++, E.y++, S.x += dx1, E.x += dx2)
			drawLine(S.x, S.y, E.x, E.y, r, g, b, screen);
		S.x = B->x;
		S.y = B->y;
		for (; S.y <= C->y; S.y++, E.y++, S.x += dx3, E.x += dx2)
			drawLine(S.x, S.y, E.x, E.y, r, g, b, screen);
	}
	
}

void fill_triangle_sort(vec A, vec B, vec C, unsigned char rc, unsigned char gc, unsigned char bc, BYTE*screen) {
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

	printf("%f,%f,%f\n",a->y,b->y,c->y);

	fillTriangle(a,b,c, rc, gc, bc, screen);
}

void fillTriangleGourad(vec A, vec B, vec C, colour cA, colour cB, colour cC, BYTE*screen) {
	// sort triangle

	double dx1, dx2, dx3;
	double dr, dg, db;
	double dr1, dr2, dr3, dg1, dg2, dg3, db1, db2, db3;

	if (B->y - A->y > 0) {
		dx1 = (B->x - A->x) / (B->y - A->y);
		dr1 = (cB->r - cA->r) / (B->y - A->y);
		dg1 = (cB->g - cA->g) / (B->y - A->y);
		db1 = (cB->b - cA->b) / (B->y - A->y);
	}
	else dx1=dr1=dg1=db1= 0;
	
	if (C->y - A->y > 0) {
		dx2 = (C->x - A->x) / (C->y - A->y);
		dr2 = (cC->r - cA->r) / (C->y - A->y);
		dg2 = (cC->g - cA->g) / (C->y - A->y);
		db2 = (cC->b - cA->b) / (C->y - A->y);
	}
	else dx2=dr2=dg2=db2 = 0;
	
	if (C->y - B->y > 0) {
		dx3 = (C->x - B->x) / (C->y - B->y);
		dr3 = (cC->r - cB->r) / (C->y - B->y);
		dg3 = (cC->g - cB->g) / (C->y - B->y);
		db3 = (cC->b - cB->b) / (C->y - B->y);
	} 
	else dx3=dr3=dg3=db3 = 0;
	
	vec_t S = { A->x,A->y }, E = { A->x,A->y };
	colour_t cS = {cA->r,cA->g,cA->b}, cE = { cA->r,cA->g,cA->b };

	if (dx1 > dx2) {
		for (; S.y <= B->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (cE.r - cS.r) / (E.x - S.x);
				dg = (cE.g - cS.b) / (E.x - S.x);
				db = (cE.b - cS.g) / (E.x - S.x);
			}else dr = dg = db = 0;
			vec_t P = {S.x,S.y};
			colour_t cP = {cS.r,cS.g,cS.b};
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, cP.r, cP.g, cP.b, screen);
				cP.r += dr; cP.g += dg; cP.b += db;
			}
			S.x += dx2; cS.r += dr2; cS.g += dg2; cS.b += db2;
			E.x += dx1; cE.r += dr1; cE.g += dg1; cE.b += db1;
		}
		E.x = B->x;
		E.y = B->y;
		cE.r = cB->r;
		cE.g = cB->g;
		cE.b = cB->b;
		for (; S.y <= C->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (cE.r - cS.r) / (E.x - S.x);
				dg = (cE.g - cS.b) / (E.x - S.x);
				db = (cE.b - cS.g) / (E.x - S.x);
			}
			else dr = dg = db = 0;
			vec_t P = { S.x,S.y };
			colour_t cP = { cS.r,cS.g,cS.b };
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, cP.r, cP.g, cP.b, screen);
				cP.r += dr; cP.g += dg; cP.b += db;
			}
			S.x += dx2; cS.r += dr2; cS.g += dg2; cS.b += db2;
			E.x += dx3; cE.r += dr3; cE.g += dg3; cE.b += db3;
		}
	}
	else {
		for (; S.y <= B->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (cE.r - cS.r) / (E.x - S.x);
				dg = (cE.g - cS.b) / (E.x - S.x);
				db = (cE.b - cS.g) / (E.x - S.x);
			}
			else dr = dg = db = 0;
			vec_t P = { S.x,S.y };
			colour_t cP = { cS.r,cS.g,cS.b };
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, cP.r, cP.g, cP.b, screen);
				cP.r += dr; cP.g += dg; cP.b += db;
			}
			S.x += dx1; cS.r += dr1; cS.g += dg1; cS.b += db1;
			E.x += dx2; cE.r += dr2; cE.g += dg2; cE.b += db2;
		}
		E.x = B->x;
		E.y = B->y;
		cE.r = cB->r;
		cE.g = cB->g;
		cE.b = cB->b;
		for (; S.y <= C->y; S.y++, E.y++) {
			if (E.x - S.x > 0) {
				dr = (cE.r - cS.r) / (E.x - S.x);
				dg = (cE.g - cS.b) / (E.x - S.x);
				db = (cE.b - cS.g) / (E.x - S.x);
			}
			else dr = dg = db = 0;
			vec_t P = { S.x,S.y };
			colour_t cP = { cS.r,cS.g,cS.b };
			for (; P.x < E.x; P.x++) {
				setPixel(P.x, P.y, cP.r, cP.g, cP.b, screen);
				cP.r += dr; cP.g += dg; cP.b += db;
			}
			S.x += dx3; cS.r += dr3; cS.g += dg3; cS.b += db3;
			E.x += dx2; cE.r += dr2; cE.g += dg2; cE.b += db2;
		}

	}

}

void FillTriangleSortGourad(vec A, vec B, vec C, colour cA, colour cB, colour cC, BYTE*screen) {
	vec a = A, b = B, c = C;
	colour ca=cA, cb=cB, cc=cC;
	
	if (a->y > c->y) {
		a = C;
		c = A;
		ca = cC;
		cc = cA;
	}
	if (a->y > b->y) {
		a = B;
		b = A;
		ca = cB;
		cb = cA;
	}
	if (b->y > c->y) {
		b = C;
		c = B;
		cb = cC;
		cc = cB;
	}

	printf("%f,%f,%f\n", a->y, b->y, c->y);

	fillTriangleGourad(a, b, c, ca, cb, cc, screen);
}


// DrawPoly(): Void
// Accepts an array 'points' of size 'n' and draws the resulting polygon using triangle decomposition.
void DrawPoly(poly p, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
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
						fill_triangle_sort(&p->v[lp + off], &p->v[cp + off], &p->v[rp + off], r, g, b, screen);
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
	fill_triangle_sort(&p->v[0], &p->v[1], &p->v[2], r, g, b, screen);
	return;
}

void IdentifyConcave(poly start, unsigned char r, unsigned char g, unsigned char b, byte*screen) {
	
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
			DrawPoly(p, r, g, b, screen);
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
			vec_t res;
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
							drawLine(v1->x, v1->y, v2->x, v2->y, 255, 255, 0, screen);
							drawLine(p->v[k].x, p->v[k].y, p->v[0].x, p->v[0].y, 0, 255, 255, screen);
							break;
						}
					}
					else {
						//printf("Checking for intersection between (%f,%f),(%f,%f) and (%f,%f),(%f,%f)\n", v1->x, v1->y, v2->x, v2->y, p->v[k].x, p->v[k].y, p->v[k + 1].x, p->v[k + 1].y);
						if (line_sect_ignore_edge(v1, v2, &p->v[k], &p->v[k + 1])) {
							valid = 0;
							//printf("Intersect found!\n");
							drawLine(v1->x, v1->y, v2->x, v2->y, 255, 255, 0, screen);
							drawLine(p->v[k].x, p->v[k].y, p->v[k + 1].x, p->v[k + 1].y, 0, 255, 255, screen);
							break;
						}
						//printf("No intersect at %i\n", k);
					}
				}
				if (valid) {

					//printf("Valid Poly Split at %i\n", convex, j);

					poly p1 = poly_new(), p2 = poly_new();

					// generate the polygons

					// poly 1			
					int i = v1_i;
					//for (int i = v1_i; i != v2_i; i++) {
					while (i != v2_i) {
						if (i >= p->len)i = 0;
						if (i == v2_i) break;
						poly_append(p1, &p->v[i]);
						i++;
					}
					poly_append(p1, v2);

					i = v2_i;
					//for (int i = v2_i; i != v1_i; i++) {
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
	res->x = (DRAW_DISTANCE_CONSTANT*x / (z + DRAW_DISTANCE_CONSTANT));
	res->y = (DRAW_DISTANCE_CONSTANT*y / (z + DRAW_DISTANCE_CONSTANT));
	return res;
}

void loadVJS(FILE * f, Object o) {
	char *buffer=0, *buf=0;
	char delimit[] = "\n\0 ";

	long length;

	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc(length);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}

	if (buffer)
	{
		buf = strtok(buffer, delimit);
		while (buf!=NULL) {
			printf("%s\n", buf);
			buf = strtok(buffer, delimit);
		}
	}
}

void DrawVJS(Object o,BYTE*screen) {

	vec_t ij;

	for (int i = 0; i < o.NumPolysObj; i++) {
		
		Polygon_t * obj = &o.ObjectPolys[i];
		
		poly p = poly_new();

		for (int j = 0; j < NumSidesPoly; j++) {
			xyztoij(o.ObjectPoints[obj->Vertices[j]].x, o.ObjectPoints[obj->Vertices[j]].y, o.ObjectPoints[obj->Vertices[j]].z, &ij);
			poly_append(p,&ij);
		}
		
		for (int i = 0; i < p->len; i++) {
			printf("(%f,%f),",p->v[i].x,p->v[i].y);
			if (i < p->len-1) drawLine(p->v[i].x, p->v[i].y, p->v[i + 1].x, p->v[i + 1].y, 255, 0, 0,screen);
			else drawLine(p->v[i].x, p->v[i].y, p->v[0].x, p->v[0].y, 255, 0, 0, screen);
		}
		printf("\n");
		poly_remove_duplicates(p);
		IdentifyConcave(p, 255, 255, 255, screen);

		//poly_free(p);
	}


	int i, j;


}

void testDrawVJS(BYTE*screen) {
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
	o.ObjectPoints[1] = {100,100,500,128,0,0};
	o.ObjectPoints[2] = {500,100,500,255,255,255};
	o.ObjectPoints[3] = {500,100,100,0,0,255};
	o.ObjectPoints[4] = {500,500,100,255,255,0};
	o.ObjectPoints[5] = {500,500,500,128,128,0};
	o.ObjectPoints[6] = {100,500,500,255,0,128};
	o.ObjectPoints[7] = {100,500,100,0,255,255};
	DrawVJS(o, screen);
}

void BuildFrame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety
	
	//char fname[FILENAME_MAX] = "sample1.vjs";
	
	//char dir[FILENAME_MAX];
	//GetCurrentDir(dir,FILENAME_MAX);

	//FILE * f = fopen(fname,"r");
	//
	//Object o;
	//if (f) {
	//	printf("File Found!\n");
	//	loadVJS(f, o);
	//	DrawVJS(o, screen);
	//	fclose(f);
	//}
	//else {
	//	//printf("Cannot open '%s'!\n, Directory: %s\n",fname,dir);
	//	perror("Error: ");
	//	//return -1;
	//	return;
	//}
	testDrawVJS(screen);
	printf("\n");
}