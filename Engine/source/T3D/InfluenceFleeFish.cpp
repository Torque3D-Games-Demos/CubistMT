#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceFleeFish.h"

bool  InfluenceFleeFish::update()
{
   if( !Parent::update() ){
      return false;
   }
   if(!mScaryObject){
      mFear = 0;
   }else{
      Point3F  rel = mFish->getPosition() - mScaryObject->getPosition();
      F32      distance = rel.magnitudeSafe();
      mFear = transitionCosine(distance, 0, mFearDistance, 1.0f, 0.0f);
      mDirection = rel;
      mDirection.normalize();
      if(mFear==0){
         mScaryObject = 0;
      }
   }
   return true;
}
void  InfluenceFleeFish::setScaryObject(ShapeBase *_scaryObject)
{
   mScaryObject = _scaryObject;
}
void  InfluenceFleeFish::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);

   mScaryObject = 0;
   mTurnSpeed = mFish->mDataBlock->fleeTurnScale;
   mMoveSpeed = mFish->mDataBlock->fleeSpeedScale;
   mFear = 0;
   mFearDistance = 1.0f;
   mAlarm.setInterval(0.1f, true);
}
F32  InfluenceFleeFish::getFear()
{
   return mFear;
}
void  InfluenceFleeFish::setFearDistance(F32 _fearDistance)
{
   mFearDistance = _fearDistance;
}
