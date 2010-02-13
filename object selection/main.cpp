#include <gl/glew.h>
#include <gl/glut.h>
#include <cstdlib>
#include <Color/wavelength to RGB.h>
#include <FPS.h>
#include <CPU clock.h>
#include <algorithms.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <color.h>
#include <Color/glColor.h>
#include <Color/Namings/double precision colors.h>
#include <OpenGL/utility.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::clog;
using std::left;


#define TABLE_ENTRIES 13107
#define COLOR_ENTRIES 17

struct RGBColor
{
	float r;
	float g;
	float b;
}; // end struct RGBColor

struct Point
{
	GLint x;
	GLint y;
}; // end struct Point


RGBColor color_table[COLOR_ENTRIES] = {
	{1,0,0},{1,1,0},{1,0.75,0},{1,0.5,0},
	{0,0.25,0},{0,0.5,0},{0,0.75,0},{0,1,0},
	{0.25,0,0},{0.5,0.5,0},{1,0,0.5},{0,0,1},
	{0.75,0.75,1},{0.5,0.5,1},{0,1,1},{0,0.5,1},
	{0.5,0.25,0}
};
Point coord_table[TABLE_ENTRIES];

GLint selection = 0xff;
int oldx;
int oldy;
double oldtime;
FPS<double> stat(20);
unsigned int frame_count;
double last_stat;
GLuint framebuffer;
GLuint depth_stencil,color;


void display()
{
	// draw ids
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebuffer);
	glEnable(GL_STENCIL_TEST);
	glPolygonMode(GL_FRONT,GL_FILL);
	glColorMask(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		glColor3fv((float*)&color_table[c % COLOR_ENTRIES]);
		glStencilFunc(GL_ALWAYS,c,0xffffffff);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[c].x,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+10,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+10,coord_table[c].y+10,-c);
			glVertex3i(coord_table[c].x,coord_table[c].y+10,-c);
		glEnd();
	} // end for

	// draw image
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glDisable(GL_STENCIL_TEST);
	glColorMask(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		glColor3fv((float*)&color_table[c % COLOR_ENTRIES]);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[c].x,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+10,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+10,coord_table[c].y+10,-c);
			glVertex3i(coord_table[c].x,coord_table[c].y+10,-c);
		glEnd();
	} // end for

	if(selection != 0xff)
	{
		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(3);
		glColor3f(1,1,0);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[selection].x,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+10,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+10,coord_table[selection].y+10,-selection);
			glVertex3i(coord_table[selection].x,coord_table[selection].y+10,-selection);
		glEnd();
		glLineWidth(1);
		glColor3f(0,0,1);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[selection].x,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+10,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+10,coord_table[selection].y+10,-selection);
			glVertex3i(coord_table[selection].x,coord_table[selection].y+10,-selection);
		glEnd();
	} // end if

	double temp = CPUclock::currentTime();
	stat.push(temp-oldtime);
	oldtime = temp;
	if(++frame_count % 20 == 0)
		last_stat = stat.fps();
	std::ostringstream sout;
	sout << std::fixed << std::setprecision(1) << last_stat << " fps";
	glLineWidth(4);
	glColor(blue);
	displayString(Vector2D<>(10,glutGet(GLUT_WINDOW_HEIGHT)-26),25,1,0,sout.str().c_str());
	glLineWidth(2);
	glColor(yellow);
	displayString(Vector2D<>(10,glutGet(GLUT_WINDOW_HEIGHT)-26),25,1,0,sout.str().c_str());
	if(frame_count % 1000 == 0)
		stat.recalculateSum();

	glutPostRedisplay();
	glutSwapBuffers();
} // end function display


void mouse_move(int x, int y)
{
	glReadPixels(x,glutGet(GLUT_WINDOW_HEIGHT)-1-y,1,1,GL_STENCIL_INDEX,GL_INT,&selection);
} // end function mouse_move


void active_motion(int x, int y)
{
	if(selection != 0xff)
	{
		coord_table[selection].x += x-oldx;
		coord_table[selection].y -= y-oldy;
	} // end if
	oldx = x;
	oldy = y;
} // end function active_motion


void mouse_klick(int button,int state,int x,int y)
{
	if(state == GLUT_DOWN)
	{
		oldx = x;
		oldy = y;
	} // end if
} // end function mouse_klick


void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:	// escape key
		exit(0);
	} // end switch
} // end function keyboard


void reshape(int w, int h)
{
	glBindRenderbuffer(GL_RENDERBUFFER,color);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA,w,h);
	glBindRenderbuffer(GL_RENDERBUFFER,depth_stencil);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,w,h);

	oldtime = CPUclock::currentTime();
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w, 0, h, 0, 2*TABLE_ENTRIES);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640,640);
	glutInitWindowPosition(32,32);
	glutCreateWindow("Creating Geometry");

	// glew initialization
	glewInit();

	// OpenGL initialization
	glGenFramebuffers(1,&framebuffer);	// framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER,framebuffer);
	glGenRenderbuffers(1,&color);	// renderbuffers
	glBindRenderbuffer(GL_RENDERBUFFER,color);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA,640,640);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,color);
	glGenRenderbuffers(1,&depth_stencil);
	glBindRenderbuffer(GL_RENDERBUFFER,depth_stencil);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_STENCIL,640,640);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,depth_stencil);
	glDrawBuffer(GL_NONE);/**/

	GL::printError();
	GL::printFramebufferCompletenessStatus();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(0xff);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	// CPU clock initialization
	CPUclock::setUnit("s");	// s for seconds

	// coordinate table initialization
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		coord_table[c].x = 10*(c & 0x7f);
		coord_table[c].y = 10*(c >> 7);
	} // end for

	// event handling initialization
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(mouse_move);
	glutMotionFunc(active_motion);
	glutMouseFunc(mouse_klick);
	glutMainLoop();
	return 0;
} // end function main
