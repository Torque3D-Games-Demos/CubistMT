#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"

static   F32   TRANSITION_TIME = 1.0f;

void  InfluenceAvoidCollision::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mAlarm.setInterval(0.1f);
   mStrength = 0;
}
F32   InfluenceAvoidCollision::getStrength()
{
   return powf(mStrength,0.5f);
}
bool  InfluenceAvoidCollision::update()
{
   if( !Parent::update() ){
      return false;
   }
   bool  maxSpeed = mFish->fullSpeed();
   mMoveSpeed = maxSpeed?  mFish->mDataBlock->fleeSpeedScale :   mFish->mDataBlock->wanderSpeedScaleLo;
   mTurnSpeed = maxSpeed?  mFish->mDataBlock->fleeTurnScale :   mFish->mDataBlock->wanderTurnScale;

   Point3F  oldDirection = mDirection;
   if( Obstacle *o = mFish->getFutureCollision()  ){
      mStrength = mFish->getFutureCollisionStrength();
      if( ! (o->object->getTypeMask() & PlayerObjectType) ){
         //---reef collision, treat like big flat wall-------------------------
         mDirection = o->normal;
      }else{
         //---fish collision, try to pass by side of it-----------------------
         AIFishPlayer *collisionFish = (AIFishPlayer *)o->object;

         F32   passDistance = mFish->getPassingDistance() + collisionFish->getPassingDistance();

         Point3F  separation = mFish->getPosition() - collisionFish->getPosition();
         F32      distanceAway = separation.len();

         if( distanceAway==0 ){
            // on top of each other, just go along x-axis
            mDirection = Point3F(1.0f, 0, 0);
         }else if( distanceAway<passDistance ){
            //overlapping, move directly away
            mDirection = separation * (1.0f/distanceAway);
         }else{
            //try and pass by, but in opposite direction to movement of blocking fish
            Point3F  heading = mFish->getHeading();
            Point3F  perp;
            perp.x = heading.y;
            perp.y = -heading.x;
            perp.z = 0;
            perp.normalize();
            if( mDot(collisionFish->getHeading(),perp) > 0 ){
               perp *= -1.0f;
            }
            Point3F  otherPosition = collisionFish->getPosition();
            Point3F  thisPosition = mFish->getPosition();
            F32   currentPassingDistance = mDot((thisPosition - otherPosition), perp);
            if( mFabs(currentPassingDistance) >= passDistance ){
               //keep on same heading, already due to miss
               mDirection = heading;
            }else{
               //turn towards passing position
               Point3F  passingPosition = collisionFish->getPosition() + perp*passDistance;
               mDirection = passingPosition - mFish->getPosition();
               mDirection.normalize();
            }
         }
      }
   }else{
      mStrength = 0;
   }
/*
   F32   delta = (mStrength - oldStrength);

   if(delta){
      F32   dt = mAlarm.getInterval();
      F32   maxChange = dt / TRANSITION_TIME;
      F32   fraction = abs(maxChange/delta);
      fraction = mClampF(fraction, 0, 1);
      mStrength  = oldStrength + delta*fraction;
      Point3F  move = mDirection - oldDirection;
      Point3F  v = oldDirection + fraction*move;
      F32   len = v.len();
      if(len>0){
         mDirection = v*(1.0f/len);
      }else{
         //set to end direction;
      }
   }
   */

   return true;
}
