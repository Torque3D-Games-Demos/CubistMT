#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceWander.h"

const static F32  BOUNDARY_SIZE = 0.2f;   //when fish get this near boundary, the try and turn back
const static F32  BOUNCE_ANGLE = 0.5f;    //when cross boundary, this is the angle they turn in by (radians)

const static F32  TAN_BOUNCE_ANGLE = tanf(BOUNCE_ANGLE);
const static Point3F BOUNDARY_VECTOR = Point3F(BOUNDARY_SIZE,BOUNDARY_SIZE,BOUNDARY_SIZE);

bool  InfluenceWander::update()
{
   if( !Parent::update() ){
      return false;
   }
   //---choose new direction ever so often----------------------
   if(mTimeOut<getTimeSeconds()){
      onExpired();
   }
   return true;
}
void           InfluenceWander::onExpired()
{
#ifdef   DEBUG_HUD
//   mFish->addLog("wander expired");
#endif
   Point3F  wanderDirection = mFish->getHeading();
   reset( &wanderDirection );
}
void  InfluenceWander::reset(Point3F *d)
{
   mTimeOut = gRandGen.randF(mFish->mDataBlock->wanderTimeMin, mFish->mDataBlock->wanderTimeMax) + getTimeSeconds();
   mDirection = randomVectorRotation(d? *d : mFish->getHeading());
   mMoveSpeed = gRandGen.randF(  mFish->mDataBlock->wanderSpeedScaleLo, 
                                 mFish->mDataBlock->wanderSpeedScaleHi);
}
void  InfluenceWander::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mDirection = mFish->getHeading();
   mTimeOut = gRandGen.randF(mFish->mDataBlock->wanderTimeMin, mFish->mDataBlock->wanderTimeMax) + getTimeSeconds();
   mTurnSpeed = mFish->mDataBlock->wanderTurnScale;
   mAlarm.setInterval(0.1f, true);
}
