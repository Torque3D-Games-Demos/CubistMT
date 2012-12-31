#ifndef _InfluenceFleeTrigger_h_
#define _InfluenceFleeTrigger_h_

#include "influence.h"

class Trigger;

class   InfluenceFleeTrigger : public Influence
{
   typedef  Influence Parent;
public:
   InfluenceFleeTrigger(){}
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   bool  isActive();
   void  onEnter(GameBase *_trigger);
   void  onExit(GameBase *_trigger);
   F32   getStrength(){return mStrength;}
   int   getTriggerCount();
   Trigger  *getTrigger(int _index);
private:
#define  MAX_SCAREBOX  4
   struct   ScareBox
   {
      Point3F     escapeVector;
      Trigger     *trigger;
      F32         fadeTime;
      F32         distanceAway;
      bool        fading;
   };
   ScareBox       mScareBox[MAX_SCAREBOX];
   int            mScareBoxCount;
   F32            mStrength;
   FishPlayerData *mFishPlayerData;

   const static   F32   SCARE_FADE_TIME;
   //---
   int   getScareBoxIndex(GameBase *_trigger);
};

#endif
