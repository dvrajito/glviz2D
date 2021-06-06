==================================================================
#  Project: glViz2D 
#  Author: Dana Vrajitoru 
#  Creative Commons, Attribution 
#  Updated: June 2021 
#  User instructions, compilation and execution guide. 
==================================================================
A simple visualization tool for roads. 

Uses OpenGL + GLUT for the GUI.

The road is shown as a ribbon.

A road is represented in a file as a list of pairs of distance from the start and curvature. See a couple of examples in the files alpine2CahP2.txt and curvEroadL.txt.

Added June 2021: representing a road as a set of centerline points.

===================================================================
#  Compilation
===================================================================

<> Needs General.h and General.cc from the GAD_traj project.

<> If you compile on Linux, comment out the constant M_PI in General.h.

<> If you compile in Visual Studio:
- Add the src folder from the GAD_traj project to the list of additional include directories in Project - Properties - C/C++ - General.  
- Add the include folder in the local freeglut installation to the include directories in properties.
- Add the lib folder in the local freeglut install to the Additional Library Directories in Properties - Linker - General. 