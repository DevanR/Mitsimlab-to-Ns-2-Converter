//-*-c++-*------------------------------------------------------------
// FILE: WcsPoint.h
// DATE: Thu Oct 19 09:45:28 1995
//--------------------------------------------------------------------

#ifndef WCSPOINT_HEADER
#define WCSPOINT_HEADER

#include <iostream.h>

const double POINT_EPSILON = 0.1; // equal test 

class WcsPoint
{
   protected:

	  double x_;
	  double y_;

   public:

      WcsPoint(void) { x_ = 0; y_ = 0; }
      WcsPoint(double x, double y) { x_ = x; y_ = y; }
      WcsPoint(const WcsPoint& p) {
		 x_ = p.x_; y_ = p.y_;
	  }
      WcsPoint(const WcsPoint&, const WcsPoint&, double);
      virtual ~WcsPoint() { }

      inline double x() const { return x_; }
      inline double y() const { return y_; }
      inline double east() { return x_; }
      inline double north() { return y_; }

      inline void set(double x, double y) {
		 x_ = x;  y_ = y;
      }

      // Logical comparison
 
      int operator == (const WcsPoint& p) { return equal(p);}
      int operator != (const WcsPoint& p) { return !equal(p);}
      int equal(const WcsPoint&, double epsilon = POINT_EPSILON);

      // Operators

      int operator == (const WcsPoint &p) const {
		 return (x_ == p.x_ && y_ == p.y_);
      }

      int operator != (const WcsPoint &p) const {
		 return x_ != p.x_ || y_ != p.y_;
      }

      WcsPoint& operator = (const WcsPoint &p) {
		 x_ = p.x_; y_ = p.y_;
		 return *this;
      }

      // provides easy access to the points elements 

      double operator [] (const int i) const {
		 return i ? y_ : x_;
	  }

      // Distance to another point 

      double distance(const WcsPoint&) const;

      // (distance to another point) ^ 2 
  
      double distance_squared(const WcsPoint&) const;

      // Interpolate a point between this point and p with ratio r,
      // where ratio r corresponses to the distance from p

      WcsPoint intermediate(const WcsPoint &p, double r) const;

      // a point with a distance of 'offset' and an angle of 'alpha'
      // wrt point 'p'. The angle must be in radian.

      WcsPoint bearing(double offset, double alpha) const;

      double angle(const WcsPoint&) const;
	
      // Called by RN_Parser

      virtual int init(double x, double y);

      // Prints in north-east form

      //virtual void print(ostream &os); // in float
      //virtual void PrintLonLat(ostream &os); // in integer
};

ostream& operator << (ostream &os, const WcsPoint &p);

extern WcsPoint theWcsDummyPoint;

#endif
