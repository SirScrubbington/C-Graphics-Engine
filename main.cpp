#include "main.h"

//===== Forward Declarations ========

////////////////////////////////////////////////////////
// Program Entry Poinr
////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

	FILE*f = fopen("vjs/graphics_compass.vjs", "r");
	load_vjs(f, compass);

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
	glutIdleFunc(on_idle);
	glutDisplayFunc(on_display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(on_key_press);
	glutMouseFunc(on_mouse);

	//-- run the program
	glutMainLoop();
	return 0;
}

////////////////////////////////////////////////////////
// Event Handers
////////////////////////////////////////////////////////

// on_idle(void): void
// Performs the functions required when the program is idle.
void on_idle(void)
{
	draw_frame();
	glutPostRedisplay();
}

// on_display(void): void
// Performs the gl display functions.
void on_display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);
	glDrawPixels(FRAME_WIDE, FRAME_HIGH, GL_RGB,GL_UNSIGNED_BYTE, (GLubyte*)pFrameR);
	glutSwapBuffers();
	glFlush();
}

// reshape(w,h): void
// Reshapes the view port to a given width and height.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// on_mouse(button,state,x,y): void
// Runs the required code given a specific mouse state.
void on_mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		printf("(%i,%i)",x,y);
	}
}

// handle_key_w(void): void
// Handles all of the different possible functions which can be called when the 'w' key is used
void handle_key_w() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], 0, 0, -10); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.z*=.9; break;//scale_obj(&obj_lst[selected_obj],1,1,.9); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.x +=10; break;//rotate_x(&obj_lst[selected_obj], D2R(10)); rotate_x(compass, D2R(10));  break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// handle_key_s(void): void
// Handles all of the different possible functions which can be called when the 's' key is used
void handle_key_s() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], 0, 0, 10); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.z*=1.1; break;//scale_obj(&obj_lst[selected_obj],1,1,1.1); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.x +=-10; break;//rotate_x(&obj_lst[selected_obj], D2R(-10)); rotate_x(compass, D2R(-10)); break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// handle_key_a(void): void
// Handles all of the different possible functions which can be called when the 'a' key is used
void handle_key_a() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], 10, 0, 0); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.x*=1.1; break;//scale_obj(&obj_lst[selected_obj],1.1,1,1); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.z +=10; break;//rotate_z(&obj_lst[selected_obj], D2R(10)); rotate_z(compass, D2R(10)); break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// handle_key_d(void): void
// Handles all of the different possible functions which can be called when the 'd' key is used
void handle_key_d() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], -10, 0, 0); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.x*=.9; break;//scale_obj(&obj_lst[selected_obj],.9,1,1); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.z +=-10; break;//rotate_z(&obj_lst[selected_obj], D2R(-10)); rotate_z(compass, D2R(-10)); break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// handle_key_q(void): void
// Handles all of the different possible functions which can be called when the 'q' key is used
void handle_key_q() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], 0, -10, 0); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.y*=.9; break;//scale_obj(&obj_lst[selected_obj], 1, .9, 1); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.y +=10; break; // rotate_y(compass, D2R(10)); break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// handle_key_e(void): void
// Handles all of the different possible functions which can be called when the 'e' key is used
void handle_key_e() {
	switch (objectviewmode) {
	case MODE_TRANSLATE: translate_obj(&obj_lst[selected_obj], 0, 10, 0); break;
	case MODE_SCALE: obj_lst[selected_obj].ObjProps.Scale.y*=1.1; break;//scale_obj(&obj_lst[selected_obj], 1,1.1,1); break;
	case MODE_ROTATE: obj_lst[selected_obj].ObjProps.Rotation.y +=-10; break;//rotate_y(&obj_lst[selected_obj], D2R(-10)); rotate_y(compass, D2R(-10)); break;
	default: printf("Invalid mode!\n"); objectviewmode = 0; break;
	}
	//printObject(&obj_lst[selected_obj]);
}

// load_file(filename): void
// Loads the VJS file at the given filename into the currently selected model slot, if available
void load_file(char*filename) {
	f = fopen(filename, "r");
	if (f) {
		obj_lst[selected_obj] = Blank_Object;
		load_vjs(f, &obj_lst[selected_obj]);
		play_sound_effect("blip.wav");
		loaded = obj_loaded[selected_obj] = 1;
		print_object(&obj_lst[selected_obj]);
		fclose(f);
	}
	else {
		printf("Could not open file! %s\n", filename);
		loaded = obj_loaded[selected_obj] = 0;
	}
}

// on_key_press(key,x,y): void
// Runs the required code given a specific key state.
void on_key_press(unsigned char key, int x, int y)
{
	if (key == 'l') {
		printf("Please Enter a File Path: ");
		fscanf(stdin, "%s", filename);
		load_file(filename);
	}

	else if (key == 'u') {
		if (obj_loaded[selected_obj]) {
			obj_loaded[selected_obj] = 0;
		}
	}

	else if (key == 'k') {
		load_file(filename);
	}

	else {
		switch (key)
		{
		case ' ': xypos.x = xypos.y = 0; break;
		case 'p': stereo ^= 1, eyes = 10; break;
		case ']': eyes++;	break;
		case '[': eyes--;	break;

		case '1': selected_obj++; selected_obj %= MAX_OBJECTS; printf("Current Object Slot: %i\n", selected_obj); 
				  break;
		case '!': selected_obj--; if (selected_obj < 0) selected_obj += MAX_OBJECTS; printf("Current Object Slot: %i\n", selected_obj);
				  break;
		case '2': objectviewmode++; objectviewmode %= MODES_MAX; printf("Current Mode: %i\n", objectviewmode );  break;
		case '@': objectviewmode--; if (objectviewmode < 0) objectviewmode += MODES_MAX; printf("Current Mode: %i\n", objectviewmode );  break;

		case 't': if (test_mode == 0)test_mode = 1; else test_mode = 0; break;

		case 'w': handle_key_w(); break;
		case 's': handle_key_s(); break;
		case 'a': handle_key_a(); break;
		case 'd': handle_key_d(); break;
		case 'q': handle_key_q(); break;
		case 'e': handle_key_e(); break;

		case 27: exit(0);
		}
	}
}

////////////////////////////////////////////////////////
// Utility Functions
////////////////////////////////////////////////////////

// clear_screen(void): Void
// Wipes the display
void clear_screen()
{
	memset(pFrameL, 0, FRAME_WIDE * FRAME_HIGH * 3);
	memset(pFrameR, 0, FRAME_WIDE * FRAME_HIGH * 3);
}

// interlace(pL,pR): void
// interlaces the two frames pointed to by pL and pR
void interlace(BYTE* pL, BYTE* pR)
{
	int rowlen = 3 * FRAME_WIDE;
	for (int y = 0; y < FRAME_HIGH; y+=2)
	{
		for (int x = 0; x < rowlen; x++) *pR++ = *pL++;
		pL += rowlen;
		pR += rowlen;
	}
}

// draw_frame(void): Void
// Draws the frame objects to the screen
void draw_frame()
{
	clear_screen();
	
	if (!stereo) build_frame(pFrameR, 0);
	else {
		build_frame(pFrameL, -eyes);
		build_frame(pFrameR, +eyes);
		interlace((BYTE*)pFrameL, (BYTE*)pFrameR);
	}
}

// play_sound_effect(filename): void
// Plays the sound effect at the given filename
void play_sound_effect(char * filename)		
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

// point_inside_frame(p,x_high,x_low,y_high,y_low): int
// Checks if a given point p(x,y) is inside the frame (x_high,x_low,y_high,y_low)
int point_inside_frame(vec p, int x_high, int x_low, int y_high, int y_low) {
	return (p->x < x_high && p->y < y_high && p->x > 0 && p->y > 0);
}

// check_z_buf(x,y,z,view): int
// Checks if for a given point (x,y,z) on the screen if a pixel with a greater z value has already been inserted. If so, 0 is returned. Otherwise, 1 is returned and the new z value is assigned in that position of the Z buffer.
int check_z_buf(unsigned int x, unsigned int y,int z,int view) {
	vec_t temp = { x,y };
	if ((point_inside_frame(&temp, FRAME_WIDE, 0, FRAME_HIGH, 0)) && (ZBuffer[x + view + (y*FRAME_WIDE)] > z)) {
		ZBuffer[x + view + (y*FRAME_WIDE)] = z;
		return 1;
	}
	return 0;
}

// set_pixel(x,y,r,g,b,screen,view): void
// Puts a pixel in the display at position x,y with colours r,g,b.
void set_pixel(unsigned int x,unsigned int y, unsigned char r, unsigned char g, unsigned char b, BYTE* screen, int view){	
	int channels = 3;
	int colour = 0;
	screen[channels*((x + view + (y * FRAME_WIDE))) + 0] = r;
	screen[channels*((x + view + (y * FRAME_WIDE))) + 1] = g;
	screen[channels*((x + view + (y * FRAME_WIDE))) + 2] = b;
}

// set_pixel_3d(x,y,z,r,g,b,screen,view): void
// Checks the Z buffer, and sets the given pixel if a pixel with a greater z value has not already been assigned.
void set_pixel_3d(unsigned int x,unsigned int y, int z, unsigned char r, unsigned char g, unsigned char b, BYTE*screen, int view) {
	if (check_z_buf(x, y, z, view)) set_pixel(x, y, r, g, b, screen, view);
}

// draw_line(x1,y1,x2,y2,r,g,b,screen,view): void
// Very basic function for drawing a flat shaded line between x1,y1 and x2,y2 of colour r,g,b.
void draw_line(double x1, double y1, double x2, double y2, unsigned char r, unsigned char g, unsigned char b,BYTE* screen, int view) {
	
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

	set_pixel((unsigned int)ROUND(x), (unsigned int)ROUND(y), r, g, b, screen, view);
	for (int i = 0; i < steps; i++, x += x_inc, y += y_inc) set_pixel((unsigned int)ROUND(x), (unsigned int)ROUND(y), r, g, b, screen, view);
}

// compute_code(p,x_min,x_max,y_min,y_max): int
// Computes the 'code' with respect to the Cohen Sutherland line clipping algorithm for point 'p' using bounding box [x_min,x_max,y_min,y_max]
int compute_code(vec p, int x_min, int x_max, int y_min, int y_max) {
	int code = INSIDE;
	
	if (p->x < x_min) {
		code |= LEFT;
	}
	
	else if (p->x > x_max) {
		code |= RIGHT;
	}
	
	if (p->y < y_min) {
		code |= BOTTOM;
	}
	
	else if (p->y > y_max) {
		code |= TOP;
	}

	return code;
}

// line_clip(p1,p2,x_min,x_max,y_min,y_max): void
// Clips a given line p1->p2 using the Cohen Sutherland line clipping algorithm with the bounding box [x_min,x_max,y_min,y_max]
// This program doesn't work in the actual version and I couldn't figure out why in the remaining time, so I removed it - But I've left it
// here to demonstrate some level of understanding of the algorithm.
void line_clip(vec p1, vec p2 ,int x_min, int x_max, int y_min, int y_max) {
	int code1, code2;
	code1 = compute_code(p1, 0, FRAME_WIDE, 0, FRAME_HIGH);
	code2 = compute_code(p2, 0, FRAME_WIDE, 0, FRAME_HIGH);

	int accept = 0;

	while (true) {
		if ((code1 == 0) && (code2 == 0)) {
			accept = 1;
			break;
		}
		else if ((code1&&code2))break;
		else {
			int code_out;
			double x, y;

			if (code1 != 0) {
				code_out = code1;
			}
			else {
				code_out = code2;
			}

			if (code_out & TOP) {
				x = p1->x + (p2->x - p1->x)*(y_max - p1->y) / (p2->y - p1->y);
				y = y_max;
			}

			if (code_out & BOTTOM) {
				x = p1->x + (p2->x - p1->x)*(y_min - p1->y) / (p2->y - p1->y);
				y = y_min;
			}

			if (code_out & RIGHT) {
				y = p1->y + (p2->y - p1->y)*(x_max - p1->x) / (p2->x - p1->x);
				x = x_max;
			}

			if (code_out & LEFT) {
				y = p1->y + (p2->y - p1->y)*(x_min - p1->x) / (p2->x - p1->x);
				x = x_min;
			}

			if (code_out == code1) {
				p1->x = x;
				p1->y = y;
				code1 = compute_code(p1, x_min, x_max, y_min, y_max);
			}
			else {
				p2->x = x;
				p2->y = y;
				code2 = compute_code(p1, x_min, x_max, y_min, y_max);
			}
		}
	}
}

// draw_line_shaded(p1,p2,screen,view): void
// Draws a shaded line between p1->p2
void draw_line_shaded(vec p1, vec p2, BYTE*screen, int view) {

	if (p1->x == p2->x && p1->y == p2->y)return;

	double dx, dy, dz;
	double dr, dg, db;
	int steps;

	dx = p2->x - p1->x;
	dy = p2->y - p1->y;

	dr = p2->r - p1->r;
	dg = p2->g - p1->g;
	db = p2->b - p1->b;

	dz = p2->z - p1->z;
	
	if (abs(dx) > abs(dy)) steps = (int)ROUND(abs(dx));
	else steps = (int)ROUND(abs(dy));

	double x_inc = dx / (double)steps;
	double y_inc = dy / (double)steps;
	double r_inc = dr / (double)steps;
	double g_inc = dg / (double)steps;
	double b_inc = db / (double)steps;
	double z_inc = dz / (double)steps;

	double x = p1->x, 
		   y = p1->y,
		   z = p1->z;

	double r = p1->r,
		   g = p1->g,
		   b = p1->b;

	set_pixel_3d((unsigned int)(x),(unsigned int)(y),(int)(z),(unsigned char)(r), (unsigned char)(g), (unsigned char)(b), screen, view);
	for (int i = 0; i < steps; i++, x += x_inc, y += y_inc, z+= z_inc, r+=r_inc, g+= g_inc, b+= b_inc) set_pixel_3d((unsigned int)(x), (unsigned int)(y), (int)(z), (unsigned char)(r), (unsigned char)(g), (unsigned char)(b), screen, view);
}

// draw_line_shaded_clip(p1,p2,x_min,x_max,y_min,y_max,screen,view): void
// Clips a given line to the bounding box [x_min,x_max,y_min,y_max] and then draws the resulting shaded line to the screen
void draw_line_shaded_clip(vec p1, vec p2, int x_min, int x_max, int y_min, int y_max, BYTE*screen, int view) {
	vec_t t_p1, t_p2;
	copy(p1, &t_p1);
	copy(p2, &t_p2);
	
	line_clip(&t_p1, &t_p2, x_min, x_max, y_min, y_max);
	draw_line_shaded(&t_p1, &t_p2, screen, view);
}

// fill_triangle(A,B,C,screen,view): void
// Draws a shaded triangle of coordinates (A,B,C) to the screen 
void fill_triangle(vec A, vec B, vec C, BYTE*screen, int view) {
	vec_t dAC, dAB, dBC;
	if (A->y == C->y) return;
	dAC.x = (C->x - A->x) / (C->y - A->y);
	dAC.r = (C->r - A->r) / (C->y - A->y);
	dAC.g = (C->g - A->g) / (C->y - A->y);
	dAC.b = (C->b - A->b) / (C->y - A->y);
	dAC.z = (C->z - A->z) / (C->y - A->y);

	vec_t Vb, Ve;
	copy(A, &Vb);
	copy(A, &Ve);


	if (B->y != A->y) {
		dAB.x = (B->x - A->x) / (B->y - A->y);
		dAB.r = (B->r - A->r) / (B->y - A->y);
		dAB.g = (B->g - A->g) / (B->y - A->y);
		dAB.b = (B->b - A->b) / (B->y - A->y);
		dAB.z = (B->z - A->z) / (B->y - A->y);
		while (Vb.y <= B->y) {
			draw_line_shaded_clip(&Vb, &Ve,0,FRAME_WIDE,0,FRAME_HIGH,screen, view);
			Vb.x += dAC.x; Vb.z += dAC.z; Vb.r += dAC.r; Vb.g += dAC.g; Vb.b += dAC.b; Vb.y++;
			Ve.x += dAB.x; Ve.z += dAB.z; Ve.r += dAB.r; Ve.g += dAB.g; Ve.b += dAB.b; Ve.y++;
		}
	}

	if (C->y != B->y) {
		dBC.x = (C->x - B->x) / (C->y - B->y);
		dBC.r = (C->r - B->r) / (C->y - B->y);
		dBC.g = (C->g - B->g) / (C->y - B->y);
		dBC.b = (C->b - B->b) / (C->y - B->y);
		dBC.z = (C->z - B->z) / (C->y - B->y);
		Ve.x = B->x;
		while (Vb.y < C->y) {
			draw_line_shaded_clip(&Vb, &Ve, 0, FRAME_WIDE, 0, FRAME_HIGH, screen, view);
			Vb.x += dAC.x; Vb.z += dAC.z; Vb.r += dAC.r; Vb.g += dAC.g; Vb.b += dAC.b; Vb.y++;
			Ve.x += dBC.x; Ve.z += dBC.z;  Ve.r += dBC.r; Ve.g += dBC.g; Ve.b += dBC.b; Ve.y++;
		}
	}
}

// fill_triangle_sort(A,B,C,screen,view): void
// Sorts a provided set of triangle points so that A.y <= B.y <=C.y and then passes it to the fill_triangle function.
void fill_triangle_sort(vec A, vec B, vec C, BYTE*screen, int view) {

	vec a = A, b = B, c = C;

	if (b->y < a->y) {
		vec temp = a;
		a = b;
		b = temp;
	}

	if (c->y < a->y) {
		vec temp = a;
		a = c;
		c = temp;
	}

	if (c->y < b->y) {
		vec temp = b;
		b = c;
		c = temp;
	}

	fill_triangle(a,b,c, screen, view);

}

// draw_poly(): Void
// Accepts an array 'points' of size 'n' and draws the resulting polygon using triangle decomposition.
void draw_poly(poly p,BYTE*screen, int view) {
	while (p->len > 3) {

		// l = left point, c = centre point, r = right point, off = offset
		int lp = 0, cp = 1, rp = 2, off = 0;
		for (int h = 0; h < p->len; h++) {
			int valid = 0;
			for (int i = 0; i < p->len; i++) {
				if (i != lp && i != cp && i != rp) {
					if (triangle_inside(&p->v[i], &p->v[lp], &p->v[cp], &p->v[rp])) {
						continue;
					}
					else {
						fill_triangle_sort(&p->v[lp + off], &p->v[cp + off], &p->v[rp + off],screen,view);
						poly_remove(p, cp + off);
						valid = 1;
						break;
					}
				}
			}
			lp++; cp++; rp++;
			if (lp > p->len)lp -= p->len;
			if (cp > p->len)cp -= p->len;
			if (rp > p->len)rp -= p->len;
			if (valid) break;
		}
	}
	if (p->len == 3) {
		fill_triangle_sort(&p->v[0], &p->v[1], &p->v[2], screen, view);
	}
	else if (p->len == 2) {
		draw_line_shaded_clip(&p->v[0], &p->v[1], 0, FRAME_WIDE, 0, FRAME_HIGH, screen, view);
	}
	return;
}

// identify_concave(start,screen,view): void
// Draws a given polygon start by splitting it into separate polygons if they are concave until they are convex, and then evaluating and drawing each resulting polygon individually using a queue structure.
void identify_concave(poly start,byte*screen, int view) {

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

			if (cross < 0) {
				neg++;
				crossarr[i] = -1;
			}
			else {
				pos++;
				crossarr[i] = 1;
			}
		}
		if (pos == p->len || neg == p->len || p->len < 4) {
			draw_poly(p,screen,view);
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
				// make sure it doesnt cross over any lines
				for (int k = 0; k < p->len; k++) {
					if (k == convex || k == convex - 1 || k == convex + 1)continue;
					if ((convex - 1 < 0 && k == p->len - 1) || (convex + 1 > p->len && j == 0))continue;
					if (k == j || k == j - 1 || k == j + 1)continue;
					if ((j - 1 < 0 && k == p->len - 1) || (j + 1 > p->len && j == 0))continue;
					if (k == p->len - 1) {
						if (line_sect_ignore_edge(v1, v2, &p->v[k], &p->v[0])) {
							valid = 0;
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

// draw_vjs(o,screen,view): void
// Converts a given 3d object 'o' to a 2d polygon and draws it to the screen
void draw_vjs(obj o,BYTE*screen,int view) {

	vec_t ij;

	for (int i = 0; i < o->NumPolysObj; i++) {
		Polygon_t * obj = &o->ObjectPolys[i];
		poly p = poly_new();
		for (int j = 0; j < NumSidesPoly; j++) {
			xyztoij(o->ObjectPoints[obj->Vertices[j]].x, o->ObjectPoints[obj->Vertices[j]].y, o->ObjectPoints[obj->Vertices[j]].z, &ij);
			ij.r = o->ObjectPoints[obj->Vertices[j]].r;
			ij.g = o->ObjectPoints[obj->Vertices[j]].g;
			ij.b = o->ObjectPoints[obj->Vertices[j]].b;
			ij.z = o->ObjectPoints[obj->Vertices[j]].z;
			poly_append(p, &ij);
		}
		poly_remove_duplicates(p);
		identify_concave(p, screen, view);
	}
}

#define TESTDRAWPOLY 4

// test_draw_poly(screen,view): void
// Draws a test polygon to the screen
void test_draw_poly(BYTE*screen,int view) {


	vec_t pt[TESTDRAWPOLY];

	int n = TESTDRAWPOLY;

	pt[0].x = 100;
	pt[0].y = 500;
	pt[0].r = 255;
	pt[0].g = 0;
	pt[0].b = 0;

	pt[1].x =150;
	pt[1].y =400;
	pt[1].r =255;
	pt[1].g =255;
	pt[1].b =0;

	pt[2].x =100;
	pt[2].y =450;
	pt[2].r =255;
	pt[2].g =255;
	pt[2].b =255;

	pt[3].x =50;
	pt[3].y =400;
	pt[3].r =255;
	pt[3].g =0;
	pt[3].b =255;

	poly p = poly_new();

	for (int i = 0; i < TESTDRAWPOLY; i++) {
		poly_append(p, &pt[i]);
	}

	identify_concave(p,screen,view);

	pt[0].x = 50;
	pt[0].y = 50;
	pt[0].r = 255;
	pt[0].g = 0;
	pt[0].b = 0;

	pt[1].x =50;
	pt[1].y =125;
	pt[1].r =255;
	pt[1].g =255;
	pt[1].b =0;

	pt[2].x =125;
	pt[2].y =125;
	pt[2].r =255;
	pt[2].g =255;
	pt[2].b =255;

	pt[3].x =125;
	pt[3].y =50;
	pt[3].r =255;
	pt[3].g =0;
	pt[3].b =255;

	p = poly_new();

	for (int i = 0; i < TESTDRAWPOLY; i++) {
		poly_append(p, &pt[i]);
	}

	identify_concave(p, screen, view);

}

// test_2d(screen,view): void
// Displays the test objects to demonstrate 2D shape drawing to the markers
void test_2d(BYTE*screen, int view) {
	test_draw_poly(screen, view);
	fill_triangle_sort(&test_tri_p1, &test_tri_p2, &test_tri_p3, screen, view);
	rotate_point_around_point(&test_tri_p1, &test_tri_centre, .01, &test_tri_p1);
	rotate_point_around_point(&test_tri_p2, &test_tri_centre, .01, &test_tri_p2);
	rotate_point_around_point(&test_tri_p3, &test_tri_centre, .01, &test_tri_p3);
}

// build_frame(pFrame,view): void
// Builds the frame for a single iteration of the program
void build_frame(BYTE *pFrame, int view)
{
	BYTE*	screen = (BYTE*)pFrame;		// use copy of screen pointer for safety

	if (test_mode) {
		test_2d(screen, view);
	}

	for (int i = 0; i < MAX_OBJECTS; i++) {
		if (obj_loaded[i] == 1) {
			obj_tmp[i]=obj_lst[i];
			scale_obj(&obj_tmp[i],(double)obj_tmp[i].ObjProps.Scale.x/100,(double)obj_tmp[i].ObjProps.Scale.y/100,(double)obj_tmp[i].ObjProps.Scale.z/100);
			rotate_x(&obj_tmp[i],D2R(obj_lst[i].ObjProps.Rotation.x));
			rotate_y(&obj_tmp[i],D2R(obj_lst[i].ObjProps.Rotation.y));
			rotate_z(&obj_tmp[i],D2R(obj_lst[i].ObjProps.Rotation.z));

			draw_vjs(&obj_tmp[i],screen,view);
		}
	}

	compass_t_temp = compass_t;

	rotate_x(compass_temp,D2R(obj_lst[selected_obj].ObjProps.Rotation.x));
	rotate_y(compass_temp,D2R(obj_lst[selected_obj].ObjProps.Rotation.y));
	rotate_z(compass_temp,D2R(obj_lst[selected_obj].ObjProps.Rotation.z));

	draw_vjs(compass_temp, screen, view);

	for (int i = 0; i < PIXELS; i++) {
		ZBuffer[i] = INT_MAX;
	}
}
