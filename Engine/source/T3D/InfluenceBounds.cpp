#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceBounds.h"


F32   MOVE_BACK_TIME = 2.0f;
F32   UPDATE_INTERVAL = 0.1f;
bool  InfluenceBounds::update()
{
   if( !Parent::update() ){
      return false;
   }
   if( mFish->ignoreCollision() ){
      mOutsideBounds = false;
   }else{
      mOutsideBounds = false;
      switch(mFish->mActivity.activity){
      default:
         mOutsideBounds = false;
         break;
      case ActivitySwim:
         sphereLimitCheck();
         break;
      case ActivityNone:
         {
            Point3F  lo, hi;
            mFish->wanderGetRange(lo, hi);
            boxLimitCheck(lo, hi);
         }
         break;
      }
   }
   return true;
}
void  InfluenceBounds::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);

   mMoveSpeed = mFish->mDataBlock->wanderSpeedScaleLo;
   mTurnSpeed = mFish->mDataBlock->wanderTurnScale;
   mOutsideBounds = false;
   mTimeout = 0;
   mAlarm.setInterval(UPDATE_INTERVAL, true);
}
bool  InfluenceBounds::isOutsideBounds()
{
   return mOutsideBounds;
}
void  InfluenceBounds::sphereLimitCheck()
{
   Point3F scale = mFish->mActivity.shape->getScale();
   Point3F center = mFish->mActivity.shape->getPosition();
   boxLimitCheck(center-scale, center+scale);
}
void  InfluenceBounds::boxLimitCheck(Point3F lo, Point3F hi)
{
   Point3F  p = mFish->getPosition();

   Point3F  d = Point3F::Zero;
   bool  outside = false;
   //-------------------
   if(p.x>hi.x){
      outside = true;
      d.x = -1.0f; 
   }else if(p.x<lo.x){
      outside = true;
      d.x = +1.0f; 
   }
   //-------------------
   if(p.y>hi.y){
      outside = true;
      d.y = -1.0f; 
   }else if(p.y<lo.y){
      outside = true;
      d.y = +1.0f; 
   }
   //-------------------
   if(p.z>hi.z){
      outside = true;
      d.z = -1.0f; 
   }else if(p.z<lo.z){
      outside = true;
      d.z = +1.0f; 
   }
   if(outside ){
      mOutsideBounds = true;
      d.normalize();
      mDirection = d;
      mTimeout = MOVE_BACK_TIME;
   }else{
      if(mTimeout>0){
         mOutsideBounds = true;
         mTimeout -= UPDATE_INTERVAL;
      }else{
         mOutsideBounds = false;
      }
   }
}

