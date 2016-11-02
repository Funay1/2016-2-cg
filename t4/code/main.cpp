#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <cstring>
#include <cmath>
#include <list>
#include <sstream>

#include <GL/glut.h> // GLUT, includes glu.h and gl.h
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

#define PI 3.1415927

typedef enum type {
        RECT,
        CIRCLE
} Type;

class Point {
public:
double x;
double y;
Point(double a,
      double b) {
        this->x = a;
        this->y = b;
}

Point* clone(){
        return new Point(this->x, this->y);
}

bool equal(Point* p){
        return (this->x == p->x) && (this->y == p->y);
}

string toString(){
        ostringstream convert;
        convert << this->x << ", " << this->y;
        return convert.str();
}

Point* vetor(Point* b){
        return new Point (this->x - b->x, this->y - b->y);
}

double angle(){
        double angle = atan2(this->y, this->x)*(180.0/PI);
        if(angle < 0) {
                return angle + 360;
        }
        return angle;
}

double angle(Point* b){
        double angle = atan2(b->y - this->y, b->x - this->x)*(180.0/PI);
        if(angle < 0) {
                return angle + 360;
        }
        return angle;
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
string name;
float R;
float G;
float B;

Color(float R, float G, float B){
        this->R = R;
        this->G = G;
        this->B = B;
}

Color(string color) {
        this->name = color;
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
        else if(!color.compare("darkgreen")) {
                this->R = 20.0/256;
                this->G = 73.0/256;
                this->B = 40.0/256;
        }
        else if(!color.compare("lightblue")) {
                this->R = 29.0/256;
                this->G = 112.0/256;
                this->B = 187.0/256;
        }
        else if(!color.compare("darkpurple")) {
                this->R = 90.0/256;
                this->G = 3.0/256;
                this->B = 67.0/256;
        }
        else {
                this->R = 0;
                this->G = 0;
                this->B = 0;
        }
}

Color* clone(){
        return new Color(this->R, this->G, this->G);
}

string toString(){
        ostringstream convert;
        convert << this->name << ":(" << this->R << ", " << this->G  << ", " << this->B << ")";
        return convert.str();
}
};

class Shape {
public:
Type type;
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

void change_color(Color *c){
        this->fill = c;
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
        this->type = CIRCLE;
        this->radius = r;
        this->start  = start;
        this->end  = end;
}

Circle* clone(){
        return new Circle(this->id, this->center->clone(), this->fill->clone(), this->radius, this->start, this->end);
}
/*
 * Function that answer if a circle is inside of another circle.
 * Params:
 *	s (Shape *) - the other circle.
 * Return:
 * False: if the object is inside of the other.
 */
bool inside(Shape *s) {
        Circle *c = (Circle *)(s);
        if (this->center->distance(c->center) <= (c->radius - this->radius)) {
                return false;
        }
        return true;
}

/*
 * Function that answer if a circle colide with another circle.
 * Params:
 *	s (Shape *) - the other circle.
 * Return:
 * False: if colide.
 * True: if not
 */

bool colide(Shape *s) {
        Circle *c = (Circle *)(s);
        if (this->center->distance(c->center) <= (c->radius + this->radius)) {
                return false;
        }
        return true;
}

/*
 * Function that answer if a circle colide with any other circle.
 * Params:
 *	s (list<Shape *>) - a list of objects which you wish to check.
 * False: if colide to any.
 * True: if not
 */

bool colide(list<Shape *>s) {
        for (std::list<Shape *>::const_iterator i = s.begin(); i != s.end(); ++i) {
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
        this->type = RECT;
        this->width  = w;
        this->height = h;
}

/*
 * Function that handles the drawing of a rectangle using GL_QUADS
 * method. This will create a filled rectangle
 */

Rect* clone(){
        return new Rect(this->id, new Point(this->center->x, this->center->y), this->fill->clone(), this->width, this->height);
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

class Scenario {
public:
Circle *outter_ring;
Circle *inner_ring;
Rect   *largada;
Scenario(Circle *outter_ring, Circle *inner_ring, Rect *largada){
        this->outter_ring = outter_ring;
        this->inner_ring = inner_ring;
        this->largada = largada;
}

void draw(){
        outter_ring->draw();
        inner_ring->draw();
        largada->draw();
}
};

class Shot {
public:
Shape* formato;
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

bool out_of_range(Window* window){
        if(window->center->distance(this->formato->center) > window->width)
                return true;
        return false;
}

void draw(Window *window, double ratio) {
        glPushMatrix();
        glTranslatef(-(window->center->x - this->formato->center->x)/(window->width/2),
                     -(window->center->y - this->formato->center->y)/(window->height/2),
                     0);
        glScalef(ratio, ratio, 0);
        glTranslatef((window->center->x - this->formato->center->x)/(window->width/2),
                     (window->center->y - this->formato->center->y)/(window->height/2),
                     0);
        formato->draw();
        glPopMatrix();
}

void kinematics(double t, double speed_shot){
        this->formato->center->x += t*(speed_shot*cos((this->attitute - 90)*PI/180));
        this->formato->center->y += t*(speed_shot*sin((this->attitute - 90)*PI/180));
}
};

class Car {
public:
list<Shape *> parts;
list<Shape *> steps;

/* Needed for kinematics */
Point  *axis_back_center;
Circle *body;
Circle *colisor;
double d;
double angle_body;
double angle_wheel;

Rect   *cannon;
Point  *start_cannon;
double angle_cannon;

double ratio;
double size_step;
double ref_step;

double i_error;

Car(Window* window, Circle* body, list<Shape*> parts){
        Point* axis_front_center;
        list<Shape*> s;
        list<Shape*> new_parts;
        for (std::list<Shape *>::const_iterator i = parts.begin(); i != parts.end(); ++i) {
                if((*i)->type == CIRCLE) {
                        Circle* c = (Circle*)(*i);
                        new_parts.push_back(c->clone());
                }
                else {
                        Rect* r = (Rect*)(*i);
                        new_parts.push_back(r->clone());
                }
        }

        for (std::list<Shape *>::iterator i = new_parts.begin(); i != new_parts.end(); i++) {
                if((*i)->id == "body1") {
                        Rect* r = (Rect*)(*i);
                        r->change_color(body->fill);
                        this->axis_back_center = new Point(r->center->x + r->width/2, r->center->y);
                }
                else if((*i)->id == "body2" || (*i)->id == "body3") {
                        Rect* r = (Rect*)(*i);
                        r->change_color(body->fill);
                }
                else if((*i)->id == "front_axis") {
                        Rect* r = (Rect*)(*i);
                        axis_front_center = new Point(r->center->x + r->width/2, r->center->y - r->height/2);
                }
                else if((*i)->id == "strip") {
                        Rect* r = (Rect*)(*i);
                        this->ref_step = r->center->y - r->height;
                }
                else if((*i)->id  == "left_front" || (*i)->id  == "right_front") {
                        Rect* r = (Rect*)(*i);
                        s.push_back(r);
                        i = new_parts.erase(i);
                        i--; //Gambiarra!!!
                        this->size_step = r->width;
                }
                else if((*i)->id == "driver") {
                        Circle* c = (Circle*)(*i);
                        this->start_cannon = new Point(c->center->x, c->center->y);
                }
                else if((*i)->id == "cannon") {
                        Rect* r = (Rect*)(*i);
                        i = new_parts.erase(i);
                        i--; //Gambiarra!!!
                        this->cannon = r;
                }
        }

        this->d  = axis_front_center->distance(this->axis_back_center);
        this->body =  body;
        this->colisor =  new Circle("colisor", new Point(0,0), new Color("black"), body->radius, 0, 2*PI);
        this->parts = new_parts;
        this->steps = s;
        this->angle_wheel = 0;
        this->angle_body = this->body->center->vetor(window->center)->angle();
        this->ratio = body->radius/(this->d);
        this->i_error = 0;
}

void draw(Window* window, list<Shot *> shots) const {
        glPushMatrix();
        glTranslatef((this->body->center->x - this->axis_back_center->x)/(window->width/2),
                     (this->body->center->y - this->axis_back_center->y)/(window->height/2),
                     0);
        glTranslatef(-(window->center->x - this->axis_back_center->x)/(window->width/2),
                     -(window->center->y - this->axis_back_center->y)/(window->height/2),
                     0);
        glRotatef(this->angle_body, 0,0,1);
        glScalef(this->ratio, this->ratio, 0);
        glTranslatef((window->center->x - this->axis_back_center->x)/(window->width/2),
                     (window->center->y - this->axis_back_center->y)/(window->height/2),
                     0);
        for (std::list<Shape *>::const_iterator i = this->parts.begin();
             i != this->parts.end(); ++i) {
                (*i)->draw();
        }

        glPushMatrix();
        glTranslatef(-(window->center->x - this->start_cannon->x)/(window->width/2),
                     -(window->center->y - this->start_cannon->y)/(window->height/2),
                     0);
        glRotatef(this->angle_cannon, 0,0,1);
        glTranslatef((window->center->x - this->start_cannon->x)/(window->width/2),
                     (window->center->y - this->start_cannon->y)/(window->height/2),
                     0);
        this->cannon->draw();
        glPopMatrix();

        /* Draw the 2 wheels. To do that, it is needed to move to the center of the screen and then move to the next point */
        for (std::list<Shape *>::const_iterator i = this->steps.begin();
             i != this->steps.end(); ++i) {
                Rect* r = (Rect*)(*i);
                glPushMatrix();
                glTranslatef(-(window->center->x - (r->center->x + r->width/2))/(window->width/2),
                             -(window->center->y - (r->center->y - r->height/2))/(window->height/2),
                             0);
                glRotatef(angle_wheel, 0,0,1);
                glTranslatef((window->center->x - (r->center->x + r->width/2))/(window->width/2),
                             (window->center->y - (r->center->y - r->height/2))/(window->height/2),
                             0);
                r->draw();
                glPopMatrix();
        }
        glPopMatrix();

        for (std::list<Shot *>::iterator i = shots.begin(); i != shots.end(); i++) {
                if((*i)->out_of_range(window)) {
                        i = shots.erase(i);
                        i--;
                }
                if(i != shots.end()) {
                        (*i)->draw(window, this->ratio);
                }
        }
}

void set_angle_wheel(double error){
        if(error > 45) {
                this->angle_wheel = 45;
        }
        else if(error < -45) {
                this->angle_wheel = -45;
        }
        else {
                this->angle_wheel = error;
        }
}

void set_angle_body(double value){
        if(value < -180) {
                this->angle_body = value + 360;
        }
        else if(value > 180) {
                this->angle_body =  value - 360;
        }
}

double get_angle_body(){
        if(this->angle_body < 0) {
                this->angle_body = this->angle_body + 360;
        }
        return this->angle_body;
}

void modify_angle_wheel(double value){
        if(this->angle_wheel + value <= 45 && this->angle_wheel + value >= -45) {
                this->angle_wheel += value;
        }
        else if (value > 0) {
                this->angle_wheel = 45;
        }
        else {
                this->angle_wheel = -45;
        }
}

void modify_cannon(Window* window, double x){
        this->angle_cannon = 45*(1 - x/(window->width/2));
}

void kinematics(double t, Window* window, Scenario* scenario, double speed_current){
        /* Based on the model of a car-like vehicle, we calculate the moviment and in the case of a colision, it undo it */
        this->body->center->x = t*(speed_current*cos((this->angle_body - 90)*PI/180)) + this->body->center->x;
        this->colisor->center->x = this->body->center->x + cos((this->angle_body - 90)*PI/180)*this->d*(this->ratio)/2;
        if (!this->colisor->colide(scenario->inner_ring) || this->colisor->inside(scenario->outter_ring)) {
                this->body->center->x = this->body->center->x - t*(speed_current*cos((this->angle_body - 90)*PI/180));
        }
        this->body->center->y = t*(speed_current*sin((this->angle_body - 90)*PI/180)) + this->body->center->y;
        this->colisor->center->y = this->body->center->y + sin((this->angle_body - 90)*PI/180)*this->d*(this->ratio)/2;
        if (!this->colisor->colide(scenario->inner_ring) || this->colisor->inside(scenario->outter_ring)) {
                this->body->center->y = this->body->center->y -  t*(speed_current*sin((this->angle_body - 90)*PI/180));
        }
        this->angle_body  = t*(speed_current*tan((this->angle_wheel)*PI/180)) + this->angle_body;
        if (!this->colisor->colide(scenario->inner_ring) || this->colisor->inside(scenario->outter_ring)) {
                this->angle_body  = this->angle_body - t*(speed_current*tan((this->angle_wheel)*PI/180));
        }

        static int counter = 0;
        for (std::list<Shape *>::const_iterator i = this->parts.begin(); i != this->parts.end(); ++i) {
                if((*i)->id == "strip" && speed_current != 0) {
                        (*i)->center->y = this->ref_step - counter;
                        counter = counter%(int)this->size_step;
                        counter++;
                }
        }
}
};

class Caracter {
public:
string id;
Car*   car;
list<Shot*> shots;

double speed_car;
double speed_current;
double speed_shot;

double set_point;
Point *prev_pos;
int counter;
double freq_shot;

int score;

Caracter (Window* window, Circle* car, list<Shape*> parts, double speed_car, double speed_shot) {
        this->id = car->id.c_str();
        this->car = new Car(window, car, parts);
        this->speed_car = speed_car;
        this->speed_shot = speed_shot;
        this->set_point = car->center->distance(window->center);
        this->prev_pos = new Point(car->center->x, car->center->y);
        this->counter = 1;
        this->score = 0;
}

Caracter (Window* window, Circle* car, list<Shape*> parts, double speed_car, double speed_shot, double freq_shot) {
        this->id = car->id.c_str();
        this->car = new Car(window, car, parts);
        this->speed_car = speed_car;
        this->speed_shot = speed_shot;
        this->set_point = car->center->distance(window->center);
        this->prev_pos = new Point(car->center->x, car->center->y);
        this->counter = 1;
        this->score = 0;
        this->freq_shot = freq_shot;
}

void modify_speed(double proportion){
        this->speed_current = proportion*this->speed_car;
}

void modify_angle_wheel(double value){
        this->car->modify_angle_wheel(value);
}

void modify_cannon(Window* window, double x){
        this->car->modify_cannon(window, x);
}

void new_shot(){
        double x = this->car->colisor->center->x + this->car->ratio*this->car->cannon->height*cos((this->car->angle_cannon + this->car->angle_body - 90)*PI/180);
        double y = this->car->colisor->center->y + this->car->ratio*this->car->cannon->height*sin((this->car->angle_cannon + this->car->angle_body - 90)*PI/180);
        Point *position = new Point(x, y);
        this->shots.push_back(new Shot(this->car->cannon->width/2, position, this->car->angle_cannon + this->car->angle_body));
}

void kinematics(double timeDiference, Window* window, Scenario* scenario) {
        this->prev_pos = new Point(this->car->body->center->x, this->car->body->center->y);
        this->car->kinematics(timeDiference, window, scenario, this->speed_current);
        for (std::list<Shot *>::const_iterator i = this->shots.begin(); i != this->shots.end(); ++i) {
                (*i)->kinematics(timeDiference, this->speed_shot);
        }
}

void draw(Window* window){
        this->car->draw(window, this->shots);
}
};

class AI {
public:
static void score(Scenario* scenario, Caracter* player){
        double linha = scenario->largada->center->y;
        double coluna_1 = scenario->largada->center->x;
        double coluna_2 = scenario->largada->center->x + scenario->largada->width;
        double prev_y = player->prev_pos->y;
        double x = player->car->body->center->x;
        double y = player->car->body->center->y;

        if(coluna_1 > coluna_2) {
                double aux = coluna_1;
                coluna_1 = coluna_2;
                coluna_2 = aux;
        }

        if(x >= coluna_1 && x <= coluna_2) {
                if(prev_y > linha &&  y < linha) {
                        player->score += 1;
                }
                else if(prev_y < linha &&  y > linha) {
                        player->score -= 1;
                }
        }
}

static void move_enemy_pid(double timeDiference, Window* window, Caracter* player, list<Caracter *> enemies){
        for (std::list<Caracter*>::iterator i = enemies.begin(); i != enemies.end(); i++) {
                if((*i)->prev_pos->equal((*i)->car->body->center)) {
                        (*i)->modify_speed(-1);
                        (*i)->counter = 30;
                }

                (*i)->counter -= 1;

                if ((*i)->counter == 0) {
                        (*i)->modify_speed(1);
                        (*i)->counter = 30;
                }

                double set_point = ((*i)->car->body->center->vetor(window->center))->angle(); //NOTE must be (0,360)
                double error = set_point - (*i)->car->get_angle_body();
                double dist_error = ((*i)->car->body->center->distance(window->center) - (*i)->set_point)/(*i)->set_point;
                (*i)->car->i_error += dist_error;

                if(error < 180) {
                        (*i)->car->modify_angle_wheel(error - (*i)->car->i_error);
                }
                else {
                        (*i)->car->modify_angle_wheel((-1)*error - (*i)->car->i_error);
                }
        }
}

static void move_enemy(double timeDiference, Window* window, Caracter* player, list<Caracter *> enemies){
        for (std::list<Caracter*>::iterator i = enemies.begin(); i != enemies.end(); i++) {
                (*i)->car->angle_body = ((*i)->car->body->center->vetor(window->center))->angle();
                (*i)->car->angle_wheel = (-1)*asin(((*i)->car->d)/(*i)->set_point)*(180/PI);

                if((*i)->prev_pos->equal((*i)->car->body->center)) {
                        (*i)->modify_speed(-1);
                        (*i)->counter = 30;
                }

                (*i)->counter -= 1;

                if ((*i)->counter == 0) {
                        (*i)->modify_speed(1);
                        (*i)->counter = 30;
                }
        }
}

static void shot_enemy(double timeDiference, Window* window, Caracter* player, list<Caracter *> enemies) {
        for (std::list<Caracter*>::iterator i = enemies.begin(); i != enemies.end(); i++) {
                double car_angle = (*i)->car->get_angle_body() - 90; //NOTE angulo do cado de (0,360)
                car_angle = car_angle < 0 ? (car_angle + 360) : (car_angle);

                double angle = player->car->body->center->vetor((*i)->car->body->center)->angle();

                if(abs(car_angle - angle) <= 45 || abs(car_angle - angle - 360) <= 45) {
                        (*i)->car->angle_cannon = angle - car_angle;
                        (*i)->new_shot();
                }
        }

}

static void print_score(char* str, void* font, GLfloat x, GLfloat y, Caracter* player) {
        //Create a string to be printed
        char *tmpStr;
        sprintf(str, "Player 1 Score: %d", player->score);
        //Define the position to start printing
        glRasterPos2f(x, y);
        //Print  the first Char with a certain font
        //glutBitmapLength(font,(unsigned char*)str);
        tmpStr = str;
        //Print each of the other Char at time
        while( *tmpStr ) {
                glutBitmapCharacter(font, *tmpStr);
                tmpStr++;
        }
}
};

/* Global Variables */
GLfloat gx = 0, gy = 0;
int keypress[256];

/* Text variable */
static char str[2000];
void* font = GLUT_BITMAP_9_BY_15;

Window* window;
Scenario* scenario;
Caracter* player;
list<Caracter *> enemies;


list<Shape*> parserCarSVG(const char *svg_car) {
        XMLDocument xml_doc;
        xml_doc.LoadFile(svg_car);
        std::list<Shape*> parts;

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

                if (name == "rect") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        Point *p = new Point(x, y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                        parts.push_back(new Rect(id, p, fill, width, height));
                }
                else if (name == "circle") {
                        obj->QueryDoubleAttribute("cx", &cx);
                        obj->QueryDoubleAttribute("cy", &cy);
                        obj->QueryDoubleAttribute("r", &r);
                        Point *p = new Point(cx, cy);
                        parts.push_back(new Circle(id, p, fill, r, 0, 2*PI));
                }
                else if (name == "path") {
                        obj->QueryDoubleAttribute("sodipodi:cx", &cx);
                        obj->QueryDoubleAttribute("sodipodi:cy", &cy);
                        obj->QueryDoubleAttribute("sodipodi:rx", &r);
                        obj->QueryDoubleAttribute("sodipodi:start", &start);
                        obj->QueryDoubleAttribute("sodipodi:end", &end);
                        Point *p = new Point(cx, cy);
                        parts.push_back(new Circle(id, p, fill, r, start, end));
                }
                if ((obj = obj->NextSiblingElement()) == NULL) {
                        break;
                }
        }
        return parts;
}

//TODO Velocidade diferente para inimigos e Jogador
//TODO Colisao tiro
//TODO Colisao carros
//TODO Exibir cronometro
//TODO Finalizacao do jogo

void parserSVG(const char *svg_img, double speed_car, double speed_shot, double freqTiro, double velCarro, double velTiro) {
        XMLDocument xml_doc;
        xml_doc.LoadFile(svg_img);
        XMLNode *pRoot  = xml_doc.FirstChild();
        XMLElement *obj = pRoot->FirstChildElement("circle");

        int counter_arena = 0;
        Circle *arena[2];
        Rect   *largada;
        list<Shape*> caracters;
        list<Shape*> parts = parserCarSVG("./car.svg");

        for (int i = 0; i < 10; i++) {
                double cx, cy, r, height, width, x, y;
                string fill_value, id, name;

                fill_value = obj->Attribute("fill");
                Color *fill = new Color(fill_value);

                id   = obj->Attribute("id");
                name = string(obj->Name());

                if (name == "rect") {
                        obj->QueryDoubleAttribute("x", &x);
                        obj->QueryDoubleAttribute("y", &y);
                        obj->QueryDoubleAttribute("height", &height);
                        obj->QueryDoubleAttribute("width", &width);
                }
                else if (name == "circle") {
                        obj->QueryDoubleAttribute("cx", &cx);
                        obj->QueryDoubleAttribute("cy", &cy);
                        obj->QueryDoubleAttribute("r", &r);
                }

                if (!id.compare("Jogador") || !id.compare("Inimigo")) {
                        Point *p = new Point(cx, cy);
                        caracters.push_back(new Circle(id, p, fill, r, 0, 2*PI));
                }
                else if (!id.compare("Pista")) {
                        Point *p = new Point(cx, cy);
                        arena[counter_arena++] = new Circle(id, p, fill, r, 0, 2*PI);
                }
                else if (!id.compare("LargadaChegada")) {
                        Point *p = new Point(x, y);
                        largada = new Rect(id, p, fill, width, height);
                }
                if ((obj = obj->NextSiblingElement()) == NULL) break;
        }


        Circle *tmp;
        if(arena[0]->radius < arena[1]->radius) {
                tmp = arena[0];
                arena[0] = arena[1];
                arena[1] = tmp;
        }

        float win_size = 2*arena[0]->radius;

        arena[0]->center->invert(win_size);
        arena[1]->center->invert(win_size);
        largada->center->invert(win_size);

        window = new Window(arena[0]->center->x, arena[0]->center->y, 2*arena[0]->radius, 2*arena[0]->radius);
        scenario = new Scenario(arena[0], arena[1], largada);

        for (std::list<Shape*>::const_iterator i = caracters.begin(); i != caracters.end(); ++i) {
                (*i)->center->invert(win_size);
                if(!(*i)->id.compare("Jogador")) {
                        player = new Caracter(window, (Circle*)(*i), parts, speed_car, speed_shot);
                }
                else {
                        enemies.push_back(new Caracter(window, (Circle*)(*i), parts, velCarro, velTiro, freqTiro));
                }
        }
}

string parserXML(const char *path, double* speed_car, double* speed_shot, double* freqTiro, double* velCarro, double* velTiro) {
        XMLDocument xml_doc;
        string svg_name, svg_ext, svg_path;
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
        pCarAttr->QueryDoubleAttribute("velTiro",  speed_shot);
        pCarAttr->QueryDoubleAttribute("velCarro", speed_car);

        XMLElement *pCarEnemyAttr = pRoot->FirstChildElement("carroInimigo");
        pCarEnemyAttr->QueryDoubleAttribute("freqTiro", freqTiro);
        pCarEnemyAttr->QueryDoubleAttribute("velCarro", velCarro);
        pCarEnemyAttr->QueryDoubleAttribute("velTiro", velTiro);

        return svg_path + svg_name + "." + svg_ext;
}

void keypressed(unsigned char key, int x, int y) {
        keypress[key] = 1;
}

void keyunpressed(unsigned char key, int x, int y) {
        keypress[key] = 0;
}

void mouse(int button, int mouse_state, int x, int y) {
        list<Caracter*>::iterator it = enemies.begin();
        if(mouse_state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
                player->new_shot();
                for(list<Caracter*>::const_iterator it = enemies.begin(); it != enemies.end(); it++) {
                        (*it)->new_shot();
                }
        }
}

void idle(void) {
        if(((keypress['w'] == 1) || (keypress['W'] == 1)) && ((keypress['s'] == 1) || (keypress['S'] == 1))) {
                player->modify_speed(0);
        }
        else if ((keypress['w'] == 1) || (keypress['W'] == 1)) {
                player->modify_speed(1);
        }
        else if ((keypress['s'] == 1) || (keypress['S'] == 1)) {
                player->modify_speed(-1);
        }
        else {
                player->modify_speed(0);
        }

        if ((keypress['d'] == 1) || (keypress['D'] == 1)) {
                player->modify_angle_wheel(-5);
        }
        if ((keypress['a'] == 1) || (keypress['A'] == 1)) {
                player->modify_angle_wheel(+5);
        }

        GLdouble currentTime;
        GLdouble timeDiference;
        static GLdouble previousTime = 0;

        // Elapsed time from the initiation of the game.
        currentTime = glutGet(GLUT_ELAPSED_TIME);
        // Elapsed time from the previous frame.
        timeDiference = currentTime - previousTime;
        //Update previous time
        previousTime = currentTime;
        AI::score(scenario, player);
        AI::move_enemy(timeDiference, window, player, enemies);
        AI::shot_enemy(timeDiference, window, player, enemies);

        player->kinematics(timeDiference, window, scenario);
        for(list<Caracter*>::const_iterator it = enemies.begin(); it != enemies.end(); it++) {
                (*it)->kinematics(timeDiference, window, scenario);
        }
        glutPostRedisplay();
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        scenario->draw();
        for (std::list<Caracter *>::iterator i = enemies.begin(); i != enemies.end(); ++i) {
                (*i)->draw(window);
        }
        player->draw(window);
        AI::print_score (str, font, 10, 10, player);
        glutSwapBuffers();
}

void motion(int x, int y) {
        player->modify_cannon(window, x);
}

/* Main function: GLUT runs as a console application starting at main()	*/
int main(int argc, char **argv) {

        double speed_car, speed_shot, freqTiro, velCarro, velTiro;
        string path = parserXML(argv[1], &speed_car, &speed_shot, &freqTiro, &velCarro, &velTiro );
        parserSVG(path.c_str(), speed_car, speed_shot, freqTiro, velCarro, velTiro);

        // Initialize GLUT Set the window's initial width & height
        glutInit(&argc, argv);
        glutInitDisplayMode (GLUT_DOUBLE);
        glutInitWindowSize(window->width, window->height);

        // Position the window's  initial top-left corner Create window with the given title
        glutInitWindowPosition(window->center->x, window->center->y);
        glutCreateWindow("Rodolfo Valentim");

        // Set clipping area's left, right, bottom, top
        gluOrtho2D(window->center->x - window->width/2,
                   window->center->x + window->width/2,
                   window->center->y - window->height/2,
                   window->center->y + window->height/2);

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
