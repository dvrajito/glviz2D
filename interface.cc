/********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    main.cc
   Updated: March 2019

   Some glut interface functions.

********************************************************************/

#include <GL/glut.h>
#include <cstdlib>
#include "road.h"
#include "interface.h"

Road *rd = NULL;
bool timer_on = false;
int winWidth = 1200, winHeight = 900;
char roadFile[100] = ROAD_FILE_ROOT"trajectory21/ALpine2center.txt";
//char roadFile[100] = ROAD_FILE_ROOT"curvature/curvALpine2R.txt";
char trajFile[100] = "";// ROAD_FILE_ROOT"trajectory18/trajGlbsEtrack5.txt";

// initialize the window and GUI, create the road
void glMainInit(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Road Visualizer");
    glutDisplayFunc(display);
    glutIdleFunc(NULL);
    glutKeyboardFunc(key);
    glutSpecialFunc(spkey);
    glutMouseFunc(mouse);
    createObjects();
    myinit();
    glutMainLoop();
}

// Displays the window
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    rd->display();
    glFlush();
    glutSwapBuffers();
}

// Advance to the next frame, update everything
void nextFrame()
{
    glutPostRedisplay();
}

// Callback for the mouse function
void mouse(int btn, int state, int x, int y)
{
    if (state != GLUT_DOWN)
        nextFrame();
}

// Callback for Ascii keyboard function
void key(unsigned char key, int x, int y)
{
    char trj[] = "traj.txt";
    char rtrj[] = "realTraj.txt";
    switch (key) {
    case 'q':
    case 'Q':
        exit(0);
    case 'a':
    case 'A':
        if (!timer_on) {
            timer_on = true;
            timer(1);
        }
        else
            timer_on = false;
        break;
    case 's':
    case 'S':
        rd->smoothTrajectory(10);
        glutPostRedisplay();
        break;
    case 'w':
    case 'W':
        rd->writeTrajFile(trj);
        break;
    case 't':
    case 'T':
        rd->writeRealPts(rtrj);
        break;
    case ' ':
        rd->optimizeTraj();
        glutPostRedisplay();
        break;
    }
}

// Callback for special keys keyboard function
void spkey(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        glutPostRedisplay();
        break;
    }
}

// Timer function: update everything and restart the timer
GLvoid timer(int value)
{
    if (timer_on) {
        nextFrame();
        glutPostRedisplay();
        glutTimerFunc(120, timer, value);
    }
}

// Set the view on the coordinate i so that we can see the whole area.
void setView(Point3f &vMin, Point3f &vMax, int i)
{
    if (vMin[i] == vMax[i])
    {
        vMin[i] = 0;
        vMax[i] = 1;
    }
    else if (fabs(vMin[i]) < fabs(vMax[i]))
    {
        vMax[i] *= 1.1;
        vMin[i] -= 0.09091 * vMax[i];
    }
    else
    {
        vMin[i] *= 1.1;
        vMax[i] -= 0.09091 * vMin[i];
    }
}

// Initialize the view and the road 
void myinit()
{
    Point3f viewMin, viewMax;
    float viewW = 2610, viewH = 3070;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (rd)
    {
        viewMin = rd->min;
        viewMax = rd->max;
        setView(viewMin, viewMax, 0); // set x
        setView(viewMin, viewMax, 1); // set y
        cout << "viewMin: " << viewMin << " viewMax: " << viewMax << endl;
        gluOrtho2D(viewMin.x(), viewMax.x(), viewMin.y(), viewMax.y());
        viewW = viewMax.x() - viewMin.x();
        viewH = viewMax.y() - viewMin.y();
    }
    else
        gluOrtho2D(-90, 2700, -2700, 370);
    
    if (viewW / viewH > float(winWidth) / winHeight) // the view is larger than the window
        winHeight = winWidth * (viewH / viewW);
    else
        winWidth = winHeight * (viewW / viewH);
    glutReshapeWindow(winWidth, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0, 0, 0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
}

// Read the trajectory file 
void createObjects()
{
    //rd = new Road("alpine2curv.txt");
    //rd = new Road("CGSpeedway1Curv.txt");
    
    //rd = new Road(roadFile);
    
    rd = new Road();
    rd->readCenter(roadFile, linear, 0.2);
    rd->draw();
    //rd->outputKeyFrames();
    rd->computeCurvChangePts();
    rd->outputCurvChangePts();
    //rd->outputPoints();

    if (trajFile[0] != '\0')
        rd->readTrajFile(trajFile);
    else
        rd->setConstTraj(); // set all to 0
}

