#ifndef _InfluenceUserTouch_h_
#define _InfluenceUserTouch_h_

#include "influence.h"

class   InfluenceUserTouch : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
};

#endif
