#include <GL/glut.h>
#include <stdio.h>

using namespace std;

GLfloat gx = 0, gy = 0;

int keypress[256];

void keypressed(unsigned char key, int x, int y){
	keypress[key] = 1;
}

void keyunpressed(unsigned char key, int x, int y){
	keypress[key] = 0;
}

void idle(void) {
	if(keypress['w'] == 1 || keypress['W'] == 1)
		gy += 0.0001;
	if(keypress['s'] == 1 || keypress['S'] == 1)
		gy -= 0.0001;
	if(keypress['d'] == 1 || keypress['D'] == 1)
		gx += 0.0001;
	if(keypress['a'] == 1 || keypress['A'] == 1)
		gx -= 0.0001;
	glutPostRedisplay();
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_POLYGON);
		glVertex3f(0.25 + gx,0.25 + gy,0.0);
		glVertex3f(0.75 + gx,0.25 + gy,0.0);
		glVertex3f(0.75 + gx,0.75 + gy,0.0);
		glVertex3f(0.25 + gx,0.75 + gy,0.0);
	glEnd();
	//glutSwapBuffers(); // When using double buffer
	glFlush();
}

void mouse(int button, int mouse_state, int x, int y){
	if(mouse_state == GLUT_DOWN)
		printf("x = %f; y = %f\n", (float)x/500, 1 - (float)y/500);
}

void init (void) {
	glClearColor(0.0,0.0,0.0,0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,1,0,1,-1,1);
}

void reshape(int w, int h){
	float size =5.0
	glViewport(0,0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w <= h)
		glOrtho(-size, size, -size*(GLfloat)h/(GLfloat)w),
				size*(GLfloat)h/(GLfloat)w, -size, size);
	else
		glOrtho(-size*(GLfloat)w/(GLfloat)h,
				 size*(GLfloat)h/(GLfloat)w, -size, size, -size, size);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // For complex applications
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow("hello");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keypressed);
	glutKeyboardUpFunc(keyunpressed);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
