#include <stdlib.h>			//- for exit()
#include <stdio.h>			//- for sprintf()
#include <string.h>			//- for memset()
#include <math.h>			//- for round()
#include <cmath>
#include "vector.h"

#define M_PI acos(-1)

#ifdef _WIN32
	#include "libs/glut.h"
	#include <windows.h>
	#pragma comment(lib, "winmm.lib")			//- not required but have it in anyway
	#pragma comment(lib, "libs/glut32.lib")
	
	void sleep(unsigned milliseconds) {
		Sleep(milliseconds);
	}
#elif __APPLE__
	#include <GLUT/glut.h>
	#include <unistd.h>

	void sleep(unsigned milliseconds)
	{
		usleep(milliseconds * 1000); // takes microseconds
	}
#elif __unix__		// all unices including  __linux__
	#include <GL/glut.h>
	#include <unistd.h>

	void sleep(unsigned milliseconds)
	{
		usleep(milliseconds * 1000); // takes microseconds
	}
#endif

//====== Macros and Defines =========

#define FRAME_WIDE	1000
#define FRAME_HIGH	600
#define COLOUR_MAX 255

#define ROUND(x) ((int)(x+0.5))
#define ABS(x) ((x)<0 ? -(x) : (x))
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

// Converts degrees to radians.
#define D2R(angleDegrees) (angleDegrees * M_PI / 180.0)

// Converts radians to degrees.
#define R2D(angleRadians) (angleRadians * 180.0 / M_PI)
//====== Structs & typedefs =========
typedef unsigned char BYTE;
struct POINT2D {int x, y;};

//====== Global Variables ==========
BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];
int		shade = 0;
POINT2D	xypos = {0,0};
int		stereo = 0;
int		eyes = 10;

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
		sprintf(command, "afplay %s &", filename);
	#else
		sprintf(command, "play %s &", filename);
	#endif	
	system(command);
#endif
}

////////////////////////////////////////////////////////
// Drawing Function
////////////////////////////////////////////////////////

void setPixel(unsigned int x,unsigned int y, unsigned char r, unsigned char g, unsigned char b, BYTE* screen){
	int channels = 3;
	int colour = 0;
	screen[channels*((x + (y * FRAME_WIDE)))+0] = r;
	screen[channels*((x + (y * FRAME_WIDE)))+1] = g;
	screen[channels*((x + (y * FRAME_WIDE)))+2] = b;
}

void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int steps;
	
	if (abs(dx) > abs(dy)) steps = abs(dx);
	else steps = abs(dy);

	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;
	
	double x = x1;
	double y = y1;

	setPixel(ROUND(x), ROUND(y),r,g,b,screen);
	for (int i = 0;i < steps;i++, x += x_inc, y += y_inc) setPixel(ROUND(x), ROUND(y), r, g, b,screen);

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

void fillFlatTopTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	float slp1 = (v3->x - v1->x) / (v3->y - v1->y);
	float slp2 = (v3->x - v2->x) / (v3->y - v2->y);

	float curx1 = v3->x;
	float curx2 = v3->x;

	for (int y = v3->y; y <= v1->y; y++) {
		drawLine((int)curx1, y, (int)curx2, y, r, g, b, screen);
		curx1 -= slp1;
		curx2 -= slp2;
	}
}

void fillFlatBottomTriangle(vec v1, vec v2, vec v3,unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	float slp1 = (v2->x - v1->x) / (v2->y - v1->y);
	float slp2 = (v3->x - v1->x) / (v3->y - v1->y);

	float curx1 = v1->x;
	float curx2 = v1->x;

	for (int y = v1->y; y <= v2->y; y++) {
		drawLine((int)curx1, y, (int)curx2, y, r, g, b, screen);
	}
	curx1 += slp1;
	curx2 += slp2;
}

void fillTriangle(vec v1, vec v2, vec v3, unsigned char r, unsigned char g, unsigned char b, BYTE*screen) {
	int sorted = 0;
	
	int sort[3];
	
	sort[0] = 1;
	sort[1] = 2;
	sort[2] = 3;

	if (v1->y < v3->y)SWAP(sort[0], sort[2], int);
	if (v1->y < v2->y)SWAP(sort[0], sort[1], int);
	if (v2->y < v3->y)SWAP(sort[1], sort[2], int);

	printf("%f,%f,%f\n",v1->y,v2->y,v3->y);
	printf("%i,%i,%i\n",sort[0],sort[1],sort[2]);

	if (v2->y == v3->y) {
		fillFlatBottomTriangle(v1, v2, v3, r, g, b, screen);
	}
	if (v1->y == v2->y) {
		fillFlatTopTriangle(v1, v2, v3, r, g, b, screen);
	}
	else {
		vec_t vector4;
		vec v4 = &vector4;
		v4->x = (int)(v1->x + ((float)(v2->y - v1->y) / (float)(v3->y - v1->y))*(v3->x - v1->x));
		v4->y = v2->y;
		fillFlatBottomTriangle(v1,v2,v4,r,g,b,screen);
		fillFlatTopTriangle(v2,v4,v3,r,g,b,screen);
	}
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b,BYTE*screen){
	
	int lEdge[FRAME_HIGH],rEdge[FRAME_HIGH];

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

	double * mL, * mR;
	int hL, hR,sR,sL;

	dv[0] = (double)(xv[2]-xv[0]) / (double)(yv[2] - yv[0]);
	dv[1] = (double)(xv[1]-xv[0]) / (double)(yv[1] - yv[0]);

	double xL = xv[0];
	double xR = xv[0];

	// point 1 is on the left side
	if (dv[0] < dv[1]){
		mL = dv;
		mR = dv+1;

		sR = 2;
		sL = 1;

		hL = yv[0] - yv[2];
		hR = yv[0] - yv[1];

	// point 2 is on the left side
	}else{
		mL = dv+1;
		mR = dv;

		sR = 1;
		sL = 2;

		hL = yv[0] - yv[1];
		hR = yv[0] - yv[2];
	}

	int high = yv[0] - yv[2]; // highest y value - lowest y value

	if(yv[0]!=yv[1]){
		for (int y = 0; y < high; y++) {
			lEdge[y] = xL;
			rEdge[y] = xR;
			if (y == hL)*mL = (double)(xv[sL] - xv[sR]) / (double)(yv[sL] - yv[sR]);
			if (y == hR)*mR = (double)(xv[sR] - xv[sL]) / (double)(yv[sR] - yv[sL]);
			xL -= *mL;
			xR -= *mR;
			drawLine(lEdge[y], yv[0] - y, rEdge[y], yv[0] - y, r, g, b, screen);
		}
	} else {
		for (int y = 0; y < high; y++) {

		}
	}
	//drawLine(x1, y1, x2, y2, 255, 0, 0, screen);
	//drawLine(x2, y2, x3, y3, 0, 255, 0, screen);
	//drawLine(x3, y3, x1, y1, 0, 0, 255, screen);

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

// LineClipping(): Void
// Uses the Liang Barsky method to detect and remove pixels outside the draw area
void LineClipping(RECT clip,int x1,int y1,int x2,int y2){

}

// PolyClipping(): Void
// Uses the Sutherland Hodgeman polygon method to detect and remove pixels outside the draw area
void PolyClipping(){

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

// DrawPoly(): Void
// Accepts an array 'points' of size 'n' and draws the resulting polygon using triangle decomposition.
void DrawPoly(poly p, unsigned char r, unsigned char g, unsigned char b, BYTE*screen){



	printf("%i\n",p->len);

	if (p->len <= 3) {
		drawTriangle(p->v[0].x,p->v[0].y,p->v[1].x,p->v[1].y,p->v[2].x,p->v[2].y,r,g,b,screen);
		return;
	}

	// l = left point, c = centre point, r = right point, off = offset
	int lp = 0, cp = 1, rp = 2, off = 0;

	for (int i = 0; i < p->len; i++) {
		if (i != lp && i != cp && i != rp) {
			if (triangle_inside(&p->v[i], &p->v[lp+off], &p->v[cp + off], &p->v[rp + off])) {
				continue;
			}
			else {
				//drawTriangle(p->v[lp + off].x, p->v[lp + off].y, p->v[cp + off].x, p->v[cp + off].y, p->v[rp + off].x, p->v[rp + off].y, r, g, b, screen);
				fillTriangle(&p->v[lp + off],&p->v[cp + off],&p->v[rp + off],r,g,b,screen);
				poly_remove(p,cp+off);
				break;
			}
		}
	}
	DrawPoly(p, r, g, b, screen);
}

void TestDrawPoly(BYTE*screen){

	vec_t pt[7];

	int n = 7;

	pt[0].x = FRAME_WIDE * .5;
	pt[0].y = FRAME_HIGH * 0;

	pt[1].x = FRAME_WIDE * .9;
	pt[1].y = FRAME_HIGH * .2;

	pt[2].x = FRAME_WIDE * 1 - 1;
	pt[2].y = FRAME_HIGH * .6;

	pt[3].x = FRAME_WIDE * .75;
	pt[3].y = FRAME_HIGH * 1 - 1;

	pt[4].x = FRAME_WIDE * .25;
	pt[4].y = FRAME_HIGH * 1 - 1;

	pt[5].x = FRAME_WIDE * 0;
	pt[5].y = FRAME_HIGH * .6;

	pt[6].x = FRAME_WIDE * .1;
	pt[6].y = FRAME_HIGH * .2;

	poly p = poly_new();

	for (int i = 0; i < 7; i++) {
		poly_append(p,&pt[i]);
	}

	DrawPoly(p,255, 255, 255, screen);

	// Draw Polygon using lines for testing
	for (int i = 0; i<7; i++) {
		if (i == 7 - 1) {
			drawLine(pt[i].x,pt[i].y,pt[0].x,pt[0].y, 255, 0, 0, screen);
			printf("%i: Drawing (%f,%f) to (%f,%f). \n", i, pt[i].x, pt[i].y, pt[0].x, pt[0].y);
		}
		else {
			drawLine(pt[i].x, pt[i].y, pt[i+1].x, pt[i+1].y, 255, 0, 0, screen);
			printf("%i: Drawing (%f,%f) to (%f,%f). \n", i, pt[i].x, pt[i].y, pt[i + 1].x, pt[i + 1].y);
		}
	}
}

void BuildFrame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety
	//testSetPixel(screen);
	//testDrawLine(screen);
	//testDrawLineShaded(screen);
	//drawTriangle(200,200,100,100,200,100,255,255,255,screen);
	//drawTriangle(100,100,150,150,125,200,255,255,255,screen);
	//drawTriangle(50,20,80,100,100,25,255,255,255,screen);
	//testDrawTriangle(screen);
	//testDrawTriangleShaded(screen);
	TestDrawPoly(screen);
	//drawTriangle(100, 200, 200, 200, 150, 50, 255, 255, 255, screen);
	
	//vec_t v1, v2, v3;
	//v1 = {100.0f,200.0f};
	//v2 = {200.0f,200.0f};
	//v3 = {150.0f,50.0f};
	//fillTriangle(&v1,&v2,&v3, 255, 255, 255, screen);
}