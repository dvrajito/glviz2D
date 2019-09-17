/********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    main.h
   Updated: March 2019

   Some prototypes.

********************************************************************/

#ifndef MAIN_PROTOTYPES_H
#define MAIN_PROTOTYPES_H

#include <GL/glut.h>

#define FILE_ROOT "C:/Users/Dana/Documents/develop/gazelle/data/"

void display(void);

void next_frame();

void mouse(int btn, int state, int x, int y);

void key(unsigned char key, int x, int y);

// To be completed with the movements left and right of the player.
void spkey(int key, int x, int y);

GLvoid timer(int value);

void myinit();

void create_objects();

#endif
