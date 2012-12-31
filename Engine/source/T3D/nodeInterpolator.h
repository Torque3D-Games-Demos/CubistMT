//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// start ds - Brendan Ledwich 2005-11-9

#ifndef _NODEINTERPOLATOR_H_
#define _NODEINTERPOLATOR_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif
#ifndef _MMATH_H_
#include "math/mMath.h"
#endif
#ifndef _TVECTOR_H_
#include "core/util/tVector.h"
#endif

class NodeInterpolator {

public:

protected:
	/// Vector of points in the spline
	Vector <Point3F> mPoints;

public:
   
		/// constructor
		NodeInterpolator();

		/// destructor
		virtual ~NodeInterpolator();
   
		/// Gets the number of points in the spline
		U32               getNumPoints(){ return mPoints.size(); }
		/// Gets the point at the given index
		/// @param pointNum index of the point in question
		const Point3F *   getPoint( U32 pointNum );
		/// Sets a point at the given index to the point given
		/// @param point New value for the given point
		/// @param pointNum index of the given point
		void              setPoint( Point3F &point, U32 pointNum );
		/// Adds a point to the end of the spline
		/// @param point New point to be added
		void              addPoint( Point3F &point );
		/// Clears existing points and enters new points
		/// @param pts List of points to be added
		/// @param num Number of points to be added
		void              submitPoints( Point3F *pts, U32 num );
		/// Calculate Point on interpolation
		/// @param tval Position along interpolation (0 = start, 1 = end)
		virtual const Point3F	getPointAtT( const F32 tval ) { return Point3F(); }
		/// Calculate direction vector to follow interpolation
		/// @param fromPos The origin of the direction vector
		/// @param fromDirVec The orientation of the origin vector
		/// @param dist The distance to the next point (result should aim to be of length dis)
		/// @return direction vector, default zero vector
		/// @return Point on interpolation, default zero vector
		virtual Point3F	getWaypoint
					( const Point3F & fromPos,  const Point3F & fromDirVec, const F32 dist )
								{ return Point3F(); }
		
		virtual void		init() {}

};

typedef struct CUBIC_CURVE 
{
	Point3F *pp0;
	Point3F p1;
	Point3F p2;
	Point3F *pp3;
	U32 numSplits;
	struct CUBIC_CURVE_SPLIT *split;	
	
	CUBIC_CURVE(Point3F *iPp0,Point3F iP1, Point3F iP2, Point3F *iPp3)
	{
		pp0 = iPp0;
		p1.set(iP1);
		p2.set(iP2);
		pp3 = iPp3;
		numSplits = 0;
	}
} CubicCurve;

typedef struct CUBIC_CURVE_SPLIT
{
	Point3F splitPoint;
	CubicCurve *left;
	CubicCurve *right;
	
	CUBIC_CURVE_SPLIT()
	{
		left = NULL;
		right = NULL;
	}
} CubicCurveSplit;

class BezierNodeInterpolator : public NodeInterpolator {
private:
		Point3F *bezVector;
		F32 coeffs[4];
		F32 solution;
		Point3F progSolPoint;
		CubicCurve *cubicCurve;
		U32 wayPoint;
		
public:
		/// constructor
		BezierNodeInterpolator();
		/// destructor
		virtual		~BezierNodeInterpolator();

		/// Calculate Point on interpolation
		/// Overrides nodeInterpolator::getPoint
		/// @param tval Position along interpolation (0 = start, 1 = end)
		/// @return Point on interpolation, default zero vector
		virtual		const Point3F	getPointAtT( const F32 tval );
		/// Calculate direction vector to follow interpolation
		/// Overrides nodeInterpolator::getDirectionVector
		/// @param fromPos The origin of the direction vector
		/// @param fromDirVec The orientation of the origin vector
		/// @param dist The distance to the next point (result should aim to be of length dis)
		/// @return direction vector, default zero vector
		virtual		Point3F	getWaypoint
					( const Point3F & fromPos,  const Point3F & fromDirVec, const F32 dist );

		virtual		void		init();
public:
		F32 bezier3ClosestTval(const Point3F & point, Point3F * solPoint);
		F32 bezier2ClosestTval(const Point3F & point, Point3F * solPoint);
		F32 linearClosestTval(const Point3F & point, Point3F * solPoint);
		
		inline void calcBezier2Matrix();
		inline void calcBezier3Matrix();
		inline Point3F getBezier2Point(const F32 tval);
		inline Point3F getBezier3Point(const F32 tval);

private:
		F32 getArea(const Point3F &p0,const Point3F &p1,const Point3F &p2,const Point3F &p3);
		U32 addSplit(CubicCurve* curve);
		U32 getNextCubicCurveWayPoint(const CubicCurve *curve, const U32 current, Point3F &resultPoint);
		void deleteCubicCurve(CubicCurve *curve);
};

#endif

// end ds
