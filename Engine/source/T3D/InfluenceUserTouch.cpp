#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/InfluenceUserTouch.h"
#include "T3D/gameFunctions.h"

bool  InfluenceUserTouch::update()
{
   if( !Parent::update() ){
      return false;
   }
   Point3F  velocity;
   MatrixF  matrix;
   GameGetCameraTransform(&matrix, &velocity);
   if(mFish->mDataBlock->faceUser){
      mDirection = Point3F(0,0,0) - mFish->getPosition();
   }else{
      mDirection = mFish->getHeading();
   }
//   mDirection = Point3F(0,-1,0);
   mDirection.normalize();
   return true;
}
void  InfluenceUserTouch::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mAlarm.setInterval(TickSec);
   mDirection = Point3F::Zero;
   mTurnSpeed = 4.0f;
   mMoveSpeed = 0.01f;
}
