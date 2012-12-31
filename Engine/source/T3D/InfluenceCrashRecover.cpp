#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceCrashRecover.h"

const static   F32   COLLISION_CYCLE_TIME = 25.0f;

bool  InfluenceCrashRecover::update()
{
   if( !Parent::update() ){
      return false;
   }

//   F32   timeSinceStart = mFish->mGameTime - mStartTime;
//   F32   f = fmodf(timeSinceStart,COLLISION_CYCLE_TIME);

   switch(mPhase%4){
   case 0:
      mDirection = Point3F(-mStartDirection.x, -mStartDirection.y, 0);
      break;
   case 1:
      mDirection = Point3F(-mStartDirection.y, +mStartDirection.x, 0);
      break;
   case 2:
      mDirection = Point3F(+mStartDirection.y, -mStartDirection.x, 0);
      break;
   case 3:
      mDirection = Point3F(+mStartDirection.x, +mStartDirection.y, 0);
      break;
/*
   case 4:
      mDirection = Point3F(0, 0, +1);
      break;
   case 5:
      mDirection = Point3F(0, 0, -1);
      break;
      */
   }

   if(mFacingDirection)
   {
      F32   timeSince = mFish->mGameTime - mStartEscapeTime;
      if(timeSince>3.0f){
         mPhase++;
         mFacingDirection = false;
      }
   }
   else
   {
      F32   dot = mDot(mDirection,mFish->getHeading());
      if(dot>0.9f){
         mFacingDirection = true;
         mStartEscapeTime = mFish->mGameTime;
      }
   }
   mTurnSpeed = mFish->mDataBlock->wanderTurnScale;
   mMoveSpeed = mFish->mDataBlock->wanderSpeedScaleHi;
   if(recovered()){
      mCrashed = false;
   }
   return true;
}
void  InfluenceCrashRecover::trigger()
{
   if( recovered() ){   //dont be continually resetting recover code
      mCrashed = true;
      mStartTime = mFish->mGameTime;
      mStartPosition = mFish->getPosition();
      mStartDirection = mFish->getHeading();
      mStartDirection.z = 0;
      F32   len = mStartDirection.len();
      if(len){
         mStartDirection *= (1.0f/len);
      }else{
         mStartDirection = Point3F(1.0f, 0, 0);
      }
      mPhase = 0;
      mFacingDirection = false;
      mStartEscapeTime = 0;
   }
}
bool  InfluenceCrashRecover::recovered()
{
   return getStrength()==0;
}
F32  InfluenceCrashRecover::getStrength()
{
   Point3F  pos = mFish->getPosition();
   F32   unstuckDistance = mFish->mDataBlock->maxUnderwaterForwardSpeed * mMoveSpeed;  //distance can move in a second if not stuck
   F32   distanceMoved = (pos - mStartPosition).len();
   F32   strength = unstuckDistance>0?    (unstuckDistance - distanceMoved) / unstuckDistance : 0;
   return mClampF(strength, 0, 1);
}
void  InfluenceCrashRecover::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mAlarm.setInterval(TickSec);
}
