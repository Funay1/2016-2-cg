#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>

GLfloat ctrlpoints[4][3] = {(-4.0 -4.0 0.0), (-2.0 4.0 0.0), (2.0 -4.0 0.0), (4.0 4.0 0.0)};

void init(void){
	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_FLAT);
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
	glEnable(GL_MAP1_VERTEX_3);
}

void display(void){
	int i;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1,1,1);
	glBegin(GL_LINE_STRIP);
		for(i = 0; i <= 30; i++)
			glEvalCoord1f(GLfloat, i/30.0);
	glEnd();

	glPointSize(5.0);
	glColor3f(1,1,1);
	glBegin(GL_POINTS);
		for(i = 0; i < 4; i++)
			glVertex3fv(&ctrlpoints[i][0]);
	glEnd();
	glFlush();
}

void reshape(int w, int h){
	float size = 5.0;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w <= h)
		glOrtho(-size, size, -size*(GLfloat)h/(GLfloat)w,
				size*(GLfloat)h/(GLfloat)w, -size, size);
	else
		glOrtho(-size*(GLfloat)w/(GLfloat)h,
				 size*(GLfloat)h/(GLfloat)w, -size, size, -size, size);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // For complex applications
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[1]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
