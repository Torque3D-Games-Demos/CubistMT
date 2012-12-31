#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"

const static   F32   pitchVariation = mDegToRad(10.0f);
const static   F32   yawVariation = mDegToRad(10.0f);
const static   F32   maxDeviation = mCos( mDegToRad(20.0f) );

void  InfluenceFollowLeader::setLeader(AIFishPlayer *_leader)
{
   if(_leader!=mLeader ){
      mLeader = _leader;
      mTimeOut = 0;
   }
}
void  InfluenceFollowLeader::init(AIFishPlayer *_fish, const char *_name)
{
   Parent::init(_fish, _name);
   mAlarm.setInterval(0.1f, true);
   mLeader = 0;
   mTimeOut = gRandGen.randF(mFish->mDataBlock->wanderTimeMin, mFish->mDataBlock->wanderTimeMax) + getTimeSeconds();
   mPitchOffset = 0;
   mYawOffset = 0;
}
bool  InfluenceFollowLeader::update()
{
   if( !Parent::update() ){
      return false;
   }
   F32   nearDistance = mFish->getPassingDistance()*3.0f;
   F32   farDistance = mFish->mDataBlock->localFishRadius;
   if( !mLeader ){
      mDirection = Point3F::Zero;
      mMoveSpeed = 0;
      mTurnSpeed = 0;
   }else{
      if( mLeader->isFleeing() ){
         //follow leader exactly when fleeing
         mDirection = mLeader->getHeading();
         mMoveSpeed = mLeader->getMoveSpeedMultiplier();
         mTurnSpeed = mLeader->getTurnRateMultiplier();
      }else{
         F32   dot = mDot( mLeader->getHeading(), mFish->getHeading());
         if( dot<maxDeviation ){
            //if direction deviates to much, try and follow exactly
            mDirection = mLeader->getHeading();
            mMoveSpeed = mLeader->getMoveSpeedMultiplier()*1.2f;
            mTurnSpeed = mLeader->getTurnRateMultiplier()*1.2f;
         }else if(mTimeOut<getTimeSeconds()){
            //randomly change direction ever so often
            mTimeOut = gRandGen.randF(mFish->mDataBlock->wanderTimeMin, mFish->mDataBlock->wanderTimeMax) + getTimeSeconds();
            Point3F  toLeader = mLeader->getPosition() - mFish->getPosition();
            F32   distanceToLeader = toLeader.len();
            if( distanceToLeader<nearDistance ){
               mYawOffset = gRandGen.randF(  -yawVariation,+yawVariation);
               mPitchOffset = gRandGen.randF(-pitchVariation,+pitchVariation);
            }else{
               mYawOffset = 0;
               mPitchOffset = 0;
            }
            if( distanceToLeader>0 ){
               toLeader *= (1.0f/distanceToLeader);
            }
            F32   fractionDirect;
            if(distanceToLeader>farDistance){
               fractionDirect = 1.0f;
            }else if( distanceToLeader>nearDistance ){
               fractionDirect = (distanceToLeader - nearDistance) / (farDistance-nearDistance);
            }else{
               fractionDirect = 0;
            }
            mDirection = (toLeader*fractionDirect) + (mLeader->getHeading()*(1.0f-fractionDirect));
            mDirection.normalizeSafe();

            F32   scale;
            if(distanceToLeader<nearDistance ){
               scale = 1.0f;
            }else if(distanceToLeader<farDistance ){
               scale = 1.1f;
            }else{
               scale = 1.2f;
            }

            mMoveSpeed = mLeader->getMoveSpeedMultiplier() * scale;   //slightly faster so can catch up, then match speed when close
            mTurnSpeed = mLeader->getTurnRateMultiplier();

            F32   pitch, yaw;
            directionToPitchYaw(mDirection, pitch, yaw);

            pitch += mPitchOffset;
            yaw += mYawOffset;

            pitchYawToDirection(mDirection, pitch, yaw);
         }
      }
   }
   return true;
}
