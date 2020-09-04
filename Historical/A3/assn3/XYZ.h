#ifndef XYZ_H
#define XYZ_H

struct XYZ {
	double x, y, z;
}; 

typedef struct XYZ XYZ;

double dotProduct(XYZ lhs, XYZ rhs);
 
XYZ crossProduct(XYZ lhs, XYZ rhs);

XYZ multiply(double factor, XYZ v1); 

XYZ subtract(XYZ v1, XYZ v2);
XYZ minus(double c, XYZ v2);

XYZ plus(XYZ v1, XYZ v2);
XYZ cPlus(double c, XYZ v2);

double squaredLength(XYZ v);
double dist(XYZ v1, XYZ v2);
 
XYZ normalize(XYZ v);

int equals(XYZ v1, XYZ v2);

#endif
     
