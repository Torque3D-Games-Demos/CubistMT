#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceFleeTrigger.h"

const F32   InfluenceFleeTrigger::SCARE_FADE_TIME = 0.0f;

//*************************************************************************
int      InfluenceFleeTrigger::getTriggerCount()
{
   return mScareBoxCount;
}
Trigger  *InfluenceFleeTrigger::getTrigger(int _index)
{
   return mScareBox[_index].trigger;
}

//*************************************************************************
void  InfluenceFleeTrigger::onEnter(GameBase *_trigger)
{
   int index = getScareBoxIndex(_trigger);
   if( index<0 ){
      //new scare box
      if( mScareBoxCount<MAX_SCAREBOX ){
         ScareBox *e = &mScareBox[mScareBoxCount++];
         e->trigger = (Trigger *)_trigger;
         e->fadeTime = 0;
         e->escapeVector = Point3F::Zero;
         e->distanceAway = 0;
         e->fading = false;
      }
   }else{
      //re-entered scare box
      ScareBox *e = &mScareBox[index];
      e->fading = false;
      e->fadeTime = 0;
   }
}
//*************************************************************************
void  InfluenceFleeTrigger::onExit(GameBase *_trigger)
{
   int   index = getScareBoxIndex(_trigger);
   if( index>=0 ){
      ScareBox *e = &mScareBox[index];
      if( !e->fading ){
         e->fading = true;
         e->fadeTime = 0;
      }
   }
}
//*************************************************************************
bool InfluenceFleeTrigger::update()
{
   if( !Parent::update() ){
      return false;
   }

   //---update scare boxes----------------------------
   for(int j=0; j<mScareBoxCount; j++){
      ScareBox *sb = &mScareBox[j];
      if(sb->fading){
         sb->fadeTime += TickSec;
      }else{
         Trigger *t = sb->trigger;
         const MatrixF &m = t->getTransform();
         Point3F  relative = mFish->getPosition() - m.getPosition();
         Point3F  forward;
         m.getColumn(2, &forward);
         F32      projection = mDot(relative, forward );
         Point3F  radial = relative - (projection*forward);
         sb->distanceAway = radial.len();
         if( sb->distanceAway>0 ){
            sb->escapeVector = radial * (1.0f/sb->distanceAway);
         }else{
            sb->escapeVector =Point3F::Zero;
         }
      }
   }
   //---get rid of scare boxes that have expired---------------------------
   int   iTo = 0;
   for(int iFrom=0; iFrom<mScareBoxCount; iFrom++){
      ScareBox *sb = &mScareBox[iFrom];
      if( (!sb->fading) ||  sb->fadeTime<SCARE_FADE_TIME){
         mScareBox[iTo++] = mScareBox[iFrom];
      }
   }
   mScareBoxCount = iTo;

   //---find closest active scare box---------------------------
   ScareBox *scariest = 0;
   F32      scariestDistance = FLT_MAX;
   for(int j=0; j<mScareBoxCount; j++){
      ScareBox *sb = &mScareBox[j];
      if( sb->fading )continue;
      if(scariestDistance > sb->distanceAway ){
         scariestDistance = sb->distanceAway;
         scariest = sb;
      }
   }
   if( !scariest ){
      //---no active scare box, so continue running away from recently expired ones----------------------------
      for(int j=0; j<mScareBoxCount; j++){
         ScareBox *sb = &mScareBox[j];
         if(!sb->fading)continue;
         if(scariestDistance > sb->distanceAway ){
            scariestDistance = sb->distanceAway;
            scariest = sb;
         }
      }
   }
   if( scariest ){
      mStrength = 1.0f;
      mDirection = scariest->escapeVector;
   }else{
      mStrength = 0;
      mDirection = Point3F::Zero;
   }
   return true;
}
//*************************************************************************
void  InfluenceFleeTrigger::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);

   mAlarm.setInterval(0.1f, true);

   mScareBoxCount = 0;
   mFishPlayerData = (FishPlayerData *)mFish->getDataBlock();
   mTurnSpeed = mFishPlayerData->fleeTurnScale;
   mMoveSpeed = mFishPlayerData->fleeSpeedScale;
   mStrength = 0;
}
//*************************************************************************
bool  InfluenceFleeTrigger::isActive()
{
   return mScareBoxCount>0;
}
//*************************************************************************
int   InfluenceFleeTrigger::getScareBoxIndex(GameBase *_trigger)
{
   for(int j=0; j<mScareBoxCount; j++){
      if( mScareBox[j].trigger ==_trigger ){
         return j;
      }
   }
   return -1;
}
