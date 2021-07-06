/**********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    roadPt.cc
   Updated: July 2021

   Definition of a class drawing a road point and handling some of the 
   interpolation functions. 

**********************************************************************/

#include <iostream>
#include <fstream>
using namespace std;
#include "point3f.h"
#include <cmath>
#include "roadPt.h"

#define MMAX(a, b) a > b ? a : b
#define MMIN(a, b) a > b ? b : a
#define LINEAR_INTERP(a, b, t) (1.0 - t) * a + t * b

// Assign values to the target object based on the 
// linear interpolation of the two parameters.
void RoadPt::linearInterpolate(RoadPt& pt1, RoadPt &pt2, float alpha)
{
    pt[0] = LINEAR_INTERP(pt1.pt[0], pt2.pt[0], alpha);
    pt[1] = LINEAR_INTERP(pt1.pt[1], pt2.pt[1], alpha);
    pt[2] = LINEAR_INTERP(pt1.pt[2], pt2.pt[2], alpha);
    norm[0] = LINEAR_INTERP(pt1.norm[0], pt2.norm[0], alpha);
    norm[1] = LINEAR_INTERP(pt1.norm[1], pt2.norm[1], alpha);
    norm[2] = LINEAR_INTERP(pt1.norm[2], pt2.norm[2], alpha);
    dist = LINEAR_INTERP(pt1.dist, pt2.dist, alpha);
    curv = LINEAR_INTERP(pt1.curv, pt2.curv, alpha);
    traj = LINEAR_INTERP(pt1.traj, pt2.traj, alpha);
}

// Assign values to the target object based on the 
// quadratic interpolation of the two parameters, using the tangent as extra constraint.
void RoadPt::quadraticInterpolate(RoadPt& pt1, RoadPt& pt2, float alpha)
{
    static Point3f t1, t2;
    static float a0, a1, a2, b0, b1, b2; 
    // compute the polynomial again for alpha == 0
    if (alpha == 0) {
        t1.set_data(pt1.norm[1], -pt1.norm[0], 0);
        t2.set_data(pt2.norm[1], -pt2.norm[0], 0);
        a0 = pt1.pt[0];
        b0 = pt1.pt[1];
        if (t1[0] == 0 && t2[0] == 0 || t1[1] == 0 && t2[1] == 0)
            return linearInterpolate(pt1, pt2, alpha);
        if (t1[1] == 0) {
            b1 = 0;
            b2 = pt2.pt[1] - b0;
            a1 = (2 * pt2.pt[0] * t2[1] - 2 * a0 * t2[1] - b2 * t2[0]) / t2[1];
            a2 = pt2.pt[0] - a0 - a1;
        }
        else {
            if (t1[1] * t2[0] - 2 * t1[0] * t2[1] == 0) {
                // no solution, fall back to linear
                return linearInterpolate(pt1, pt2, alpha);
            }
            else {
                b1 = (2 * a0 * t2[1] - 2 * pt2.pt[0] * t2[1] - 2 * b0 * t2[0] + 2 * pt2.pt[1] * t2[0])
                    * t1[1] / (t1[1] * t2[0] - 2 * t1[0] * t2[1]);
                b2 = pt2.pt[1] - b0 - b1;
                a1 = b1 * t1[0] / t1[1];
                a2 = pt2.pt[0] - a0 - a1;
            }
        }
    }
    // now we can compute the point
    pt[0] = a0 + a1 * alpha + a2 * alpha * alpha;
    pt[1] = b0 + b1 * alpha + b2 * alpha * alpha;
    pt[2] = 0;
    norm[0] = b1 + 2 * b2 * alpha;
    norm[1] = -a1 - 2 * a2 * alpha; // perpendicular to the tangent
    norm[2] = 0;
    curv = LINEAR_INTERP(pt1.curv, pt2.curv, alpha);
    traj = LINEAR_INTERP(pt1.traj, pt2.traj, alpha);
}

void copyPoint(RoadPt& pt1, RoadPt pt2)
{
    pt1.curv = pt2.curv;
    pt1.dist = pt2.dist;
    pt1.norm = pt2.norm;
    pt1.pt = pt2.pt;
    pt1.traj = pt2.traj;
    pt1.trjPt = pt2.trjPt;
}