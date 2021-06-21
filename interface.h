/********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    interface.h
   Updated: March 2019

   Some prototypes for glut interface functions.

********************************************************************/

#ifndef INTERFACE_H
#define INTERFACE_H

#include <GL/glut.h>

#define ROAD_FILE_ROOT "D:/develop/meep/data/"

// initialize the window and GUI, create the road
void glMainInit(int argc, char **argv);

// Displays the window
void display(void);

// Advance to the next frame, update everything
void nextFrame();

// Callback for the mouse function
void mouse(int btn, int state, int x, int y);

// Callback for Ascii keyboard function
void key(unsigned char key, int x, int y);

// Callback for special keys keyboard function
void spkey(int key, int x, int y);

// Timer function: update everything and restart the timer
GLvoid timer(int value);

// Set the view on the coordinate i so that we can see the whole area.
void setView(Point3f &vMin, Point3f &vMax, int i);

// Initialize the view and the road 
void myinit();

// Read the trajectory file 
void createObjects();

#endif
