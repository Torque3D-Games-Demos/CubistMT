#ifndef _Influence_h_
#define _Influence_h_

#include "alarm.h"

class AIFishPlayer;
struct Collision;

class Influence{
public:
   virtual  void  init(AIFishPlayer *_fish, const char *_name);
   virtual  bool  update();
   const    char  *getName(){return mName;}
   void     setWeightTarget(F32 _weight); //the weight reaches this level over time
   void     setWeightInstant(F32 _weight);  //the weight reaches this level instantly
   F32      getWeight(){return mWeight;}
   bool     active(){ return mWeight>0;}
   //---
   Point3F        mDirection;
   F32            mTurnSpeed;
   F32            mMoveSpeed;
   //---
protected:
   void           pitchYawToDirection(Point3F &_d, const F32 &_pitch, const F32 &_yaw);
   void           directionToPitchYaw(const Point3F &d, F32 &_pitch, F32 &_yaw);
   F32            transitionCosine(F32  x, F32 x0, F32 x1, F32 y0, F32 y1);
   Point3F        randomVectorRotation(Point3F oldHeading);
   float          getTimeSeconds();
   Alarm          mAlarm;
   S32            mUpdateInterval;
   AIFishPlayer   *mFish;
   const char     *mName;
private:
   F32            mTargetWeight;
   F32            mWeight;
};

#endif
