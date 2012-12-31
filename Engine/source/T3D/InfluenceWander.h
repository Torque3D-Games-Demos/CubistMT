#ifndef _InfluenceWander_h_
#define _InfluenceWander_h_

#include "influence.h"

class   InfluenceWander : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   void  reset(Point3F *direction=0);
private:
   float       mTimeOut;
   //---
   void        onExpired();
};

#endif
