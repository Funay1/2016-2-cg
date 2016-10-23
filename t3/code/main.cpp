#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <cstring>
#include <cmath>
#include <vector>
#include <sstream>

#include <GL/glut.h> // GLUT, includes glu.h and gl.h
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

#define PI 3.1415927

class Point {
public:
double x;
double y;
Point(double a,
      double b) {
        x = a;
        y = b;
}

string toString(){
        ostringstream convert;
        convert << x << ", " << y;
        return convert.str();
}

double distance(Point *p2) {
        return sqrt(pow((this->x - p2->x), 2) + pow((this->y - p2->y), 2));
}

void invert(float win_size) {
        this->y = win_size - this->y;
}

void add(Point *point) {
        this->x = this->x + point->x;
        this->y = this->y + point->y;
}
};

class Color {
public:

float R;
float G;
float B;
Color(string color) {
        if (!color.compare("black")) {
                this->R = 0;
                this->G = 0;
                this->B = 0;
        }
        else if (!color.compare("white")) {
                this->R = 1;
                this->G = 1;
                this->B = 1;
        }
        else if (!color.compare("blue")) {
                this->R = 0;
                this->G = 0;
                this->B = 1;
        }
        else if (!color.compare("red")) {
                this->R = 1;
                this->G = 0;
                this->B = 0;
        }
        else if (!color.compare("green")) {
                this->R = 0;
                this->G = 1;
                this->B = 0;
        }
        else if (!color.compare("dimgray")) {
                this->R = 0.412;
                this->G = 0.412;
                this->B = 0.412;
        }
        else if(!color.compare("gainsboro")) {
                this->R = 0.863;
                this->G = 0.863;
                this->B = 0.863;
        }
        else if(!color.compare("darkred")) {
                this->R = 0.545;
                this->G = 0;
                this->B = 0;
        }
}
};

class Shape {
public:

string id;
Point *center;
Color *fill;
Shape(string i,
      Point *c,
      Color *f) {
        id     = i;
        center = c;
        fill   = f;
}

virtual void draw() const {
}

virtual bool colide(Shape *) const {
        return false;
}
};

class Circle : public Shape {
public:
double radius;
double start;
double end;
Circle(string i,
       Point *c,
       Color *f,
       float r,
       double start,
       double end ) : Shape(i, c, f) {
        this->radius = r;
        this->start  = start;
        this->end  = end;
}

bool inside(Shape *s) {
        Circle *c = (Circle *)(s);

        if (this->center->distance(c->center) <= (c->radius - this->radius)) {
                return false;
        }
        return true;
}

bool colide(Shape *s) {
        Circle *c = (Circle *)(s);
        if (this->center->distance(c->center) <= (c->radius + this->radius)) {
                return false;
        }
        return true;
}

bool colide(vector<Shape *>s) {
        for (std::vector<Shape *>::const_iterator i = s.begin(); i != s.end(); ++i) {
                Circle *c = (Circle *)(*i);
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
        // # of triangles used to draw circle
        int triangleAmount = 60;
        // GLfloat radius = 0.8f; //radius
        GLfloat twicePi = 2.0f * PI;

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(this->fill->R, this->fill->G, this->fill->B);
        glVertex2f(this->center->x, this->center->y); // center of circle

        for (i = triangleAmount*(this->start)/(2*PI); i <= triangleAmount * (this->end)/(2*PI); i++) {
                glVertex2f(this->center->x +
                           (this->radius * cos(i * twicePi / triangleAmount)),
                           this->center->y +
                           (this->radius * sin(i * twicePi / triangleAmount))
                           );
        }
        glEnd();
}
};

class Rect : public Shape {
public:
double height;
double width;
Rect(string i,
     Point *c,
     Color *f,
     double w,
     double h) : Shape(i, c, f) {
        width  = w;
        height = h;
}

void draw(void) const {
        glBegin(GL_QUADS);
        glColor3f(this->fill->R, this->fill->G, this->fill->B);
        glVertex2f(this->center->x, this->center->y);
        glVertex2f(this->center->x + this->width, this->center->y);
        glVertex2f(this->center->x + this->width, this->center->y - this->height);
        glVertex2f(this->center->x, this->center->y - this->height);
        glEnd();
}
};


class Shot {
public:
Circle* formato;
double attitute;

Shot(double radius, Point *position, double angle_cannon) {
        this->formato = new Circle("shot",
                                   new Point(position->x, position->y),
                                   new Color("red"),
                                   radius,
                                   0,
                                   2*PI);
        this->attitute = angle_cannon;
}

void draw(Point *window_center, double window_size, double ratio) {
        glPushMatrix();
        glTranslatef(-(window_center->x - this->formato->center->x)/window_size,
                     -(window_center->y - this->formato->center->y)/window_size,
                     0);
        glScalef(ratio, ratio, 0);
        glTranslatef((window_center->x - this->formato->center->x)/window_size,
                     (window_center->y - this->formato->center->y)/window_size,
                     0);
        formato->draw();
        glPopMatrix();
}

void kinematics(double t, double speed_shot){
        this->formato->center->x += t*(speed_shot*cos((this->attitute - 90)*PI/180));
        this->formato->center->y += t*(speed_shot*sin((this->attitute - 90)*PI/180));
}
};

class Window {
public:
Point *center;
double height;
double width;
Window(double x, double y, double height, double width){
        this->center = new Point(x, y);
        this->height = height;
        this->width = width;
}
};

class Car {
public:
//Given by the arena;
vector<Shape *> shapes;
vector<Rect *> steps;

/* Needed for ajusts */
Point  *center;
Point  *position;
Point  *window_center;
double window_size;

/* Needed for kinematics */
Point  *axis_back_center;
Circle *body;
Circle *colisor;
double d;
double angle_body;
double angle_wheel;
double speed_current;
Rect   *cannon;
double angle_cannon;
double speed_car;
double speed_shot;
double ratio;
double size_step;
double ref_step;

void draw(vector<Shot *> shots) const {

        glPushMatrix();
        glTranslatef((this->position->x - this->axis_back_center->x)/this->window_size,
                     (this->position->y - this->axis_back_center->y)/this->window_size,
                     0);
        glTranslatef(-(this->window_center->x - this->axis_back_center->x)/this->window_size,
                     -(this->window_center->y - this->axis_back_center->y)/this->window_size,
                     0);
        glRotatef(this->angle_body, 0,0,1);
        glScalef(this->ratio, this->ratio, 0);
        glTranslatef((this->window_center->x - this->axis_back_center->x)/this->window_size,
                     (this->window_center->y - this->axis_back_center->y)/this->window_size,
                     0);
        for (std::vector<Shape *>::const_iterator i = this->shapes.begin();
             i != this->shapes.end(); ++i) {
                (*i)->draw();
        }

        glPushMatrix();
        glTranslatef(-(this->window_center->x - this->center->x)/this->window_size,
                     -(this->window_center->y - this->center->y)/this->window_size,
                     0);
        glRotatef(this->angle_cannon, 0,0,1);
        glTranslatef((this->window_center->x - this->center->x)/this->window_size,
                     (this->window_center->y - this->center->y)/this->window_size,
                     0);
        this->cannon->draw();
        glPopMatrix();

        /* Draw the 2 wheels. To do that, it is needed to move to the center of the screen and then move to the next point */
        for (std::vector<Rect *>::const_iterator i = this->steps.begin();
             i != this->steps.end(); ++i) {
                glPushMatrix();
                glTranslatef(-(window_center->x - ((*i)->center->x + (*i)->width/2))/this->window_size,
                             -(window_center->y - ((*i)->center->y - (*i)->height/2))/this->window_size,
                             0);
                glRotatef(angle_wheel, 0,0,1);
                glTranslatef((window_center->x - ((*i)->center->x + (*i)->width/2))/this->window_size,
                             (window_center->y - ((*i)->center->y - (*i)->height/2))/this->window_size,
                             0);
                (*i)->draw();
                glPopMatrix();
        }
        glPopMatrix();

        for (std::vector<Shot *>::const_iterator i = shots.begin();
             i != shots.end(); ++i) {
                (*i)->draw(this->window_center, this->window_size, this->ratio);
        }
}

void modify_angle_wheel(double value){
        if(this->angle_wheel + value <= 45 && this->angle_wheel + value >= -45)
                this->angle_wheel += value;
}

void kinematics(double t, vector<Shape *> scenario, Circle *arena[2]){
        /* Based on the model of a car-like vehicle, we calculate the moviment and in the case of a colision, it undo it */
        this->position->x = t*(this->speed_current*cos((angle_body - 90)*PI/180)) + this->position->x;
        this->colisor->center->x = this->position->x + cos((angle_body - 90)*PI/180)*this->d*(this->ratio)/2;
        if (!this->colisor->colide(scenario) || !this->colisor->colide(arena[1]) || this->colisor->inside(arena[0]))
                this->position->x = this->position->x - t*(this->speed_current*cos((angle_body - 90)*PI/180));

        this->position->y = t*(this->speed_current*sin((angle_body - 90)*PI/180)) + this->position->y;
        this->colisor->center->y = this->position->y + sin((angle_body - 90)*PI/180)*this->d*(this->ratio)/2;
        if (!this->colisor->colide(scenario) || !this->colisor->colide(arena[1]) || this->colisor->inside(arena[0]))
                this->position->y = this->position->y -  t*(this->speed_current*sin((angle_body - 90)*PI/180));

        this->angle_body  = t*(this->speed_current*tan((this->angle_wheel)*PI/180)) + this->angle_body;
        if (!this->colisor->colide(scenario) || !this->colisor->colide(arena[1]) || this->colisor->inside(arena[0]))
                this->angle_body  = this->angle_body - t*(this->speed_current*tan((this->angle_wheel)*PI/180));


        static int counter = 0;
        counter++;
        for (std::vector<Shape *>::const_iterator i = this->shapes.begin();
             i != this->shapes.end(); ++i) {
                if((*i)->id == "strip" && this->speed_current != 0) {
                        (*i)->center->y = this->ref_step - counter%(int)this->size_step;
                        counter = counter%(int)this->size_step;
                }
        }
}
};

/* Global Variables */
string svg_name, svg_ext, svg_path;
GLfloat speed_shot, speed_car;
GLfloat gx = 0, gy = 0;
int keypress[256];
vector<Shape *> scenario;
vector<Shot *> shots;
int counter_arena = 0;
Circle *arena[2];
Circle *player;
Rect *largada;
Window* window;
Car *car = new Car();

void keypressed(unsigned char key, int x, int y) {
        keypress[key] = 1;
}

void keyunpressed(unsigned char key, int x, int y) {
        keypress[key] = 0;
}

void mouse(int button, int mouse_state, int x, int y) {
        if(mouse_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
                double x = car->colisor->center->x + car->ratio*car->cannon->height*cos((car->angle_cannon + car->angle_body - 90)*PI/180);
                double y = car->colisor->center->y + car->ratio*car->cannon->height*sin((car->angle_cannon + car->angle_body - 90)*PI/180);
                Point *position = new Point(x, y);
                shots.push_back(new Shot(car->cannon->width/2, position, car->angle_cannon + car->angle_body));
        }
}

void idle(void) {
        if(((keypress['w'] == 1) || (keypress['W'] == 1)) && ((keypress['s'] == 1) || (keypress['S'] == 1))) {
                car->speed_current = 0;
        }
        else if ((keypress['w'] == 1) || (keypress['W'] == 1)) {
                car->speed_current = car->speed_car;
        }
        else if ((keypress['s'] == 1) || (keypress['S'] == 1)) {
                car->speed_current = (-1)*car->speed_car;
        }
        else {
                car->speed_current = 0;
        }
        if ((keypress['d'] == 1) || (keypress['D'] == 1)) {
                car->modify_angle_wheel(-5);
        }
        if ((keypress['a'] == 1) || (keypress['A'] == 1)) {
                car->modify_angle_wheel(+5);
        }

        GLdouble currentTime;
        GLdouble timeDiference;
        static GLdouble previousTime = 0;

        // Elapsed time from the initiation of the game.
        currentTime = glutGet(GLUT_ELAPSED_TIME);
        timeDiference = currentTime - previousTime;    // Elapsed time from the previous frame.
        previousTime = currentTime;    //Update previous time

        car->kinematics(timeDiference, scenario, arena);

        for (std::vector<Shot *>::const_iterator i = shots.begin();
             i != shots.end(); ++i) {
                (*i)->kinematics(timeDiference, speed_shot);
        }
        glutPostRedisplay();
}

/* Function that parser a xml document and saves the content into global
 *   variables
 *	Global variables were used cause it was the only way I could come up to
 *  pass ahead to the
 *	callback functions
 */
void parserXML(const char *path) {
        XMLDocument xml_doc;

        string file = string(path);

        file = file + "config.xml";
        const char *filename = file.c_str();
        xml_doc.LoadFile(filename);

        XMLNode *pRoot       = xml_doc.FirstChild();
        XMLElement *pSVGAttr = pRoot->FirstChildElement("arquivoDaArena");

        svg_name = pSVGAttr->Attribute("nome");
        svg_ext  = pSVGAttr->Attribute("tipo");
        svg_path = pSVGAttr->Attribute("caminho");

        XMLElement *pCarAttr = pRoot->FirstChildElement("carro");
        pCarAttr->QueryDoubleAttribute("velTiro",  &car->speed_shot);
        pCarAttr->QueryDoubleAttribute("velCarro", &car->speed_car);
        speed_shot = car->speed_shot;
}

void parserSVG(const char *svg_img) {
        XMLDocument xml_doc;

        xml_doc.LoadFile(svg_img);

        XMLNode *pRoot  = xml_doc.FirstChild();
        XMLElement *obj = pRoot->FirstChildElement("circle");

        for (int i = 0; i < 10; i++) {
                double cx, cy, r, height, width, x, y;
                string fill_value, id, name;
                obj->QueryDoubleAttribute("cx", &cx);
                obj->QueryDoubleAttribute("cy", &cy);
                Point *p = new Point(cx, cy);

                fill_value = obj->Attribute("fill");
                Color *fill = new Color(fill_value);

                id   = obj->Attribute("id");
                name = string(obj->Name());

                if (name == "rect") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        Point *p = new Point(x, y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                        largada = new Rect(id, p, fill, width, height);
                }
                else if (!id.compare("Jogador")) {
                        obj->QueryDoubleAttribute("r", &r);
                        player = new Circle(id, p, fill, r, 0, 2*PI);
                }
                else if (!id.compare("Pista")) {
                        obj->QueryDoubleAttribute("r", &r);
                        arena[counter_arena++] = new Circle(id, p, fill, r, 0, 2*PI);
                }
                else if (!id.compare("Inimigo")) {
                        obj->QueryDoubleAttribute("r", &r);
                        scenario.push_back(new Circle(id, p, fill, r, 0, 2*PI));
                }

                if ((obj = obj->NextSiblingElement()) == NULL) break;
        }

        window = new Window(arena[0]->center->x, arena[0]->center->y,
                            2*arena[0]->radius, 2*arena[0]->radius);

        Circle *tmp;
        if(arena[0]->radius < arena[1]->radius) {
                tmp = arena[0];
                arena[0] = arena[1];
                arena[1] = tmp;
        }

        float win_size = 2*arena[0]->radius;

        arena[0]->center->invert(win_size);
        arena[1]->center->invert(win_size);
        for (std::vector<Shape*>::const_iterator i = scenario.begin(); i != scenario.end(); ++i) {
                (*i)->center->invert(win_size);
        }
        largada->center->invert(win_size);
        player->center->invert(win_size);
        car->position = player->center;
}

void parserCarSVG(const char *svg_car) {
        XMLDocument xml_doc;
        xml_doc.LoadFile(svg_car);
        std::vector<Shape*> v;
        std::vector<Rect*> s;
        Point *axis_front_center;

        XMLNode *pRoot  = xml_doc.FirstChild();
        XMLElement *obj = pRoot->FirstChildElement("rect");

        for (int i = 0; i < 17; i++) {
                double cx, cy, r, height, width, x, y;
                double start, end;
                string fill_value, id, name;

                fill_value = obj->Attribute("fill");
                Color *fill = new Color(fill_value);

                id   = obj->Attribute("id");
                name = string(obj->Name());

                if (id == "cannon") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        Point *p = new Point(x, y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                        car->cannon = new Rect(id, p, fill, width, height);
                }
                else if (id == "right_front" || id == "left_front") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        Point *p = new Point(x, y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                        s.push_back(new Rect(id, p, fill, width, height));
                        car->size_step = height;
                }
                else if (name == "rect") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        Point *p = new Point(x, y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                        v.push_back(new Rect(id, p, fill, width, height));
                }
                else if (name == "path") {
                        obj->QueryDoubleAttribute("sodipodi:cx", &cx);
                        obj->QueryDoubleAttribute("sodipodi:cy", &cy);
                        obj->QueryDoubleAttribute("sodipodi:rx", &r);
                        obj->QueryDoubleAttribute("sodipodi:start", &start);
                        obj->QueryDoubleAttribute("sodipodi:end", &end);
                        Point *p = new Point(cx, cy);
                        v.push_back(new Circle(id, p, fill, r, start, end));
                }
                else if (name == "circle") {
                        obj->QueryDoubleAttribute("cx", &cx);
                        obj->QueryDoubleAttribute("cy", &cy);
                        obj->QueryDoubleAttribute("r", &r);
                        Point *p = new Point(cx, cy);
                        v.push_back(new Circle(id, p, fill, r, 0, 2*PI));

                }

                if(id == "body1") {
                        car->axis_back_center = new Point(x + width/2, y);
                }
                else if(id == "front_axis") {
                        axis_front_center = new Point(x + width/2, y - height/2);
                }
                else if(id == "driver") {
                        car->center = new Point(cx, cy);
                }
                else if(id == "strip") {
                        car->ref_step = y - height;
                }


                if ((obj = obj->NextSiblingElement()) == NULL) {
                        break;
                }
        }

        car->d  = axis_front_center->distance(car->axis_back_center);
        car->body =  player;
        car->colisor =  new Circle("colisor", new Point(0,0), new Color("black"), player->radius, 0, 2*PI);
        car->shapes = v;
        car->steps = s;

        /* Needed for ajusts */
        car->window_center = arena[0]->center;
        car->window_size   = arena[0]->radius;

        /* Needed for kinematics */
        car->angle_wheel = 0;
        car->angle_body = 180;
        car->ratio = player->radius/(car->d);
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        arena[0]->draw();
        arena[1]->draw();
        for (std::vector<Shape *>::const_iterator i = scenario.begin();
             i != scenario.end(); ++i) {
                (*i)->draw();
        }
        largada->draw();
        car->draw(shots);
        glutSwapBuffers();
}

void motion(int x, int y) {
        if(x < arena[0]->radius) {
                car->angle_cannon = 45*(1 - x/arena[0]->radius);
        }
        else {
                car->angle_cannon = 45*(1 - x/arena[0]->radius);
        }
}

/* Main function: GLUT runs as a console application starting at main()	*/
int main(int argc, char **argv) {

        parserXML(argv[1]);
        string file = svg_path + svg_name + "." + svg_ext;
        parserSVG(file.c_str());
        parserCarSVG("car.svg");
        // Initialize GLUT Set the window's initial width & height
        glutInit(&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE);
        glutInitWindowSize(2 * arena[0]->radius, 2 * arena[0]->radius);

        // Position the window's  initial top-left corner Create window with the given
        // title
        glutInitWindowPosition(arena[0]->center->x, arena[0]->center->y);
        glutCreateWindow("Jogo");

        // Set clipping area's left, right, bottom, top
        gluOrtho2D(arena[0]->center->x - arena[0]->radius,
                   arena[0]->center->x + arena[0]->radius,
                   arena[0]->center->y - arena[0]->radius,
                   arena[0]->center->y + arena[0]->radius);

        // Select the Projection matrix for operation
        glMatrixMode(GL_PROJECTION);

        // Reset Projection matrix
        glLoadIdentity();

        // Register display callback handler for  window re-paint
        glutDisplayFunc(display);
        glutKeyboardFunc(keypressed);
        glutKeyboardUpFunc(keyunpressed);
        glutMouseFunc(mouse);
        glutPassiveMotionFunc(motion);
        glutIdleFunc(idle);
        glutMainLoop(); // Enter the event-processing loop
        return 0;
}
