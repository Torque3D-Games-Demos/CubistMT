//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// start ds - Brendan Ledwich 2005-11-9

#include "T3D/nodeInterpolator.h"
#include "math/mMath.h"
#include "math/mMathFn.h"
//#include "core/realComp.h"
#include "math/mMatrix.h"
#include "T3D/gameBase/moveManager.h"
//#include <stdio.h>

/**
 * constructor
 */
NodeInterpolator::NodeInterpolator() {}

/**
 * destructor
 */
NodeInterpolator::~NodeInterpolator()
{
	mPoints.clear();
}

/**
 * Gets the point at the given index
 * @param pointNum index of the point in question
 */
const Point3F * NodeInterpolator::getPoint( U32 pointNum )
{
	return (getNumPoints()>pointNum && pointNum>=0)?&mPoints[pointNum]:NULL;
}
/**
 * Sets a point at the given index to the point given
 * @param point New value for the given point
 * @param pointNum index of the given point
 */
void NodeInterpolator::setPoint( Point3F &point, U32 pointNum )
{
	if (getNumPoints()>pointNum && pointNum>=0) mPoints[pointNum] = point;
}
/**
 * Adds a point to the end of the spline
 * @param point New point to be added
 */
void NodeInterpolator::addPoint( Point3F &point )
{
	mPoints.push_back( point );
}
/**
 * Clears existing points and enters new points
 * @param pts List of points to be added
 * @param num Number of points to be added
 */
void NodeInterpolator::submitPoints( Point3F *pts, U32 num )
{
	mPoints.clear();
	for (int i=0; i<num; i++)
	{
		mPoints.push_back( pts[i] );
	}
	init();
}
/**
 * Returns the section for a given offset
 * @param tval Position along interpolation (0 = start, 1 = end)
 
U32 NodeInterpolator::getSection( const F32 tval )
{
	const U32 numPoints = getNumPoints();
	if (!transitionPoints && numPoints>2)
	{
		transitionPoints = new F32[numPoints-2];
		F32 totalLen = 0.0;
		for (U32 i=0; i<numPoints-2; i++)
		{
			transitionPoints[i] = (mPoints[i+1]-mPoints[i]).len();
			totalLen += transitionPoints[i];
		}
		totalLen += (mPoints[numPoints-1]-mPoints[numPoints-2]).len();
		
		for (U32 i=0; i<numPoints-2; i++)
		{
			transitionPoints[i] /= totalLen;
		}
	}
	for (U32 i=0; tval>=transitionPoints[i]; i++){}
	return i;
}*/

#define POLY3EVAL(t, c0, c1, c2, c3) ((c0) + t*((c1) + t*((c2) + t*(c3))))
#define BEZIER3_INTERPOLATION(t, p0, tripleP1, tripleP2, p3) POLY3EVAL(t, BEZIER3_COEF4(p0,tripleP1,tripleP2,p3),BEZIER3_COEF3(p0,tripleP1,tripleP2,p3),BEZIER3_COEF2(p0,tripleP1,tripleP2,p3),BEZIER3_COEF1(p0,tripleP1,tripleP2,p3))
#define BEZIER3_COEF1(p0, tripleP1, tripleP2, p3) (-p0+tripleP1+-tripleP2+p3)
#define BEZIER3_COEF2(p0, tripleP1, tripleP2, p3) (p0+p0+p0-tripleP1-tripleP1-tripleP2)
#define BEZIER3_COEF3(p0, tripleP1, tripleP2, p3) (-p0-p0-p0+tripleP1)
#define BEZIER3_COEF4(p0, tripleP1, tripleP2, p3) (p0)

#define BEZIER3_TAN(t, p0, tripleP1, tripleP2, p3) POLY2EVAL(t, BEZIER3_TAN_COEF3(p0,tripleP1,tripleP2,p3), BEZIER3_TAN_COEF2(p0,tripleP1,tripleP2,p3), BEZIER3_TAN_COEF1(p0,tripleP1,tripleP2,p3))
#define BEZIER3_TAN_COEF1(p0, tripleP1, tripleP2, p3) (BEZIER3_COEF1(p0,tripleP1,tripleP2,p3)+BEZIER3_COEF1(p0,tripleP1,tripleP2,p3)+BEZIER3_COEF1(p0,tripleP1,tripleP2,p3))
#define BEZIER3_TAN_COEF2(p0, tripleP1, tripleP2, p3) (BEZIER3_COEF2(p0,tripleP1,tripleP2,p3)+BEZIER3_COEF2(p0,tripleP1,tripleP2,p3))
#define BEZIER3_TAN_COEF3(p0, tripleP1, tripleP2, p3) BEZIER3_COEF3(p0,tripleP1,tripleP2,p3)

#define BEZIER3_CURVINESS(t, p0, tripleP1, tripleP2, p3) POLY1EVAL(t,BEZIER3_CURVINESS_COEF2(p0,tripleP1,tripleP2,p3),BEZIER3_CURVINESS_COEF1(p0,tripleP1,tripleP2,p3))
#define BEZIER3_CURVINESS_COEF1(p0, tripleP1, tripleP2, p3) (BEZIER3_TAN_COEF1(p0,tripleP1,tripleP2,p3)+BEZIER3_TAN_COEF1(p0,tripleP1,tripleP2,p3))
#define BEZIER3_CURVINESS_COEF2(p0, tripleP1, tripleP2, p3) BEZIER3_TAN_COEF2(p0,tripleP1,tripleP2,p3)

#define POLY2EVAL(t, c0, c1, c2) ((c0) + t*((c1) + t*(c2)))
#define BEZIER2_INTERPOLATION(t, p0, doubleP1, p2) POLY2EVAL(t,BEZIER2_COEF3(p0,doubleP1,p2),BEZIER2_COEF2(p0,doubleP1,p2),BEZIER2_COEF1(p0,doubleP1,p2))
#define BEZIER2_COEF1(p0, doubleP1, p2) (p0-doubleP1+p2)
#define BEZIER2_COEF2(p0, doubleP1, p2) (-p0-p0+doubleP1)
#define BEZIER2_COEF3(p0, doubleP1, p2) (p0)

#define BEZIER2_TAN(t, p0, doubleP1, p2) POLY1EVAL(t, BEZIER2_TAN_COEF2(p0,doubleP1,p2), BEZIER2_TAN_COEF1(p0,doubleP1,p2))
#define BEZIER2_TAN_COEF1(p0, doubleP1, p2) (BEZIER2_COEF1(p0,doubleP1,p2)+BEZIER2_COEF1(p0,doubleP1,p2))
#define BEZIER2_TAN_COEF2(p0, doubleP1, p2) BEZIER2_COEF2(p0,doubleP1,p2)

#define BEZIER2_CURVINESS(t, p0, doubleP1, p2) BEZIER2_TAN_COEF1(p0,doubleP1,p2)

#define POLY1EVAL(t, c0, c1) ((c0) + t*(c1))
#define LINEAR_INTERPOLATION(t, p0, p1) POLY1EVAL(t,p0,p1-p0)

#define LINEAR_TAN(t, p0, p1) (p1-p0)

#define LINEAR_CURVINESS 0

inline void BezierNodeInterpolator::calcBezier2Matrix()
{
	/*
	 * | 1 -2  1| |p0|
	 * |-2  2  0|x|p1|
	 * | 1  0  0| |p2|
	 */
   if (bezVector) delete [] bezVector;
	Point3F doubleP1 = mPoints[1]+mPoints[1];
	bezVector = new Point3F[3];
	bezVector[0] = BEZIER2_COEF1(mPoints[0],doubleP1,mPoints[2]);
	bezVector[1] = BEZIER2_COEF2(mPoints[0],doubleP1,mPoints[2]);
	bezVector[2] = BEZIER2_COEF3(mPoints[0],doubleP1,mPoints[2]);
}

inline void BezierNodeInterpolator::calcBezier3Matrix()
{
	/*
	 * |-1  3 -3  1| |p0|
	 * | 3 -6  3  0|x|p1|
	 * |-3  3  0  0| |p2|
	 * | 1  0  0  0| |p3|
	 */
   if (bezVector) delete [] bezVector;
	Point3F tripleP1 = mPoints[1]*3;
	Point3F tripleP2 = mPoints[2]*3;
	bezVector = new Point3F[4];
	bezVector[0] = BEZIER3_COEF1(mPoints[0],tripleP1,tripleP2,mPoints[3]);
	bezVector[1] = BEZIER3_COEF2(mPoints[0],tripleP1,tripleP2,mPoints[3]);
	bezVector[2] = BEZIER3_COEF3(mPoints[0],tripleP1,tripleP2,mPoints[3]);
	bezVector[3] = BEZIER3_COEF4(mPoints[0],tripleP1,tripleP2,mPoints[3]);
}

inline Point3F BezierNodeInterpolator::getBezier2Point(const F32 tval)
{
	return POLY2EVAL(tval, bezVector[2], bezVector[1], bezVector[0]);
}

inline Point3F BezierNodeInterpolator::getBezier3Point(const F32 tval)
{
	return POLY3EVAL(tval, bezVector[3], bezVector[2], bezVector[1], bezVector[0]);
}

F32 BezierNodeInterpolator::linearClosestTval(const Point3F & point, Point3F * solPoint)
{
	Point3F diff = mPoints[1] - mPoints[0];
	F32 difMag = diff.magnitudeSafe();
	F32 t = -mDot(mPoints[0]-point,diff)/(difMag*difMag);
	if (t<0) t = 0;
	if (t>1) t = 1;
	*solPoint = mPoints[0] + diff*t;
	return t;
}

F32 BezierNodeInterpolator::bezier2ClosestTval(const Point3F & point, Point3F * solPoint)
{
	Point3F doubleP1 = mPoints[1]+mPoints[1];
	F32 min = -1.0;
	F32 results[3];

	if (solution >= 0.0)
	{
		min = (progSolPoint-point).lenSquared();
		//printf("starting with sol=%f\n",solution);
	}
	else 
	{
		//printf("new path\n");
	}
	coeffs[2] = 2*mDot(bezVector[0],mPoints[0]-point)+bezVector[1].lenSquared();
	coeffs[3] = mDot(bezVector[1],mPoints[0]-point);
	U32 numResults = mSolveCubic(coeffs[0],coeffs[1],coeffs[2],coeffs[3],results);

	F32 testPointLenSqrd = (mPoints[0]-point).lenSquared();
	if (solution < 0)
	{
		//load start point as solution
		min = testPointLenSqrd;
		progSolPoint = mPoints[0];
		solution = 0.0;
		//printf("first sol=%f solPoint=(%f,%f,%f) distance=%f\n", solution, solPoint->x,solPoint->y,solPoint->z,min);
	}
	//test mid points from results
	if (numResults > 0)
	{
		for (U32 i = 0; i<numResults; i++)
		{
			if (results[i] < 0.f || results[i] > 1.f)
			{
				//outside path
				continue;
			}
			Point3F testPoint = getBezier2Point(results[i]);
			testPointLenSqrd = (testPoint-point).lenSquared();
			if (results[i] > solution && (min < 0 || testPointLenSqrd <= min))
			{
				//closest so far
				min = testPointLenSqrd;
				progSolPoint = testPoint;
				solution = results[i];
				//printf("closest so far sol=%f solPoint=(%f,%f,%f) distance=%f\n", solution, solPoint->x,solPoint->y,solPoint->z,min);
			}
		}
	}
	testPointLenSqrd = (mPoints[2]-point).lenSquared();
	// test endPoint
	if ((min < 0 || testPointLenSqrd <= min) && solution < 1.0)
	{
		//end point is closest
		min = testPointLenSqrd;
		progSolPoint = mPoints[2];
		solution = 1.f;
		//printf("closest so far sol=%f solPoint=(%f,%f,%f) distance=%f\n", solution, solPoint->x,solPoint->y,solPoint->z,min);
	}
	solPoint->set(progSolPoint);
	return solution;
}

F32 BezierNodeInterpolator::bezier3ClosestTval(const Point3F & point, Point3F * solPoint)
{
   static Point3F result;
   *solPoint = result;
	//check distance from current waypoint
	//check
	return 0;
}

/**
 * Calculate Point on interpolation
 * Overrides nodeInterpolator::getPoint
 * @param tval Position along interpolation (0 = start, 1 = end)
 * @return Point on interpolation, default zero vector
 */
const Point3F BezierNodeInterpolator::getPointAtT( const F32 tval )
{
   static Point3F result;
	switch(getNumPoints())
	{
	case 0:
		return result;
	case 1:
		return mPoints[0];
	case 2:
		result = LINEAR_INTERPOLATION(tval,mPoints[0],mPoints[1]);
      return result;
	case 3:
      result = getBezier2Point(tval);
		return result;
	default:
      result = getBezier3Point(tval);
		return result;
	}
}

/**
 * Calculate direction vector to follow interpolation
 * Overrides nodeInterpolator::getDirectionVector
 * @param fromPos The origin of the direction vector
 * @param fromDirVec The orientation of the origin vector
 * @param dist The distance to the next point (result should aim to be of length dis)
 * @return direction vector, default zero vector
 */
Point3F BezierNodeInterpolator::getWaypoint
			( const Point3F & fromPos,  const Point3F & fromDirVec, const F32 dist )
{
	Point3F finalPos = fromPos + (fromDirVec*dist);
	switch(getNumPoints())
	{
	case 0:
		return finalPos;
	case 1:
		return mPoints[0];
	case 2:
	{
		static Point3F result;
		//printf("finalPos=(%f,%f,%f)\n", finalPos.x,finalPos.y,finalPos.z);
		F32 t = linearClosestTval(finalPos, &result);
		t = (t > 0.9)?1.0:t+0.1;
		result = getPointAtT(t);
		//printf("result=(%f,%f,%f) t=%f\n", result->x,result->y,result->z, t);
		return result;
	}
	case 3:
	{
		static Point3F result;
		//printf("finalPos=(%f,%f,%f)\n", finalPos.x,finalPos.y,finalPos.z);
		F32 t = bezier2ClosestTval(finalPos, &result);
		t = (t > 0.95)?1.0:t+0.05;
		result = getPointAtT(t);
      //Con::printf("result=(%f,%f,%f) t=%f\n", result->x,result->y,result->z, t);
		return result;
	}
	case 4:
	{
		Point3F diff = progSolPoint-finalPos;
      diff.z = 0.0;
      //Con::printf("diff.lenSquared() = %f",diff.lenSquared());
		if ( diff.lenSquared() < 9.0)
		{
			wayPoint = getNextCubicCurveWayPoint(cubicCurve,wayPoint,progSolPoint);
         //Con::printf("wayPoint %d is (%f,%f,%f)\n",wayPoint, progSolPoint.x,progSolPoint.y,progSolPoint.z);
		}
		return progSolPoint;
	}
	default:
		return mPoints[getNumPoints()];
	}	
}

F32 BezierNodeInterpolator::getArea(const Point3F &p0,const Point3F &p1,const Point3F &p2,const Point3F &p3)
{
	Point3F p02 = p2-p0;
	Point3F p13 = p3-p1;
	Point3F crossResult;
	mCross(p02,p13,&crossResult);
	return crossResult.len()/2; //optimizations possible here return areaSquared
}

U32 BezierNodeInterpolator::addSplit(CubicCurve* curve)
{
	Point3F halfLeft = (*curve->pp0+(curve->p1))/2;
	Point3F halfRight = (*curve->pp3+(curve->p2))/2;
	Point3F halfMiddle = (curve->p2+curve->p2)/2;
	Point3F halfLeftMiddle = (halfLeft+halfMiddle)/2;
	Point3F halfRightMiddle = (halfRight+halfMiddle)/2;
	curve->split = new CubicCurveSplit();
	curve->split->splitPoint.set((halfLeftMiddle+halfRightMiddle)/2);
	//printf("splitPoint=(%f,%f,%f)\n",curve->split->splitPoint.x,curve->split->splitPoint.y,curve->split->splitPoint.z);
	curve->numSplits = 1;
	F32 area = getArea(*curve->pp0, halfLeft,halfLeftMiddle,curve->split->splitPoint);
	//printf("leftarea=%f\n", area);
	if (area > 0.5 && ((curve->split->splitPoint)-(*curve->pp0)).len() > 0.5)
	{
		//printf("splittingLeft\n");
		curve->split->left = new CubicCurve(curve->pp0,	halfLeft, halfLeftMiddle, &curve->split->splitPoint);		
		curve->numSplits += addSplit(curve->split->left);
	}
	else
	{
		curve->split->left = NULL;
	}
	area = getArea(curve->split->splitPoint, halfRightMiddle,halfRight,*curve->pp3);
	//printf("rightarea=%f\n", area);
	if (area > 0.5 && ((*curve->pp3)-(curve->split->splitPoint)).len() > 0.5)
	{
		//printf("splittingright\n");
		curve->split->right = new CubicCurve(&curve->split->splitPoint, halfRightMiddle, halfRight, curve->pp3);
		curve->numSplits += addSplit(curve->split->right);
	}
	else
	{
		curve->split->right = NULL;
	}
	//printf("returning numSplits = %d\n",curve->numSplits);
	return curve->numSplits;
}

U32 BezierNodeInterpolator::getNextCubicCurveWayPoint(const CubicCurve *curve, const U32 current, Point3F &resultPoint)
{
	if (current >= curve->numSplits)
	{
		//printf("lastPoint\n");
		resultPoint.set(*(curve->pp3));
		return curve->numSplits+1;
	}
	U32 numLeftSplits = (curve->split->left)?curve->split->left->numSplits:0;
	if (current == numLeftSplits)
	{
		//printf("split\n");
		resultPoint.set(curve->split->splitPoint);
		return current + 1;		
	}
	if (current < numLeftSplits)
	{
		//printf("left\n");
		return getNextCubicCurveWayPoint(curve->split->left,current, resultPoint);
	}
	if ((curve->split->right))
	{
		//printf("right\n");
		return numLeftSplits+getNextCubicCurveWayPoint(curve->split->right,current-numLeftSplits,resultPoint);
	}
   //printf("error, no waypoint found\n");
   return 0;
}

void BezierNodeInterpolator::init()
{
	if (cubicCurve) 
	{
		deleteCubicCurve(cubicCurve);
		cubicCurve = NULL;
	}
	switch (getNumPoints())
	{
	case 3:
		calcBezier2Matrix();
		solution = -1.0;
		coeffs[0] = 2*((bezVector[0]).lenSquared());
		coeffs[1] = 3*mDot(bezVector[0],bezVector[1]);
		break;
	case 4:
		calcBezier3Matrix();
		wayPoint = 0;
		progSolPoint.set(mPoints[0]);
		//printf("mPoints[0]=(%f,%f,%f)\n",mPoints[0].x,mPoints[0].y,mPoints[0].z);
		//printf("mPoints[1]=(%f,%f,%f)\n",mPoints[1].x,mPoints[1].y,mPoints[1].z);
		//printf("mPoints[2]=(%f,%f,%f)\n",mPoints[2].x,mPoints[2].y,mPoints[2].z);
		//printf("mPoints[3]=(%f,%f,%f)\n",mPoints[3].x,mPoints[3].y,mPoints[3].z);
		cubicCurve = new CubicCurve(&mPoints[0],mPoints[1],mPoints[2],&mPoints[3]);
      {
         U32 splits = addSplit(cubicCurve);
         //Con::printf("%d splits created",splits);
      }
		break;
	default:
		break;
	}
}

void BezierNodeInterpolator::deleteCubicCurve(CubicCurve *curve)
{
	if (curve->split)
	{
		if (curve->split->left) 
		{
			deleteCubicCurve(curve->split->left);
			curve->split->left = NULL;
		}
		if (curve->split->right) 
		{
			deleteCubicCurve(curve->split->right);
			curve->split->right = NULL;
		}
		delete curve->split;
	}
	delete curve;
}

/**
 * destructor
 */
BezierNodeInterpolator::~BezierNodeInterpolator()
{
	if (cubicCurve) 
	{
		deleteCubicCurve(cubicCurve);
		cubicCurve = NULL;
	}
   if (bezVector)
   {
      delete [] bezVector;
   }
}

/// constructor
BezierNodeInterpolator::BezierNodeInterpolator() 
{
	cubicCurve = NULL;
   bezVector = NULL;
}
// end ds