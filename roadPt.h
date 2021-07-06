/**********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    roadPt.h
   Updated: July 2021

   Definition of a class drawing a road point and handling some of the 
   interpolation functions. 

**********************************************************************/

#ifndef ROAD_PT_H
#define ROAD_PT_H

#include "point3f.h"

#define MMAX(a, b) a > b ? a : b
#define MMIN(a, b) a > b ? b : a
#define LINEAR_INTERP(a, b, t) (1.0 - t) * a + t * b

enum InterpType {none, linear, quadr, cubic};

class RoadPt {
public:
    float dist;
    Point3f pt, norm, trjPt;
    float curv;
    float traj;

    // Assign values to the target object based on the 
    // linear interpolation of the two parameters.
    void linearInterpolate(RoadPt& pt1, RoadPt &pt2, float alpha);
    // Assign values to the target object based on the 
    // quadratic interpolation of the two parameters, using the tangent as extra constraint.
    void quadraticInterpolate(RoadPt& pt1, RoadPt& pt2, float alpha);

    // need to be able to move the data around
    friend void copyPoint(RoadPt& pt1, RoadPt pt2);
};



#endif
