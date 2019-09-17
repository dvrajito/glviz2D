/********************************************************************
  File:         obj_container.cc
  Author:       Dana Vrajitoru
  Project:      Space Invaders
  Last updated: September 10, 2007.

  Implementation of a class containing a collection of objects.

********************************************************************/

#include "obj_container.h"

// Constructor specifying 
Obj_container::Obj_container(int dimx, int dimy, int howmany)
  : Object()
{
  group = new Object[howmany];
  for (int i=0; i<howmany; i++)
    group[i].dimension.set_data(dimx, dimy);
  count = howmany;

  // default color is white.
  color.set_data(1, 1, 1);
}

// Double the size of the group and copy all objects from the
// previous one to the new one.
void Obj_container::double_size()
{
  int i;
  Object *newgr = new Object[2*count];
  // Copy all the objects into the new array.
  for (i=0; i<count; i++)
    newgr[i] = group[i];
  // Copy the information from the last object in the old array into
  // all the extra objects in the new arra.
  for (i=count; i<2*count; i++)
    newgr[i] = group[count-1];  
  count *= 2;
}

// Sets the color of the objects.
void Obj_container::set_color(float r, float g, float b)
{
  color.set_data(r, g, b);
}

// Create the display lists for the two phases of the object. To be
// implemented by the derived class.
void Obj_container::draw()
{
  phase1_id = glGenLists(1);
  phase2_id = glGenLists(1);
}

// Call the display lists for all the objects that are alive. We
// might not have to implement anything in the derived class.
void Obj_container::display()
{
  glColor3f(color[0], color[1], color[2]);
  glPushMatrix();
  position.gl_translate();
  for (int i=0; i<count; i++) 
    if (group[i].alive) {
      glPushMatrix();
      // Translate by the position of the current object, then draw it.
      group[i].position.gl_translate();
      if (group[i].phase)
	glCallList(phase1_id);
      else
	glCallList(phase2_id);
      glPopMatrix();
    }
  glPopMatrix();
}

// A function that will make one of the objects in the array come to
// life at a given position on the screen. It finds the first object
// in the array that is not alive, and brings it to life at the
// given position. It returns the subscript of the activated object.
int Obj_container::activate_any(int posx, int posy)
{
  for (int i=0; i<count; i++) 
    if (!group[i].alive) {
      group[i].resurect();
      if (!group[i].move_to(posx, posy)) {
	group[i].move_to(0, 0);
	return i;
      }
    }
  return -1;
}

// Main action for any of these objects defining how they move from
// one frame to the next. To be defined by the derivate classes.
void Obj_container::move()
{
  for (int i=0; i<count; i++) 
    group[i].next_phase();
}

// Computes the bounding box of all the the objects in the container
// and stores it in the dimension of the container object.
void Obj_container::compute_dimension()
{
  // added here for compilation. Replace with your own code.
  dimension.set_data(0, 0);
}
