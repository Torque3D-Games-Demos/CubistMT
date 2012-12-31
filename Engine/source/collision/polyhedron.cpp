//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/console.h"
#include "collision/polyhedron.h"
#include "math/mMath.h"


//----------------------------------------------------------------------------

void Polyhedron::buildBox(const MatrixF& transform,const Box3F& box)
{
   // Box is assumed to be axis aligned in the source space.
   // Transform into geometry space
   Point3F xvec,yvec,zvec,min;
   transform.getColumn(0,&xvec);
   xvec *= box.len_x();
   transform.getColumn(1,&yvec);
   yvec *= box.len_y();
   transform.getColumn(2,&zvec);
   zvec *= box.len_z();
   transform.mulP(box.minExtents,&min);

   // Initial vertices
   pointList.setSize(8);
   pointList[0] = min;
   pointList[1] = min + yvec;
   pointList[2] = min + xvec + yvec;
   pointList[3] = min + xvec;
   pointList[4] = pointList[0] + zvec;
   pointList[5] = pointList[1] + zvec;
   pointList[6] = pointList[2] + zvec;
   pointList[7] = pointList[3] + zvec;

   // Initial faces
   planeList.setSize(6);
   planeList[0].set(pointList[0],xvec);
   planeList[0].invert();
   planeList[1].set(pointList[2],yvec);
   planeList[2].set(pointList[2],xvec);
   planeList[3].set(pointList[0],yvec);
   planeList[3].invert();
   planeList[4].set(pointList[0],zvec);
   planeList[4].invert();
   planeList[5].set(pointList[4],zvec);

   // The edges are constructed so that the vertices
   // are oriented clockwise for face[0]
   edgeList.setSize(12);
   Edge* edge = edgeList.begin();

   for (int i = 0; i < 4; i++) {
      S32 n = (i == 3)? 0: i + 1;
      S32 p = (i == 0)? 3: i - 1;
      edge->vertex[0] = i;
      edge->vertex[1] = n;
      edge->face[0] = i;
      edge->face[1] = 4;
      edge++;
      edge->vertex[0] = 4 + i;
      edge->vertex[1] = 4 + n;
      edge->face[0] = 5;
      edge->face[1] = i;
      edge++;
      edge->vertex[0] = i;
      edge->vertex[1] = 4 + i;
      edge->face[0] = p;
      edge->face[1] = i;
      edge++;
   }
}

// start ds
void Polyhedron::buildRect(const Point3F &center, const Point3F &forward, F32 width, F32 height)
{
   Point3F left(-mCross(forward,Point3F(0, 0, 1)));
	if (left.isZero())
	{
		left.set(-1,0,0);
	}
	else
	{
		left.normalize();
	}
   left *= (width/2);
   Point3F up(-mCross(left, forward));
   up.normalize();
   up *= (height/2);
//start ds
   Point3F back = forward;
   back.normalize();
   back *= 0.5;
// end ds
   
   pointList.setSize(8);
   pointList[5].set(center + left + up);
   pointList[6].set(center - left + up);
   pointList[2].set(center - left - up);
   pointList[1].set(center + left - up);
//start ds
/*
   pointList[4].set(pointList[5] - forward);
   pointList[7].set(pointList[6] - forward);
   pointList[0].set(pointList[1] - forward);
   pointList[3].set(pointList[2] - forward);
*/
   pointList[4].set(pointList[5] + back);
   pointList[7].set(pointList[6] + back);
   pointList[0].set(pointList[1] + back);
   pointList[3].set(pointList[2] + back);
// end ds

   // Initial faces
   planeList.setSize(6);
   planeList[0].set(pointList[0],left);
   planeList[1].set(pointList[2],forward);
   planeList[2].set(pointList[2],left);
   planeList[2].invert();
   planeList[3].set(pointList[0],forward);
   planeList[3].invert();
   planeList[4].set(pointList[0],up);
   planeList[4].invert();
   planeList[5].set(pointList[4],up);

   // The edges are constructed so that the vertices
   // are oriented clockwise for face[0]
   edgeList.setSize(12);
   Edge* edge = edgeList.begin();
   for (int i = 0; i < 4; i++) {
      S32 n = (i == 3)? 0: i + 1;
      S32 p = (i == 0)? 3: i - 1;
      edge->vertex[0] = i;
      edge->vertex[1] = n;
      edge->face[0] = i;
      edge->face[1] = 4;
      edge++;
      edge->vertex[0] = 4 + i;
      edge->vertex[1] = 4 + n;
      edge->face[0] = 5;
      edge->face[1] = i;
      edge++;
      edge->vertex[0] = i;
      edge->vertex[1] = 4 + i;
      edge->face[0] = p;
      edge->face[1] = i;
      edge++;
   }
}

Box3F *Polyhedron::getBoundingBox()
{
   Point3F *point = pointList.begin();
   static Box3F box;
   for(U32 i = 0; i < pointList.size(); i++)
   {
      if (i == 0)
      {
         box.minExtents.set(*point);
         box.maxExtents.set(*point);
      }
      else
      {
         box.minExtents.setMin(*point);
         box.maxExtents.setMax(*point);
      }
      point++;
   }
   return &box;
}
// end ds

