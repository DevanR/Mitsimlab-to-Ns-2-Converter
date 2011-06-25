#ifndef RN_ARC_HEADER
#define RN_ARC_HEADER

#include <iostream.h>

#include "WcsPoint.h"

const double BULGE_EPSILON = 1.0e-5;  // zero bulge 

class RN_Arc
{
   protected:

      // update arcAngle_, radius_, and center_ based on startPnt(),
      // endPnt(), and bulge().
	 
      WcsPoint startPnt_;
      WcsPoint endPnt_;
      double bulge_;

      double startAngle_;
      double endAngle_;
      double arcAngle_;
      WcsPoint center_;
      double radius_;
      double length_;

   public:

      RN_Arc(const WcsPoint& startpnt, const WcsPoint& endpnt,
			 double bulge);
      RN_Arc() : radius_(0.0), arcAngle_(0.0) { }

      virtual ~RN_Arc() { }

      // Create an arc that offset from reference arc (< 0 => right, >
      // 0 => left)

      RN_Arc(RN_Arc& arc, double offset) {
		 parallel(arc, offset);
      }
      RN_Arc(RN_Arc* arc, double offset) {
		 parallel(*arc, offset);
      }
      void parallel(RN_Arc& arc, double offset);
	 
      inline int isArc() {
		 return (bulge_ > BULGE_EPSILON || bulge_ < -BULGE_EPSILON);
      }

      inline WcsPoint& startPnt() { return startPnt_; }
      inline WcsPoint& endPnt() { return endPnt_; }
      inline void startPnt(WcsPoint& p) {startPnt_ = p;}
      inline void endPnt(WcsPoint& p) {endPnt_ = p;}

      // Middle point along the arc

      inline WcsPoint midPoint() {
		 return intermediatePoint(0.5);
      }

      // Returns a point on the arc with a fraction of distance wrt
      // end-point, i.e. fraction is zero if end-point and if
      // start-point.

      WcsPoint intermediatePoint(double fraction = .5);

      WcsPoint offsetPoint(double frac, double offset);
      WcsPoint nearestPoint(const WcsPoint& pnt, double epsilon);
      double distance(const WcsPoint& pnt);
	
      inline double length() { return length_; }
      inline double bulge() { return bulge_; }

      // bulge is zero for straight, for curve, bulge is ratio of dist
      // from chord to midpoint of arc to the half-length of arc.
	 
      // the following are only useful if bulge != 0
      // All angles are radian CCW, 0=3 O'clock.

      inline double startAngle() {
	 
		 // For a straight line, startAngle is the angle of the
		 // line. For a curve, startAngle is the angle of the line
		 // from the center to the point startPnt.

		 return startAngle_;
      }

      inline double endAngle() {

		 // For a straight line, endAngle is the angle of the
		 // line. For a curve, endAngle is the angle of the line from
		 // the center to the point endPnt. Note: arcAngle is zero for
		 // a straight line.

		 return endAngle_;
      }

      inline double arcAngle() {
		 // Negative if endAngle < startAngle
		 return arcAngle_;
      }
      double startTangentAngle(); // grade CCW 
      double endTangentAngle();   // grade CCW 
      double tangentAngle(double);
      WcsPoint& center() { return center_; }
      double radius() { return radius_; }
      void computeArc();
      
      ////////////////////////////////////////////////////////////////////
      WcsPoint getCenterPoint() { return center_; }
      WcsPoint getWcsPosition( float position );
      double getPositionRatio ( double position );
      void display();
      ///////////////////////////////////////////////////////////////////

      virtual int initArc(double x1, double y1, double b,
						  double x2, double y2);
     // virtual void arcprint(ostream &os);

      // Find the containing rectangle

      virtual float min_x();
      virtual float min_y();
      virtual float max_x();
      virtual float max_y();
};

#endif
