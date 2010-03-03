#include <gl/glew.h>
#include <gl/freeglut.h>
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
#include <OpenGL/shader loader.h>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::clog;
using std::left;


#define SQUARES 13107
#define COLORS 17

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
	GLint z;
}; // end struct Point

RGBColor color_table[COLORS] = {
	{1,0,0},{1,1,0},{1,0.75,0},{1,0.5,0},
	{0,0.25,0},{0,0.5,0},{0,0.75,0},{0,1,0},
	{0.25,0,0},{0.5,0.5,0},{1,0,0.5},{0,0,1},
	{0.75,0.75,1},{0.5,0.5,1},{0,1,1},{0,0.5,1},
	{0.5,0.25,0}
};

GLuint clearID[4] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};

GLuint vertex_array;
GLuint buffer_object;

GLuint color_program;
GLint color_location;
GLuint id_program;
GLint id_location;
GLint color_attachment;

int oldx;
int oldy;
double oldtime;
FPS<double> stat(20);
unsigned int frame_count;
double last_stat;

GLuint selection = 0xffffffff;
GLuint framebuffer;
GLuint color,depth;


void display()
{
	// draw color
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glDrawBuffer(GL_FRONT);
	glUseProgram(color_program);
	glBindVertexArray(vertex_array);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int c = 0 ; c < SQUARES ; ++c)
	{
		glUniform3fv(color_location,1,(float*)&color_table[c % COLORS]);
		glDrawArrays(GL_QUADS,4*c,4);
	} // end for


	// draw ids
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,framebuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0+color_attachment);
	glUseProgram(id_program);

	glClear(GL_DEPTH_BUFFER_BIT);
	glClearBufferuiv(GL_COLOR,0,clearID);

	for(unsigned int c = 0 ; c < SQUARES ; ++c)
	{
		glUniform1ui(id_location,c);
		glDrawArrays(GL_QUADS,4*c,4);
	} // end for

	// draw highlight
	if(selection != 0xffffffff)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
		glDrawBuffer(GL_FRONT);
		glUseProgram(color_program);

		glPolygonMode(GL_FRONT,GL_LINE);

		glLineWidth(3);
		glUniform3f(color_location,1.0,1.0,0.0);
		glDrawArrays(GL_QUADS,4*selection,4);

		glLineWidth(1);
		glUniform3f(color_location,0.0,0.0,1.0);
		glDrawArrays(GL_QUADS,4*selection,4);

		glPolygonMode(GL_FRONT,GL_FILL);
	} // end if


	// draw fps
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glDrawBuffer(GL_FRONT);
	glUseProgram(0);
	glBindVertexArray(0);

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

	GL::printError();
	glutPostRedisplay();
	//glutSwapBuffers();
	glFinish();
	//glFlush();
} // end function display


void mouse_move(int x, int y)
{
	glReadPixels(x,glutGet(GLUT_WINDOW_HEIGHT)-1-y,1,1,GL_RED_INTEGER,GL_UNSIGNED_INT,&selection);
} // end function mouse_move


void active_motion(int x, int y)
{
	if(selection != 0xffffffff)
	{
		Point *buffer = (Point *)glMapBufferRange(GL_ARRAY_BUFFER,4*selection*3*sizeof(GLint),4*3*sizeof(GLint),GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);

		buffer[0].x += x-oldx;
		buffer[0].y -= y-oldy;
		buffer[1].x = buffer[0].x+10;
		buffer[1].y = buffer[0].y;
		buffer[2].x = buffer[0].x+10;
		buffer[2].y = buffer[0].y+10;
		buffer[3].x = buffer[0].x;
		buffer[3].y = buffer[0].y+10;

		glUnmapBuffer(GL_ARRAY_BUFFER);
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
	glRenderbufferStorage(GL_RENDERBUFFER,GL_R32UI,w,h);
	glBindRenderbuffer(GL_RENDERBUFFER,depth);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,w,h);

	oldtime = CPUclock::currentTime();
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w,0,h,0,2*SQUARES);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(640,640);
	glutInitWindowPosition(32,32);
	glutCreateWindow("Creating Geometry");

	// glew initialization
	glewInit();
	glBindFragDataLocation = (void (__stdcall*)(GLuint,GLuint,const char*))glutGetProcAddress("glBindFragDataLocation");
	glGetFragDataLocation = (GLint (__stdcall*)(GLuint,const char*))glutGetProcAddress("glGetFragDataLocation");
	glUniform1ui = (void (__stdcall*)(GLint,GLuint))glutGetProcAddress("glUniform1ui");
	glClearBufferuiv = (void (__stdcall*)(GLenum,GLint,const GLuint *))glutGetProcAddress("glClearBufferuiv");

	// CPU clock initialization
	CPUclock::setUnit("s");	// s for seconds

	// shader initialization
	color_program = load_shader_program("color.vert","color.frag");
	color_location = glGetUniformLocation(color_program,"color");
	id_program = load_shader_program("ID.vert","ID.frag");
	id_location = glGetUniformLocation(id_program,"idin");
	color_attachment = glGetFragDataLocation(id_program,"idout");

	// OpenGL initialization
	glGenFramebuffers(1,&framebuffer);	// framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);

	glGenRenderbuffers(1,&color);	// color renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER,color);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_R32UI,640,640);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+color_attachment,GL_RENDERBUFFER,color);

	glGenRenderbuffers(1,&depth);	// depth renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER,depth);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,640,640);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depth);

	glDrawBuffer(GL_COLOR_ATTACHMENT0+color_attachment);
	glReadBuffer(GL_COLOR_ATTACHMENT0+color_attachment);

	GL::printError();
	GL::printFramebufferCompletenessStatus();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.05f,0.05f,0.05f,0.0f);
	glClearBufferuiv(GL_COLOR,0,clearID);


	// vertex array object initialization
	glGenVertexArrays(1,&vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1,&buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER,buffer_object);
	glBufferData(GL_ARRAY_BUFFER,SQUARES*4*3*sizeof(GLint),NULL,GL_DYNAMIC_DRAW);

	Point *buffer = (Point *)glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
	for(int c = 0 ; c < SQUARES ; ++c)
	{
		buffer[4*c+0].x = 10*((c & 0x7f));
		buffer[4*c+0].y = 10*(c >> 7);
		buffer[4*c+0].z = -c;
		buffer[4*c+1].x = buffer[4*c+0].x+10;
		buffer[4*c+1].y = buffer[4*c+0].y;
		buffer[4*c+1].z = buffer[4*c+0].z;
		buffer[4*c+2].x = buffer[4*c+0].x+10;
		buffer[4*c+2].y = buffer[4*c+0].y+10;
		buffer[4*c+2].z = buffer[4*c+0].z;
		buffer[4*c+3].x = buffer[4*c+0].x;
		buffer[4*c+3].y = buffer[4*c+0].y+10;
		buffer[4*c+3].z = buffer[4*c+0].z;
	} // end for
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexPointer(3,GL_INT,0,0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
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
