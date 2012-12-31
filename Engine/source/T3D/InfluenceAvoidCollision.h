#ifndef _InfluenceAvoidCollision_h_
#define _InfluenceAvoidCollision_h_

#include "influence.h"

class InfluenceAvoidCollision  : public Influence 
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   F32      getStrength();
protected:
   F32      mStrength;

};

#endif
