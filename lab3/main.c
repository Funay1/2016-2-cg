//gcc main\ \(cópia\).c -lglut -lGLU -lGL -lm

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define paddleHeight 80
#define	paddleWidth 10
#define baseHeight 40
#define baseWidth 100
#define radiusWheel 30
#define zAxis 0
#define delta 0.5

int keystatus[256];
float r=1,g=1,b=1;
GLfloat gx=0 , gy= -100,tw = 0,t1 = 0,t2 = 0,t3 = 0 ;
GLfloat theta1 = 0, theta2 = 0, theta3 = 0;

void draw_rectangle(int height, int widith, float r,float g,float b){
	glColor3f(r,g,b);
	glBegin(GL_QUADS);
		glVertex2f(-widith/2.0,0.0);
		glVertex2f(-widith/2.0,height);
		glVertex2f(widith/2.0,height);
		glVertex2f(widith/2.0,0.0);
	glEnd();
}

void draw_circle(float radius,float r,float g,float b){
	glColor3f(r,g,b);
	glPointSize(3);
	glBegin(GL_POINTS);
		for(int i = 0; i < 360 ;i += 20){
			glVertex2f(radius*cos(M_PI*i/180),radius*sin(M_PI*i/180));
		}
	glEnd();
}

void desenhaBraco(float x, float y){
	glPushMatrix();
	glColor3f(r,g,b);
	glTranslatef(0,y,0);
	glRotatef(theta1,0,0,1);
	draw_rectangle(paddleHeight, paddleWidth, 0,0,1);
	glTranslatef(0,paddleHeight,0);
	glRotatef(theta2,0,0,1);
	draw_rectangle(paddleHeight, paddleWidth, 0,1,0);
	glTranslatef(0,paddleHeight,0);
	glRotatef(theta3,0,0,1);
	draw_rectangle(paddleHeight, paddleWidth, 1,0,0);
	glPopMatrix();

}

void draw_robo(float x,float y,float tw,float t1,float t2,float t3){
	glTranslatef(gx, gy, 0);
	draw_rectangle(baseHeight,baseWidth, 1,0,0);

	glPushMatrix();
		glTranslatef(-baseWidth/2,0,0);
		glRotatef(tw,0,0,1);
		draw_circle(radiusWheel,1,1,0);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(baseWidth/2,0,0);
		glRotatef(tw,0,0,1);
		draw_circle(radiusWheel,1,1,0);
	glPopMatrix();

	desenhaBraco(0, baseHeight);

}

void display(void)
{
glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

		draw_robo(gx,gy,tw,t1,t2,t3);

	glPopMatrix();

glutSwapBuffers();
}

void init (void){
glClearColor(0.0,0.0,0.0,0.0);

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-250.0,250.0,-250,250,100.0,-100.0);
}

void keyup(unsigned char key,int x,int y){
	keystatus[key] = 0;
}

void keydown(unsigned char key,int x,int y){
	keystatus[key] = 1;
}

void mouse(int button, int state, int x, int y)
{
	y = 1000-y;
	printf("%f\t%f\n",x/100.0,y/100.0);

	if (state){
		r = (int)(rand() % 100);
		r = r/100;
		g = (int)(rand() % 100);
		g = g/100;
		b = (int)(rand() % 100);
		b = b/100;
	}
}

void idle(void)
{
	for (int i=0;i<256;i++){
		if (keystatus[i]){
			switch(i){
				case 'w' :
				case 'W' :
					gy += delta;
					break;
				case 's' :
				case 'S' :
					gy -= delta;
					break;
				case 'a' :
				case 'A' :
					gx -= delta;
					tw += (delta*180/(M_PI*radiusWheel));
					break;
				case 'd' :
				case 'D' :
					gx += delta;
					tw -= (delta*180/(M_PI*radiusWheel));
					break;
				case 'e' :
				case 'E' :
					theta1 += 0.5;
					break;
				case 'r' :
				case 'R' :
					theta1 -= 0.5;
					break;
				case 't' :
				case 'T' :
					theta2 += 0.5;
					break;
				case 'y' :
				case 'Y' :
					theta2 -= 0.5;
					break;
				case 'u' :
				case 'U' :
					theta3 += 0.5;
					break;
				case 'i' :
				case 'I' :
					theta3 -= 0.5;
					break;
			}
		}
	}

	glutPostRedisplay();
}


int main (int argc, char** argv){

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("quadradeta");
	init();
	glutDisplayFunc(display);
	glutKeyboardUpFunc(keyup);
	glutKeyboardFunc(keydown);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);


	glutMainLoop();
	return 0;
}
