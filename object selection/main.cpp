#include <gl/glew.h>
#include <gl/glut.h>
#include <cstdlib>
#include <Color/wavelength to RGB.h>

#define TABLE_ENTRIES 16


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


RGBColor color_table[TABLE_ENTRIES];
Point coord_table[TABLE_ENTRIES];

GLint selection = 255;
int oldx;
int oldy;


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glPolygonMode(GL_FRONT,GL_FILL);
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		glColor3fv((float*)&color_table[c]);
		glStencilFunc(GL_ALWAYS,c,0xffffffff);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[c].x,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+60,coord_table[c].y,-c);
			glVertex3i(coord_table[c].x+60,coord_table[c].y+90,-c);
			glVertex3i(coord_table[c].x,coord_table[c].y+90,-c);
		glEnd();
	} // end for

	if(selection != 255)
	{
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glPolygonMode(GL_FRONT,GL_LINE);
		glLineWidth(3);
		glColor3f(1,1,0);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[selection].x,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+60,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+60,coord_table[selection].y+90,-selection);
			glVertex3i(coord_table[selection].x,coord_table[selection].y+90,-selection);
		glEnd();
		glLineWidth(1);
		glColor3f(0,0,1);
		glBegin(GL_QUADS);
			glVertex3i(coord_table[selection].x,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+60,coord_table[selection].y,-selection);
			glVertex3i(coord_table[selection].x+60,coord_table[selection].y+90,-selection);
			glVertex3i(coord_table[selection].x,coord_table[selection].y+90,-selection);
		glEnd();
	} // end if


	glutPostRedisplay();
	glutSwapBuffers();
} // end function display


void mouse_move(int x, int y)
{
	glReadPixels(x,glutGet(GLUT_WINDOW_HEIGHT)-1-y,1,1,GL_STENCIL_INDEX,GL_INT,&selection);
} // end function mouse_move


void active_motion(int x, int y)
{
	if(selection != 255)
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
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, w, 0, h, 0, 2*TABLE_ENTRIES);
} // end function reshape


int main(int argc, char **argv)
{
	// glut initialization
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(640,640);
	glutInitWindowPosition(32,32);
	glutCreateWindow("Creating Geometry");

	// glew initialization
	glewInit();

	// OpenGL initialization
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_STENCIL_TEST);
	glClearStencil(255);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glClear(GL_STENCIL_BUFFER_BIT);

	// color table initialization
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		float f = (640-380)*((float)c / TABLE_ENTRIES) + 380;
		color_table[c].r = spectrumRed(f);
		color_table[c].g = spectrumGreen(f);
		color_table[c].b = spectrumBlue(f);
	} // end for

	// coordinate table initialization
	for(int c = 0 ; c < TABLE_ENTRIES ; ++c)
	{
		coord_table[c].x = 30*c;
		coord_table[c].y = 30*c;
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
