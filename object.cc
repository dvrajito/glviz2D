/********************************************************************
  File:         object.h
  Author:       Dana Vrajitoru
  Project:      Space Invaders
  Last updated: September 10, 2007.

  Definition of a class containing a collection of objects.

********************************************************************/

#include "object.h"

// Constructor specifying position and state.
Object::Object(int posx, int posy, int dimx, int dimy, bool live)
{
  phase = true;
  alive = live;;
  position.set_data(posx, posy);
  dimension.set_data(dimx, dimy);
}

// Asignment operator. Might come in handy.
Object &Object::operator=(Object &data)
{
  if (this != &data) {
    phase = data.phase;
    alive = data.alive;
    position = data.position;
    dimension = data.dimension;
  }
  return *this;
}

// verifies if a given position is on the table for an object with
// these dimensions.
bool Object::is_on_table(int posx, int posy)
{
  if (posx >= 0 && posy >= 0 &&
      posx + dimension.x < twidth &&
      posy + dimension.y < theight)
    return true;
  else
    return false;
}

// Moves the object by the specified x and y displacement. It needs
// to know if the position is still within the dimensions of the
// table, and return false if it isn't without performing the move.
bool Object::move_to(int posx, int posy)
{
  if (is_on_table(posx, posy)) {
    position.set_data(posx, posy);
    return true;
  }
  else
    return false;
}

// Moves the object by the specified x and y displacement. It needs
// to know if the position is still within the dimensions of the
// table, and return false if it isn't without performing the move.
bool Object::move_by(int vx, int vy)
{
  if (is_on_table(position.x+vx, position.y+vy)) {
    position.x += vx;
    position.y += vy;
    return true;
  }
  else
    return false;
}

// Switches the phase to the next one - will have an effect on the
// display list to be used in the display.
void Object::next_phase()
{
  phase = !phase;
}

// Terminates the object. We can activate it again later.
void Object::kill()
{
  alive = false;
}

// Brings the object back to life.
void Object::resurect()
{
  alive = true;
}
