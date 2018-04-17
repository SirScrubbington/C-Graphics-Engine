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

void clear_screen();
void draw_frame();
void interlace(BYTE* pL, BYTE* pR);
void play_sound_effect(char * filename);
void build_frame(BYTE *pFrame, int view);
void on_idle(void);
void on_display(void);
void reshape(int w, int h);
void on_mouse(int button, int state, int x, int y);
void on_key_press(unsigned char key, int x, int y);
void set_pixel(unsigned int x,unsigned int y, unsigned char r, unsigned char g, unsigned char b, BYTE*frame, int view);
void draw_line(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b, BYTE* screen, int view);
void fill_triangle(vec A, vec B, vec C, BYTE*screen, int view);
void fill_triangle_sort(vec A, vec B, vec C, BYTE*screen, int view);
void identify_concave(poly start, byte*screen, int view);
vec xyztoij(int x, int y, int z, vec res);
void draw_vjs(obj o, BYTE*screen, int view);

//====== Global Variables ==========

#define MODE_TRANSLATE 0
#define MODE_SCALE 1
#define MODE_ROTATE 2
#define MODES_MAX 3;

Object compass_t;
obj compass=&compass_t;

Object compass_t_temp;
obj compass_temp=&compass_t_temp;

int objectviewmode = 0;
int test_mode = 0;
int selected_obj = 0;

vec_t test_tri_p1 = {50,250,255,0,0}, test_tri_p2 = { 150,250,0,255,0}, test_tri_p3 = { 150,150,0,0,255};
vec_t test_tri_centre = { (test_tri_p1.x + test_tri_p2.x + test_tri_p3.x) / 3,(test_tri_p1.y + test_tri_p2.y + test_tri_p3.y)/3,255,255,255};

char filename[FILENAME_MAX];
FILE * f;
int loaded = 0;

// -- Hitboxes --
double ball_diam_x = 10, ball_diam_y = 10;
double p1_diam_x = 80, p1_diam_y = 20;
double p2_diam_x = 80, p2_diam_y = 20;
double w1_diam_x = 30, w1_diam_y = 260;
double w2_diam_x = 30, w2_diam_y = 260;

vec_t ball_hitbox[4];
vec_t player_1_hitbox[4];
vec_t player_2_hitbox[4];
vec_t wall_1_hitbox[4];
vec_t wall_2_hitbox[4];

char file_opened = 0;
int movespeed = 2;
int movspd_modh = 100;
int movspd_modv = 100;

int movedir_h, movedir_v;

double p1_coll_x1, p1_coll_x2, p2_coll_x1, p2_coll_x2;

char fname[FILENAME_MAX] = "test.vjs";

BYTE	pFrameL[FRAME_WIDE * FRAME_HIGH * 3];
BYTE	pFrameR[FRAME_WIDE * FRAME_HIGH * 3];

int PIXELS = FRAME_WIDE * FRAME_HIGH;

int	ZBuffer[FRAME_WIDE * FRAME_HIGH] = { INT_MAX };

int		shade = 0;
POINT2D	xypos = { 0,0 };
int		stereo = 0;
int		eyes = 10;

Object obj_lst[MAX_OBJECTS] = {};
Object obj_tmp[MAX_OBJECTS] = {};
int obj_loaded[MAX_OBJECTS] = {0};