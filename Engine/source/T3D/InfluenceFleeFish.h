#ifndef _InfluenceFleeFish_h_
#define _InfluenceFleeFish_h_

#include "influence.h"

class   InfluenceFleeFish : public Influence
{
   typedef  Influence Parent;
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();

   void     setScaryObject(ShapeBase *_scaryObject);
   void     setFearDistance(F32 _fearDistance);
   ShapeBase   *getScaryObject(){return mScaryObject;}
   F32      getFear();  //0=safe 1=maximum mFear
private:
   ShapeBase   *mScaryObject;
   F32         mFear;
   F32         mFearDistance;

};

#endif
