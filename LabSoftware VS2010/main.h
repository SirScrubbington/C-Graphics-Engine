#pragma once

#include <stdlib.h>			//- for exit()
#include <stdio.h>			//- for s//printf()
#include <string.h>			//- for memset()
#include <math.h>			//- for round()
#include <cmath>

#include "vector.h"
#include "queue.h"
#include "common.h"

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
void fillTriangle(vec A, vec B, vec C, BYTE*screen, int view);
void fillTriangleSort(vec A, vec B, vec C, BYTE*screen, int view);
void IdentifyConcave(poly start, byte*screen, int view);
vec xyztoij(int x, int y, int z, vec res);
void DrawVJS(obj o, BYTE*screen, int view);
void testDrawVJS(BYTE*screen, int view);
void load_models();
void init_game();
void update_game();
void end_game();
int check_collision(obj o, double x_low, double y_low, double x_high, double y_high);
void initialise_test_polygons();

//====== Global Variables ==========

vec_t test_tri_p1 = {50,250,255,0,0}, test_tri_p2 = { 150,250,0,255,0}, test_tri_p3 = { 150,150,0,0,255};

vec_t test_tri_centre = { (test_tri_p1.x + test_tri_p2.x + test_tri_p3.x) / 3,(test_tri_p1.y + test_tri_p2.y + test_tri_p3.y),255,255,255};

char file_opened = 0;

int movespeed = 2;

int movspd_modh = 100;
int movspd_modv = 100;

int movedir_h, movedir_v;

double p1_coll_x1, p1_coll_x2, p2_coll_x1, p2_coll_x2;

char fname[FILENAME_MAX] = "test.vjs";

BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];

int	ZBuffer[FRAME_WIDE * FRAME_HIGH];

int		shade = 0;
POINT2D	xypos = { 0,0 };
int		stereo = 0;
int		eyes = 10;

int game_running = 0;

Object obj_player1, obj_player2, obj_ball, obj_wall1, obj_wall2;

poly bound = poly_new();

vec_t bound_p1 = { 0.0f,0.0f };
vec_t bound_p2 = { 0.0f,FRAME_WIDE };
vec_t bound_p3 = { FRAME_HIGH,FRAME_WIDE };
vec_t bound_p4 = { FRAME_HIGH,0.0f };

// 
Object o_t;
obj o = &o_t;