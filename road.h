/**********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    road.cc
   Updated: April 2019

   Definition of a class drawing a road out of a set of points defined
   as distance plus angle.

**********************************************************************/

#ifndef ROAD_H
#define ROAD_H

#include <iostream>
#include <fstream>
using namespace std;
#include "point3f.h"
#include <cmath>

#define MAX_TRAJ 0.8
#define MMAX(a, b) a>b ? a : b
#define MMIN(a, b) a>b ? b : a

struct RoadPt {
    float dist;
    Point3f pt, norm, trjPt;
    float curv;
    float traj;
};

struct KeyFrame {
    int pt;
    int length;
    int sign;
};

enum RoadType {allScale, skipStep};

class Road {
private:
    int roadId, trajId; // ids for the display lists

public:
    Point3f min, max; // corners of the bounding box
    float maxCurv,    // maximum observed curvature 
          almostFlat, // what value of the curvature can be considered flat
                      // E-Track 5: 0.01, Alpine 2: 0.002, E-Road: 0.004, E-Track 4: 0.001 
          inc,        // increment in the trajectory optimization
          curvScale,  // scale factor for the curvature in optimizing the trajectory
          leftScale; // scale factor for the left-turning curves

    float roadScale;  // scale the curvature by it when drawing the road
    float roadWidth;  // lateral width of the road; only relevant for drawing it
    float roadStep;   // step (delta d) in drawing the road
    int trajStep;     // step in determining the trajectory, used by the GA
    int flatLength;   // the count of flat points to break and assign a 0 trajectory
    int curveLength;  // the count of curve points in one direction to assign an anchor
    bool hasWidth, hasTraj;
    RoadType rdType;

    vector<RoadPt> points;
    vector<int> ctrlPts;
    vector<KeyFrame> keyframes;

    // constructor from a file
    Road(char *filename = "");

    // Constructor from a file with start and end points
    Road(char *filename, float startPt, float endPt);

    // initialize the road from a file 
    void init(char *filename);

    // initialize from a file between the indexes startPt and endPt
    void init(char *filename, float startPt, float endPt);

    // Initialize settings from a dictionary 
    void init(char **aDict, int nOpt);

    // set the values of a particular point
    void setPt(int i, float d, Point3f &p, float c);

    // Call the display list.
    void display();

    ////////////////////////// Draw //////////////////////////////

    // To be reimplemented for this class.
    void draw(char *filename);
    // To be reimplemented for this class.
    void draw(char *filename, float startPt, float endPt);

    // Draw from the list of points already stored.
    void draw();
    // Create the display list from a list of points already stored. 
    // Display the road as a line
    void drawLineFromPoints();
    // Create the display list from a list of points already stored. 
    // Display the road as a ribbon
    void drawRibbonFromPoints();

    // set the color of the trajectory based on various options
    void setTrajColor(int i, GLfloat &red, GLfloat &blue);
    // Create the display list from the stored trajectory 
    void drawTrajFromPoints();

    // Read the data from the file, calculate and draw the points 
    void drawPointList(ifstream &fin);
    
    // Draw the points list as a ribbon if the road has a width
    void drawRibbonPtList(ifstream &fin);

    ////////////////////////// Read from file ////////////////////

    // Read the road from a file and store the points in a vector
    void read(char *filename);

    // Read the data from the file, calculate and store the points 
    void readPointList(ifstream &fin);
    
    // Read the data from the file, calculate and store the points 
    void readPointList(ifstream &fin, float startPt, float endPt);

    // Read the data from the file using a step, storing only 1 in a number, 
    // then calculate and store the points 
    void readStepPointList(ifstream &fin, float startPt, float endPt);

    // Read the trajectory points from a file and interpolate it to match the points we have
    void readTrajFile(char *filename, bool redraw = true);

    ////////////////////////// Write to file ////////////////////

    // write the stored trajectory in a file for use in Gazelle
    void writeTrajFile(char *filename);

    // set the trajectory as a constant
    void setConstTraj(float tr = 0.0, bool redraw = true);

    // write the real points of the trajectory together with the real curvature
    void writeRealPts(char *filename);

    ////////////////////////// Trajectory computing ///////////////////////////

    // Compute the real value of the trajectory point given the normal to the centerline
    // and the trajectory value
    void computeTrajPt(int i);

    // Compute the real value of the trajectory points between start and end indexes
    void computeTrajPts(int start, int end);

    // Optimize the trajectory by moving the points along the real curvature direction 
    // while they still remain in the bounds of the road
    void optimizeTraj();

    // Averages the values with those around in a given radius.
    void smoothTrajectory(int radius);

    // find the control points of the trajectory, which would be the ones 
    // where the trajectory curves the most, or the middle of a continuous stretch
    void findControlPoints(vector<int> &data);

    // find the key frames of the trajectory, which would be the ones 
    // where the trajectory curves the most, or the middle of a continuous stretch
    void findKeyFrames();

    // update the boudaries points so that we know how to draw the road
    void updateMinMax(Point3f &pt, float curv);

    // given an index in the points array, computes the curvature of the trajectory
    float realTrajCurv(int i);

    // Is the road almost flat at this index?
    bool isFlat(int pt);

    ////////////////////////// GA-based Trajectory ///////////////////////////
    
    // Find the next anchor assuming that we do have the keyframes computed. 
    void findNextAnchorKF(int &kfStart, int &kfEnd);

    // Find the center of the next stretch where the road is almost flat for a number
    // of points equal to flatLength.
    int findNextAnchor(int start, bool &flat);

    // Set the trajectory between start and end points with given density
    void setTrajectory(double traj[], int size, int startPt, int endPt, int step);
    
    // Set the trajectory between start and end points with given density
    void setTrajectoryKF(double traj[], int size, int startKF, int endKF, int step);
    
    // Set the trajectory between start and end points with given density 
    // with the trajectory coming in as a vector
    void setTrajectoryKF(vector<double> traj, int startKF, int endKF, int interm);

    // Set the trajectory between start and end points with given density
    void setTrajectory(vector<double> traj, int startPt, int endPt, int step);

    // Calculate the real distance along the trajectory between the start and end points
    double sumDistance(int startPt, int endPt);

    // Find the maximum absolute value of the curvature between start and end points
    double findMaxCurv(int startPt, int endPt);

    // Sum the curvature between start and end points
    double sumCurv(int startPt, int endPt);
};

#endif
