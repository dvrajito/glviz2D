/**********************************************************************
   Project: Gl Visualizer
   License: Creative Commons, Attribution
   Author:  Dana Vrajitoru
   File:    road.cc
   Updated: June 2021

   Definition of a class drawing a road out of a set of points defined
   as distance plus angle.

**********************************************************************/

#include <cmath>
#include <cstring>
#include <algorithm>
#include "road.h"
#include "General.h"

// Optimal road scales + left scale:
// E-Track4 0.85  left
// E-Track4 0.06  right *
// E-Road   0.065 right *
// E-Road   0.036 left
// Alpine2  0.067 left
// Alpine2  0.15  0.62 right *

/////////// Skip step values
// E-Track5 2.5 right
// E-Road   3.8 right

// Constructor from a file
Road::Road(char *filename)
    : min(0, 0, 0), max(0, 0, 0)
{
    init();
    if (filename) 
        init(filename);
}

// Constructor from a file with start and end points
Road::Road(char *filename, float startPt, float endPt)
    : min(0, 0, 0), max(0, 0, 0)
{
    init();
    if (filename)
        init(filename, startPt, endPt);
}

// initialize the road with default values
void Road::init()
{
    roadScale = 0.15; // taken from the comments above
    leftScale = 0.62;
    almostFlat = 0.0005;
    inc = 0.1;
    curvScale = 20;
    roadWidth = 15;
    hasWidth = true;
    // To avoid warnings:
    curveLength = 10;
    flatLength = 10;
    maxCurv = 1;
    roadId = 0;
    trajId = 0;
    almostFlat = 0.1;
    curvScale = 2;
    curveLength = 1;
    flatLength = 5;
    hasWidth = true;
    inc = 0;
    // up to here the values don't have to make sense
    hasTraj = true;
    rdType = allScale; // skipStep;
    roadStep = 3.8;
    trajStep = 5;
}

// initialize the road from a file 
void Road::init(char *filename)
{
    if (filename) {
        read(filename);
        draw();
    }
}

// initialize the road from a file containing the centerpoints
void Road::initCenter(char* filename)
{
    if (filename) {
        readCenter(filename);
        draw();
    }
}

// initialize from a file between the indexes startPt and endPt
void Road::init(char *filename, float startPt, float endPt)
{
    if (filename[0] != '\0') {
        read(filename);
        draw(filename, startPt, endPt);
    }
}

// Initialize settings from a dictionary 
void Road::init(char **aDict, int nOpt)
{
    for (int i = 0; i < nOpt; i++)
        if (strcmp(aDict[2 * i], "almost flat") == 0)
            almostFlat = atof(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "road scale") == 0)
            roadScale = atof(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "left scale") == 0)
            leftScale = atof(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "curvature scale") == 0)
            curvScale = atof(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "trajectory step") == 0)
            trajStep = atoi(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "flat length") == 0)
            flatLength = atoi(aDict[2 * i + 1]);
        else if (strcmp(aDict[2 * i], "curve length") == 0)
            curveLength = atoi(aDict[2 * i + 1]);
}

// set the values of a particular point
void Road::setPt(int i, float d, Point3f &p, float c)
{
    points[i].dist = d;
    points[i].pt = p;
    points[i].curv = c;
    points[i].traj = 0;
}

// Read the road from a file and store the points in the vector
void Road::read(char *filename)
{
    ifstream fin(filename);
    if (fin.good())
        readPointList(fin);
    else
        cout << "Could not open the road file " << filename << endl;
    fin.close();
}

// Read the road from a file containing the centerline points
// and store the points in the vector
void Road::readCenter(char* filename)
{
    ifstream fin(filename);
    if (fin.good())
        readCenterList(fin);
    else
        cout << "Could not open the road centerline file " << filename << endl;
    fin.close();
}

// Call the display list.
void Road::display()
{
    //glColor3f(color[0], color[1], color[2]);
    glPushMatrix();
    //position.gl_translate();
    glCallList(roadId);

    if (hasTraj)
        glCallList(trajId);
    glPopMatrix();
}

// To be reimplemented for this class.
void Road::draw(char *filename)
{
    // get id numbers for the display lists
    roadId = glGenLists(1);
    ifstream fin(filename);
    if (fin.good())
        if (!hasWidth)
            drawPointList(fin);
        else
            drawRibbonPtList(fin);
    else
        cout << "Could not open the road file " << filename << endl;
    fin.close();
}

// To be reimplemented for this class.
void Road::draw(char *filename, float startPt, float endPt)
{
    // get id numbers for the display lists
    roadId = glGenLists(1);
    ifstream fin(filename);
    if (fin.good())
        readPointList(fin, startPt, endPt);
    else
        cout << "Could not open the road file " << filename << endl;
    fin.close();
}

// Draw from the list of points already stored.
void Road::draw()
{
    // get id numbers for the display lists
    roadId = glGenLists(1);
    trajId = glGenLists(1);

    if (!hasWidth)
        drawLineFromPoints();
    else
        drawRibbonFromPoints();

    if (hasTraj)
        drawTrajFromPoints();
}

// Create the display list from a list of points already stored. 
// Display the road as a line
void Road::drawLineFromPoints()
{
    glNewList(roadId, GL_COMPILE);
    glColor3f(1, 1, 0);  // yellow
    glBegin(GL_LINE_STRIP);
    for (unsigned int i = 0; i < points.size(); i++)
        glVertex2f(points[i].pt.x(), points[i].pt.y());
    glEnd();
    glEndList();
    //cout << "min: " << min << " max: " << max << endl;
}

// Create the display list from a list of points already stored. 
// Display the road as a ribbon
void Road::drawRibbonFromPoints()
{
    Point3f pt(0, 0, 0), dir(1, 0, 0), nor(0, 0, 0),
            pt1(points[0].pt.x(), points[0].pt.y() + 2*roadWidth, 0), 
            pt2(points[0].pt.x(), points[0].pt.y() - roadWidth, 0);
    int nrpt = points.size();
    glNewList(roadId, GL_COMPILE);
    glColor3f(1, 1, 0);  // yellow
    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(pt1.x(), pt1.y());
    glVertex2f(pt2.x(), pt2.y());
    for (unsigned int i = 1; i < nrpt; i++) 
    {
        pt = points[i].pt;
        dir = pt;
        dir -= points[i - 1].pt;
        nor = points[i].norm; // already computed
        if (nrpt > 2000 && i < 100)
            nor *= 2 * roadWidth;
        else
            nor *= roadWidth;
        pt1 = pt;
        pt1 += nor;
        pt2 = pt;
        pt2 -= nor;
        glVertex2f(pt1.x(), pt1.y());
        glVertex2f(pt2.x(), pt2.y());
    }
    glEnd();
    glEndList();
    if (points.size() > 1)
        points[0].norm = points[1].norm;
    //cout << "min: " << min << " max: " << max << endl;
}

// set the color of the trajectory based on various options
void Road::setTrajColor(int i, GLfloat &red, GLfloat &blue)
{
    int select = 3;
    static int kf = 0;
    switch(select)
    {
    case 0:
        blue = red = 0;
        // red for real curvature and road  curvature going in the same direction
        if (points[i].curv * realTrajCurv(i) >= 0)
            red = 1;
        else
            blue = 1;
        break;
    case 1:
        blue = red = 0;
        blue = 0.5 * (points[i].curv + 1) / maxCurv;
        red = 0.5 * (realTrajCurv(i) + 1) / maxCurv;
        break;
    case 2:
        blue = red = 0;
        // if i is in the control points
        if (find(ctrlPts.begin(), ctrlPts.end(), i) != ctrlPts.end())
            blue = 1;
        else
            red = 1;
        break;
    case 3:
        if (i == 1)
            kf = 1;
        // every time we hit a control point, switch
        if (kf < keyframes.size() && keyframes[kf].pt == i)
        {
            kf++;
            red = 1 - red;
            blue = 1 - blue;
        }
        break;
    }
}

// Create the display list from the stored trajectory 
void Road::drawTrajFromPoints()
{
    Point3f pt(0, 0, 0), dir(1, 0, 0), nor(0, 0, 0), pt1(0, 0, 0);
    GLfloat blueCmp = 0, redCmp = 1;
    glDeleteLists(trajId, 1);
    glNewList(trajId, GL_COMPILE);
    glColor3f(0.5, 0, 0.5);  // purple
    glLineWidth(1.5);
    glBegin(GL_LINE_STRIP);
    glVertex2f(points[0].pt.x(), points[0].pt.y());
    for (unsigned int i = 1; i < points.size(); i++)
    {
        pt1 = points[i].trjPt;
        // now set the color
        setTrajColor(i, redCmp, blueCmp);
        glColor3f(redCmp, 0, blueCmp);
        glVertex2f(pt1.x(), pt1.y());
    }
    glEnd();
    glEndList();
}

// Read the data from the file, calculate and draw the points 
void Road::drawPointList(ifstream &fin)
{
    glNewList(roadId, GL_COMPILE);
    glColor3f(1, 1, 0);  // yellow
    glBegin(GL_LINE_STRIP);
    Point3f pt(0, 0, 0), dir(1, 0, 0);
    float dist, cosTau, sinTau, oldDist = 0, deltad, dx, dy;
    fin >> oldDist >> sinTau >> dist;
    glVertex2f(pt.x(), pt.y());
    updateMinMax(pt, sinTau);
    while (fin.good() & !fin.eof()) {
        // calculate new pt and dir

        deltad = dist - oldDist;
        sinTau *= roadScale; // scale down the angle
        // rotate the direction by the angle
        cosTau = sqrt(1 - sinTau*sinTau);
        dx = dir.x() * cosTau + dir.y() * sinTau;
        dy = -dir.x() * sinTau + dir.y() * cosTau;
        dir.x() = dx;
        dir.y() = dy;
        //if (sinTau > 0)
            //cout << dist << '\t' << cosTau << ' ' << sinTau << '\t' << dir << endl;
        // set the length od dir to deltad
        dir.normalize();
        if (deltad != 0) {
            //dir *= deltad; // simple
            //dir *= deltad*(0.0001 + fabs(sinTau)); // adjusted
            dir *= deltad * (0.0001 + cosTau); // other version
            pt += dir;
            // draw the new point
            glVertex2f(pt.x(), pt.y());
            updateMinMax(pt, sinTau);
        }

        // update the data
        oldDist = dist;
        fin >> sinTau >> dist;
    }
    glEnd();
    glEndList();
    cout << "min: " << min << " max: " << max << endl;
}

// Draw the points list as a ribbon if the road has a width
void Road::drawRibbonPtList(ifstream &fin)
{
    glNewList(roadId, GL_COMPILE);
    glColor3f(1, 1, 0);  // yellow
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLE_STRIP);
    Point3f pt(0, 0, 0), oldPt(0, 0, 0), nor(0, 0, 0),
            pt1(0, 2*roadWidth, 0), pt2(0, -roadWidth, 0), dir(1, 0, 0);
    float dist, cosTau, sinTau, oldDist = 0, deltad, dx, dy;
    fin >> oldDist >> sinTau >> dist;
    glVertex2f(pt1.x(), pt1.y());
    glVertex2f(pt2.x(), pt2.y());
    updateMinMax(pt, sinTau);
    while (fin.good() & !fin.eof()) {
        // calculate new pt and dir

        deltad = dist - oldDist;
        sinTau *= roadScale; // scale down the angle
                             // rotate the direction by the angle
        cosTau = sqrt(1 - sinTau*sinTau);
        dx = dir.x() * cosTau + dir.y() * sinTau;
        dy = -dir.x() * sinTau + dir.y() * cosTau;
        dir.x() = dx;
        dir.y() = dy;
        //if (sinTau > 0)
        //cout << dist << '\t' << cosTau << ' ' << sinTau << '\t' << dir << endl;
        // set the length od dir to deltad
        dir.normalize();
        if (deltad != 0) {
            //dir *= deltad; // simple
            //dir *= deltad*(0.0001 + fabs(sinTau)); // adjusted
            dir *= deltad * (0.0001 + cosTau); // other version
            pt += dir;
            nor.x() = dir.y();   // perpendicular in the xy plane
            nor.y() = -dir.x();
            nor.normalize();
            nor *= roadWidth;
            pt1 = pt;
            pt1 += nor;
            pt2 = pt;
            pt2 -= nor;
            // draw the new points
            glVertex2f(pt1.x(), pt1.y());
            glVertex2f(pt2.x(), pt2.y());
            updateMinMax(pt, sinTau);
        }

        // update the data
        oldDist = dist;
        fin >> sinTau >> dist;
    }
    glEnd();
    glEndList();
    cout << "min: " << min << " max: " << max << endl;
}

// Read the data from the file, calculate and store the points 
void Road::readPointList(ifstream &fin)
{
    if (rdType == allScale)
        readPointList(fin, 0, 1000000);
    else
        readStepPointList(fin, 0, 1000000);

}

// Read the centerline points from the file, calculate and store the curvature 
void Road::readCenterList(ifstream &fin)
{
    int nrPoints;
    float cosTau, sinTau, totalDist, realDist, scaleF;
    fin >> nrPoints >> realDist;
    RoadPt point;
    Point3f nor(0, 1, 0), pt1, pt2;
    point.norm = nor;
    fin >> point.pt[0] >> point.pt[1];
    point.pt[2] = 0;
    point.dist = 0;
    point.curv = 0;
    point.traj = 1;
    points.push_back(point);
    point.traj = 0;
    for (int i = 1; i < nrPoints; i++) {
        fin >> point.pt[0] >> point.pt[1];
        point.dist = points[i - 1].dist + point.pt.distance(points[i - 1].pt);
        points.push_back(point);
        if (i > 1) {
            // calculate the normal as the average of the normal to the previous two segments
            pt1 = points[i - 1].pt;
            pt1 -= points[i - 2].pt;
            pt1.rotate_z(RADIANS(90));
            pt2 = points[i].pt;
            pt2 -= points[i - 1].pt;
            pt2.rotate_z(RADIANS(90));
            points[i - 1].norm = pt1;
            points[i - 1].norm += pt2;
            points[i - 1].norm.normalize();
            // Calculate the curvature based on the angle between the two segments used above
            cosTau = pt1.scalarprod(pt2) / (pt1.norm() * pt2.norm());
            sinTau = sqrt(1 - cosTau * cosTau);
            if (pt1[0] * pt2[1] - pt1[1] * pt2[0] < 0) // z coordinate of the cross-product
                sinTau = -sinTau; // going the other way
            points[i - 1].curv = sinTau;
            updateMinMax(point.pt, sinTau);
        }
    }
    totalDist = points[nrPoints - 1].dist + points[nrPoints - 1].pt.distance(points[0].pt);
    cout << "min: " << min << " max: " << max << " maxCurv " << maxCurv 
        << " total dist " << totalDist << endl;
}

// Read the data from the file, calculate and store the points 
void Road::readPointList(ifstream &fin, float startPt, float endPt)
{
    if (points.size()) // delete old data
        points.clear();
    Point3f pt(0, 0, 0), dir(1, 0, 0), nor(0, 1, 0);
    RoadPt point;
    int i = 0;
    float dist, cosTau, sinTau, oldDist = 0, deltad, dx, dy;
    fin >> oldDist >> sinTau >> dist;
    if (oldDist >= startPt && oldDist <= endPt) {
        points.push_back(point);
        points[0].norm = nor;
        setPt(i++, oldDist, pt, sinTau);
    }
    updateMinMax(pt, sinTau);
    while (fin.good() & !fin.eof()) {
        // calculate new pt and dir

        if (dist >= startPt && dist <= endPt) {
            deltad = dist - oldDist;
            if (sinTau >= 0)
                sinTau *= roadScale; // scale down the angle
                                     // rotate the direction by the angle
            else
                sinTau *= leftScale;
            cosTau = sqrt(1 - sinTau*sinTau);
            dx = dir.x() * cosTau + dir.y() * sinTau;
            dy = -dir.x() * sinTau + dir.y() * cosTau;
            dir.x() = dx;
            dir.y() = dy;
            //if (sinTau > 0)
            //cout << dist << '\t' << cosTau << ' ' << sinTau << '\t' << dir << endl;
            // set the length od dir to deltad
            dir.normalize();
            nor.x() = -dir.y();   // perpendicular in the xy plane
            nor.y() = dir.x();
            if (deltad != 0) {
                //dir *= deltad; // simple
                //dir *= deltad*(0.0001 + fabs(sinTau)); // adjusted
                dir *= deltad * (0.0001 + cosTau); // other version
                pt += dir;
                // add the new point
                points.push_back(point);
                points[i].norm = nor;
                setPt(i++, oldDist, pt, sinTau);
            }
            updateMinMax(pt, sinTau);
        }

        // update the data
        oldDist = dist;
        fin >> sinTau >> dist;
    }
    cout << "min: " << min << " max: " << max << " maxCurv " << maxCurv << endl;
}

// Read the data from the file using a step, storing only 1 in a number, 
// then calculate and store the points 
void Road::readStepPointList(ifstream &fin, float startPt, float endPt)
{
    if (points.size()) // delete old data
        points.clear();
    Point3f pt(0, 0, 0), dir(1, 0, 0), nor(0, 1, 0);
    RoadPt point;
    int i = 0, j = 0, aveCount=0;
    float dist, cosTau, sinTau, sumSinTau=0, oldDist = 0, deltad=0, dx, dy, stepSum = 0;
    fin >> oldDist >> sinTau >> dist;
    if (oldDist >= startPt && oldDist <= endPt) {
        points.push_back(point);
        points[0].norm = nor;
        setPt(j++, oldDist, pt, sinTau);
        i++;
        stepSum += 1;
        aveCount++;
    }
    updateMinMax(pt, sinTau);
    while (fin.good() & !fin.eof()) {
        // calculate new pt and dir

        if (dist >= startPt && dist <= endPt) {
            deltad += dist - oldDist;
            sumSinTau += sinTau; 

            if (stepSum >= roadStep) {
                stepSum -= roadStep;
                sinTau = sumSinTau / aveCount; // average the angle
                aveCount = 0;
                cosTau = sqrt(1 - sinTau*sinTau);
                dx = dir.x() * cosTau + dir.y() * sinTau;
                dy = -dir.x() * sinTau + dir.y() * cosTau;
                dir.x() = dx;
                dir.y() = dy;
                //if (sinTau > 0)
                //cout << dist << '\t' << cosTau << ' ' << sinTau << '\t' << dir << endl;
                // set the length od dir to deltad
                dir.normalize();
                nor.x() = -dir.y();   // perpendicular in the xy plane
                nor.y() = dir.x();
                if (deltad != 0) {
                    //dir *= deltad; // simple
                    //dir *= deltad*(0.0001 + fabs(sinTau)); // adjusted
                    dir *= deltad * (0.0001 + cosTau); // other version
                    pt += dir;
                    // add the new point
                    points.push_back(point);
                    points[j].norm = nor;
                    setPt(j++, oldDist, pt, sinTau);
                }
            }
            updateMinMax(pt, sinTau);

            // reset the averaged values
            sumSinTau = 0;
            deltad = 0;
        }

        // update the data
        oldDist = dist;
        fin >> sinTau >> dist;
        i++;
        stepSum += 1;
        aveCount++;
    }
    cout << "min: " << min << " max: " << max << " maxCurv " << maxCurv << endl;
}

// Compute the real value of the trajectory point given the normal to the centerline
// and the trajectory value
void Road::computeTrajPt(int i)
{
    Point3f pt, pt1, nor;
    pt = points[i].pt;
    nor = points[i].norm;   // perpendicular in the xy plane
    nor *= roadWidth * points[i].traj;
    pt1 = pt;
    pt1 += nor;
    points[i].trjPt = pt1;
}
// Compute the real value of the trajectory points between start and end indexes
void Road::computeTrajPts(int start, int end)
{
    for (unsigned int i = start; i < end && i < points.size(); i++)
        computeTrajPt(i);
}

// Read the trajectory points from a file and interpolate it to match the points we have
void Road::readTrajFile(char *filename, bool redraw)
{
    ifstream fin(filename);
    if (!fin.good())
    {
        cout << "could not read the trajectory from file " << filename << endl;
        return;
    }
    float dist1 = 0, dist2, traj1 = 0, traj2, alpha;
    int nrPoints, scan, maxDist;
    maxDist = int(points[points.size() - 1].dist);
    nrPoints = points.size();
    scan = 0;
    // read the data from the file
    while (!fin.eof() && fin.good())
    {
        fin >> dist2 >> traj2; //read the two values
        if (fin.good())  // store them in the vectors
        {
            while (scan < nrPoints && points[scan].dist <= dist2)
            {
                if (dist1 == dist2)
                    points[scan].traj = traj2;
                else
                {
                    alpha = (points[scan].dist - dist1) / (dist2 - dist1);
                    points[scan].traj = (1 - alpha) * traj1 + alpha * traj2;
                }
                computeTrajPt(scan);
                if (isnan(points[scan].traj))
                    cout << "nan at " << scan << " dist " << points[scan].dist << endl;
                scan++;
            }
            dist1 = dist2;
            traj1 = traj2;
        }
    }
    fin.close();
    // fill out the points at the end if the trajectory stops short of the last point
    while (scan < points.size())
    {
        points[scan].traj = traj1;
        computeTrajPt(scan);
        scan++;
    }
    // redraw the trajectory
    if (redraw)
        drawTrajFromPoints();
}

// write the stored trajectory in a file for use in Gazelle
void Road::writeTrajFile(char *filename)
{
    ofstream fout(filename);
    if (!fout.good())
    {
        cout << "Could not open the file " << filename << " to write the trajectory" << endl;
        return;
    }
    for (unsigned int i = 0; i < points.size(); i++)
        fout << points[i].dist << "\t" << points[i].traj << endl;
    fout.close();
}

// set the trajectory as a constant
void Road::setConstTraj(float tr, bool redraw)
{
    for (unsigned int i = 0; i < points.size(); i++)
    {
        points[i].traj = tr;
        computeTrajPt(i);
    }
    // redraw the trajectory
    if (redraw)
        drawTrajFromPoints();
}

// write the real points of the trajectory together with the real curvature
void Road::writeRealPts(char *filename)
{
    ofstream fout(filename);
    if (!fout.good())
    {
        cout << "Could not open the file " << filename << " to write the real points" << endl;
        return;
    }
    float realCrv;
    for (unsigned int i = 0; i < points.size(); i++)
    {
        realCrv = realTrajCurv(i);
        fout << points[i].dist << "\t" << points[i].trjPt << "\t" << realCrv << endl;
    }
    fout.close();
}

// Optimize the trajectory by moving the points along the real curvature direction 
// while they still remain in the bounds of the road
void Road::optimizeTraj()
{
    float realTC;
    //findControlPoints(ctrlPts);
    for (unsigned int i = 1; i < points.size() - 1; i++)
    {
        realTC = realTrajCurv(i);
        // if the real curve is not flat and we're not already at the maximum trajectory
        if (fabs(realTC) > almostFlat && (fabs(points[i].traj) < MAX_TRAJ || realTC * points[i].curv < 0))
        {
            if (realTC > 0)
            {
                if (points[i].traj < MAX_TRAJ - inc)
                    points[i].traj += MMIN(inc, curvScale*sqrt(realTC));
                else
                    points[i].traj = MAX_TRAJ;
            }
            else
            {
                if (points[i].traj > -MAX_TRAJ + inc)
                    points[i].traj += MMAX(-inc, -curvScale*sqrt(-realTC));
                else
                    points[i].traj = -MAX_TRAJ;
            }
        }
    }
    // recompute the actual points
    computeTrajPts(1, points.size());
    // and redraw the trajectory
    drawTrajFromPoints();
}

// Averages the values with those around in a given radius.
void Road::smoothTrajectory(int radius)
{
    unsigned int i, j;
    float value;
    for (i = radius; i < points.size() - radius; i++)
    {
        value = points[i].traj;
        for (j = 1; j < radius; j++)
        {
            value += ((radius - j)*points[i - j].traj) / radius;
            value += ((radius - j)*points[i + j].traj) / radius;
        }
        points[i].traj = value/radius;
    }
    // recompute the actual points
    computeTrajPts(1, points.size());
    // and redraw the trajectory
    drawTrajFromPoints();
}

// find the control points of the trajectory, which would be the ones 
// where the trajectory curves the most, or the middle of a continuous stretch
void Road::findControlPoints(vector<int> &data)
{
    if (points.size() < 3)
        return;
    float realT1, realT2, realT3;
    int startStr=0, endStr=0;
    data.clear();
    data.push_back(0);
    realT1 = realTrajCurv(0);
    realT2 = realTrajCurv(1);
    for (unsigned int i = 1; i < points.size()-1; i++)
    {
        realT3 = realTrajCurv(i + 1);
        // add points of maximum or minimum real curvature
        if ((realT2 - realT1) * (realT2 - realT3) > 0)
            data.push_back(i);
        else if (realT1 == realT2 && realT2 != realT3)
        {
            endStr = i;
            // add the middle of a continuous stretch
            data.push_back((startStr + endStr) / 2);
        }
        // the beginning of a continuous stretch
        else if (realT1 != realT2 && realT2 == realT3)
            startStr = i;
        // move the values left
        realT1 = realT2;
        realT2 = realT3;
    }
}

// find the control points of the trajectory, which would be the ones 
// where the trajectory curves the most, or the middle of a continuous stretch
void Road::findKeyFrames()
{
    if (points.size() < 3)
        return;
    float c1, c2;
    KeyFrame kf = { 0, 1, 0 };
    bool flat = false;
    int startStr = 0, endStr = 0;
    keyframes.clear();
    keyframes.push_back(kf);
    c1 = points[0].curv;
    if (isFlat(0))
        flat = true;
    else if (c1 > 0)
        keyframes[0].sign = 1;
    else
        keyframes[0].sign = -1;
    for (unsigned int i = 1; i < points.size(); i++)
    {
        if (i < points.size() - 1 && isFlat(i) && flat) // flat stretch continues
            endStr++;
        else
        {
            c2 = points[i].curv;
            if (i < points.size()-1 && c1*c2 > 0) // goes in the same direction
                endStr++;
            else // end of stretch
            {
                // record the key frame
                kf.pt = (startStr + endStr) / 2; // middle point
                kf.length = endStr - startStr + 1;
                if (flat)
                    kf.sign = 0;
                else if (c1 > 0)
                    kf.sign = 1;
                else
                    kf.sign = -1;
                cout << keyframes.size() << "\tkey frame:\t" << kf.pt << "\tlength:\t" << kf.length
                     << "\tsign:\t" << kf.sign << " dist:\t" << points[kf.pt].dist << endl;
                if (kf.pt > 0)
                    keyframes.push_back(kf); 
                else
                    keyframes[0].length = kf.length;
                // start the next stretch
                startStr = endStr = i;
                flat = isFlat(i);
                c1 = points[i].curv;
            }
        }
    }
    cout << "Total key frames: " << keyframes.size() << endl;
}

// Output the key frames. If they're not computed, compute them first.
void Road::outputKeyFrames()
{
    if (keyframes.size() == 0)
        findKeyFrames();
    cout << "Nr\tKeyframe\tlength\tsign\tdist" << endl;
    for (int i = 0; i < keyframes.size(); i++) {
        cout << i << " " << keyframes[i].pt << " " << keyframes[i].length
            << " " << keyframes[i].sign << " " << points[keyframes[i].pt].dist << endl;
    }
}

// Compute the keyframes as the points where the curvature changes sign.
void Road::computeCurvChangePts()
{
    KeyFrame kf;
    for (int i = 0; i < points.size(); i++) {
        if (i == 0 || i == points.size() - 1 || points[i - 1].curv * points[i].curv <= 0) {
            kf.pt = i;
            if (points[i].curv < 0)
                kf.sign = -1;
            else if (points[i].curv == 0)
                kf.sign = 0;
            else
                kf.sign = 1;
            if (i == 0)
                kf.length = 0;
            else
                kf.length = i - keyframes[keyframes.size() - 1].pt;
            cout << keyframes.size() << " " << kf.pt << " " << kf.length << " " << kf.sign << endl;
            keyframes.push_back(kf);
        }
    }
}

// Output all the points where the trajectory changes sign or it is 0.
void Road::outputCurvChangePts()
{
    for (int i = 0; i < points.size(); i++) {
        if (i == 0 || i == points.size() - 1 || points[i - 1].curv * points[i].curv <= 0)
            cout << i << " " << points[i].dist << " " << points[i].curv << endl;
    }
}

// Output all the points with distance and curvature
void Road::outputPoints()
{
    for (int i = 0; i < points.size(); i++) {
        cout << i << " " << points[i].pt << " "
             << points[i].dist << " " << points[i].curv << endl;
    }
}

// update the min and max coordinates in the whole road
void Road::updateMinMax(Point3f &pt, float curv)
{
    if (pt.x() < min.x())
        min.x() = pt.x();
    if (pt.y() < min.y())
        min.y() = pt.y();
    if (pt.x() > max.x())
        max.x() = pt.x();
    if (pt.y() > max.y())
        max.y() = pt.y();
    if (fabs(curv) > maxCurv)
        maxCurv = fabs(curv);
}

// given an index in the points array, computes the curvature of the trajectory
float Road::realTrajCurv(int i)
{
    if (i <= 0 || i >= points.size() - 1)
        return 0;
    Point3f prev, next, up;
    float cosTau, sinTau;
    prev = points[i].trjPt;
    prev -= points[i - 1].trjPt;
    next = points[i + 1].trjPt;
    next -= points[i].trjPt;
    if (prev.norm()*next.norm() == 0)
        return 0;
    cosTau = prev.scalarprod(next)/(prev.norm()*next.norm());
    cosTau = clamp(cosTau, -1, 1);
    up.normcrossprod(prev, next);
    if (up.z() > 0)
        sinTau = sqrt(1 - cosTau*cosTau);
    else
        sinTau = -sqrt(1 - cosTau*cosTau);
    if (isnan(sinTau))
        cout << "nan from cos " << cosTau << endl;
    return sinTau;
}

// Is the road almost flat at this index?
bool Road::isFlat(int pt)
{
    return fabs(points[pt].curv) <= almostFlat;
}

// Find the next anchor assuming that we do have the keyframes computed. 
void Road::findNextAnchorKF(int &kfStart, int &kfEnd)
{
    kfStart = kfEnd;
    if (kfEnd >= keyframes.size() - 1)
        return;
    kfEnd++;
    // we want to stop either when we reach the end, or when we find a flat stretch 
    // of length at least = flatLength or we find a non-flat stretch of length
    // at least = trajStep.
    while (kfEnd < keyframes.size() && (keyframes[kfEnd].length < flatLength ||
           ((keyframes[kfEnd].sign != 0) && (keyframes[kfEnd].length < curveLength))))
        kfEnd++;
    if (kfEnd > keyframes.size() - 1)
        kfEnd = keyframes.size() - 1;
}

// Find the center of the next stretch where the road is almost flat for a number
// of points equal to flatLength.
int Road::findNextAnchor(int start, bool &flat)
{
    unsigned int first = start, current = start;
    flat = true;
    // get out of the current flat patch first
    while (start < points.size() && isFlat(start))
        start++;
    while (current < points.size())
    {
        // find the beginning of the next one
        while (start < points.size() && !isFlat(start)) {
            start++;
            flat = false;
        }
        // then find the end of the patch
        current = start;
        while (current < points.size() && isFlat(current))
            current++;
        // if it's long enough, return it
        if (current - start >= flatLength)
            return (start + current - 1) / 2;
        else // look for another one
            start = current;
    }
    return points.size() - 1;
}

// Set the trajectory between start and end points with given density
void Road::setTrajectory(double traj[], int size, int startPt, int endPt, int step)
{
    int i, j = 0, k = 0;
    double trj = 0;

    for (i = startPt; i < endPt; i++)
    {
        if (j >= size - 1)
            trj = traj[size - 1];
        else
            trj = traj[j] * double(step - k) / step + traj[j + 1] * double(k) / step;
        points[i].traj = trj;
        k++;
        if (k >= step)
        {
            k = 0;
            j++;
        }
    }
}

// Set the trajectory between start and end points with given density
void Road::setTrajectoryKF(double traj[], int size, int startKF, int endKF, int interm)
{
    int i = 0, j = 0, k = 0, p1, p2, step;
    double t1, t2, trj = 0;

    for (k = startKF; k < endKF; k++)
    {
        p1 = keyframes[k].pt;
        step = int(ceil(double(keyframes[k + 1].pt - p1) / interm));
        while (i < size && p1 < keyframes[k + 1].pt)
        {
            t1 = traj[i];
            if (i < size - 1)
                t2 = traj[i + 1];
            else
                t2 = traj[size - 1];
            p2 = p1 + step;
            if (p2 > keyframes[k + 1].pt)
                p2 = keyframes[k + 1].pt;
            for (j = 0; j < step; j++)
            {
                trj = t1 * double(step - j) / step + t2 * double(j) / step;
                points[p1 + j].traj = trj;
            }
            i++;
            p1 = p2;
        }
    }
}

// Set the trajectory between start and end points with given density 
// with the trajectory coming in as a vector
void Road::setTrajectoryKF(vector<double> traj, int startKF, int endKF, int interm)
{
    int i = 0, j = 0, k = 0, p1, p2, p3, step;
    double t1, t2, trj = 0;

    int size = traj.size();
    for (k = startKF; k < endKF && k < keyframes.size(); k++)
    {
        p1 = keyframes[k].pt;
        if (k < keyframes.size() - 1)
            p3 = keyframes[k + 1].pt;
        else
            p3 = points.size() - 1;
        step = int(ceil(double(p3 - p1) / interm));
        while (i < size && p1 < p3)
        {
            t1 = traj[i];
            if (i < size - 1)
                t2 = traj[i + 1];
            else if (k < keyframes.size() - 1)
                t2 = traj[size - 1];
            else
                t2 = 0;
            p2 = p1 + step;
            if (p2 > p3)
                p2 = p3;
            for (j = 0; j < step; j++)
            {
                trj = t1 * double(step - j) / step + t2 * double(j) / step;
                points[p1 + j].traj = trj;
            }
            i++;
            p1 = p2;
        }
    }
}

// Set the trajectory between start and end points with given density
void Road::setTrajectory(vector<double> traj, int startPt, int endPt, int step)
{
    int i, j = 0, k = 0, size = traj.size();
    double trj = 0;

    for (i = startPt; i < endPt; i++)
    {
        if (j >= size - 1)
            trj = traj[size - 1];
        else
            trj = traj[j] * double(step - k) / step + traj[j + 1] * double(k) / step;
        points[i].traj = trj;
        k++;
        if (k >= step)
        {
            k = 0;
            j++;
        }
    }
}

// Calculate the real distance along the trajectory between the start and end points
double Road::sumDistance(int startPt, int endPt)
{
    double sum = 0;
    Point3f prevPt = points[startPt].trjPt, thisPt;
    for (unsigned int i = startPt+1; i < endPt && i < points.size(); i++)
    {
        thisPt = points[i].trjPt;
        sum += thisPt.distance(prevPt);
        prevPt = thisPt;
    }
    return sum;
}

// Find the maximum absolute value of the curvature between start and end points
double Road::findMaxCurv(int startPt, int endPt)
{
    double crv = 0; // it won't be smaller than this
    for (int i = startPt; i < endPt && i < points.size(); i++)
        if (fabs(realTrajCurv(i)) > crv)
            crv = fabs(realTrajCurv(i));
    return crv;
}

// Sum the curvature between start and end points
double Road::sumCurv(int startPt, int endPt)
{
    double crv = 0, rc = 0, crv1 = 0; // it won't be larger than this
    for (unsigned int i = startPt; i < endPt && i < points.size(); i++)
    {
        rc = realTrajCurv(i);
        crv += fabs(rc);
        if (isnan(crv))
            cout << "nan at point " << i << " real curv: " << rc << endl;
        crv1 = crv;
    }
    return crv;
}

// Scale the trajectory uniformly by a scale factor.
void Road::scaleTraj(float scaleFactor)
{
    // the norm, curvature, and trajectory should stay the same
    for (int i = 0; i < points.size(); i++) {
        points[i].pt *= scaleFactor;
        points[i].trjPt *= scaleFactor;
        points[i].dist *= scaleFactor;
    }
    min *= scaleFactor;
    max *= scaleFactor;
}

// Translate the trajectory uniformly by a vector.
void Road::translateTraj(Point3f vect)
{
    // the norm, curvature, distance, and trajectory should stay the same
    for (int i = 0; i < points.size(); i++) {
        points[i].pt += vect;
        points[i].trjPt += vect;
    }
    min += vect;
    max += vect;
}

// Set the starting point of the trajectory by moving it along x. 
void Road::setStartingX(float stx)
{
    float diff = stx - points[0].pt.x();
    points[0].pt[0] = stx;
    points[points.size() - 1].pt[0] += diff;
    // points[points.size() - 1].dist += diff; // doesn't actually change
    for (int i = 0; i < points.size()-1; i++) {
        points[i].dist -= diff;
    }
}