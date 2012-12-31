//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------


#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"

F32   WEIGHT_TRANSITION_TIME = 1.0f;   //how quickly weights respond to change

float Influence::getTimeSeconds()
{
   return ((float)Sim::getCurrentTime()) / 1000.0f;
}
F32 Influence::transitionCosine(F32  x, F32 x0, F32 x1, F32 y0, F32 y1)
{
   if(x<x0){
      return y0;
   }else if(x>x1){
      return y1;
   }else{
      F32   x10 = x1-x0;
      if(x10==0){
         return y1;
      }else{
         F32 fraction = (x - x0)/x10;
         F32 angle = M_PI_F*fraction;
         return (1.0f - mCos(angle))*0.5f*(y1-y0) + y0;         
      }
   }
}
//*************************************************************************
void  Influence::pitchYawToDirection(Point3F &_d, const F32 &_pitch, const F32 &_yaw)
{
   mSinCos(_yaw, _d.x, _d.y);
   F32   s, c;
   mSinCos(_pitch, s, c);
   _d.x *= c;
   _d.y *= c;
   _d.z = s;
}
//*************************************************************************
void  Influence::directionToPitchYaw(const Point3F &_d, F32 &_pitch, F32 &_yaw)
{
   F32 o = _d.z;
   F32 a = mSqrt(_d.x*_d.x + _d.y*_d.y);
   _pitch = mAtan2(o,a);
   _yaw = mAtan2(_d.x, _d.y);
}
//*************************************************************************
// Create vector random rotated by a small amount from input vector
//*************************************************************************
Point3F        Influence::randomVectorRotation(Point3F oldHeading)
{
   F32   yaw = mAtan2(oldHeading.x, oldHeading.y);
   F32   pitch = mAtan2(oldHeading.z, sqrtf(oldHeading.x*oldHeading.x + oldHeading.y*oldHeading.y));
   directionToPitchYaw(oldHeading, pitch, yaw);

   yaw += gRandGen.randF(  -mFish->mDataBlock->wanderYawAngleChange, 
                           +mFish->mDataBlock->wanderYawAngleChange);
   pitch += gRandGen.randF(-mFish->mDataBlock->wanderPitchAngleChange, 
                           +mFish->mDataBlock->wanderPitchAngleChange);

   FishPlayerData  *d = (FishPlayerData *)mFish->getDataBlock();
   pitch = mClampF(pitch, d->minLookAngle, d->maxLookAngle);
   yaw = mFmod(yaw+M_2PI, M_2PI_F);

   Point3F  heading;
   pitchYawToDirection(heading, pitch, yaw);
   return heading;
}
void  Influence::init(AIFishPlayer *_fish, const char *_name)
{
   mDirection = Point3F::Zero;
   mWeight = 0;
   mTargetWeight = 0;
   mTurnSpeed = 0;
   mMoveSpeed = 0;
   mName = "?";
   mFish = _fish;
   mUpdateInterval = 1000;
   mAlarm.setInterval(1.0f);
   mName = _name;
}
//returns true if influence is due for an update
bool  Influence::update()
{
   //update weight
   if(mTargetWeight>mWeight){
      mWeight += (TickSec/WEIGHT_TRANSITION_TIME);
   }else if(mTargetWeight<mWeight){
      mWeight -= (TickSec/WEIGHT_TRANSITION_TIME);
   }
   mWeight = mClampF(mWeight, 0, 1);

   //update timer
   mAlarm.update();
   return mAlarm.isRinging();
}
void  Influence::setWeightTarget(F32 _weight)
{
   mTargetWeight = _weight;
}
void  Influence::setWeightInstant(F32 _weight)
{
   mTargetWeight = _weight;
   mWeight = _weight;
}
