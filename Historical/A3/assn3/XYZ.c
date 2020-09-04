#include "XYZ.h"
#include <math.h>
#include <stdio.h>

XYZ crossProduct(XYZ lhs, XYZ rhs) {
	XYZ result; 
	
	result.x = lhs.y * rhs.z  -  lhs.z * rhs.y;
	result.y = lhs.z * rhs.x  -  lhs.x * rhs.z;
	result.z = lhs.x * rhs.y  -  lhs.y * rhs.x;

	return result;
}

double dotProduct(XYZ lhs, XYZ rhs) {
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

XYZ subtract(XYZ v1, XYZ v2) {
	XYZ result; 
	
	result.x = v1.x - v2.x; 
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	
	return result;
}

XYZ plus(XYZ v1, XYZ v2) {
	XYZ result;
	
	result.x = v1.x + v2.x; 
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	
	return result;
}

XYZ multiply(double factor, XYZ v1) {
	XYZ result; 
	
	result.x = factor * (double)(v1.x); 
	result.y = factor * (double)(v1.y);
	result.z = factor * (double)(v1.z);
	
	return result; 
}

XYZ cPlus(double c, XYZ v) {
	XYZ result; 
	
	result.x = c + v.x; 
	result.y = c + v.y;
	result.z = c + v.z;
	
	return result; 
}

XYZ minus(double c, XYZ v) {
	XYZ result; 
	
	result.x = v.x - c; 
	result.y = v.y - c;
	result.z = v.z - c;

	return result; 
}


double dist(XYZ v1, XYZ v2) {
	return (sqrt( ((v1.x - v2.x)*(v1.x - v2.x)) + ((v1.y - v2.y)*(v1.y - v2.y)) + ((v1.z - v2.z)*(v1.z - v2.z))));
}

XYZ normalize(XYZ v) {
	XYZ result;
	
	double length;
	length = sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
	result.x = v.x/length; 
	result.y = v.y/length; 
	result.z = v.z/length; 
	
	return result; 
}

double squaredLength(XYZ v) { 
	return (v.x*v.x + v.y*v.y + v.z*v.z); 
}

double length(XYZ v) { 
	return sqrt(squaredLength(v));
}

int equals(XYZ v1, XYZ v2) {
	double epsilon = 0.0000001;
	
	return ((fabs(v1.x - v2.x) < epsilon) && (fabs(v1.y - v2.y) < epsilon) && (fabs(v1.z - v2.z) < epsilon));
}

