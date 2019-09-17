/********************************************************************
  File:         object.h
  Author:       Dana Vrajitoru
  Project:      Space Invaders
  Last updated: September 10, 2007.

  Definition of a class containing a simple object.

********************************************************************/

#ifndef OBJECT_H
#define OBJECT_H

#include "table.h"
#include "point2d.h"

class Object {
 public:

  bool phase, alive;
  Point2d position;
  Point2d dimension;

  // Constructor specifying position and state.
  Object(int posx=0, int posy=0, int dimx=1, int dimy=1, bool live=true);

  // Asignment operator. Might come in handy.
  Object &operator=(Object &data);

  // verifies if a given position is on the table for an object with
  // these dimensions.
  bool is_on_table(int posx, int posy);

  // Moves the object by the specified x and y displacement. It needs
  // to know if the position is still within the dimensions of the
  // table, and return false if it isn't without performing the move.
  bool move_to(int posx, int posy);

  // Moves the object to the specified position. It needs to know if
  // the position is still within the dimensions of the table, and
  // return false if it isn't without performing the move.
  bool move_by(int vx, int vy);

  // Switches the phase to the next one - will have an effect on the
  // display list to be used in the display.
  void next_phase();

  // Terminates the object. We can activate it again later.
  void kill();
  // Brings the object back to life.
  void resurect();

};

#endif
