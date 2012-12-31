#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceFeed.h"

const static F32  MUNCH_DISTANCE = 0.5;
bool  InfluenceFeed::update()
{
   if( !Parent::update() ){
      return false;
   }
   if(!mFood){
      mMunching = false;
   }else{
      mDirection = mFood->getPosition() - mFish->getPosition();
      F32   distance = mDirection.len();
      if( distance < MUNCH_DISTANCE ){
         //leave direction as what it was when approaching
         mMunching = true;
         mTurnSpeed = 0;
         mMoveSpeed = 0;
      }else{
         mMunching = false;
         mDirection *= (1.0f/distance);
         mTurnSpeed = 1.0f;
         mMoveSpeed = 0.1f;
      }
   }
   return true;
}
void  InfluenceFeed::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mAlarm.setInterval(TickSec);
   mFood = 0;
   mMunching = false;
}
void  InfluenceFeed::setFood(StaticShape *_food)
{
   mFood = _food;
   update();
}
bool  InfluenceFeed::isEating()
{
   return mMunching;
}
