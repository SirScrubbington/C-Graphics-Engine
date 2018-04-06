#include "main.h"

//===== Forward Declarations ========

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
		//PlaySoundEffect("Laser.wav"); 
		//if (++shade > 16) shade = 0;	
	}
}

void OnKeypress(unsigned char key, int x, int y)
{
	if (key == 'g') {
		init_game();
		game_running = 1;
	}

	else {
		switch (key)
		{
		case ' ': xypos.x = xypos.y = 0; break;
		case 'p': stereo ^= 1, eyes = 10; break;
		case ']': eyes++;	break;
		case '[': eyes--;	break;
		case 'a': translate_obj(&obj_player1,-10, 0, 0); break;
		case 'd': translate_obj(&obj_player1, 10, 0, 0); break;
		case 'j': translate_obj(&obj_player2,-10, 0, 0); break;
		case 'l': translate_obj(&obj_player2, 10, 0, 0); break;
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
	return 0;
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

void setPixel_3d(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, BYTE*screen, int view) {
	if (checkZBuf(x, y, z, view)) setPixel(x, y, r, g, b, screen, view);
}

void drawLine(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen, int view) {
	
	double dx, dy;
	int dr, dg, db, steps;

	dx = x2 - x1;
	dy = y2 - y1;

	if (abs(dx) > abs(dy)) steps = (int)ROUND(abs(dx));
	else steps = (int)ROUND(abs(dy));

	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;

	double x = x1,
		y = y1;

	setPixel((int)ROUND(x), (int)ROUND(y), r, g, b, screen, view);
	for (int i = 0; i < steps; i++, x += x_inc, y += y_inc) setPixel((int)ROUND(x), (int)ROUND(y), r, g, b, screen, view);
}

void drawLineShaded(vec p1, vec p2, BYTE*screen, int view) {
	double dx, dy;
	int dr,dg,db,steps;

	dx = p2->x - p1->x;
	dy = p2->y - p1->y;

	dr = p2->r - p1->r;
	dg = p2->g - p1->g;
	db = p2->b - p1->b;
	
	if (abs(dx) > abs(dy)) steps = (int)ROUND(abs(dx));
	else steps = (int)ROUND(abs(dy));

	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;
	double r_inc = dr / (double)steps;
	double g_inc = dg / (double)steps;
	double b_inc = db / (double)steps;

	double x = p1->x, 
		   y = p1->y;

	int r = p1->r,
		g = p1->g,
		b = p1->b;

	setPixel((int)ROUND(x),(int)ROUND(y),(unsigned char)ROUND(r), (unsigned char)ROUND(g), (unsigned char)ROUND(b), screen, view);
	for (int i = 0; i < steps; i++, x += x_inc, y += y_inc, r+=r_inc, g+= g_inc, b+= b_inc) setPixel((int)ROUND(x),(int)ROUND(y), (unsigned char)ROUND(r), (unsigned char)ROUND(g), (unsigned char)ROUND(b), screen, view);
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

	double dx1, dx2, dx3;
	double dr1, dg1, db1, dr2, dg2, db2, dr3, dg3, db3;

	if (B->y - A->y > 0) {
		dx1 = (B->x - A->x) / (B->y - A->y);
		dr1 = (B->r - A->r) / (B->y - A->y);
		dg1 = (B->g - A->g) / (B->y - A->y);
		db1 = (B->b - A->b) / (B->y - A->y);
	}
	else dx1 = dr1 = dg1 = db1 = 0;

	if (C->y - A->y > 0) {
		dx2 = (C->x - A->x) / (C->y - A->y);
		dr2 = (C->r - A->r) / (C->y - A->y);
		dg2 = (C->g - A->g) / (C->y - A->y);
		db2 = (C->b - A->b) / (C->y - A->y);
	}
	else dx2 = dr2 = dg2 = db2 = 0;

	if (C->y - B->y > 0) {
		dx3 = (C->x - B->x) / (C->y - B->y);
		dr3 = (C->r - B->r) / (C->y - B->y);
		dg3 = (C->g - B->g) / (C->y - B->y);
		db3 = (C->b - B->b) / (C->y - B->y);
	}
	else dx3 = dr3 = dg3 = db3 = 0;

	vec_t S = { A->x,A->y,A->r,A->g,A->b },
		E = { A->x,A->y,A->r,A->g,A->b };

	if (dx1 > dx2) {
		for (; S.y <= B->y; S.y++, E.y++) {
			drawLineShaded(&S, &E, screen, view);
			S.x += dx2, S.r += dr2, S.g += dg2, S.b += db2;
			E.x += dx1, E.r += dr1, S.g += dg1, S.b += db1;
		}
		E.x = B->x;
		E.y = B->y;
		E.r = B->r;
		E.g = B->g;
		E.b = B->b;
		for (; S.y <= C->y; S.y++, E.y++) {
			drawLineShaded(&S, &E, screen, view);
			S.x += dx2, S.r += dr2, S.g += dg2, S.b += db2;
			E.x += dx3, E.r += dr3, S.g += dg3, S.b += db3;
		}
	}
	else {
		for (; S.y <= B->y; S.y++, E.y++) {
			drawLineShaded(&S, &E, screen, view);
			S.x += dx1, S.r += dr1, S.g += dg1, S.b += db1;
			E.x += dx2, E.r += dr2, S.g += dg2, S.b += db2;
		}
		S.x = B->x;
		S.y = B->y;
		S.r = B->r;
		S.g = B->g;
		S.b = B->b;
		for (; S.y <= C->y; S.y++, E.y++) {
			drawLineShaded(&S, &E, screen, view);
			S.x += dx3, S.r += dr3, S.g += dg3, S.b += db3;
			E.x += dx2, E.r += dr2, S.g += dg2, S.b += db2;
		}
	}
}

void fillTriangleSort(vec A, vec B, vec C, BYTE*screen, int view) {
	vec a = A, b = B, c = C;

	printf("(%f,%f),(%f,%f),(%f,%f)\n", a->x, a->y, b->x, b->y, c->x, c->y);

	if (a->y > c->y) {
		vec temp = a;
		a = C;
		c = temp;
	}
	if (a->y > b->y) {
		vec temp = a;
		a = B;
		b = temp;
	}
	if (b->y > c->y) {
		vec temp = b;
		b = C;
		c = temp;
	}

	printf("(%f,%f),(%f,%f),(%f,%f)\n", a->x, a->y, b->x, b->y, c->x, c->y);

	//if (b->y == c->y)getchar();

	fillTriangle(a,b,c, screen, view);
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
						fillTriangleSort(&p->v[lp + off], &p->v[cp + off], &p->v[rp + off],screen,view);
						printf("Drawing Triangle at (%f,%f),(%f,%f),(%f,%f)\n", p->v[lp + off].x, p->v[lp + off].y, p->v[cp + off].x, p->v[cp + off].y, p->v[rp + off].x, p->v[rp + off].y);
						drawLineShaded(&p->v[lp + off],&p->v[cp + off],screen,view);
						drawLineShaded(&p->v[cp + off],&p->v[rp + off],screen,view);
						drawLineShaded(&p->v[rp + off],&p->v[lp + off],screen,view);
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
	fillTriangleSort(&p->v[0], &p->v[1], &p->v[2], screen, view);
	printf("Drawing Triangle at (%f,%f),(%f,%f),(%f,%f)\n", p->v[0].x, p->v[0].y, p->v[1].x, p->v[1].y, p->v[2].x, p->v[2].y);
	drawLineShaded(&p->v[0],&p->v[1],screen, view);
	drawLineShaded(&p->v[1],&p->v[2],screen, view);
	drawLineShaded(&p->v[2],&p->v[0],screen, view);
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
		if (pos == p->len || neg == p->len || p->len < 4) {
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

void DrawVJS(obj o,BYTE*screen,int view) {

	vec_t ij;

	backfaceculling(o);

	for (int i = 0; i < o->NumPolysObj; i++) {
		
		//if (o->ObjectPolys[i].can_draw == 0)continue;

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

void load_models() {
	FILE * vjs_plr1, *vjs_plr2, *vjs_ball,*vjs_wall;

	vjs_plr1 = fopen("vjs/player1.vjs", "r");
	vjs_plr2 = fopen("vjs/player2.vjs", "r");
	vjs_ball = fopen("vjs/ball.vjs", "r");
	vjs_wall = fopen("vjs/wall.vjs", "r");

	if (!vjs_plr1 || !vjs_plr2 || !vjs_ball || !vjs_wall) {
		perror("Unable to open files!");
		return;
	}

	loadVJS(vjs_plr1, &obj_player1);
	loadVJS(vjs_plr2, &obj_player2);
	loadVJS(vjs_ball, &obj_ball);
	loadVJS(vjs_wall, &obj_wall1);

	vjs_wall = fopen("vjs/wall.vjs", "r");

	loadVJS(vjs_wall, &obj_wall2);

	fclose(vjs_plr1);
	fclose(vjs_plr2);
	fclose(vjs_ball);
	fclose(vjs_wall);
}

void init_game() {
	load_models();

	movespeed = 2;
	movedir_h = -1;
	movedir_v = 1;


	// setup positions
	move_to_point(&obj_player2, FRAME_WIDE/4, -FRAME_HIGH/2 - 50, 0);
	move_to_point(&obj_player1, FRAME_WIDE/4, FRAME_HIGH/2 + 100, 0);
	move_to_point(&obj_ball, FRAME_WIDE / 2, FRAME_HIGH / 2, 0);
	move_to_point(&obj_wall1, -FRAME_WIDE/4, FRAME_HIGH / 16, 0);
	move_to_point(&obj_wall2, FRAME_WIDE - (FRAME_WIDE/4), FRAME_HIGH / 16, 0);

	scale_obj(&obj_player2, 2, .25, 0.5);
	scale_obj(&obj_player1, 2, .25, 0.5);
	//scale_obj(&obj_ball, 2, 2, 2);
	scale_obj(&obj_wall1, 0.5, 8, 0.5);
	scale_obj(&obj_wall2, 0.5, 8, 0.5);
}

void update_game() {
	int col_p1, col_p2, col_wall1, col_wall2;
	
	vec_t plr1_p1, plr1_p2;
	vec_t plr2_p1, plr2_p2;

	xyztoij(obj_player1.ObjectPoints[0].x, obj_player1.ObjectPoints[0].x, obj_player1.ObjectPoints[0].x, &plr1_p1);
	xyztoij(obj_player1.ObjectPoints[4].x, obj_player1.ObjectPoints[4].x, obj_player1.ObjectPoints[4].x, &plr1_p2);

	xyztoij(obj_player1.ObjectPoints[0].x, obj_player1.ObjectPoints[0].x, obj_player1.ObjectPoints[0].x, &plr2_p1);
	xyztoij(obj_player1.ObjectPoints[4].x, obj_player1.ObjectPoints[4].x, obj_player1.ObjectPoints[4].x, &plr2_p2);
	
	col_p1 = check_collision(&obj_ball,plr1_p1.x,plr1_p1.y,plr1_p2.x,plr1_p2.y);
	col_p2 = check_collision(&obj_ball,plr2_p1.x,plr2_p1.y,plr2_p2.x,plr2_p2.y);

	if ((obj_ball.ObjProps.Center.y < 100 || obj_ball.ObjProps.Center.y > 540)){
		if (true) {
			PlaySoundEffect("blip.wav");
			if (obj_ball.ObjProps.Center.y < 100)translate_obj(&obj_ball, 0, movespeed, 0);
			else translate_obj(&obj_ball, 0, -movespeed, 0);

			if (movedir_v == 1)movedir_v = -1;
			else movedir_v = 1;
			movspd_modv = rand() % 100 + 80;
			movspd_modh = rand() % 100 + 80;

		}
		else {
			end_game();
		}
	}
	
	if (obj_ball.ObjProps.Center.x < 250 || obj_ball.ObjProps.Center.x > FRAME_WIDE - (FRAME_WIDE/4)+75){
		PlaySoundEffect("blip.wav");
		if (obj_ball.ObjProps.Center.x < 250) translate_obj(&obj_ball,movespeed,0,0);
		else translate_obj(&obj_ball, -movespeed, 0, 0);
		
		if (movedir_h == 1)movedir_h = -1;
		else movedir_h = 1;
		movspd_modv = rand() % 100 + 80;
		movspd_modh = rand() % 100 + 80;
	}

	translate_obj(&obj_ball, movespeed*movedir_h*((double)movspd_modh*0.01), movespeed*movedir_v*((double)movspd_modv*0.01)*0.5, 0);

}

void end_game() {

}

void test_polygon_rendering(BYTE*screen, int view) {
	poly test_convex = poly_new(), test_concave = poly_new();
	//poly test_convex = poly_new(), test_concave=poly_new();
	vec_t append_concave[4] = { {100,500},{50,400},{100,450},{150,400} };
	for (int i = 0; i < 4; i++) {
		poly_append(test_concave, &append_concave[i]);
	}

	vec_t append_convex[4] = { { 50,300 },{ 50,200 },{ 150,200 },{ 150,300 } };
	for (int i = 0; i < 4; i++) {
		poly_append(test_convex, &append_convex[i]);
	}

	IdentifyConcave(test_concave, screen, view);
	IdentifyConcave(test_convex, screen, view);
}

#define TESTDRAWPOLY 4

void TestDrawPoly(BYTE*screen,int view) {


	vec_t pt[TESTDRAWPOLY];

	int n = TESTDRAWPOLY;

	pt[0] = { 100,500,255,0,0 };
	pt[1] = { 150,400,255,255,0 };
	pt[2] = { 100,450,255,255,255 };
	pt[3] = {  50,400,255,0,255 };

	poly p = poly_new();

	for (int i = 0; i < TESTDRAWPOLY; i++) {
		poly_append(p, &pt[i]);
	}

	IdentifyConcave(p,screen,view);

	//DrawPoly(p, 255, 255, 255, screen);

	// Draw Polygon using lines for testing
	for (int i = 0; i<TESTDRAWPOLY; i++) {
		if (i == TESTDRAWPOLY - 1) {
			//drawLine(pt[i].x, pt[i].y, pt[0].x, pt[0].y, 255, 0, 0, screen,view);
			printf("%i: Drawing (%f,%f) to (%f,%f). \n", i, pt[i].x, pt[i].y, pt[0].x, pt[0].y);
		}
		else{
			//drawLine(pt[i].x, pt[i].y, pt[i + 1].x, pt[i + 1].y, 255, 0, 0, screen,view);
			printf("%i: Drawing (%f,%f) to (%f,%f). \n", i, pt[i].x, pt[i].y, pt[i + 1].x, pt[i + 1].y);
		}
	}
	//poly_free(p);
}

void BuildFrame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety
	
	if (game_running) {
		DrawVJS(&obj_player1, screen, view);
		DrawVJS(&obj_player2, screen, view);
		DrawVJS(&obj_ball, screen, view);
		DrawVJS(&obj_wall1, screen, view);
		DrawVJS(&obj_wall2, screen, view);
		update_game();
		
	}

	TestDrawPoly(screen, view);
	
	fillTriangleSort(&test_tri_p1,&test_tri_p2,&test_tri_p3,screen,view);
	rotate_point_around_point(&test_tri_p1, &test_tri_centre, 1, &test_tri_p1);
	rotate_point_around_point(&test_tri_p2, &test_tri_centre, 1, &test_tri_p2);
	rotate_point_around_point(&test_tri_p3, &test_tri_centre, 1, &test_tri_p3);

	FILE*f = fopen("vjs/test.vjs", "r");
	if (f) {
		Object o;
		loadVJS(f, &o);
		translate_obj(&o, -150, 200, 0);
		DrawVJS(&o,screen,view);
		fclose(f);
	}
	printf("Next Frame From Here \n");
	

}

int check_collision(obj o, double x_low, double y_low, double x_high, double y_high) {
	for (int i = 0; i < o->NumPtsObj; i++) {
		if (o->ObjectPoints[i].x > x_low && o->ObjectPoints[i].x < x_high && o->ObjectPoints[i].y > y_low && o->ObjectPoints[i].y < y_high) return 1;
	}
	return 0;
}