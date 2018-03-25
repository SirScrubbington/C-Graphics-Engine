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
void setPixel(unsigned int x,unsigned int y, unsigned char r, unsigned char g, unsigned char b, BYTE* screen){
	int channels = 3;
	int colour = 0;
	if (x > 0 && x < FRAME_WIDE && y > 0 && y < FRAME_HIGH) {
		screen[channels*((x + (y * FRAME_WIDE))) + 0] = r;
		screen[channels*((x + (y * FRAME_WIDE))) + 1] = g;
		screen[channels*((x + (y * FRAME_WIDE))) + 2] = b;
	}
	
}

void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen) {
	
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
		for (int i = 0;i < steps;i++, x += x_inc, y += y_inc) setPixel(ROUND(x), ROUND(y), r, g, b,screen);
	//}
}

void drawLineShaded(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2, BYTE* screen) {

	int dx = x2 - x1;
	int dy = y2 - y1;

	int length = sqrt(pow((double)dx, 2) + pow((double)dy, 2));

	double dr = r1, dg = g1, db = b1;

	double rdiff, gdiff, bdiff;

	rdiff = (((double)abs(r2 - r1)) / length);
	gdiff = (((double)abs(g2 - g1)) / length);
	bdiff = (((double)abs(b2 - b1)) / length);

	int steps;

	if (abs(dx) > abs(dy)) steps = abs(dx);
	else steps = abs(dy);

	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;

	double x = x1;
	double y = y1;

	setPixel(ROUND(x), ROUND(y), r1, g1, b1, screen);
	for (int i = 0;i < steps;i++, x += x_inc, y += y_inc) {
		setPixel(ROUND(x), ROUND(y),(unsigned char)dr, (unsigned char)dg, (unsigned char)db, screen);
		if (dr + rdiff < 255 && dr + rdiff > 0) dr += rdiff;
		if (dg + gdiff < 255 && dr + rdiff > 0) dg += gdiff;
		if (db + bdiff < 255 && dr + rdiff > 0) db += bdiff;
	}
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

void fillFlatTopTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	double slp1 = (v1->x - v3->x) / (v1->y - v3->y);
	if (isinf(slp1))slp1 = 0;

	double slp2 = (v1->x - v2->x) / (v1->y - v2->y);
	if (isinf(slp1))slp2 = 0;

	double curx1 = v1->x;
	double curx2 = v1->x;

	for (int y = v1->y; y <= v3->y; y++) {
		drawLine((int)curx1, y, (int)curx2, y, r, g, b, screen);
		curx1 += slp1;
		curx2 += slp2;
		//printf("%i\n", y);
	}
}

void fillFlatBottomTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	double slp1 = (v2->x - v1->x) / (v2->y - v1->y);
	if (isinf(slp1))slp1 = 0;

	double slp2 = (v3->x - v1->x) / (v3->y - v1->y);
	if (isinf(slp1))slp2 = 0;

	double curx1 = v1->x;
	double curx2 = v1->x;

	for (int y = v3->y; y >= v1->y; y--) {
		drawLine((int)curx1, y, (int)curx2, y, r, g, b, screen);
		curx1 += slp1;
		curx2 += slp2;
		//printf("%i\n", y);
	}

}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {

	int lEdge[FRAME_HIGH], rEdge[FRAME_HIGH];

	int xv[3], yv[3];

	double dv[2];

	// xv - X Values for each Vertex
	// yv - Y Values for each Vertex
	// dv - Left, Centre or Right position for Vertex

	xv[0] = x1;
	xv[1] = x2;
	xv[2] = x3;
	yv[0] = y1;
	yv[1] = y2;
	yv[2] = y3;

	// Sorts yv by height, keeping xv parallel
	if (yv[0] < yv[2]) {
		SWAP(yv[0], yv[2], int);
		SWAP(xv[0], xv[2], int);
	}
	if (yv[1] < yv[2]) {
		SWAP(yv[1], yv[2], int);
		SWAP(xv[1], xv[2], int);
	}
	if (yv[0] < yv[1]) {
		SWAP(yv[0], yv[1], int);
		SWAP(xv[0], xv[1], int);
	}

	double * mL, *mR;
	int hL, hR, sR, sL;

	dv[0] = (double)(xv[2] - xv[0]) / (double)(yv[2] - yv[0]);
	dv[1] = (double)(xv[1] - xv[0]) / (double)(yv[1] - yv[0]);

	double xL = xv[0];
	double xR = xv[0];

	// point 1 is on the left side
	if (dv[0] < dv[1]) {
		mL = dv;
		mR = dv + 1;

		sR = 2;
		sL = 1;

		hL = yv[0] - yv[2];
		hR = yv[0] - yv[1];

		// point 2 is on the left side
	}
	else {
		mL = dv + 1;
		mR = dv;

		sR = 1;
		sL = 2;

		hL = yv[0] - yv[1];
		hR = yv[0] - yv[2];
	}

	int high = yv[0] - yv[2]; // highest y value - lowest y value



	for (int y = 0; y < high; y++) {
		lEdge[y] = xL;
		rEdge[y] = xR;
		if (y == hL)*mL = (double)(xv[sL] - xv[sR]) / (double)(yv[sL] - yv[sR]);
		if (y == hR)*mR = (double)(xv[sR] - xv[sL]) / (double)(yv[sR] - yv[sL]);
		xL -= *mL;
		xR -= *mR;
		drawLine(lEdge[y], yv[0] - y, rEdge[y], yv[0] - y, r, g, b, screen);
	}
	//drawLine(x1, y1, x2, y2, 255, 0, 0, screen);
	//drawLine(x2, y2, x3, y3, 0, 255, 0, screen);
	//drawLine(x3, y3, x1, y1, 0, 0, 255, screen);
}

void fillTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	int sorted = 0;

	int sort[3];

	vec vl[3];
	vl[0] = v1;
	vl[1] = v2;
	vl[2] = v3;

	sort[0] = 1;
	sort[1] = 2;
	sort[2] = 3;

	if (v1->y > v3->y) {
		SWAP(sort[0], sort[2], int);
		SWAP(vl[0], vl[2], vec);
	}
	if (v1->y > v2->y) {
		SWAP(sort[0], sort[1], int);
		SWAP(vl[0], vl[1], vec);
	}
	if (v2->y > v3->y) {
		SWAP(sort[1], sort[2], int);
		SWAP(vl[1], vl[2], vec);
	}
	//printf("%f,%f,%f\n", v1->y, v2->y, v3->y);
	//printf("%i,%i,%i\n", sort[0], sort[1], sort[2]);

	if (v1->y == v2->y) {
		//printf("Flat Bottom\n");
		fillFlatBottomTriangle(vl[0], vl[1], vl[2], r, g, b, screen);
	}
	else if (v2->y == v3->y) {
		//printf("Flat Top\n");
		fillFlatTopTriangle(vl[0], vl[1], vl[2], r, g, b, screen);
	}
	else {
		//printf("Other Triangle\n");
		drawTriangle(vl[0]->x, vl[0]->y, vl[1]->x, vl[1]->y, vl[2]->x, vl[2]->y, r, g, b, screen);
	}
}

void drawTriangleShaded(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2, BYTE*screen) {

	int lEdge[FRAME_HIGH], rEdge[FRAME_HIGH];

	int xv[3], yv[3];

	double dv[2];

	// xv - X Values for each Vertex
	// yv - Y Values for each Vertex
	// dv - Left, Centre or Right position for Vertex

	xv[0] = x1;
	xv[1] = x2;
	xv[2] = x3;
	yv[0] = y1;
	yv[1] = y2;
	yv[2] = y3;

	// Sorts yv by height, keeping xv parallel
	if (yv[0] < yv[2]) {
		SWAP(yv[0], yv[2], int);
		SWAP(xv[0], xv[2], int);
	}
	if (yv[1] < yv[2]) {
		SWAP(yv[1], yv[2], int);
		SWAP(xv[1], xv[2], int);
	}
	if (yv[0] < yv[1]) {
		SWAP(yv[0], yv[1], int);
		SWAP(xv[0], xv[1], int);
	}

	double * mL, *mR;
	int hL, hR, sR, sL;

	dv[0] = (double)(xv[2] - xv[0]) / (double)(yv[2] - yv[0]);
	dv[1] = (double)(xv[1] - xv[0]) / (double)(yv[1] - yv[0]);

	double xL = xv[0];
	double xR = xv[0];

	// point 1 is on the left side
	if (dv[0] < dv[1]) {
		mL = dv;
		mR = dv + 1;

		sR = 2;
		sL = 1;

		hL = yv[0] - yv[2];
		hR = yv[0] - yv[1];

		// point 2 is on the left side
	}
	else {
		mL = dv + 1;
		mR = dv;

		sR = 1;
		sL = 2;

		hL = yv[0] - yv[1];
		hR = yv[0] - yv[2];
	}

	int high = yv[0] - yv[2]; // highest y value - lowest y value

	double dr = r1, dg = g1, db = b1;

	double rdiff, gdiff, bdiff;

	rdiff = (((double)abs(r2 - r1)) / high);
	gdiff = (((double)abs(g2 - g1)) / high);
	bdiff = (((double)abs(b2 - b1)) / high);

	for (int y = 0; y < high; y++) {
		lEdge[y] = xL;
		rEdge[y] = xR;
		if (y == hL)*mL = (double)(xv[sL] - xv[sR]) / (double)(yv[sL] - yv[sR]);
		if (y == hR)*mR = (double)(xv[sR] - xv[sL]) / (double)(yv[sR] - yv[sL]);
		xL -= *mL;
		xR -= *mR;

		if (dr + rdiff < 255 && dr + rdiff > 0) dr += rdiff;
		if (dg + gdiff < 255 && dr + rdiff > 0) dg += gdiff;
		if (db + bdiff < 255 && dr + rdiff > 0) db += bdiff;

		drawLine(lEdge[y], yv[0] - y, rEdge[y], yv[0] - y, (unsigned char)dr, (unsigned char)dg, (unsigned char)db, screen);
	}

	//drawLine(x1, y1, x2, y2, 255, 0, 0, screen);
	//drawLine(x2, y2, x3, y3, 0, 255, 0, screen);
	//drawLine(x3, y3, x1, y1, 0, 0, 255, screen);

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
						fillTriangle(&p->v[lp + off], &p->v[cp + off], &p->v[rp + off], r, g, b, screen);
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
	fillTriangle(&p->v[0], &p->v[1], &p->v[2], r, g, b, screen);
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
	int verts, polys;
	fscanf_s(f, "%i", &verts);
	fscanf_s(f, "%i", &polys);

	for (int i = 0; i < verts; i++) {
		int x, y, z, r, g, b;
		fscanf_s(f, "%i", &x);
		fscanf_s(f, "%i", &y);
		fscanf_s(f, "%i", &z);
		fscanf_s(f, "%i", &r);
		fscanf_s(f, "%i", &g);
		fscanf_s(f, "%i", &b);
		o.ObjectPoints[i] = {x,y,z,(unsigned char)r,(unsigned char)g,(unsigned char)b};
	}

	for (int i = 0; i < polys; i++) {
		int pvert, v1;
		fscanf_s(f, "%i", &pvert);
		for (int j = 0; j < pvert; j++) {
			fscanf_s(f, "%i", &o.ObjectPolys[i].Vertices[j]);
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
		
		IdentifyConcave(p, 255, 255, 255, screen);

		poly_free(p);
	}


	int i, j;


}

void BuildFrame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety
	
	//char fname[FILENAME_MAX] = "sample1.vjs";
	
	FILE * f = fopen("/sample1.vjs","r");
	
	Object o;
	if (f) {
		loadVJS(f, o);
		DrawVJS(o, screen);
	}
	else {
		printf("Cannot open '%s'!\n", "/sample1.vjs");
	}
}