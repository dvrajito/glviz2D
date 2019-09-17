/********************************************************************
  File:         obj_container.h
  Author:       Dana Vrajitoru
  Project:      Space Invaders
  Last updated: March, 2019.

  Definition of a class containing a collection of objects.

********************************************************************/

#ifndef OBJ_CONTAINER_H
#define OBJ_CONTAINER_H

#include "object.h"
#include "point3f.h"

class Obj_container : public Object{
 public:
  // The group will have the size equal to count. We may have several
  // copies of the same object with different positions and states,
  // but using the same display lists.
  Object *group;
  int count;
  float vx, vy;

  Point3f color;

  // IDs for the display listscontaining the OpenGL commands for the
  // two phases of the object.
  GLuint phase1_id, phase2_id;

 public:
  // Constructor specifying the number of possible objects to be
  // created. By default it will create them all as unalive, but
  // derivate classes may implement as many as they want alive in the
  // constructor.
  Obj_container(int dimx=1, int dimy=1, int howmany = 1);

  // Double the size of the group and copy all objects from the
  // previous one to the new one.
  void double_size();

  // Sets the color of the objects.
  void set_color(float r, float g, float b);

  // Create the display lists for the two phases of the object. To be
  // implemented by the derived class.
  virtual void draw();

  // Call the display lists for all the objects that are alive. We
  // might not have to implement anything in the derived class.
  virtual void display();

  // A function that will make one of the objects in the array come to
  // life at a given position on the screen. It finds the first object
  // in the array that is not alive, and brings it to life at the
  // given position. It returns the subscript of the activated object.
  virtual int activate_any(int posx=0, int posy=0);

  // Main action for any of these objects defining how they move from
  // one frame to the next. To be defined by the derivate classes.
  virtual void move();

  // Computes the bounding box of all the the objects in the container
  // and stores it in the dimension of the container object.
  void compute_dimension();
};

#endif
