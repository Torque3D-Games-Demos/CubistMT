#ifndef _InfluenceSwish_h_
#define _InfluenceSwish_h_

#include "influence.h"

class Trigger;

class   InfluenceSwish : public Influence
{
   typedef  Influence Parent;
public:
   InfluenceSwish(){}
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   bool  isSwishing();
   void  onEnter(GameBase *_trigger);
   void  onExit(GameBase *_trigger);
private:
#define  MAX_FINGERS  8
   struct   Effector
   {
      Point3F     directionToFinger;
      Point3F     lastPosition;
      Point3F     position;
      F32         exitTime;
      Trigger     *trigger;
      F32         weight;
      bool        insideTrigger;
      F32         speedWeight;
   };
   Effector       mFinger[MAX_FINGERS];
   int            mFingerCount;
   F32            mStrength;
   FishPlayerData *mFishPlayerData;
   //---
   Effector    *findEffector(GameBase *_trigger);
};

#endif
