#ifndef _InfluenceBounds_h_
#define _InfluenceBounds_h_

#include "influence.h"

class   InfluenceBounds : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   bool     isOutsideBounds();  //0=safe 1=maximum mFear
private:
   bool  mOutsideBounds;
   F32   mTimeout;
   void  sphereLimitCheck();
   void  boxLimitCheck(Point3F lo, Point3F hi);
};

#endif
