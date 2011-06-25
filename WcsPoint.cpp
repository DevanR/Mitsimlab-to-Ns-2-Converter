//-*-c++-*------------------------------------------------------------
// WcsPoint.C
//
// Qi Yang
// Copyright (C) 1997
// Massachusetts Institue of Technology
// All Rights Reserved
//
// This file contains the class implementation for the class WcsPoint
//--------------------------------------------------------------------


#include <iostream.h>
#include <iomanip.h>

#include "Math.h"

#include "WcsPoint.h"
//#include "RoadNetwork.h"

WcsPoint theWcsDummyPoint;

ostream& operator << (ostream &os, const WcsPoint &p) 
{
   return os << setw(8) << setprecision(8) << p.x() << " "
			 << setw(8) << setprecision(8) << p.y();
}

WcsPoint::WcsPoint(const WcsPoint& spt, const WcsPoint& ept, double r)
{
   *this = spt.intermediate(ept, r);
}

int
WcsPoint::init(double x, double y)
{
   x_ = x; y_ = y;
   return 0;
}

/*
void
WcsPoint::print(ostream &os)
{
   WcsPoint p = theNetwork->worldSpace().databasePoint(*this);
   os << p.x() << ' ' << p.y();
}

void
WcsPoint::PrintLonLat(ostream &os)
{
   WcsPoint p = theNetwork->worldSpace().databasePoint(*this);
   os << Round(p.x()) << ' ' << Round(p.y()) ;
}
*/

// Calculate a point with a distance of 'offset' and an angle of
// 'alpha' wrt point 'p'. The angle must be in radian.


WcsPoint
WcsPoint::bearing(double offset, double alpha) const
{
   return WcsPoint(x_ + offset * cos(alpha),
		   y_ + offset * sin(alpha));
}


/*
 * Interpolate a point between this and p with ratio r.  Notice that r
 * corresponses to the distance from p.
 */

WcsPoint
WcsPoint::intermediate(const WcsPoint& p, double r) const
{
   return WcsPoint(r * x_ + (1.0 - r) * p.x_,
		   r * y_ + (1.0 - r) * p.y_);
}

double
WcsPoint::distance_squared(const WcsPoint& p) const
{
   double n_diff = p.y_ - y_;
   double e_diff = p.x_ - x_;
   return (n_diff * n_diff + e_diff * e_diff);
}

int
WcsPoint::equal(const WcsPoint& p, double epsilon)
{
   return ((fabs(y_ - p.y_) < epsilon)
	   && (fabs(x_ - p.x_) < epsilon));
}


/*
 * Compute the angle (in radian) of a line from this point to
 * point 'pnt'.
 */

double
WcsPoint::angle(const WcsPoint& pnt) const
{
   double dx, dy, dis, alpha;
	
   dx = pnt.x_ - x_;
   dy = pnt.y_ - y_;
   dis = dx * dx + dy * dy;
	
   if (dis > 1.0E-10) {
      alpha = acos(dx / sqrt(dis));
      if (dy < 0.0) alpha = TWO_PI - alpha;
   } else {
      alpha = 0.0;
   }

   return alpha;
}

double
WcsPoint::distance(const WcsPoint& p) const
{
   return sqrt(distance_squared(p));
}
