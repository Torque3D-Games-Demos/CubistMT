#ifndef _InfluenceSchool_h_
#define _InfluenceSchool_h_

#include "influence.h"

class   InfluenceSchool : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   void           setTarget(  Point3F &_offset, ShapeBase *_target,
                              F32 _NearDistance,F32 _FarDistance);
private:
   Point3F        mTargetPosition;
   Point3F        mNodeDirection;
   Point3F        mTargetVelocity;
   Point3F        mOffset;
   F32            mTargetSpeed;
   F32            mFarDistance;
   F32            mNearDistance;
   F32            mSpeed;
   ShapeBase      *mTarget;
   bool           mTargetPositionSet;
   bool           mReachedDestination;

   F32            wanderPeriod;
   F32            wanderSizeUp;
   F32            wanderSizeSide;
   F32            wanderPhase;

   //---      
   void           updateTargetPosition();
   Point3F        getGravityDirection();
   void           updateSpeed();
};
#endif
