// Pong by Konstantinos Egarhos (aka konsnos)
 
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>

//Key status
int keyStatus[256];

// Window dimensions
const int Width = 500;
const int Height = 500;

// Dimensions
#define paddleHeight 80
#define paddleWidth 10
#define zAxis 0
#define xRPad 10
#define xLPad 480
#define gameSize 500
 
float p1 = 220;
float p2 = 220;
const float pspeed = 0.5; //Units per miliseconds
 
int score[2];// = {0, 0}; // Score for players 1 and 2
 

#define bsize 10
GLdouble bpos[2] = {235,235};
GLfloat bvx = 1; // ball vector in left-right axis
GLfloat bvy = 1; // ball vector in top-bottom axis
GLfloat bspeed = 0.2; //Units per miliseconds
 
// Text variable
static char str[2000];
void * font = GLUT_BITMAP_9_BY_15;
 
void drawRect(GLfloat x, GLfloat y, int height, int width, float R, float G, float B)
{
     glColor3f(R,G,B);
 
     glBegin(GL_QUADS);
          glVertex2f( x, y);                        // Top Left
          glVertex2f( x, y + height);               // Bottom Left
          glVertex2f( x + width, y + height);       // Bottom Right
          glVertex2f( x + width, y);                // Top Right
     glEnd();
}

void PrintScore(GLfloat x, GLfloat y)
{
    //Create a string to be printed
    char *tmpStr;
    sprintf(str, "Player 1 Score: %d, Player 2 Score: %d", score[0], score[1] );
    //Define the position to start printing
    glRasterPos2f(x, y);
    //Print  the first Char with a certain font
    //glutBitmapLength(font,(unsigned char*)str);
    tmpStr = str;
    //Print each of the other Char at time
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }

}

void renderScene(void)
{
    
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    PrintScore(10.0, 10.0);
    
    // Draws the 1st paddle and the 2nd.
    drawRect(xRPad, p1, paddleHeight, paddleWidth, 0, 0, 1); // Left blue paddle.
    drawRect(xLPad, p2, paddleHeight, paddleWidth, 1, 0, 0); // Right red paddle.

    drawRect(bpos[0], bpos[1], bsize, bsize, 1, 1, 0); // Yellow ball.

    glutSwapBuffers(); // Draw the new frame of the game.
}
 
void keyPress(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'w':
        case 'W':
             keyStatus[(int)('w')] = 1;
             break;
        case 's':
        case 'S':
             keyStatus[(int)('s')] = 1;
             break;
        case 'o':
        case 'O':
             keyStatus[(int)('o')] = 1;
             break;
        case 'l':
        case 'L':
             keyStatus[(int)('l')] = 1;
             break;
        case 27 :
             exit(0);
    }
    glutPostRedisplay();
}

void keyup(unsigned char key, int x, int y)
{
    keyStatus[(int)(key)] = 0;
    glutPostRedisplay();
}

/*********************************************
    Let there be initial variables.
*********************************************/
void init(void)
{
    int i;
    // The color the windows will redraw. Its done to erase the previous frame.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black, no opacity(alpha).

    // Orthographic projection matrix (initiating 2D window).
    gluOrtho2D(0,gameSize,0,gameSize);

    // Initialize score.
    score[0] = score[1] = 0;

    //Initialize keyStatus
    for(i = 0; i < 256; i++)
        keyStatus[i] = 0; 
}

void updateBall( GLdouble timeDiff )
{
    // Ball collision with the border of the window.
    if(bpos[0] + bspeed > 500-bsize)
    {
         bvx = -1;
         score[0]++;
         bpos[0] = 500-bsize;
    }
    else if (bpos[0] - bspeed < bsize)
    {
         bvx = 1;
         score[1]++;
         bpos[0] = bsize;
    }
    if(bpos[1] + bspeed > 500-bsize)
    {
         bvy = -1;
         bpos[1] = 500-bsize;
    }
    else if (bpos[1] - bspeed < bsize)
    {
         bvy = 1;
         bpos[1] = bsize;
    }

    /******** Collision with the paddles ********/
    // Collision with left paddle.
    if(     bpos[0] - bsize <= xRPad + paddleWidth 
            && bpos[1] - bsize >= p1 
            && bpos[1] + bsize <= p1 + paddleHeight)
    {
         bvx *= -1;
         bpos[0] = xRPad + paddleWidth + bsize;
    }
    // Collision with right paddle.
    if(     bpos[0] + bsize >= xLPad - paddleWidth 
            && bpos[1] - bsize >= p2 
            && bpos[1] + bsize <= p2 + paddleHeight)
    {
         bvx *= -1;
         bpos[0] = xLPad - paddleWidth - bsize;
    }

    // New position of the ball is dependent of the speed ***************
    // Because frame rate isn't always the same that might affect objects speed.
    // That's why we multiply the frame speed and multiply it with the object speed.
    // In that way the object speed is always the same.
    bpos[0] += bspeed * bvx * timeDiff;
    bpos[1] += bspeed * bvy * timeDiff;
}

void updatePaddle( GLdouble timeDiff )
{
    float speed = pspeed * timeDiff; //Correct speed of movement with the time difference between calls
    //Treat pressed key
    if(keyStatus[(int)('w')] && 420>=p1+speed)
    {
        p1 += speed; 
    }
    if(keyStatus[(int)('s')] && 0<=p1-speed)
    {
        p1 -= speed; 
    }
    if(keyStatus[(int)('o')] && 420>=p2+speed)
    {
        p2 += speed; 
    }
    if(keyStatus[(int)('l')] && 0<=p2-speed)
    {
        p2 -= speed; 
    }
}

void idle(void)
{
    static GLdouble previousTime = 0;
    GLdouble currentTime;
    GLdouble timeDiference;
    
    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDiference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time

    updateBall( timeDiference ); 

    updatePaddle( timeDiference ); 

    glutPostRedisplay();
}
 
/* Main function */
int main(int argc, char *argv[])
{
    // Initialize openGL with Double buffer and RGB color without transparency.
    // Its interesting to try GLUT_SINGLE instead of GLUT_DOUBLE.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 
    // Create the window.
    glutInitWindowSize(Width, Height);
    glutInitWindowPosition(150,50);
    glutCreateWindow("Pong by kon_nos");
 
    // Define the draw function.
    glutDisplayFunc(renderScene);
 
    // Define the keyboard input function.
    glutKeyboardFunc(keyPress);
 
    // Define which function to execute when nothing except the gameplay is updating.
    glutIdleFunc(idle);

    // Define which function to execute when key is released.
    glutKeyboardUpFunc(keyup);
    
    init();
 
    glutMainLoop();
 
    return 0;
}
