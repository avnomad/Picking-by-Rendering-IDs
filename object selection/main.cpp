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

RGBColor color_table[TABLE_ENTRIES];

GLint selection = 255;

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
			glVertex3i(c,c,-c);
			glVertex3i(c+2,c,-c);
			glVertex3i(c+2,c+3,-c);
			glVertex3i(c,c+3,-c);
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
			glVertex3i(selection,selection,-selection);
			glVertex3i(selection+2,selection,-selection);
			glVertex3i(selection+2,selection+3,-selection);
			glVertex3i(selection,selection+3,-selection);
		glEnd();
		glLineWidth(1);
		glColor3f(0,0,1);
		glBegin(GL_QUADS);
			glVertex3i(selection,selection,-selection);
			glVertex3i(selection+2,selection,-selection);
			glVertex3i(selection+2,selection+3,-selection);
			glVertex3i(selection,selection+3,-selection);
		glEnd();
	} // end if


	glutPostRedisplay();
	glutSwapBuffers();
} // end function display


void mouse_move(int x, int y)
{
	glReadPixels(x,glutGet(GLUT_WINDOW_HEIGHT)-1-y,1,1,GL_STENCIL_INDEX,GL_INT,&selection);
} // end function mouse_move


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
	glOrtho(0.0, TABLE_ENTRIES+1, 0, TABLE_ENTRIES+2, 0, 2*TABLE_ENTRIES);
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

	// event handling initialization
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(mouse_move);
	glutMainLoop();
	return 0;
} // end function main
