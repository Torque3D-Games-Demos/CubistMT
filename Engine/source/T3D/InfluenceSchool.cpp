#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceSchool.h"

const static   bool  SCHOOL_WANDER = true;

void  InfluenceSchool::updateTargetPosition()
{
   Point3F  oldTargetPosition = mTargetPosition;

   const MatrixF  &m = mTarget->getTransform();

   mTarget->getWorldBox().getCenter( &mTargetPosition );

   Point3F  rightVector = m.getRightVector();
   Point3F  upVector = m.getUpVector();
   Point3F  forwardVector = mCross( rightVector, upVector );
   mNodeDirection = forwardVector;
   
   mTargetPosition = mTargetPosition  + (upVector*mOffset.z)
                                    + (rightVector*mOffset.y)
                                    + (forwardVector*mOffset.x);

   if( mTargetPositionSet ){
      Point3F  delta = mTargetPosition - oldTargetPosition;
      mTargetVelocity = delta *(1.0f/TickSec);
      mTargetSpeed = mTargetVelocity.magnitudeSafe();
   }
   mTargetPositionSet = true;
}
// --------------------------------------------------------------------------------------------
void  InfluenceSchool::setTarget(Point3F &_offset, ShapeBase *_target,
                                    F32 _nearDistance,
                                    F32 _farDistance)
{
   mOffset = _offset;
   mTarget = _target;
   mNearDistance = _nearDistance;
   mFarDistance = _farDistance;
}
// --------------------------------------------------------------------------------------------
Point3F InfluenceSchool::getGravityDirection()
{
   Point3F  separation = mTargetPosition - mFish->getPosition();
   separation.normalize();  

   if( SCHOOL_WANDER ){
      F32   angle = (getTimeSeconds()*M_2PI)/wanderPeriod;
      angle = mFmod(angle+wanderPhase, M_2PI_F);
      if( wanderSizeUp!=0 ){       
         separation.z += wanderSizeUp*mSin(angle);         
      }
      if( wanderSizeSide!=0 ){       
         separation.x += wanderSizeSide*mCos(angle);         
         separation.y += wanderSizeSide*mSin(angle);         
      }
      separation.normalize();   
   }
   return separation;
}
void  InfluenceSchool::updateSpeed()
{
   if( !mTargetPositionSet ){
      mSpeed = 0;
   }else{
      Point3F  v =  mTargetPosition - mFish->getPosition();
      F32   distanceToTarget = v.magnitudeSafe();
      if(mReachedDestination){
         if( distanceToTarget > mFarDistance ){
            mReachedDestination = false;
         }
      }else{
         if( distanceToTarget < mNearDistance ){
            mReachedDestination = true;
         }
      }
      F32   nodeSpeed = mTargetVelocity.magnitudeSafe();
      F32   maxSpeed = nodeSpeed * 8.0f;
      if( mReachedDestination ){
         mSpeed = nodeSpeed*0.9;     
      }else if(distanceToTarget >= mFarDistance){
         mSpeed = maxSpeed;
      }else{
         F32   f = (distanceToTarget - mNearDistance) / (mFarDistance - mNearDistance);
         mSpeed = nodeSpeed + (maxSpeed-nodeSpeed)*f;
      }
   }
}
void  InfluenceSchool::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mTargetPosition = Point3F::Zero;
   mNodeDirection = Point3F::Zero;
   mTargetVelocity = Point3F::Zero;
   mOffset = Point3F::Zero;
   mTargetPositionSet = false;
   mTargetSpeed = 0;
   mReachedDestination = false;
   mTurnSpeed = 1.0f;
   mMoveSpeed = 1.0f;

   mAlarm.setInterval(0.1f);

   FishPlayerData *fd = (FishPlayerData *)mFish->mDataBlock;

   wanderPeriod = gRandGen.randF(fd->schoolWanderPeriodLo, fd->schoolWanderPeriodHi);
   wanderSizeSide = gRandGen.randF(fd->schoolWanderSizeSideLo, fd->schoolWanderSizeSideHi);
   wanderSizeUp = gRandGen.randF(fd->schoolWanderSizeUpLo, fd->schoolWanderSizeUpHi);
   wanderPhase = gRandGen.randF(0.0f, M_2PI_F);
}
bool  InfluenceSchool::update()
{
   if( !Parent::update() ){
      return false;
   }
   if(!active())return false;

   updateTargetPosition(); 
   updateSpeed();

   mDirection = getGravityDirection();

   mMoveSpeed = mSpeed/((FishPlayerData *)mFish->getDataBlock())->maxUnderwaterForwardSpeed;
   if(mMoveSpeed>2.0) mMoveSpeed = 2.0;
   return true;
}

