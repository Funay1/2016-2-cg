#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <cstring>
#include <math.h>
#include <vector>

#include <GL/glut.h>	// GLUT, includes glu.h and gl.h
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

#define PI 3.14159265359

class Point {
	public:
		double x;
		double y;
		Point(double a, double b){
			x = a;
			y = b;
		}
		double distance(Point *p2){
			return sqrt(pow((this->x - p2->x), 2) + pow((this->y - p2->y), 2));
		}
};

class Color {
	public:
		float R;
		float G;
		float B;
		Color(string color){
			if(!color.compare("black")){
				this->R = 0;
				this->G = 0;
				this->B = 0;
			}
			else if(!color.compare("white")){
				this->R = 1;
				this->G = 1;
				this->B = 1;
			}
			else if(!color.compare("blue")){
				this->R = 0;
				this->G = 0;
				this->B = 1;
			}
			else if(!color.compare("red")){
				this->R = 1;
				this->G = 0;
				this->B = 0;
			}
			else if(!color.compare("green")){
				this->R = 0;
				this->G = 1;
				this->B = 0;
			}
		}
};

enum Type {circle, rectangle};

class Shape {
	public:
		string id;
		Type type;
		Point *center;
		Color *fill;
		Shape (string i, Type t, Point *c, Color *f){
			id = i;
			type = t;
			center = c;
			fill = f;
		}
		virtual void draw() const {
		}
		virtual bool colide(Shape*) const {
			return false;
		}
};

class Circle : public Shape {
	public:
		float radius;
		Circle(string i, Type t, Point *c, Color *f, float r) :Shape (i, t, c, f){
			radius = r;
		}

		void modify_x(float inc) {
			this->center->x = this->center->x + inc;
		}

		void modify_y(float inc) {
			this->center->y = this->center->y + inc;
		}

		bool inside (Shape* s){
			Circle *c = (Circle*)(s);
			if (this->center->distance(c->center) <= (c->radius - this->radius)) {
				return false;
			}
			return true;
		}

		bool colide (Shape* s){
			Circle *c = (Circle*)(s);
			if (this->center->distance(c->center) <= (c->radius + this->radius)) {
				return false;
			}
			return true;
		}

		bool colide (vector<Shape*> s){
			for (std::vector<Shape*>::const_iterator i = s.begin(); i != s.end(); ++i){
				Circle *c = (Circle*)(*i);
				if (this->center->distance(c->center) <= (c->radius + this->radius)) {
					return false;
				}
			}
			return true;
		}
	/*
	* Function that handles the drawing of a circle using the triangle fan
	* method. This will create a filled circle.
	*
	* Params:
	*	x (GLFloat) - the x position of the center point of the circle
	*	y (GLFloat) - the y position of the center point of the circle
	*	radius (GLFloat) - the radius that the painted circle will have
	*/
	void draw() const {
		int i;
		int triangleAmount = 50	; //# of triangles used to draw circle
		//GLfloat radius = 0.8f; //radius
		GLfloat twicePi = 2.0f * PI;

		glBegin(GL_TRIANGLE_FAN);
			glColor3f(this->fill->R, this->fill->G, this->fill->B);
			glVertex2f(this->center->x, this->center->y); // center of circle
			for(i = 0; i <= triangleAmount;i++) {
				glVertex2f(this->center->x + (this->radius * cos(i *	twicePi / triangleAmount)),
									 this->center->y + (this->radius * sin(i * twicePi / triangleAmount))
									 );
			}
		glEnd();
	}
};


class Rect : public Shape {
	int height;
	int width;
	public:
		Rect(string i, Type t, Point *c, Color *f, int w, int h) :Shape (i, t, c, f){
			width = w;
			height = h;
		}

		void draw(void) const {
			glBegin(GL_QUADS);
				glColor3f(this->fill->R, this->fill->G, this->fill->B);
				glVertex2f(this->center->x, this->center->y);
				glVertex2f(this->center->x + this->width, this->center->y);
				glVertex2f(this->center->x + this->width, this->center->y + this->height);
				glVertex2f(this->center->x, this->center->y + this->height);
			glEnd();
	}
};

/* Global Variables */
string svg_name, svg_ext, svg_path;
GLfloat gx = 0, gy = 0;
int keypress[256];
vector<Shape*> scenario;
int counter_arena = 0;
Circle* arena[2];
Circle* player;
Rect* largada;

void keypressed(unsigned char key, int x, int y){
	keypress[key] = 1;
}

void keyunpressed(unsigned char key, int x, int y){
	keypress[key] = 0;
}

void idle(void) {
	if(keypress['w'] == 1 || keypress['W'] == 1){
		player->modify_y(0.1);
		if(!player->colide(scenario) || !player->colide(arena[1]) || player->inside(arena[0])){
			player->modify_y(-0.1);
		}
	}
	if(keypress['s'] == 1 || keypress['S'] == 1){
		player->modify_y(-0.1);
		if(!player->colide(scenario) || !player->colide(arena[1]) || player->inside(arena[0])){
			player->modify_y(0.1);
		}
	}
	if(keypress['d'] == 1 || keypress['D'] == 1){
		player->modify_x(0.1);
		if(!player->colide(scenario) || !player->colide(arena[1]) || player->inside(arena[0])){
			player->modify_x(-0.1);
		}
	}
	if(keypress['a'] == 1 || keypress['A'] == 1){
		player->modify_x(-0.1);
		if(!player->colide(scenario) || !player->colide(arena[1]) || player->inside(arena[0])){
			player->modify_x(0.1);
		}
	}
	glutPostRedisplay();
}


/* Function that parser a xml document and saves the content into global variables
*	Global variables were used cause it was the only way I could come up to pass ahead to the
*	callback functions
*/

void parserXML(const char* path) {
	XMLDocument xml_doc;

	string file = string(path);
	file = file + "config.xml";
	const char* filename = file.c_str();
	xml_doc.LoadFile(filename);

	XMLNode * pRoot = xml_doc.FirstChild();
	XMLElement * pSVGAttr = pRoot->FirstChildElement("arquivoDaArena");

	svg_name = pSVGAttr->Attribute("nome");
	svg_ext = pSVGAttr->Attribute("tipo");
	svg_path = pSVGAttr->Attribute("caminho");

	return;
}

void parserSVG(const char* svg_img) {
	XMLDocument xml_doc;

	xml_doc.LoadFile(svg_img);

	XMLNode * pRoot = xml_doc.FirstChild();
	XMLElement * obj	= pRoot->FirstChildElement("circle");

	for (int i = 0; i < 10; i++){
		int cx, cy, r, height, width, x, y;
		string fill_value, id, name;
		obj->QueryIntAttribute("cx", &cx);
		obj->QueryIntAttribute("cy", &cy);
		Point *p = new Point(cx, cy);

		fill_value = obj->Attribute("fill");
		Color *fill = new Color(fill_value);

		id = obj->Attribute("id");
		name = string(obj->Name());

		if(name == "rect") {
			obj->QueryIntAttribute("x", &x);
			obj->QueryIntAttribute("y", &y);
			Point *p = new Point(x, y);
			obj->QueryIntAttribute("height", &height);
			obj->QueryIntAttribute("width", &width);
			largada = new Rect(id, rectangle, p, fill, width, height);
		}
		else if (!id.compare("Jogador")){
			obj->QueryIntAttribute("r",&r);
			player = new Circle(id, circle, p, fill, r);
		}
		else if (!id.compare("Pista")){
				obj->QueryIntAttribute("r",&r);
				arena[counter_arena++] = new Circle(id, circle, p, fill, r);
		}
		else if (!id.compare("Inimigo")){
			obj->QueryIntAttribute("r",&r);
			scenario.push_back(new Circle(id, circle, p, fill, r));
		}
		if((obj = obj->NextSiblingElement()) == NULL)
			break;
	}

	Circle *tmp;
	if(arena[0]->radius < arena[1]->radius) {
		tmp = arena[0];
		arena[0] = arena[1];
		arena[1] = tmp;
	}

	return;
}

/* Handler for window-repaint event. Call back when the window first appears and
	 whenever the window needs to be re-painted. */
void display() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);							 // Clean the screen and the depth buffer
		glLoadIdentity();
		arena[0]->draw();
		arena[1]->draw();
		for (std::vector<Shape*>::const_iterator i = scenario.begin(); i != scenario.end(); ++i)
			(*i)->draw();
		largada->draw();
		player->draw();

		glFlush();																											 // Render now
}

/* Main function: GLUT runs as a console application starting at main()	*/
int main(int argc, char** argv) {
		parserXML (argv[1]);
		string file = svg_path + svg_name + "." +svg_ext;
		parserSVG (file.c_str());

		glutInit(&argc, argv);													 // Initialize GLUT
		glutInitWindowSize(2*arena[0]->radius, 2*arena[0]->radius);	                 // Set the window's initial width & height
		glutInitWindowPosition(arena[0]->center->x, arena[0]->center->y);								// Position the window's initial top-left corner
		glutCreateWindow("Jogo");							          // Create window with the given title
		gluOrtho2D(arena[0]->center->x - arena[0]->radius,
			arena[0]->center->x + arena[0]->radius,
			arena[0]->center->x - arena[0]->radius,
			arena[0]->center->x + arena[0]->radius);		                // Set clipping area's left, right, bottom, top
		glMatrixMode(GL_PROJECTION);										// Select the Projection matrix for operation
		glLoadIdentity();																// Reset Projection matrix
		glutDisplayFunc(display);												// Register display callback handler for window re-paint
		glutKeyboardFunc(keypressed);
		glutKeyboardUpFunc(keyunpressed);
		glutIdleFunc(idle);
		glutMainLoop();																	// Enter the event-processing loop
		return 0;
}
