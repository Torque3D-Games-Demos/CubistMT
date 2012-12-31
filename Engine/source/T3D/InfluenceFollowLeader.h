#ifndef _InfluenceFollowLeader_h_
#define _InfluenceFollowLeader_h_

#include "alarm.h"

class AIFishPlayer;

class InfluenceFollowLeader  : public Influence{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   void  setLeader(AIFishPlayer *_leader);
protected:
   AIFishPlayer  *mLeader;
   F32   mTimeOut;
   F32   mYawOffset;
   F32   mPitchOffset;
};

#endif
