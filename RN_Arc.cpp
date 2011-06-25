#include <stdlib.h>

#include "Math.h"
//#include <Tools/ToolKit.h>
#include "RN_Arc.h"

RN_Arc::RN_Arc(const WcsPoint& start_point,
			   const WcsPoint& end_point,
			   double bulge)
{
   startPnt_ = start_point;
   endPnt_ = end_point;
   bulge_ = bulge;
   computeArc();
}


// Compute an arc parallel to arc. Offset is positive for the left
// side arc and negative for the right side arc.

void
RN_Arc::parallel(RN_Arc& arc, double offset)
{
   bulge_ = arc.bulge();

   if (isArc()) {		// Curve
      center_ = arc.center();
      arcAngle_ = arc.arcAngle_;
      if (bulge_ < 0.0) offset = -offset;
      radius_ = arc.radius() + offset;
      radius_ = (radius_ > 0) ? (radius_) : (0);
      length_ = radius_ * Abs(arcAngle_);
      startAngle_ = arc.startAngle_;
      endAngle_ = arc.endAngle_;
      startPnt_ = center_.bearing(radius_, startAngle_);
      endPnt_ = center_.bearing(radius_, endAngle_);
   } else {			// Straight line
      startAngle_ = endAngle_ = arc.startAngle_;
      double alpha;
      if (offset > 0.0) {
		 alpha = startAngle_ + HALF_PI;
      } else {
		 alpha = startAngle_ - HALF_PI;
		 offset = -offset;
      }
      startPnt_ = arc.startPnt().bearing(offset, alpha);
      endPnt_ = arc.endPnt().bearing(offset, alpha);
      length_ = arc.length();
   }
}


// Find the point on arc with a distance of 'frac*length()' from the
// end-point.

WcsPoint
RN_Arc::intermediatePoint(double frac)
{
   if (isArc()) {		// curve
      double alpha = startAngle() + arcAngle_ * (1.0 - frac);
      return center_.bearing(radius(), alpha);
   } else {			// straight line
      WcsPoint pnt(startPnt_, endPnt_, frac);
      return (pnt);
   }
}

/*
 * Returns a point with a distance of 'offset' from the arc and a
 * distance of 'frac*length()' from the end-point.  'offset' is
 * positive if the new point is on the left side and negative if it is
 * on the right side of the arc.
 */

WcsPoint
RN_Arc::offsetPoint(double frac, double offset)
{
   double alpha;
   if (isArc()) {		// curve
      alpha = startAngle_ + arcAngle_ * (1.0 - frac);
      offset = (bulge_ > 0) ? (offset) : (-offset);
      return center_.bearing(radius_ + offset, alpha);
   } else {			// straight line
      WcsPoint pnt(startPnt_, endPnt_, frac);
      if (offset > POINT_EPSILON) {
		 alpha = startAngle_ + HALF_PI;
		 return pnt.bearing(offset, alpha);
      } else if (offset < -POINT_EPSILON) {
		 alpha = startAngle_ - HALF_PI;
		 return pnt.bearing(-offset, alpha);
      } else {
		 return (pnt);
      }
   }
}


double
RN_Arc::distance(const WcsPoint& p)
{
   WcsPoint pnt = nearestPoint(p, POINT_EPSILON);
   return p.distance(pnt);
}


WcsPoint
RN_Arc::nearestPoint(const WcsPoint& p, double epsilon)
{
   WcsPoint pnt;

   if (isArc()) {		// curved

      double alpha1, alpha2;
      double gamma = center_.angle(p);

      // Let the arc be counter clockwise

      if (arcAngle() > 0) {
		 alpha1 = center_.angle(startPnt_);
		 alpha2 = center_.angle(endPnt_);
      } else {
		 alpha1 = center_.angle(endPnt_);
		 alpha2 = center_.angle(startPnt_);
      }

      int inside = alpha1 < gamma && gamma < alpha2;
      if (alpha1 < alpha2 && inside || alpha1 > alpha2 && !inside) {
		 pnt = center().bearing(radius(), gamma);
      } else {
		 double sdis = p.distance(startPnt_);
		 double edis = p.distance(endPnt_);
		 pnt = (sdis < edis) ? (startPnt_) : (endPnt_);
      }

   } else {			// straight line
      double xmin = Min(startPnt_.x(), endPnt_.x());
      double xmax = Max(startPnt_.x(), endPnt_.x());
      double ymin = Min(startPnt_.y(), endPnt_.y());
      double ymax = Max(startPnt_.y(), endPnt_.y());

      if (p.x() >= xmin - epsilon &&
		  p.x() <= xmax + epsilon &&
		  p.y() >= ymin - epsilon &&
		  p.y() <= ymax + epsilon) {
		 double beta = startPnt_.angle(p);
		 double d = startPnt_.distance(p);
		 double offset = d * sin(beta - startAngle_);
		 double alpha;
		 if (offset > 0) {
			alpha = startAngle_ - HALF_PI;
		 } else {
			offset = - offset;
			alpha = startAngle_ + HALF_PI;
		 }
		 pnt = p.bearing(offset, alpha);
      } else {
		 double sdis = p.distance(startPnt_);
		 double edis = p.distance(endPnt_);
		 pnt = (sdis < edis) ? (startPnt_) : (endPnt_);
      }
   }
   return (pnt);
}


double
RN_Arc::startTangentAngle() 
{
   const double alpha = TWO_PI - HALF_PI;
   if (bulge_ > BULGE_EPSILON) {
      return ((startAngle_ > HALF_PI) ?
			  (startAngle_ - HALF_PI) :
			  (startAngle_ + alpha));
   } else if (bulge_ < -BULGE_EPSILON) {
      return ((startAngle_ < alpha) ?
			  (startAngle_ + HALF_PI) :
			  (startAngle_ - alpha));
   } else {
      return startAngle_;
   }
}


double
RN_Arc::endTangentAngle() 
{
   const double alpha = TWO_PI - HALF_PI;
   if (bulge_ > BULGE_EPSILON) {
      return ((endAngle_ > HALF_PI) ?
			  (endAngle_ - HALF_PI) :
			  (endAngle_ + alpha));
   } else if (bulge_ < -BULGE_EPSILON) {
      return ((endAngle_ < alpha) ?
			  (endAngle_ + HALF_PI) :
			  (endAngle_ - alpha));
   } else {
      return endAngle_;
   }
}

double
RN_Arc::tangentAngle(double a) 
{
   double alpha;
   if (bulge_ > BULGE_EPSILON) {
      alpha = startAngle_ + arcAngle_ * (1.0 - a) - HALF_PI;
   } else if (bulge_ < -BULGE_EPSILON) {
      alpha = startAngle_ + arcAngle_ * (1.0 - a) + HALF_PI;
   } else {
      alpha = startAngle_;
   }
   if (alpha < 0.0) alpha += TWO_PI;
   else if (alpha >= TWO_PI) alpha -= TWO_PI;
   return alpha;
}


/*
 *-------------------------------------------------------------------
 * Set variable in an arc. Called by RN_Parser
 * 
 * CAUTION: The definition of bulge in the simulator is opposite to
 * that in network database. Here a positive bulge means the arc
 * is out on the left side, a negative bulge means the arc is out
 * on the right side.
 *-------------------------------------------------------------------
 */

int
RN_Arc::initArc(double x1, double y1, double b, double x2, double y2)
{
   startPnt_.init(x1, y1);
   endPnt_.init(x2, y2);
   bulge_ = - b;
   return 0;
}

/*
void
RN_Arc::arcprint(ostream &os)
{
   os << indent << indent << indent << "{";
   startPnt_.print(os);
   os << endc << (-bulge_) << endc;
   endPnt_.print(os);
   os << "}" << endl;
}
*/

/*
 * Update startAngle, arcAngle_, radius_, center_ and length_ based on
 * startPnt(), endPnt(), and bulge().
 */

void
RN_Arc::computeArc()
{
   if (isArc() && endPnt_ != startPnt_) {
      /*
       * Let S be the start-point (spt), V be the half-chord point, X be
       * the point half-way along the arc, and C be the center. Then the
       * bulge is defined as:
       *    bulge = |XV| / |SV|
       * Bulge is negative if the arc bumps out on the right side of the
       * chord, and positive if it bumps out on the left side of the
       * chord.
       */
      double offset;		// |CV|
      double alpha;		// angle VC
      double gamma = startPnt_.angle(endPnt_);  // angle SV
      double omega = atan(Abs(bulge_)); // angle XSV
      WcsPoint pnt(startPnt_, endPnt_, 0.5);  // V: half-chord point
      double half_chord = startPnt_.distance(pnt);    // |SV|
      double height = Abs(bulge_ * half_chord);   // |XV|
      radius_ = 0.5 * (half_chord * half_chord / height +
					   height);
      arcAngle_ = 4.0 * ((bulge_ > 0) ? (-omega) : (omega));
      offset = (Abs(arcAngle_) < ONE_PI) ?
		 (radius_ - height) :
		 (height - radius_);
      alpha = (bulge_ * (Abs(bulge_) - 1) > 0) ?
		 (gamma + HALF_PI) :
		 (gamma - HALF_PI);
      center_ = pnt.bearing(offset, alpha);
      length_ = radius_ * Abs(arcAngle_);
      startAngle_ = center_.angle(startPnt_);
      endAngle_ = startAngle_ + arcAngle_;

      if (endAngle_ < 0.0) endAngle_ += TWO_PI;
      else if (endAngle_ > TWO_PI) endAngle_ -= TWO_PI;

   } else {			// straight line
      radius_ = 0.0;
      arcAngle_ = 0.0;
      center_ = midPoint();
      length_ = startPnt_.distance(endPnt_);
      startAngle_ = endAngle_ = startPnt_.angle(endPnt_);
   }
}

/*////////////////////////////////////////////////////////////////////////
Let the center of the circle be (h,k), the angle of the arc be A, the
radius of the circle be r, and the starting point of the arc be (a,b).  
Let angle B be defined by

   cos(B) = (a-h)/r,
   sin(B) = (b-k)/r.

Then there are two ending points (c,d) for the arc, corresponding to
clockwise and counterclockwise directions around the circle. They
are given by

   c = h + r*cos(B-A),
   d = k + r*sin(B-A),

   c = h + r*cos(B+A),
   d = k + r*sin(B+A),

respectively.
*///////////////////////////////////////////////////////////////////////////

double
RN_Arc::getPositionRatio( double position )
{
  double position_ratio;
  double length_m;
  
  //error correction, length_ is in feet whereas trajectory.out is in mks system
  // converting arc's length to meters
    length_m = length_*0.3048;
  
    position_ratio = (length_ - position) / length_ ; 
    return 1-position_ratio; 
                      
}

WcsPoint
RN_Arc::getWcsPosition( float position )
{
   bool clockwise;
   double position_ratio = 0;
   double position_angle = 0;
   double x = 0;
   double y = 0;
   double B1 = 0; //angle
   double B2 = 0; //angle
   double a = 0;
   double b = 0;
   WcsPoint WcsPosition;
   
   if( bulge_ < 0 )
    clockwise = true;
   else
    clockwise = false;
    
   if(position == 0)
    return startPnt_;
   if(position == length_)
    return endPnt_;
   else
    
    position_ratio = (length_ - position) / length_ ;
    
   if( bulge_ == 0) //straight line between points
    {
     //insert code for straight lines 
    }
   else //arc between points
    {
  
      position_angle = position_ratio * arcAngle_ ;
       
      ////////////////////////////////////////////////////////////////////////
      
      B1 = acos( ( (startPnt_.x() - center_.x()) / radius_ ) ) ;
      
      //B1 = (2*ONE_PI) - B1;
      
      B2 = asin( ( ( startPnt_.y() - center_.y() ) / radius_ ) );
      
      //B2 = ONE_PI - B2;
      
      /*
      if (a1 > 180 && a2 < 180) {
        // counter clockwise and cross 0 line
        b1 = a1 ; b2 = a2 ;
    } else if (a1 < 180 && a2 > 180) {
        // clockwise and cross 0 line
        b1 = a2 ; b2 = a1 ;
    } else if (a1 < a2) {
        // counter clockwise 
        b1 = a1 ; b2 = a2 ;
    } else {
        // clockwise
        b1 = a2 ; b2 = a1 ;
    }
      */
      
      ////////////////////////////////////////////////////////////////////////
      
      if(clockwise == true)
      {
       x = center_.x() + radius_ * cos( (B1 - position_angle) ) ;
       y = center_.y() + radius_ * sin( (B2 - position_angle) ) ;
      }
      else
      {  
       x = center_.x() + radius_ * cos( (B1 + position_angle) ) ;
       y = center_.y() + radius_ * sin( (B2 + position_angle) ) ;      
      }
   
      WcsPosition.set( x, y );
      
      }
   
   return WcsPosition;
   
}

void RN_Arc::display()
{
      cout<<" Start X = "<< startPnt_.x()<<endl;
      cout<<" Start Y = "<< startPnt_.y()<<endl;
      cout<<" End X = "<< endPnt_.x()<<endl;
      cout<<" End Y = "<< endPnt_.y()<<endl;
     
      cout<<"Bulge = "<< bulge_<<endl;

      //double startAngle_;
      //double endAngle_;
      //double arcAngle_;
      //WcsPoint center_;
      //double radius_;
      //double length_;
}

/////////////////////////////////////////////////////////////////////////////

// Find the containing rectangle

float RN_Arc::min_x()
{
   double x = Min(startPnt_.x(), endPnt_.x());

   if (!isArc()) return x;

   x -= center_.x();
   double alpha1, alpha2;

   // Let the arc be counter clockwise

   if (arcAngle_ > 0) {
      alpha1 = startAngle_;
      alpha2 = endAngle_;
   } else {
      alpha1 = endAngle_;
      alpha2 = startAngle_;
   }

   int pass = alpha1 < ONE_PI && ONE_PI < alpha2;
   if (alpha1 < alpha2 && pass || alpha1 > alpha2 && !pass) {
      x = Min(-radius_, x);
   }

   return (x += center_.x());
}

float RN_Arc::min_y()
{
   const double beta = 3.0 * HALF_PI;
   double y = Min(startPnt_.y(), endPnt_.y());

   if (!isArc()) return y;

   y -= center_.y();
   double alpha1, alpha2;

   // Let the arc be counter clockwise

   if (arcAngle_ > 0) {
      alpha1 = startAngle_;
      alpha2 = endAngle_;
   } else {
      alpha1 = endAngle_;
      alpha2 = startAngle_;
   }

   int pass = alpha1 < beta && beta < alpha2;
   if (alpha1 < alpha2 && pass || alpha1 > alpha2 && !pass) {
      y = Min(-radius_, y);
   }

   return (y += center_.y());
}


float RN_Arc::max_x()
{
   double x = Max(startPnt_.x(), endPnt_.x());

   if (!isArc()) return x;

   x -= center_.x();
   double alpha1, alpha2;

   // Let the arc be counter clockwise

   if (arcAngle_ > 0) {
      alpha1 = startAngle_;
      alpha2 = endAngle_;
   } else {
      alpha1 = endAngle_;
      alpha2 = startAngle_;
   }

   if (alpha1 > alpha2) {
      x = Max(radius_, x);
   }

   return (x += center_.x());
}


float RN_Arc::max_y()
{
   double y = Max(startPnt_.y(), endPnt_.y());

   if (!isArc()) return y;

   y -= center_.y();
   double alpha1, alpha2;

   // Let the arc be counter clockwise

   if (arcAngle_ > 0) {
      alpha1 = startAngle_;
      alpha2 = endAngle_;
   } else {
      alpha1 = endAngle_;
      alpha2 = startAngle_;
   }

   int pass = alpha1 < HALF_PI && HALF_PI < alpha2;
   if (alpha1 < alpha2 && pass || alpha1 > alpha2 && !pass) {
      y = Max(radius_, y);
   }

   return (y += center_.y());
}

