#ifndef _InfluenceFeed_h_
#define _InfluenceFeed_h_

#include "influence.h"

class   InfluenceFeed : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   //---
   void  setFood(StaticShape *shape);
   StaticShape *getFood();
   bool  isEating();
private:
   StaticShape *mFood;
   bool        mMunching;
};

#endif
