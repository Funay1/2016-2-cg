#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <cstring>

#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

struct Point {
    int x;
    int y;
};

struct Color {
    float R;
    float G;
    float B;
};

struct Dimensions {
    float width;
    float height;
};

/* Global Variables */
string wdw_name;
int machine_state = 0;
Point p_clk, p_ctr;
Color c_sqr, c_wdw;
Dimensions d_wdw, d_sqr; // Dimensions was used to describe the square. In future works, objects will be describe as a list of points

/* Function that parser a xml document and saves the content into global variables 
*  Global variables were used cause it was the only way I could come up to pass ahead to the 
*  callback functions
*/

void parserXML(const char* filename) {
    XMLDocument xml_doc;
    xml_doc.LoadFile(filename);
    
    XMLNode * pRoot = xml_doc.FirstChild();
   
    XMLElement * pWindow = pRoot->FirstChildElement("janela");
    
    XMLElement * pWindowAttr[4];
    pWindowAttr[0] = pWindow->FirstChildElement("largura");
    XMLText* textNode = pWindowAttr[0]->FirstChild()->ToText();
    d_wdw.width = atof(textNode->Value());

    pWindowAttr[1] = pWindow->FirstChildElement("altura");
    textNode = pWindowAttr[1]->FirstChild()->ToText();
    d_wdw.height = atof(textNode->Value());

    pWindowAttr[2] = pWindow->FirstChildElement("fundo");
    pWindowAttr[2]->QueryFloatAttribute("corR", &c_wdw.R);
    pWindowAttr[2]->QueryFloatAttribute("corG", &c_wdw.G);
    pWindowAttr[2]->QueryFloatAttribute("corB", &c_wdw.B);

    pWindowAttr[3] = pWindow->FirstChildElement("titulo");
    textNode = pWindowAttr[3]->FirstChild()->ToText();
    wdw_name = textNode->Value();
   
    XMLElement * pSquareAttr = pRoot->FirstChildElement("quadrado");
    
    pSquareAttr->QueryFloatAttribute("tamanho", &d_sqr.width);
    pSquareAttr->QueryFloatAttribute("corR", &c_sqr.R);
    pSquareAttr->QueryFloatAttribute("corG", &c_sqr.G);
    pSquareAttr->QueryFloatAttribute("corB", &c_sqr.B);

    return;
}

/* Returns true if point p is place inside the square */
bool belongs2sqr(Point p){
    return (p.x <= p_ctr.x + d_sqr.width/2) && (p.x >= p_ctr.x - d_sqr.width/2) &&
           (p.y <= p_ctr.y + d_sqr.width/2) && (p.y >= p_ctr.y - d_sqr.width/2);
}

/* Handler for window-repaint event. Call back when the window first appears and
   whenever the window needs to be re-painted. */
void display() {
    glClearColor(c_wdw.R, c_wdw.G, c_wdw.B, 1.0f);                    // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);               // Clean the screen and the depth buffer
    glLoadIdentity();

    if(machine_state == 1 || machine_state == 2) {                                          
        glBegin(GL_QUADS);                                            // Draw a Red 1x1 Square centered at origin Each set of 4 vertices form a quad
            glColor3f(c_sqr.R, c_sqr.G, c_sqr.B);                     // Red
            glVertex2f(p_ctr.x - d_sqr.width/2, p_ctr.y - d_sqr.width/2);   // x, y are que coordinates of the click
            glVertex2f(p_ctr.x + d_sqr.width/2, p_ctr.y - d_sqr.width/2);   // d_sqr.width is the edge square size
            glVertex2f(p_ctr.x + d_sqr.width/2, p_ctr.y + d_sqr.width/2);   // relative dimentions
            glVertex2f(p_ctr.x - d_sqr.width/2, p_ctr.y + d_sqr.width/2);       
        glEnd();                                                     // finished the declaration os vertices
        machine_state = 2;
    }
    else if( machine_state == 3) {
        machine_state = 0;
    }                                              
    glFlush();                                                        // Render now
}

/*
* key is the char pressed, e.g., 'a' or 27 for ESC
* (x, y) is the mouse location in Windows' coordinates
*/
void keyboard(unsigned char key, int x, int y){
}

/* 
*(x, y) is the mouse-click location.
* button: GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, GLUT_MIDDLE_BUTTON
* state: GLUT_UP, GLUT_DOWN 
*/
void mouse(int button, int mouse_state, int x, int y){
    if(mouse_state == GLUT_DOWN) {
        p_clk.x = x;
        p_clk.y = d_wdw.height - y;

        if(machine_state == 0) {
            p_ctr.x = p_clk.x;
            p_ctr.y = p_clk.y;
            machine_state = 1;
        }
        else if(machine_state == 2 && belongs2sqr(p_clk) && button == GLUT_LEFT_BUTTON) {
            p_ctr.x = p_clk.x;
            p_ctr.y = p_clk.y;
        }
        else if(machine_state == 2 && belongs2sqr(p_clk) && button == GLUT_RIGHT_BUTTON) { 
            machine_state = 3;
        }
        glutPostRedisplay();
    }
}
 
/*
* where (x, y) is the mouse location in Window's coordinates
*/
void motion(int x, int y){
    p_clk.x = x;
    p_clk.y = d_wdw.height - y;

    if(machine_state == 2 && belongs2sqr(p_clk)) {
        p_ctr.x = x;
        p_ctr.y = d_wdw.height - y;
    }
    glutPostRedisplay();
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    parserXML(argv[1]);

    glutInit(&argc, argv);                           // Initialize GLUT
    glutInitWindowSize(d_wdw.width, d_wdw.height);   // Set the window's initial width & height
    glutInitWindowPosition(200, 200);                // Position the window's initial top-left corner
    glutCreateWindow(wdw_name.c_str());                // Create window with the given title
    gluOrtho2D(0, d_wdw.width, 0, d_wdw.height);     // Set clipping area's left, right, bottom, top
    glMatrixMode(GL_PROJECTION);                     // Select the Projection matrix for operation
    glLoadIdentity();                                // Reset Projection matrix
    glutDisplayFunc(display);                        // Register display callback handler for window re-paint
    glutKeyboardFunc(keyboard);                      // Register keyboard callback handler for window
    glutMouseFunc(mouse);                            // Register mouse callback handler for window
    glutMotionFunc(motion);                          // Register motion callback handler for window
    glutMainLoop();                                  // Enter the event-processing loop
    return 0;
}
