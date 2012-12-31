#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceSwish.h"


static   const F32 SWISH_BOX_FADE_OUT_TIME = 1.0f; //how long trigger effects continue when trigger is gone
static   const F32 SWISH_SPEED  = 0.4f;   //how much each finger effects swish

static   const F32 INTEREST_SPEED  = 0.2f;   //how much each finger effects swish
static   const F32 INTEREST_SPEED_FADE_RATE  = 1.0f;   //how much each finger effects swish

void  InfluenceSwish::onEnter(GameBase *_trigger)
{
   Effector *e = findEffector(_trigger);
   if( e ){
      //already effected by this trigger, mark mFish as inside
      e->insideTrigger = true;
   }else{
      //start using new mFinger if space
      if( mFingerCount==MAX_FINGERS ){
         return;
      }
      e = &mFinger[mFingerCount++];
      e->insideTrigger = true;
      e->exitTime = 0;
      e->trigger = (Trigger *)_trigger;
      e->position = _trigger->getPosition();
      e->lastPosition = _trigger->getPosition();
      e->speedWeight = 1.0f;
   }
}
void  InfluenceSwish::onExit(GameBase *_trigger)
{
   if( Effector *e = findEffector(_trigger) )
   {
      e->insideTrigger = false;
      e->exitTime = mFish->getTimeSeconds();
   }
}
bool InfluenceSwish::update()
{
   if( !Parent::update() ){
      return false;
   }
   if( !mFingerCount ){
      return true;
   }
   //Con::printf("--------");
   //---remove dead triggers----------------------------------
   int   iTo = 0;
   for(int iFrom=0; iFrom<mFingerCount; iFrom++){
      if( !mFinger[iFrom].insideTrigger){
         F32 time = mFish->getTimeSeconds() - mFinger[iFrom].exitTime;
         if(time>=SWISH_BOX_FADE_OUT_TIME){
            continue;
         }
      }
      mFinger[iTo++] = mFinger[iFrom];
   }
   mFingerCount = iTo;
   if( mFingerCount==0 ){
      return true;
   }
   // weight->fade off with distance
   //       ->fade off with time not moving
   //---for each mFinger, get weight, target position------------------
   F32   dt = mAlarm.getInterval();
   for(int i=0; i<mFingerCount; i++){
      Effector *e = &mFinger[i];
      e->weight = 1.0;

      F32   speed;
      F32   lostWeight;
      if(!e->insideTrigger){
         F32 time = mFish->getTimeSeconds() - e->exitTime;
         e->position = e->lastPosition;
         if( time > SWISH_BOX_FADE_OUT_TIME ){
            lostWeight = 0;         
         }else{
            lostWeight = (SWISH_BOX_FADE_OUT_TIME - time) /  SWISH_BOX_FADE_OUT_TIME;    
         }
         speed = 0;
         e->speedWeight -= dt*INTEREST_SPEED_FADE_RATE;
      }else{
         //project trigger onto plane going through mFish with normal facing away from camera
         lostWeight = 1.0f;
         Trigger  *trigger = e->trigger;
         F32 distance = mDot(mFish->getPosition() - trigger->getPosition(), trigger->getDirection() );
         Point3F  lastPosition = e->position;
         e->position = trigger->getPosition() + trigger->getDirection()*distance;
         speed = (e->position - lastPosition).len() / dt;
      }
      if( speed<INTEREST_SPEED){
         e->speedWeight -= (dt*INTEREST_SPEED_FADE_RATE);
      }else{
         e->speedWeight += (dt*INTEREST_SPEED_FADE_RATE);
      }
      e->speedWeight = mClampF(e->speedWeight, 0.0, 1.0);

      F32   distanceToTrigger = (e->position - mFish->getPosition()).len();
      //weight goes from 1 at closest to 0.2 when swishEffectDistance away
      F32   distanceWeight = transitionCosine(distanceToTrigger, 0, mFishPlayerData->swishEffectDistance, 1.0f, 0.2f);

//      e->weight = mClampF(distanceWeight*e->speedWeight*lostWeight, 0.0, 1.0);
      e->weight = mClampF(distanceWeight*lostWeight, 0.0, 1.0);
   }
   //---add up effectors to get new target velocity--------------------------
   Point3F  velocity = Point3F::Zero;
   F32   sumWeight=0;
   for(int i=0; i<mFingerCount; i++){
      Effector *e = &mFinger[i];
      
      if( e->insideTrigger ){
         e->directionToFinger = e->position - mFish->getPosition();
         e->directionToFinger.normalize();
      }
      Point3F dPosition = e->position - e->lastPosition;  

      F32   speed = SWISH_SPEED;
      velocity += e->directionToFinger*speed*e->weight;      

      e->lastPosition = e->position;
      sumWeight += e->weight;
   }
   mStrength = sumWeight;
   mStrength = mClampF(mStrength, 0.0f, 1.0f);

   F32 speed = velocity.len();
   if( speed ){
      mDirection = velocity*(1.0f/speed);      
   }else{
      mDirection = Point3F::Zero;
   }
   mMoveSpeed = speed/mFishPlayerData->maxUnderwaterForwardSpeed;
   mMoveSpeed = mClampF( mMoveSpeed, 0, mFishPlayerData->swishSpeedScale );
   return true;
}
void  InfluenceSwish::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);

   mAlarm.setInterval(0.1f, true);

   mFingerCount = 0;
   mFishPlayerData = (FishPlayerData *)mFish->getDataBlock();
   mTurnSpeed = mFishPlayerData->swishTurnScale;
   mStrength = 0;
}
bool  InfluenceSwish::isSwishing()
{
   for(int i=0; i<mFingerCount; i++){
      Effector *e = &mFinger[i];
      if(e->insideTrigger){
         return true;
      }
   }
   return false;
}
InfluenceSwish::Effector *InfluenceSwish::findEffector(GameBase *_trigger)
{
   for(int j=0; j<mFingerCount; j++){
      if( mFinger[j].trigger ==_trigger ){
         return &mFinger[j];
      }
   }
   return 0;
}
