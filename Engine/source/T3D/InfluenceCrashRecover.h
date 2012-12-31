#ifndef _InfluenceCrashRecover_h_
#define _InfluenceCrashRecover_h_

#include "influence.h"

class   InfluenceCrashRecover : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();

#ifdef   OLD_CRASH
   void  addCrashEvent(const Collision *_collision);
#endif
   bool  recovered();
   F32   getStrength();
   void  trigger();
private:
#ifdef   OLD_CRASH
   void        updateBadPosition(const Point3F &_where);
   Point3F     mBadPosition;              //average of previous collision points
   F32         mBadPositionTime;          //time last got a collision point
#endif
   F32         mStartTime;
   Point3F     mStartPosition;
   Point3F     mStartDirection;
   bool        mCrashed;

   int         mPhase;
   bool        mFacingDirection;
   int         mStartEscapeTime;
};

#endif
