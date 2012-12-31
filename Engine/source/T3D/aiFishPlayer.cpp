//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#include <stdio.h>
#include "platform/platform.h"
#include "T3D/Trigger.h"
#include "T3D/aiFishPlayer.h"
#include "T3D/TrafficController.h"

#include "console/consoleInternal.h"
#include "math/mMatrix.h"
#include "T3D/gameBase/moveManager.h"
#include "console/engineAPI.h"
// start jc
#include "terrain/terrData.h"
//end jc

// start jc
#include "core/stream/bitStream.h"

#include "gfx/gfxDrawUtil.h"
#include "gfx/sim/debugDraw.h"

#define   RADAR_RAY_CAST

const static U32  RADAR_CONE_POINTS = 4;
const static F32  RADAR_CONE_ANGLE = mDegToRad(20.0f);

F32   COLLISION_WARNING_TIME = 3.0f;   //how far ahead in time to look for collisions
//F32   COLLISION_WARNING_TIME = 6.0f;   //how far ahead in time to look for collisions

//times of alarms
const static   F32 ALARMPERIOD_INFLUENCE = TickSec;
const static   F32 ALARMPERIOD_SCARYFISHSEARCH = 2.0f;
const static   F32 ALARMPERIOD_OBSTACLECHECK = TickSec;
const static   F32 ALARMPERIOD_LOCALFISHCHECK = 0.5f;
const static   F32 ALARMPERIOD_STUCKCHECK = 1.0f;

//const static   F32   COLLISION_AVOID_TIME = 1.0f;
const static   F32   COLLISION_AVOID_TIME = 3.0f;  //how long to keep on avoiding old collision
const static   F32   COLLISION_DISTANCE = 1.0f;
const static F32  BAD_LEADER_TIME = 3.0f;

IMPLEMENT_CO_NETOBJECT_V1(AIFishPlayer);

ConsoleDocClass( AIFishPlayer,
	"@brief A FishPlayer object not controlled by conventional input, but by an AI engine.\n\n"


	"@see FishPlayer for a list of all inherited functions, variables, and base description\n"

	"@ingroup AI\n"
	"@ingroup gameObjects\n");
/**
 * Constructor
 */
AIFishPlayer::AIFishPlayer()
{
#ifdef   DEBUG_HUD
   mLogCount = 0;
   mDebugLog = false;
   mLogBuffer = new char[FISH_MAXLOG*256];
   for(int j=0; j<FISH_MAXLOG; j++){
      mLog[j] = &mLogBuffer[j*256];
   }
#endif
   mRadarPhase = 0;
   mBadLeadershipTime = 0;
   mSchooling = false;

   herdId = -1;
   mActivity.time = 0;
   mActivity.shape = 0;
   mActivity.activity = ActivityNone;

   mLastHardCollisionTime = -FLT_MAX;
   mLastStuckCheckPosition = Point3F::Zero;

   mBadPosition = Point3F::Zero;
   mBadPositionTime = -FLT_MAX;    

   mObstacle.time = -100000.0f;

   mTrafficController = 0;
   mLocalFishCount = 0;

   mAlarm[AlarmInfluence].setInterval(ALARMPERIOD_INFLUENCE, true);
   mAlarm[AlarmScaryFishSearch].setInterval(ALARMPERIOD_SCARYFISHSEARCH, true);
   mAlarm[AlarmObstacleCheck].setInterval(ALARMPERIOD_OBSTACLECHECK, true);
   mAlarm[AlarmLocalFishCheck].setInterval(ALARMPERIOD_LOCALFISHCHECK, true);
   mAlarm[AlarmStuckCheck].setInterval(ALARMPERIOD_STUCKCHECK, true);

   for(int j=0; j<influenceCount; j++){
      mInfluence[j] = 0;
   }



   mMoveDestination.set( 0.0f, 0.0f, 0.0f );
// start jc - moved to player for variable motion.
//   mMoveSpeed = 1.0f;
// end jc
   mMoveTolerance = 0.25f;
   mMoveSlowdown = true;
   mMoveState = ModeStop;

   mAimObject = 0;
   mAimLocationSet = false;
   mTargetInLOS = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
   
   mIsAiControlled = true;

// start jc
   mDataBlock = 0;
// end jc

// start ds
//Added by Brendan Ledwich 2005-11-9
//   mInterpolator = new BezierNodeInterpolator();
	mAvoidPoint.set(0,0,0);
//   mMaxTurnAngle = 0.2;
   mFollowObject = NULL;
   mFollowNodeIndex = -1;
//   mFollowSlotIndex = 0;
//end ds
}

/**
 * Destructor
 */
AIFishPlayer::~AIFishPlayer()
{
// start ds
//	delete mInterpolator;
// end ds

#ifdef   DEBUG_HUD
   delete [] mLogBuffer;
#endif
}

void AIFishPlayer::initPersistFields()
{
   addGroup( "AIFish" );

      addField( "moveTolerance", TypeF32, Offset( mMoveTolerance, AIFishPlayer ), 
         "@brief Distance from destination before stopping.\n\n"
         "When the AIFishPlayer is moving to a given destination it will move to within "
         "this distance of the destination and then stop.  By providing this tolerance "
         "it helps the AIFishPlayer from never reaching its destination due to minor obstacles, "
         "rounding errors on its position calculation, etc.  By default it is set to 0.25.\n");

   endGroup( "AIFish" );

   Con::setIntVariable("$Influence::Wander",      influenceWander);
   Con::setIntVariable("$Influence::Swish",      influenceSwish);
   Con::setIntVariable("$Influence::SchoolGravity",      influenceSchool);
   Con::setIntVariable("$Influence::Count",      influenceCount);

   Con::setIntVariable("$Trigger::Swish",      TriggerTypeSwish);
   Con::setIntVariable("$Trigger::Scare",      TriggerTypeScare);

   Parent::initPersistFields();
}

bool AIFishPlayer::onAdd()
{
   if (!Parent::onAdd())
      return false;

   if( isServerObject() && getDataBlock() ){
      mInfluence[influenceSchool] = &mInfluenceSchool;
      mInfluence[influenceWander] = &mInfluenceWander;
      mInfluence[influenceSwish] = &mInfluenceSwish;
      mInfluence[influenceFleeFish] = &mInfluenceFleeFish;
      mInfluence[influenceFollowLeader] = &mInfluenceFollowLeader;
      mInfluence[influenceFeed] = &mInfluenceFeed;
      mInfluence[influenceAvoidCollision] = &mInfluenceAvoidCollision;
      mInfluence[influenceBounds] = &mInfluenceBounds;
      mInfluence[influenceFleeTrigger] = &mInfluenceFleeTrigger;
      mInfluence[influenceUserTouch] = &mInfluenceUserTouch;
      mInfluence[influenceCrashRecover] = &mInfluenceCrashRecover;

      mInfluence[influenceSchool]->init(this, "School node");
      mInfluence[influenceWander]->init(this, "wander");
      mInfluence[influenceSwish]->init(this, "Swish");
      mInfluence[influenceFleeFish]->init(this, "flee");
      mInfluence[influenceFollowLeader]->init(this, "follow");
      mInfluence[influenceFeed]->init(this, "feed");
      mInfluence[influenceAvoidCollision]->init(this, "avoid collision");
      mInfluence[influenceBounds]->init(this, "Bounds");
      mInfluence[influenceFleeTrigger]->init(this, "scare");
      mInfluence[influenceUserTouch]->init(this, "user touch");
      mInfluence[influenceCrashRecover]->init(this, "Crash Recover");

      mInfluenceFleeFish.setFearDistance(mDataBlock->fleeEndDistance);

      mLeader = 0;
   }

   return true;
}

// start jc - moved to player for variable motion.
/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
/*
void AIFishPlayer::setMoveSpeed( F32 speed )
{
   mMoveSpeed = getMax(0.0f, getMin( 1.0f, speed ));
}
*/
// end jc

/**
 * Stops movement for this AI
 */
void AIFishPlayer::stopMove()
{
   mMoveState = ModeStop;
}

/**
 * Sets how far away from the move location is considered
 * "on target"
 *
 * @param tolerance Movement tolerance for error
 */
void AIFishPlayer::setMoveTolerance( const F32 tolerance )
{
   mMoveTolerance = getMax( 0.1f, tolerance );
}
bool  AIFishPlayer::reachedTarget()
{
   // Use the eye as the current position.
   MatrixF eye;
   getEyeTransform(&eye);
   Point3F pos = eye.getPosition(); //ai code seems to use this as position. todo: find out why
//   Point3F pos = getPosition();

   Point3F  p = mMoveDestination - pos;
   return (mFabs(p.x)<=mMoveTolerance) && (mFabs(p.y)<=mMoveTolerance) && (mFabs(p.z)<=mMoveTolerance);
}
/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void AIFishPlayer::setMoveDestination( const Point3F &location, bool slowdown )
{
   clearFollowObject();
   mMoveDestination = location;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   //pg mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
// start ds
/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 * @param approachVector direction to run to location from
 */
void AIFishPlayer::setMoveDestination( const Point3F &location, const Point3F &approachVector, bool slowdown )
{
   setMoveDestination(location, slowdown);
}
// end ds

/**
 * Sets the object the bot is targeting
 *
 * @param targetObject The object to target
 */
void AIFishPlayer::setAimObject( GameBase *targetObject )
{
   mAimObject = targetObject;
   mTargetInLOS = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

// start ds
void AIFishPlayer::setFollowObject( ShapeBase *targetObject, bool slowdown )
{
   mFollowObject = targetObject;
   mFollowNodeIndex = -1;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   //pg mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
void AIFishPlayer::setFollowObjectNode( ShapeBase *targetObject, S32 nodeIndex, bool slowdown )
{
   mFollowObject = targetObject;
   mFollowNodeIndex = nodeIndex;
//   mFollowSlotIndex = slotIndex;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   //pg mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
void AIFishPlayer::clearFollowObject( )
{
   mFollowObject = NULL;
   mFollowNodeIndex = -1;
}
// end ds

/**
 * Sets the object the bot is targeting and an offset to add to target location
 *
 * @param targetObject The object to target
 * @param offset       The offest from the target location to aim at
 */
void AIFishPlayer::setAimObject( GameBase *targetObject, Point3F offset )
{
   mAimObject = targetObject;
   mTargetInLOS = false;
   mAimOffset = offset;
}

/**
 * Sets the location for the bot to aim at
 *
 * @param location Point to aim at
 */
void AIFishPlayer::setAimLocation( const Point3F &location )
{
   mAimObject = 0;
   mAimLocationSet = true;
   mAimLocation = location;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

/**
 * Clears the aim location and sets it to the bot's
 * current destination so he looks where he's going
 */
void AIFishPlayer::clearAim()
{
   mAimObject = 0;
   mAimLocationSet = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}
F32   AIFishPlayer::getPassingDistance()
{
   return getCapsuleSize().x;
}
void AIFishPlayer::onCollision( SceneObject *object, const VectorF &vec )
{
   //pghack
   //Parent::onCollision(object, vec);
}
Point3F AIFishPlayer::getTargetPoint()
{
   return mMoveDestination;
}

Point3F  AIFishPlayer::getHeading()
{
   Point3F y;
   mObjToWorld.getColumn(1, &y);
   return y;

}
void  AIFishPlayer::applyInfluences()
{
   if( !mAlarm[AlarmInfluence].isRinging() )return;

   //---add up influences----------------------------
   Point3F  velocity = Point3F::Zero;
   F32      turnSpeed = 0;
   F32      sumWeight = 0;
   for(int j=0; j<influenceCount; j++){
      Influence   *in = mInfluence[j];
      if( in->getWeight()>0 ){
         velocity += (in->mDirection*(in->mMoveSpeed*in->getWeight()));
         turnSpeed += in->mTurnSpeed*in->getWeight();
         sumWeight += in->getWeight();
      }
   }
   if( !sumWeight ){
      //shouldnt get here
      setMoveSpeedMultiplier( 0 );
      mMoveState = ModeStop;
   }else{
      //---target position and speed----------------------
      velocity *= (1.0f/sumWeight);
      F32   targetSpeed = velocity.len();

      F32   currentSpeed = getMoveSpeedMultiplier();
      F32   speed;
      if( targetSpeed>currentSpeed ){
         speed = currentSpeed + (ALARMPERIOD_INFLUENCE*mDataBlock->acceleration);
         if(speed>targetSpeed){
            speed = targetSpeed;
         }
      }else{
         speed = currentSpeed - (ALARMPERIOD_INFLUENCE*mDataBlock->acceleration);
         if(speed<targetSpeed){
            speed = targetSpeed;
         }
      }
      setMoveSpeedMultiplier( speed );
      if( speed>0 ){
         if( targetSpeed > 0 ){
            Point3F  direction = velocity*(1.0f/targetSpeed);
            setMoveDestination( getPosition() + (direction*10.0f), false );
            mMoveState = ModeMove;
         }
      }else{
         mMoveState = ModeStop;
      }
      //---turn rate--------------------------------------------
#if   1
      setTurnRateMultiplier( turnSpeed/sumWeight );
#else
      F32   targetTurnRate = turnSpeed/sumWeight;
      F32   turnRate = targetTurnRate;
      F32   currentTurnRate = getTurnRateMultiplier();
      if( targetTurnRate > currentTurnRate ){
         turnRate = currentTurnRate + (ALARMPERIOD_INFLUENCE*mDataBlock->turnAcceleration);
         if(turnRate>targetTurnRate){
            turnRate = targetTurnRate;
         }
      }else{
         turnRate = currentTurnRate - (ALARMPERIOD_INFLUENCE*mDataBlock->turnAcceleration);
         if(turnRate<targetTurnRate){
            turnRate = targetTurnRate;
         }
      }
      setTurnRateMultiplier( turnRate );
#endif

   }
}

// end ds

/**
 * This method calculates the moves for the AI player
 *
 * @param movePtr Pointer to move the move list into
 */
bool AIFishPlayer::getAIMove(Move *movePtr)
{
   *movePtr = NullMove;

   // Use the eye as the current position.
   MatrixF eye;
   getEyeTransform(&eye);
   Point3F location = eye.getPosition();
   Point3F rotation = getRotation();


   // Orient towards the aim point, aim object, or towards
   // our destination.
   applyInfluences();

   if (mAimObject || mAimLocationSet || mMoveState != ModeStop) 
   {

      // Update the aim position if we're aiming for an object
      if (mAimObject)
         mAimLocation = mAimObject->getPosition() + mAimOffset;
      else
         if (!mAimLocationSet)
            mAimLocation = mMoveDestination;

      Point3F  diff =  (mAimLocation - location);

      if (!mIsZero(diff.x) || !mIsZero(diff.y) || !mIsZero(diff.z) ) {

         // First do Yaw
         // use the cur yaw between -Pi and Pi
         F32 curYaw = rotation.z;
      // start jc
         /*
         while (curYaw > M_2PI_F)
            curYaw -= M_2PI_F;
         while (curYaw < -M_2PI_F)
            curYaw += M_2PI_F;
            */
         while (curYaw < -M_PI_F)   curYaw += M_2PI_F;
         while (curYaw >= M_PI_F)   curYaw -= M_2PI_F;
      // end jc

         // find the yaw offset
         F32 newYaw = mAtan2( diff.x, diff.y );
         F32 yawDiff = newYaw - curYaw;

         // make it between 0 and 2PI
         if( yawDiff < 0.0f )
            yawDiff += M_2PI_F;
         else if( yawDiff >= M_2PI_F )
            yawDiff -= M_2PI_F;

         // now make sure we take the short way around the circle
         if( yawDiff >= M_PI_F )
            yawDiff -= M_2PI_F;
         else if( yawDiff < -M_PI_F )
            yawDiff += M_2PI_F;
         if (yawDiff < 0.001 && yawDiff > -0.001)
         {
            if (mMoveState == ModeStop)
            {
               //pghack throwCallback("onReachDestination");
            }
         }

         movePtr->yaw = yawDiff;
	      // control the rotation of the AIFishPlayer
   
         // get the maximum amount we can yaw this tick
         F32 maxTurnRate = getMaxTurnRate() * TickSec;
         // limit the yaw if it exceeds the maxYaw
         if(mFabs(yawDiff) > maxTurnRate)
            movePtr->yaw = maxTurnRate * mSign(yawDiff);
         else
            movePtr->yaw = yawDiff;
         // Next do pitch.
         Point3F headRotation = getHeadRotation();
         // This should be adjusted to run from the
         // eye point to the object's center position. Though this
         // works well enough for now.
         // First do Yaw
	      // use the cur yaw between -Pi and Pi
         F32 curPitch = headRotation.x;

         // find the yaw offset
         F32 horzDist = diff.magnitudeSafe();
         F32 newPitch = mAtan2( horzDist, diff.z ) - ( M_PI_F / 2.0f ); //-pi->0?
//            F32 horzDist = mSqrt(diff.x*diff.x + diff.y*diff.y);
//            F32   newPitch = mAtan2(  diff.z, horzDist );

         F32 pitchDiff = newPitch - curPitch;

         // make it between 0 and 2PI
         if( pitchDiff < 0.0f )
            pitchDiff += M_2PI_F;
         else if( pitchDiff >= M_2PI_F )
            pitchDiff -= M_2PI_F;

         // now make sure we take the short way around the circle
         if( pitchDiff >= M_PI_F )
            pitchDiff -= M_2PI_F;
         else if( pitchDiff < -M_PI_F )
            pitchDiff += M_2PI_F;

         if (mFabs(pitchDiff) > 0.001f) {
            movePtr->pitch = pitchDiff;
		   }
         F32 halfMaxTurnRate = maxTurnRate*0.5f;
         movePtr->pitch = mClampF(movePtr->pitch, -halfMaxTurnRate, +halfMaxTurnRate);
      }
   }else if (mMoveState == ModeStop) {
      // Level out if we're not doing anything else
      Point3F headRotation = getHeadRotation();
      movePtr->pitch = -headRotation.x;

      // never move more than we are aloud
      F32 halfMaxTurnRate = (getMaxTurnRate() * TickSec)/20;
      if(mFabs(movePtr->pitch) > halfMaxTurnRate){
	      movePtr->pitch = halfMaxTurnRate * mSign(movePtr->pitch);
      }
   }

   // Move towards the destination
   if (mMoveState != ModeStop) 
   {
      Point3F  targetPoint = mMoveDestination;
      Point3F  diff = targetPoint - location;
      
      const Point3F finish = (mFollowObject) ? (mFollowObject->getPosition()) : targetPoint;

      Point3F  finDiff = finish - location;

      // Check if we should mMove, or if we are 'close enough'
      if (mFabs(finDiff.x) < mMoveTolerance 
         && mFabs(finDiff.y) < mMoveTolerance 
         && mFabs(finDiff.z) < mMoveTolerance)
      {
         mMoveState = ModeStop;
         //pghack throwCallback("onReachDestination");
      }
      else
      {
         // Build move direction in world space
         if(mForwardOnly){
            movePtr->x = 0;
            movePtr->y = 1.0f;
            movePtr->z = 0;
         }else{
            if (mIsZero(diff.x) && mIsZero(diff.z))
            {
               movePtr->y = (location.y > targetPoint.y)? -1 : 1;
            }
            else if (mIsZero(diff.y) && mIsZero(diff.z))
            {
      	      movePtr->x = (location.x > targetPoint.x)? -1 : 1;
            }
            else if (mIsZero(diff.x) && mIsZero(diff.y))
            {
               movePtr->z = (location.z > targetPoint.z)? -1 : 1;
            }
	         else
            {
               F32 total = diff.magnitudeSafe();
               movePtr->x = diff.x/total;
               movePtr->y = diff.y/total;
               movePtr->z = diff.z/total;
            }
            // Rotate the move into object space (this really only needs
            // a 2D matrix)
            Point3F newMove;
            MatrixF moveMatrix;
            moveMatrix.set(EulerF(0.0f, 0.0f, -(rotation.z + movePtr->yaw)));
            moveMatrix.mulV( Point3F( movePtr->x, movePtr->y, movePtr->z ), &newMove );
            movePtr->x = newMove.x;
            movePtr->y = newMove.y;
            movePtr->z = newMove.z;
         }
         // Set movement speed.  We'll slow down once we get close
         // to try and stop on the spot...
         if (mMoveSlowdown) {
            F32 speed = mMoveSpeed;
            F32 dist = finDiff.magnitudeSafe();

            F32 maxDist = 2.0f / mMoveSpeed;
            if (dist < maxDist){
               speed *= dist / maxDist;
               mMoveState = ModeSlowing;
			   }
            else{
               mMoveState = ModeMove;
            }
            if(speed < 0.002f)
               speed = 0.002f;
            movePtr->x *= speed;
            movePtr->y *= speed;
            movePtr->z *= speed;
         }else{
            movePtr->x *= mMoveSpeed;
            movePtr->y *= mMoveSpeed;
            movePtr->z *= mMoveSpeed;

            mMoveState = ModeMove;
         }

         // Reset a previous stuck mode
         mMoveState = ModeMove;
      }
   }

   // Test for target location in sight if it's an object. The LOS is
   // run from the eye position to the center of the object's bounding,
   // which is not very accurate.
   if (mAimObject) {
      MatrixF eyeMat;
      getEyeTransform(&eyeMat);
      eyeMat.getColumn(3,&location);
      Point3F targetLoc = mAimObject->getBoxCenter();

      // This ray ignores non-static shapes. Cast Ray returns true
      // if it hit something.
      RayInfo dummy;
      if (getContainer()->castRay( location, targetLoc,
            InteriorObjectType | StaticShapeObjectType | StaticObjectType |
            TerrainObjectType, &dummy)) {
         if (mTargetInLOS) {
            throwCallback( "onTargetExitLOS" );
            mTargetInLOS = false;
         }
      }else{
         if (!mTargetInLOS) {
            throwCallback( "onTargetEnterLOS" );
            mTargetInLOS = true;
         }
      }
   }

   // Replicate the trigger state into the move so that
   // triggers can be controlled from scripts.
   for( int i = 0; i < MaxTriggerKeys; i++ )
      movePtr->trigger[i] = getImageTriggerState(i);


// start jc
//yorks in start
	switch (mPose) 
	{
	   case StandPose:
		   movePtr->trigger[3] = false; 
			movePtr->trigger[4] = false;
		   break;
		case CrouchPose:				
			movePtr->trigger[3] = true; 
			movePtr->trigger[4] = false;
		   break;
		case PronePose:
			movePtr->trigger[3] = false;
			movePtr->trigger[4] = true;  
		   break;
      default:
		   movePtr->trigger[3] = false; 
			movePtr->trigger[4] = false;
		break;

	}
//yorks in end
// end jc

   return true;
}

/**
 * Utility function to throw callbacks. Callbacks always occure
 * on the datablock class.
 *
 * @param name Name of script function to call
 */
void AIFishPlayer::throwCallback( const char *name )
{
   Con::executef(getDataBlock(), name, getIdString());
}

/**
 * Calculates the angle between two Point3F vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @param discardDimension the dimension (if any) to discard for calculation (1->x, 2->y, 3->z)
 */
F32 AIFishPlayer::getVectorAngle(const Point3F &v1, const Point3F &v2, U32 discardDimension)
{
   /*
	F32 a2,b2,c2;
	F32 sign = 0;
	switch (discardDimension)
	{
	case 1:
		a2 = Point2F(v2.y-v1.y,v2.z-v1.z).lenSquared();
		b2 = Point2F(v1.y,v1.z).lenSquared();
		c2 = Point2F(v2.y,v2.z).lenSquared();
		sign = ((-v1.z*v2.y+v1.y*v2.z)>0)?-1.0:1.0;//dot(tangent(v1),v2) 
		break;
	case 2:
		a2 = Point2F(v2.x-v1.x,v2.z-v1.z).lenSquared();
		b2 = Point2F(v1.x,v1.z).lenSquared();
		c2 = Point2F(v2.x,v2.z).lenSquared();
		sign = ((-v1.z*v2.x+v1.x*v2.z)>0)?-1.0:1.0;//dot(tangent(v1),v2)
		break;
	case 3:
		a2 = Point2F(v2.x-v1.x,v2.y-v1.y).lenSquared();
		b2 = Point2F(v1.x,v1.y).lenSquared();
		c2 = Point2F(v2.x,v2.y).lenSquared();
		sign = ((-v1.y*v2.x+v1.x*v2.y)>0)?-1.0:1.0;//dot(tangent(v1),v2)
		break;
	default:
		Con::printf("AIFishPlayer::getAngle - unsupported discardDimension %d", discardDimension);
		return (F32)0.0;
	}
	//Con::printf("v1=(%f,%f,%f) v2=(%f,%f,%f) ",v1.x,v1.y,v1.z,v2.x,v2.y,v2.z);
	//Con::printf("a2=%f   b2=%f   c2=%f",a2,b2,c2);
	F32 val = (b2+c2-a2)/(2*mSqrt(b2)*mSqrt(c2));
	if (val >= 1.0) return 0.0;
	if (val <= -1.0) return (F32)sign*M_PI;
	//Con::printf("val=%f",val);
	return (F32)sign*mAcos(val);
   */
   F32 atanv1, atanv2;
	switch (discardDimension)
	{
	case 1:
      atanv1 = mAtan2(v1.y, v1.z);
      atanv2 = mAtan2(v2.y, v2.z);
		break;
   case 2:
      atanv1 = mAtan2(v1.x, v1.z);
      atanv2 = mAtan2(v2.x, v2.z);
		break;
   case 3:
      atanv1 = mAtan2(v1.x, v1.y);
      atanv2 = mAtan2(v2.x, v2.y);
		break;
	default:
		Con::printf("AIFishPlayer::getAngle - unsupported discardDimension %d", discardDimension);
		return (F32)0.0;
   }
	F32 result = atanv2 - atanv1;
	if (result > M_PI_F)
	{
		result = -M_2PI_F + result;
	}
	if (result <= -M_PI_F)
	{
		result = M_2PI_F + result;
	}
   return result;

}
/**
 * returns the result of rotating a Point3F vector around a given axis
 * @param vec the vector to rotate
 * @param angle the angle to rotate clockwise by
 * @param axis to rotate around (1->x, 2->y, 3->z)
 */
// start ds - mac fix
/*
 inline Point3F AIFishPlayer::rotateVec(const Point3F &v,F32 angle, U32 axis)
*/
Point3F AIFishPlayer::rotateVec(const Point3F &v,F32 angle, U32 axis)
// end ds
 {
 	F32 cosAngle = mCos(angle);
 	F32 sinAngle = mSin(angle);
 	switch (axis)
 	{
 	case 1:
 		return Point3F(v.x,v.y*cosAngle - v.z*sinAngle,v.y*sinAngle + v.z*cosAngle);
 	case 2:
 		return Point3F(v.x*cosAngle - v.z*sinAngle,v.y,v.x*sinAngle + v.z*cosAngle);
 	case 3:
 		return Point3F(v.x*cosAngle - v.y*sinAngle,v.x*sinAngle + v.y*cosAngle,v.z);
 	default:
		return Point3F(v);
 	} 	
}
void AIFishPlayer::changePose(S32 poseNumber)
{
	Pose Pose = StandPose;
	if(poseNumber == 1) 
	{      
		Pose = CrouchPose;
	}
	else if(poseNumber == 2)
	{
		Pose = PronePose;
	}
	else if(poseNumber == 3)
	{
		Pose = SwimPose;
	}
	setPose(Pose);
}
//yorks end

//yorks start



bool AIFishPlayer::findClosestObjectInFront(F32 range, Collision *closest, U32 mask)
{
//	PROFILE_START(AIPlayer_findClosestObjectInFront);
   static const U32 sAvoidMask = (InteriorObjectType |
                                  PlayerObjectType       | StaticShapeObjectType |
                                  VehicleObjectType      | TerrainObjectType  );
   /*
	Point3F start;
   mWorldBox.getCenter(&start);
   Point3F dif;
   mObjBox.getCenter(&dif);
   Point3F finish;
   mObjToWorld.mulP(dif+VectorF(0.0,range,0.0),&finish);
	*/
	Point3F start = getBoxCenter();
	Point3F finish;
   mObjToWorld.getColumn(1,&finish);
	finish *= range;
	finish += start;
	bool result = mContainer->castRect(start,finish,mObjBox.len_x()*M_SQRT2,mObjBox.len_z(), sAvoidMask | mask, closest);
//	PROFILE_END(AIPlayer_findClosestObjectInFront);
	return result;
}
bool AIFishPlayer::onNewDataBlock(GameBaseData* dptr, bool reload)
{
 //  mDataBlock = dynamic_cast<AIPlayerData*>(dptr);
   mDataBlock = dynamic_cast<FishPlayerData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr, reload))
      return false;
   
//	mDataBlock->mStuckT

   scriptOnNewDataBlock();
   return true;
}
// end jc

void AIFishPlayer::endActivity()
{
   mActivity.time = 0;
   mActivity.shape = 0;
   mActivity.activity = ActivityNone;
}
bool  AIFishPlayer::isFleeing()
{
   return mInfluence[influenceFleeFish]->getWeight()>0;
}
AIFishPlayer   *AIFishPlayer::getScaryObject()
{
   return (AIFishPlayer *)mInfluenceFleeFish.getScaryObject();  //pghack
}
AIFishPlayer   *AIFishPlayer::getLeader()
{
   return mLeader;
}
//check for circular leadership
bool  AIFishPlayer::validLeader(AIFishPlayer *_who)
{

   if( herdId<0 || (herdId !=_who->herdId) ){
      return false;
   }
   for(AIFishPlayer *f = _who->getLeader(); f; f=f->getLeader())
   {
      if( f==this ){
         return false;
      }
   }
   return true;
}
F32   AIFishPlayer::getLeaderAttraction(AIFishPlayer *_leader)
{
   if( _leader->mEnableUserTouch )return 0;

   Point3F  toLeader = _leader->getPosition() - getPosition();
   F32   distanceAway = toLeader.len();
   F32   near = getPassingDistance()*2.0f;
   F32   far = mDataBlock->localFishRadius;

   //---
   F32   dot = mDot(getHeading(), _leader->getHeading());
   F32  lo=0.5f, hi=1.0f;
   dot = dot<0? 0 : dot;
   F32   directionAttraction = dot*(hi-lo) + lo;      

   //---
   F32   distanceAttraction;
   if( distanceAway<near ){
      distanceAttraction = 1.0f;
   }else if(distanceAway>far ){
      distanceAttraction = 0;
   }else{
      distanceAttraction = (distanceAway - near) / (far - near);
   }
   //--
   F32   infrontAttraction;
   lo = 0.0f;
   hi = 1.0f;
   if( distanceAway==0 ){
      infrontAttraction = lo;
   }else{
      Point3F  d = toLeader * (1.0f/distanceAway);
      F32   dot = mDot(d,getHeading());
      dot = dot>0? dot : 0;
      infrontAttraction = dot*(hi-lo) + lo;      
   }
   //---
   F32   fleeAttraction = _leader->isFleeing()? 1.0f : 0.5f;
   //---
   return  fleeAttraction * distanceAttraction * directionAttraction * infrontAttraction;
}
void  AIFishPlayer::updateLeader()
{
   //---search for new leader------------------------------------------
   AIFishPlayer   *leader = 0;
   F32            leaderAttraction = 0;

   for(int j=0; j<mLocalFishCount; j++){
      AIFishPlayer *f = mLocalFish[j];

      if( !validLeader(f) )continue;

      F32   attraction = getLeaderAttraction( f);

      if( attraction > leaderAttraction ){
         leaderAttraction = attraction;
         leader = f;
      }
   }
   if( !mLeader ){
      mLeader = leader;
      mBadLeadershipTime = 0;
   }else{
      bool  badLeader;
      if( validLeader(mLeader) ){
         badLeader = leaderAttraction >= getLeaderAttraction( mLeader );
      }else{
         badLeader = true;
      }
      //give up on current leader if someone is better for a given time
      if(badLeader){
         mBadLeadershipTime += TickSec;
      }else{
         mBadLeadershipTime = 0;
      }
      if( mBadLeadershipTime >= BAD_LEADER_TIME ){
         mLeader = leader;
         mBadLeadershipTime = 0;
      }
   }
}
void  AIFishPlayer::updateActivity()
{
/*
   switch(mActivity.activity){
   case ActivityEat:
   case ActivityHide:
      setInfluenceEnable(influenceFeed, true);
      if(mInfluenceFeed.isEating()){
         mActivity.time -= TickSec;
         if(mActivity.time<=0){
            endActivity();
         }
      }
      break;
   case ActivitySwim:
      if( mInfluenceBounds.isOutsideBounds() ){
         setInfluenceEnable(influenceBounds, true);
      }else{
         setInfluenceEnable(influenceWander, true);
         mActivity.time -= TickSec;
         if(mActivity.time<=0){
            endActivity();
         }
      }
      break;
   }
   */
}
bool  AIFishPlayer::scaleAnimationSpeed()
{
   //scale animation when moving
   return (!mEnableUserTouch) && Parent::scaleAnimationSpeed();
}
bool  AIFishPlayer::fullSpeed()
{
   return getScaryFish() || (mInfluenceFleeTrigger.getStrength()>0) ;
}
bool  AIFishPlayer::justCrashed()
{
   return !mInfluenceCrashRecover.recovered();
}
void AIFishPlayer::addCollisionAvoidance(Influence *_influence)
{
   if( mInfluenceAvoidCollision.getStrength()>0 ){
      mInfluenceAvoidCollision.setWeightTarget( mInfluenceAvoidCollision.getStrength() );
      _influence->setWeightTarget(1.0f - mInfluenceAvoidCollision.getStrength());
   }
}
//int   debugId=0;
void AIFishPlayer::think()
{
//   mDebugIt = getId()==debugId;
   if(!mSchooling && herdId>=0 ){
      updateLeader();
      mInfluenceFollowLeader.setLeader(mLeader);
   }
   setForwardOnly( true );            

   //---reset all influences-------------------------
   for(int j=0; j<influenceCount; j++){
      mInfluence[j]->setWeightTarget(0);
   }

   //---choose influence to use-------------------------
   if( mEnableUserTouch ){
      for(int j=0; j<influenceCount; j++){
         mInfluence[j]->setWeightInstant(0);
      }
      mInfluenceUserTouch.setWeightInstant(1);
   }else if( mSchooling ){
      mInfluenceSchool.setWeightInstant(1);

   }else if( !mInfluenceCrashRecover.recovered() ){
//      setForwardOnly( false );            
      mInfluenceCrashRecover.setWeightTarget(1);
      mInfluenceWander.reset(&mInfluenceCrashRecover.mDirection);

   }else if( mInfluenceBounds.isOutsideBounds() ){
      mInfluenceBounds.setWeightTarget(1.0f);
      addCollisionAvoidance(&mInfluenceBounds);

   }else if( mInfluenceFleeTrigger.getStrength()>0 ){
      mInfluenceFleeTrigger.setWeightTarget(1);
      addCollisionAvoidance(&mInfluenceFleeTrigger);

   }else if(   ((mDataBlock->fishTypeFlags&FishFlagSwish)) && mInfluenceSwish.isSwishing() ){
      mInfluenceSwish.setWeightTarget(1);

   }else if( getScaryFish() ){
      mInfluenceFleeFish.setWeightTarget(1.0f);
      addCollisionAvoidance(&mInfluenceFleeFish);

   }else if( mLeader ){
      mInfluenceFollowLeader.setWeightTarget(1.0f);
      addCollisionAvoidance(&mInfluenceFollowLeader);
   
   }else{
      //make fish wander about unless some collision about in which case just avoid that
      if( mInfluenceAvoidCollision.getStrength()>0){
         mInfluenceAvoidCollision.setWeightTarget(1.0f);
         Point3F  heading = getHeading();
         mInfluenceWander.reset(&heading); //when leave collision, will continue on wandering in same direction
      }else{
         mInfluenceWander.setWeightTarget(1.0f);
      }
   }
}
void AIFishPlayer::searchForScaryFish()
{
   //---see if new monster to run away from--------------------------------------
   AIFishPlayer *monster = 0;
   F32   monsterDistanceSq = FLT_MAX;
#if   0
   gServerContainer.initRadiusSearch(getPosition(), FLEE_START_DISTANCE, PlayerObjectType);
   while (AIFishPlayer *fish = (AIFishPlayer *)gServerContainer.containerSearchNextObject())
   {
#else
   // this version reduces load from 0.9% to 0.1%
   mTrafficController->initRadiusSearch(getPosition(), mDataBlock->fleeStartDistance);
   while(AIFishPlayer *fish = mTrafficController->nextSearch())
   {
#endif
      if( fish==this )continue;
      if( ! (fish->mDataBlock->menu & mDataBlock->fishTypeFlags)  )continue;   //if not on this fishes menu, dont be scared

      F32   d = (fish->getPosition() - getPosition()).lenSquared();
      if( monsterDistanceSq > d ){
         monsterDistanceSq = d;
         monster = fish;
      }
   }
   if(monster){
      mInfluenceFleeFish.setScaryObject(monster);
   }
}
//*******************************************************
// update list of fish in my vicinity.
// nearest fish 1st in list
//*******************************************************
void AIFishPlayer::localFishCheck()
{
#if   0
   gServerContainer.initRadiusSearch( getPosition(), LOCAL_FISH_RADIUS, PlayerObjectType );
   mLocalFishCount = 0;
   while( AIFishPlayer *fish = (AIFishPlayer *)gServerContainer.containerSearchNextObject() ){
#else
   //this reduces load of this function from 2.5% -> 0.3%
   mTrafficController->initRadiusSearch(getPosition(), mDataBlock->localFishRadius);
   while(AIFishPlayer *fish = mTrafficController->nextSearch())
   {
#endif
      if( fish==this )continue;
      F32   distanceSq = (fish->getPosition()-getPosition()).lenSquared();
      int   iReplace = -1;
      for(int j=0; j<mLocalFishCount; j++){
         F32            sq = (mLocalFish[j]->getPosition()-getPosition()).lenSquared();
         if(distanceSq<sq){
            iReplace = j;
            break;
         }
      }
      if( iReplace==-1 ){
         if(mLocalFishCount<MAX_LOCAL_FISH){
            mLocalFish[mLocalFishCount++] = fish;
         }
      }else{
         for(int iTo=mLocalFishCount+1; iTo>iReplace; iTo--){
            if( iTo<MAX_LOCAL_FISH ){
               mLocalFish[iTo] = mLocalFish[iTo-1];
            }
         }
         mLocalFish[iReplace] = fish;
         mLocalFishCount++;
         if(mLocalFishCount>MAX_LOCAL_FISH){
            mLocalFishCount = MAX_LOCAL_FISH;
         }
      }
   }
}
static const F32  STUCK_TOLERANCE = 0.005f;
//static const F32  STUCK_TIME = 0.01f;
void AIFishPlayer::stuckCheck()
{
   //if havent move more than STUCK_TOLERANCE in the interval at which this is called, 
   //we are counted as stuck
   F32   distance = (getPosition()-mLastStuckCheckPosition).len();
   mLastStuckCheckPosition = getPosition();

   if( distance<STUCK_TOLERANCE ){
      onMoveStuck();
   }
}
//*******************************************************
bool AIFishPlayer::missedPlayerCollision(Obstacle *o)
{
   Point3F heading;
   mObjToWorld.getColumn(1,&heading);

   Point3F  toObstacle = o->object->getPosition() - getPosition();
   return   mDot(toObstacle,heading) < 0;
}
//*******************************************************
bool AIFishPlayer::missedReefCollision(Obstacle *o)
{
   Point3F heading;
   mObjToWorld.getColumn(1,&heading);
   return mDot(o->normal, heading) > 0;
}
Obstacle   *AIFishPlayer::getFutureCollision()
{ 
   return mGameTime - mObstacle.time < COLLISION_AVOID_TIME?  &mObstacle : 0;
}
float    AIFishPlayer::getFutureCollisionStrength()
{
   float timeSince = mGameTime - mObstacle.time;
   if( timeSince>=COLLISION_AVOID_TIME )
   {
      return 0;
   }
   else
   {
      return (COLLISION_AVOID_TIME-timeSince) / COLLISION_AVOID_TIME;
   }
}

F32   AIFishPlayer::getCollisionWarningDistance()
{
   F32   bestDistance = COLLISION_DISTANCE;
   F32   radiusCapsule = getCapsuleSize().x;
   F32   speed = mTrueVelocity.len();
   bestDistance = radiusCapsule  + speed*COLLISION_WARNING_TIME;
   return bestDistance;
}
AIFishPlayer   *AIFishPlayer::sTestFish=0;
bool AIFishPlayer::castRayCallback( RayInfo* ri )
{
   if( ri->object->getTypeMask() & PlayerObjectType ){
      AIFishPlayer   *other = (AIFishPlayer *)ri->object;
      if( other->getCapsuleSize().x < sTestFish->getCapsuleSize().x ){
         return false;  //dont care about fish smaller than me
      }else{
         //see if actually on collision course
         //sep = (o+ov*t) - (m+mv*t)
         //sep = (o-m) + (ov-mv)*t
         //sep = a + bt
         //d2 = a.a + 2(a.b)t + (b.b)t^2 
         //hit when d^2 = passing distance^2
         //At^2 + Bt + D = passing distance^2:   A = b.b B = 2a.b   D = a.a  
         //At^2 + Bt + C = 0:   A = b.b  B = 2a.b   C = D-passing distance^2 = a.a - passing distance^2

         Point3F  otherP = other->getPosition();
         Point3F  meP = sTestFish->getPosition();
         F32   passingDistance = sTestFish->getPassingDistance() + other->getPassingDistance();
         Point3F  a = otherP - meP;
         F32      distance = a.len();
         if( distance<passingDistance ){
            return true;   //overlap
         }else{
            Point3F b = other->mTrueVelocity - sTestFish->mTrueVelocity;
            F32   A = mDot(b,b);
            F32   B = 2*mDot(a,b);
            F32   C = mDot(a,a) - passingDistance*passingDistance;
            F32   times[2];
            U32   nTimes = mSolveQuadratic(A, B, C, &times[0]);
            for(U32 j=0; j<nTimes; j++){
               if(times[j]>=0 && times[j]<=COLLISION_WARNING_TIME){
                  return true;   
               }
            }
            return false;
         }
      }
   }else{
      return true;
   }
}

#if   1
//*******************************************************
// check ahead for anything might hit
//*******************************************************
void AIFishPlayer::obstacleCheck()
{
   //find closest imminent collision
   F32   bestDistance = getCollisionWarningDistance();

   SceneObject *bestObject=0;
   Point3F     bestNormal;

   RayInfo  c;

   Point3F start = getBoxCenter();
   Point3F direction;
   mObjToWorld.getColumn(1,&direction);

   disableCollision();

   sTestFish = this;
#ifdef   RADAR_RAY_CAST
   mRadarPhase++;
   int   phase = ( mRadarPhase %(RADAR_CONE_POINTS+1) );
   if( phase==0 ){
      //straight ahead
      Point3F  finish = start + (bestDistance*direction);
      bool  hit  = mContainer->castRay(   start,
                                          finish,
                                          StaticShapeObjectType | TerrainObjectType | PlayerObjectType, 
                                          &c,
                                          castRayCallback);
      if( hit  ){
         bestObject = c.object;
         bestNormal = c.normal;
         bestDistance = c.t;
      }
   }else{
      //directions paralell to direction in square face centered in fish
      Point3F  upVector = Point3F(0,0,1.0f);
      F32   ff = mDot(upVector,direction);
      upVector = upVector - ff*direction;
      F32   size = upVector.len();
      if( size ){
         upVector *= (1.0f/size);
      }
      //---
      Point3F  rightVector = Point3F(-direction.y, direction.x, 0);
      size = rightVector.len();
      if(size>0){
         rightVector*=(1.0f/size);
      }
      //---
#if   1
      F32   angle = mRandF(0, M_2PI_F);
      F32   fraction = mRandF(0.0f, 1.0f);
      F32   startRadius = fraction*getCapsuleSize().x;
      F32   endRadius = startRadius + bestDistance*sinf(RADAR_CONE_ANGLE);
      Point3F  radialVector = (cosf(angle)*rightVector) + (sinf(angle)*upVector);
      Point3F  finish = start + (bestDistance*direction);
      start += (radialVector*startRadius);
      finish += (radialVector*endRadius);
#else
      F32   startRadius = getCapsuleSize().x;
//      F32   endRadius = startRadius + bestDistance*sinf(RADAR_CONE_ANGLE);
      F32   x = mRandF(-startRadius, + startRadius);
      F32   y = mRandF(-startRadius, + startRadius);
      Point3F  offset = (x*rightVector) + (y*upVector);
      //---
      start += offset;
      Point3F  finish = start + (bestDistance*direction);
#endif

      bool  hit  = mContainer->castRay(   start,
                                          finish,
                                          StaticShapeObjectType | TerrainObjectType | PlayerObjectType, 
                                          &c,
                                          castRayCallback);
      if( hit ){
         bestObject = c.object;
         bestNormal = c.normal;
         bestDistance = c.t;
      }
   }
#else
   Point3F  finish = start + (COLLISION_DISTANCE*direction);
   bool  hit  = mContainer->castRay(   start,
                                       finish,
                                       StaticShapeObjectType | TerrainObjectType | PlayerObjectType, 
                                       &c,
                                       NULL);
   if( hit  ){
      bestObject = c.object;
      bestNormal = c.normal;
      bestDistance = c.t;
   }
#endif
   enableCollision();

//   AIFishPlayer   *f = mTrafficController->getFishCollision(this);
   AIFishPlayer   *f = 0;
   if( f ){
      Point3F  n = (getPosition() - f->getPosition());
      F32   distance = n.len();
      if( distance<bestDistance ){
         bestDistance = distance;
         bestObject = f;
         bestNormal = distance? n * (1.0f/distance) : Point3F(1.0f, 0, 0);
      }
   }
   if( bestObject ){
      mObstacle.object = bestObject;
      mObstacle.normal = bestNormal;
      mObstacle.position = start + (bestDistance*direction);
      mObstacle.time = mGameTime;
   }
}
#else
void AIFishPlayer::obstacleCheck()
{
   //see if passed last collision
   if( mObstacle.active ){
      F32   collisionDistance = (getPosition() - mObstacle.point).len();
      if( collisionDistance <= COLLISION_DISTANCE )
      {
         if( mObstacle.object->getTypeMask() & PlayerObjectType ){
            if( !missedPlayerCollision(&mObstacle) ){
               return;
            }
         }else{
            if( !missedReefCollision(&mObstacle) ){
               return;
            }
         }
      }
   }
   mObstacle.active = false;

   F32   checkDistance = COLLISION_DISTANCE;

   Point3F start = getBoxCenter();
   Point3F direction;
   mObjToWorld.getColumn(1,&direction);
   Point3F  finish = start + (checkDistance*direction);

   // this version reduces load from 2.456 -> 0.341% but misses some obstacles
   RayInfo  c;
   disableCollision();
   bool  hit  = mContainer->castRay(   start,
                                       finish,
                                       StaticShapeObjectType | TerrainObjectType | PlayerObjectType, 
                                       &c,
                                       NULL);
   enableCollision();
   if( hit ){
      //---imminent collision-------------------------------
      mObstacle.active = true;
      mObstacle.object = c.object;
      mObstacle.normal = c.normal;
      mObstacle.point = c.point;
   }else{
      //---no collision for forseeable future-------------------------------
      mObstacle.active = false;
   }
}
#endif

//*************************************************************
// using ai to move
//*************************************************************
void  AIFishPlayer::processTickAi()
{
   for(int j=0; j<AlarmCount; j++){
      mAlarm[j].update();
   }
   //------------------------------
   if( (mDataBlock->fishTypeFlags&FishFlagFlee)  && mAlarm[AlarmScaryFishSearch].isRinging() ){
      searchForScaryFish();
   }
   if( mAlarm[AlarmStuckCheck].isRinging() ){
      stuckCheck();
   }
   think();
   if( mAlarm[AlarmObstacleCheck].isRinging() ){
      obstacleCheck();
   }
   if( mAlarm[AlarmLocalFishCheck].isRinging() ){
      localFishCheck();
   }
   for(int j=0; j<influenceCount; j++){
      mInfluence[j]->update();
   }
#ifdef   DEBUG_GFX
   debugDrawing();
#endif
}
const static F32  BUBBLE_DISTANCE = 5.0f;
const static F32  BUBBLE_RADIUS = 1.0f;
const static F32  BUBBLE_TRANSITION_TIME = 1.0f;
//*************************************************************
// trapped in bubble being controlled by user touch
//*************************************************************
void  AIFishPlayer::processTickBubble()
{
   //scale to fixed size and distance from camera
      
}
void  AIFishPlayer::initBubble()
{
}

void AIFishPlayer::processTick(const Move* move)
{
   PROFILE_SCOPE(AIFishPlayer_ProcessTick);
   Parent::processTick(move);
   if(!isGhost() ){
      processTickAi();
   }
#ifdef   DEBUG_HUD
   showLog();
#endif
}
void  AIFishPlayer::onEnterTrigger(Trigger *_sender, int _type)
{
   switch(_type){
   default:
      break;
   case TriggerTypeSwish:
      if( mDataBlock->fishTypeFlags&FishFlagSwish )
      {
         mInfluenceSwish.onEnter(_sender);
      }
      break;
   case TriggerTypeScare:
      if( mDataBlock->fishTypeFlags&FishFlagFlee )
      {
         mInfluenceFleeTrigger.onEnter(_sender);
      }
      break;
   }
}
void  AIFishPlayer::onLeaveTrigger(Trigger *_sender, int _type)
{
   switch(_type){
   default:
      break;
   case TriggerTypeSwish:
      if( mDataBlock->fishTypeFlags&FishFlagSwish )
      {
         mInfluenceSwish.onExit(_sender);
      }
      break;
   case TriggerTypeScare:
      if( mDataBlock->fishTypeFlags&FishFlagFlee )
      {
         mInfluenceFleeTrigger.onExit(_sender);
      }
      break;
   }
}
F32 AIFishPlayer::getYaw()
{
  return mRot.z;
}
F32 AIFishPlayer::getPitch()
{
  return mHead.x;
}
void     AIFishPlayer::wanderSetRange(Point3F _lo, Point3F _hi)
{
   mWanderLo = _lo;
   mWanderHi = _hi;
}
void     AIFishPlayer::wanderGetRange(Point3F &_lo, Point3F &_hi)
{
   _lo = mWanderLo;
   _hi = mWanderHi;
}

void  AIFishPlayer::setHerdId(S32 _herdId)
{
   herdId = _herdId;
}
bool  AIFishPlayer::setActivity(StaticShape *_shape, F32 _time, const char *_name)
{
   mActivity.shape = _shape;
   mActivity.time = _time;

   if( !_stricmp(_name,"feeding") ){
      mActivity.activity = ActivityEat;
      mInfluenceFeed.setFood(_shape);
      return true;
   }else if( !_stricmp(_name,"hiding") ){
      mActivity.activity = ActivityHide;
      mInfluenceFeed.setFood(_shape);
      return true;
   }else if( !_stricmp(_name,"swimming") ){
      mActivity.activity = ActivitySwim;
      return true;
   }else{
      mActivity.activity = ActivityNone;
      Con::printf("Dont know how to perform %s",_name);
      return false;
   }
}

#ifdef   DEBUG_HUD
void  AIFishPlayer::addLogFormatted(const char * format, ...)
{
   va_list parg;
   va_start(parg, format);

   static   char  buffer[128];

   vsprintf(&buffer[0],format, parg);
   va_end(parg);
   addLog(&buffer[0]);

}
void  AIFishPlayer::addLog(const char* _message)
{
   char  *output;
   output = mLog[mLogCount%FISH_MAXLOG];
   mLogCount++;
   sprintf(output,"%d.%s (%6.1f)\n",mLogCount,_message,getTimeSeconds());
}
void  AIFishPlayer::addText(char *base, int *iBase, char *add)
{
   if(*iBase==255){
      return;
   }
   int   i = *iBase;
   while(*add){
      if(i==255){
         break;
      }
      base[i++] = *add++;
   }
   *iBase = i;
}
void  AIFishPlayer::showLog()
{
   if(!mDebugLog){
      setShapeName("");
   }else{
      static   char  ss[256];
      static   char  temp[256];
      int      iTemp = 0;

      sprintf(&ss[0], "id=%d\n", getId());
      addText(&temp[0], &iTemp, &ss[0]);

      sprintf(&ss[0], "speed=%6.3f\n", mTrueVelocity.len());
      addText(&temp[0], &iTemp, &ss[0]);
//      sprintf(&ss[0], "col/rad = %3.2f,%3.2f\n", getCollisionWarningDistance(),getCapsuleSize().x);
//      addText(&temp[0], &iTemp, &ss[0]);

/*
      const char *name;
      switch(mActivity.activity)
      {
      case ActivityEat: name = "Eat";  break;
      case ActivityHide: name = "Hide";  break;
      case ActivitySwim: name = "Swim";  break;
      case ActivityNone: name = "-";  break;
      default:             name = "?"; break;
      }

      sprintf(&ss[0], "Activity = %s\n", name);
      addText(&temp[0], &iTemp, &ss[0]);
      */

      if( ShapeBase *monster = mInfluenceFleeFish.getScaryObject() ){
         sprintf(&ss[0], "flee=%d\n", monster->getId());
         addText(&temp[0], &iTemp, &ss[0]);
      }
      //---
      F32   pitch = mRadToDeg(make2PiRange(getPitch() + M_2PI_F*0.5f));
      pitch = -(pitch-180.0f);
      sprintf(&ss[0], "pitch: %d\n", (int)pitch );
      addText(&temp[0], &iTemp, &ss[0]); 
      //---
      sprintf(&ss[0], "yaw: %d\n", (int)mRadToDeg(make2PiRange(getYaw())) );
      addText(&temp[0], &iTemp, &ss[0]); 
      //---
      for(int j=0; j<influenceCount; j++){
         Influence   *i = mInfluence[j];
         F32   weight = i->getWeight();
         if(!weight)continue;

         Point3F  &v = i->mDirection;
         sprintf(&ss[0], "%s w=%6.2f: s=%6.2f (%6.1f, %6.1f %6.1f)\n",i->getName(),i->getWeight(),i->mMoveSpeed,
                                                            v.x,v.y,v.z
                                                            );
         addText(&temp[0], &iTemp, &ss[0]);
      }
      if(mInfluenceAvoidCollision.getWeight()>0){
         Obstacle *o = getFutureCollision();
         if( o ){
            sprintf(&ss[0], "Obstacle = %d (%s)\n",
               o->object->getId(),
               (o->object->getTypeMask() & PlayerObjectType)? "Fish" : "Reef"
            );
            addText(&temp[0], &iTemp, &ss[0]);
         }
      }
      addText(&temp[0], &iTemp, "\n");

      int iOldest = mLogCount - FISH_MAXLOG;
      if(iOldest<0)iOldest = 0;
      for(int index=iOldest; index<mLogCount; index++){
         addText(&temp[0], &iTemp, mLog[index%FISH_MAXLOG]);
      }
      temp[iTemp] = 0;
      setShapeName(&temp[0]);
   }
}
#endif


void  AIFishPlayer::onMoveStuck()
{
   mInfluenceCrashRecover.trigger();
/*
   mUnsticking = true;
   mAlarm[AlarmUnstickWait].setInterval(ALARMPERIOD_UNSTICK);
   mAlarm[AlarmUnstickWait].restart();
   
   F32   yaw = gRandGen.randF(0, M_2PI_F);
   F32   pitch = gRandGen.randF(mDataBlock->minLookAngle, mDataBlock->maxLookAngle);

   Point3F  d;
   mSinCos(yaw, d.x, d.y);
   F32      s,c;
   mSinCos(pitch, s, c);
   d.x *= c;
   d.y *= c;
   d.z = s;
   mInfluenceWander.reset(&d);
   */
}
bool  AIFishPlayer::ignoreCollision()
{
   return mSchooling;
}

void  AIFishPlayer::onHitReef(const Collision &collision )
{
   mLastHardCollisionTime = mGameTime;
#ifdef   OLD_CRASH
   mInfluenceCrashRecover.addCrashEvent(&collision);
#endif
}
void           AIFishPlayer::onHitFish(const Collision &collision )
{
//   mInfluenceCrashRecover.addCrashEvent(&collision);
}
void     AIFishPlayer::setEnableSchool(bool a){
   mSchooling = a;
}
void  AIFishPlayer::enableUserTouch(bool _enable)
{
   mEnableUserTouch = _enable;
}
#ifdef   DEBUG_GFX
//*****************************************************************************
// debug drawing
//*****************************************************************************
static const ColorI  leaderColor = ColorI(255,255,255,255);
static const ColorI  scaryFishColor = ColorI(255,0,0,255);
static const ColorI  scaryTriggerColor = ColorI(255,242,0,255);
static const ColorI  activityColor = ColorI(0,255,0,255);
static const ColorI  capsuleColor = ColorI(255,255,255,16);
static const ColorI  textColor = ColorI(153,217,234,255);
static const ColorI  textColorOutline = ColorI(0,0,0,255);
static const ColorI  textColor_crashed = ColorI(255,0,0,255);
static const ColorI  textColorOutline_crashed = ColorI(255,255,255,255);

void  AIFishPlayer::debugDrawing()
{
//   drawCapsule();
//   drawInfluence();
   printInfluence();
#ifdef   DEBUG_HUD
   if(mDebugLog){
//      drawWanderBox();
   }
#endif
}
void  AIFishPlayer::drawWanderBox()
{
   DebugDrawer *ddraw = DebugDrawer::get();
//   ddraw->drawBox(mWanderLo, mWanderHi, ColorF(1.0f, 0.0f, 0.0f, 0.3f) );



   ColorF   color = ColorF(1.0f, 0.0f, 0.0f, 1.0f);

   Point3F  &v0 = mWanderLo;
   Point3F  &v1 = mWanderHi;

   ddraw->drawLine( Point3F(v0.x, v0.y, v0.z),  Point3F(v1.x, v0.y, v0.z), color);
   ddraw->drawLine( Point3F(v0.x, v1.y, v0.z),  Point3F(v1.x, v1.y, v0.z), color);
   ddraw->drawLine( Point3F(v0.x, v0.y, v1.z),  Point3F(v1.x, v0.y, v1.z), color);
   ddraw->drawLine( Point3F(v0.x, v1.y, v1.z),  Point3F(v1.x, v1.y, v1.z), color);

   ddraw->drawLine( Point3F(v0.x, v0.y, v0.z),  Point3F(v0.x, v0.y, v1.z), color);
   ddraw->drawLine( Point3F(v1.x, v0.y, v0.z),  Point3F(v1.x, v0.y, v1.z), color);
   ddraw->drawLine( Point3F(v0.x, v1.y, v0.z),  Point3F(v0.x, v1.y, v1.z), color);
   ddraw->drawLine( Point3F(v1.x, v1.y, v0.z),  Point3F(v1.x, v1.y, v1.z), color);

   ddraw->drawLine( Point3F(v0.x, v0.y, v0.z),  Point3F(v0.x, v1.y, v0.z), color);
   ddraw->drawLine( Point3F(v0.x, v0.y, v1.z),  Point3F(v0.x, v1.y, v1.z), color);
   ddraw->drawLine( Point3F(v1.x, v0.y, v0.z),  Point3F(v1.x, v1.y, v0.z), color);
   ddraw->drawLine( Point3F(v1.x, v0.y, v1.z),  Point3F(v1.x, v1.y, v1.z), color);
   ddraw->setLastTTL(0);
}
void  AIFishPlayer::drawInfluence()
{
   if( getLeader() ){
      drawRelationship(getLeader(), leaderColor);
   }
   drawRelationship(getScaryFish(), scaryFishColor);
   for(int j=0; j<mInfluenceFleeTrigger.getTriggerCount(); j++){
      drawRelationship(mInfluenceFleeTrigger.getTrigger(j)->getTransform().getPosition(), scaryTriggerColor);
   }
   switch(mActivity.activity){
   case ActivityEat:
   case ActivityHide:
   case ActivitySwim:
      drawRelationship(mActivity.shape->getPosition(), activityColor);
      break;
   }
}
void  AIFishPlayer::drawCapsule()
{
   DebugDrawer *ddraw = DebugDrawer::get();
   Point3F  p = getPosition();
   Point3F  capsuleSize = getCapsuleSize();
   F32   capsuleRadius = capsuleSize.x;
   F32   capsuleHeight = capsuleSize.y;
   //height seems to be 1/2 distance between caps in drawing code
   ddraw->drawCapsule( p, capsuleRadius, capsuleHeight*0.5f,  capsuleColor );
   ddraw->setLastTTL(0);
}
void  AIFishPlayer::printInfluence()
{
   String   influence = "";
   bool  blankString = true;
   for(int j=0; j<influenceCount; j++){
      if( mInfluence[j]->getWeight()>0 ){
         if(!blankString){
            influence += " + ";
         }
         blankString = false;
         switch(j){
         case influenceWander:         influence += "W"; break;
         case influenceSwish:          influence += "SW"; break;
         case influenceSchool:         influence += "S"; break;
         case influenceFleeFish:       influence += "FF"; break;
         case influenceFollowLeader:   influence += "FL"; break;
         case influenceFeed:           influence += "A"; break;
         case influenceAvoidCollision: influence += "C"; break;
         case influenceBounds:         influence += "B"; break;
         case influenceUserTouch:      influence += "T"; break;
         case influenceCrashRecover:   influence += "CR"; break;
         case influenceFleeTrigger:    influence += "FT"; break;
         default:                      influence += "?"; break;
         }
      }
   }
   DebugDrawer *ddraw = DebugDrawer::get();
   Point3F  textPos = getPosition();
   textPos.z += getPassingDistance();
   if( justCrashed() ){
      ddraw->drawOutlinedText(textPos, influence, textColor_crashed, textColorOutline_crashed);
   }else{
      ddraw->drawOutlinedText(textPos, influence, textColor, textColorOutline);
   }
   ddraw->setLastTTL(0);
}
void  AIFishPlayer::drawRelationship(ShapeBase *who, ColorI _color)
{
   if( who ){
      drawRelationship(who->getPosition(), _color);
   }
}
void  AIFishPlayer::drawRelationship(Point3F _where, ColorI _color)
{
   DebugDrawer *ddraw = DebugDrawer::get();
   ddraw->drawDirectionLine( getPosition(), _where, _color );
   ddraw->setLastTTL(0);
}

#endif

// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------
DefineEngineMethod( AIFishPlayer, setSchoolTarget, void, (Point3F _schoolOffset, ShapeBase *_target,F32 _near,F32 _far),,
   "@brief")
{
	object->getSchoolInfluence()->setTarget(_schoolOffset, _target,_near,_far);
}
DefineEngineMethod( AIFishPlayer, enableSchool, void, (bool _enable),,
   "@brief")
{
	object->setEnableSchool(_enable);
}

DefineEngineMethod( AIFishPlayer, enableUserTouch, void, (bool _enable),,
   "@brief")
{
	object->enableUserTouch(_enable);
}

DefineEngineMethod( AIFishPlayer, stop, void, ( ),,
   "@brief Tells the AIFishPlayer to stop moving.\n\n")
{
   object->stopMove();
}

DefineEngineMethod( AIFishPlayer, clearAim, void, ( ),,
   "@brief Use this to stop aiming at an object or a point.\n\n"
   
   "@see setAimLocation()\n"
   "@see setAimObject()\n")
{
   object->clearAim();
}

DefineEngineMethod( AIFishPlayer, setMoveSpeedMultiplier, void, ( F32 speed ),,
   "@brief Sets the move speed for an AI object.\n\n"

   "@param speed A speed multiplier between 0.0 and 1000.0.  "
   "This is multiplied by the AIFishPlayer's base movement rates (as defined in "
   "its FishPlayerData datablock)\n\n"
   
   "@see getMoveDestination()\n")
{
	object->setMoveSpeedMultiplier(speed);
}

// start jc
DefineEngineMethod( AIFishPlayer, getMoveSpeedMultiplier, F32, ( void ),,
   "@brief Gets the move speed for an AI object.\n\n"

   "@param speed A speed multiplier between 0.0 and 1000.0.  "
   "This is multiplied by the AIFishPlayer's base movement rates (as defined in "
   "its FishPlayerData datablock)\n\n"
   
   "@see getMoveDestination()\n")
{
	return object->getMoveSpeedMultiplier();
}
// end jc

DefineEngineMethod( AIFishPlayer, setMoveDestination, void, ( Point3F goal, bool slowDown ), ( true ),
   "@brief Tells the AI to move to the location provided\n\n"

   "@param goal Coordinates in world space representing location to move to.\n"
   "@param slowDown A boolean value. If set to true, the bot will slow down "
   "when it gets within 5-meters of its move destination. If false, the bot "
   "will stop abruptly when it reaches the move destination. By default, this is true.\n\n"

   "@note Upon reaching a move destination, the bot will clear its move destination and "
   "calls to getMoveDestination will return \"0 0 0\"."
   
   "@see getMoveDestination()\n")
{
   object->setMoveDestination( goal, slowDown);
}

DefineEngineMethod( AIFishPlayer, getMoveDestination, Point3F, (),,
   "@brief Get the AIFishPlayer's current destination.\n\n"

   "@return Returns a point containing the \"x y z\" position "
   "of the AIFishPlayer's current move destination. If no move destination "
   "has yet been set, this returns \"0 0 0\"."
   
   "@see setMoveDestination()\n")
{
	return object->getMoveDestination();
}

DefineEngineMethod( AIFishPlayer, setAimLocation, void, ( Point3F target ),,
   "@brief Tells the AIFishPlayer to aim at the location provided.\n\n"

   "@param target An \"x y z\" position in the game world to target.\n\n"
   
   "@see getAimLocation()\n")
{
	object->setAimLocation(target);
}

DefineEngineMethod( AIFishPlayer, getAimLocation, Point3F, (),,
   "@brief Returns the point the AIFishPlayer is aiming at.\n\n"

   "This will reflect the position set by setAimLocation(), "
   "or the position of the object that the bot is now aiming at.  "
   "If the bot is not aiming at anything, this value will "
   "change to whatever point the bot's current line-of-sight intercepts."

   "@return World space coordinates of the object AI is aiming at. Formatted as \"X Y Z\".\n\n"
   
   "@see setAimLocation()\n"
   "@see setAimObject()\n")
{
	return object->getAimLocation();
}


ConsoleMethod( AIFishPlayer, setAimObject, void, 3, 4, "( GameBase obj, [Point3F offset] )"
              "Sets the bot's target object. Optionally set an offset from target location."
			  "@hide")
{
   Point3F off( 0.0f, 0.0f, 0.0f );

   // Find the target
   GameBase *targetObject;
   if( Sim::findObject( argv[2], targetObject ) )
   {
      if (argc == 4)
         dSscanf( argv[3], "%g %g %g", &off.x, &off.y, &off.z );

      object->setAimObject( targetObject, off );
   }
   else
      object->setAimObject( 0, off );
}

DefineEngineMethod( AIFishPlayer, getAimObject, S32, (),,
   "@brief Gets the object the AIFishPlayer is targeting.\n\n"

   "@return Returns -1 if no object is being aimed at, "
   "or the SimObjectID of the object the AIFishPlayer is aiming at.\n\n"
   
   "@see setAimObject()\n")
{
	GameBase* obj = object->getAimObject();
   return obj? obj->getId(): -1;
}
// start jc
ConsoleMethod( AIFishPlayer, setMoveDestAndAppVec, void, 4, 5, "(Point3F goal, Point3F appVec, bool slowDown=true)"
              "Tells the AI to move to the location provided.")				
{																			
   Point3F v( 0.0f, 0.0f, 0.0f );											
   Point3F v2( 0.0f, 0.0f, 0.0f );											
   dSscanf( argv[2], "%f %f %f", &v.x, &v.y, &v.z );						
   dSscanf( argv[3], "%f %f %f", &v2.x, &v2.y, &v2.z );						
   bool slowdown = (argc > 4)? dAtob(argv[4]): true;						
   object->setMoveDestination( v, v2, slowdown);							
}

ConsoleMethod( AIFishPlayer, clearFollowObject, void, 2, 2, "()"							
              "Stop following.")									
{																			
   object->clearFollowObject();														
}	
ConsoleMethod( AIFishPlayer, setFollowObject, void, 3, 4, "( ShapeBase obj, bool slowDown=true)"	\
              "Tells the AI to follow the object.")				\
{																			\
   ShapeBase *targetObject;													\
   if( Sim::findObject( argv[2], targetObject ) )							\
   {																		\
      bool slowdown = (argc > 3)? dAtob(argv[3]): true;						\
      object->setFollowObject( targetObject, slowdown );							\
   }																		\
   else																		\
   {																		\
      Con::warnf("%s::setFollowObject - unable to find object %s","AIClass",argv[2]);          \
   }																		\
}			
ConsoleMethod( AIFishPlayer, setFollowObjectNode, void, 3, 6, "( ShapeBase obj, char* nodeName, bool slowDown=true)"	\
              "Tells the AI to follow the object.")				\
{																			\
   ShapeBase *targetObject;													\
   if( Sim::findObject( argv[2], targetObject ) )							\
   {																		\
      S32 nodeIndex = (argc > 3)? targetObject->findNode(argv[3]) : 0;		\
      bool slowdown = (argc > 4)? dAtob(argv[4]): true;						\
      object->setFollowObjectNode( targetObject, nodeIndex, slowdown );		\
   }																		\
   else																		\
   {																		\
      Con::warnf("%s::setFollowObject - unable to find object %s","AIClass",argv[2]);          \
   }																		\
}			
//      S32 slotIndex = (argc > 4)? dAtoi(argv[4]): 0;						\
// end jc		

// start jc

//yorks start
DefineEngineMethod( AIFishPlayer, setPose, void, (U32 pose),,
			  "(int pose) StandPose=0,CrouchPose=1,PronePose=2")
{	
   object->changePose(pose);
}//yorks end
// end jc


// start pg
//**************************************************
// script functions
//**************************************************
DefineEngineMethod( AIFishPlayer, setEnableSchool, void, (bool _enable),,
   "@brief ")
{
   object->setEnableSchool(_enable);
}
DefineEngineMethod( AIFishPlayer, wanderSetRange, void, (Point3F _lo, Point3F _hi),,
   "@brief Set box range for a fish to wander in. Will try and stay in range but may leave it if scared away or if avoiding collision.")
{
   object->wanderSetRange(_lo, _hi);
}
DefineEngineMethod( AIFishPlayer, addLog, void, (const char* _message),,
   "@brief Add log message to debug text above fish")
{
#ifdef   DEBUG_HUD
   object->addLog(_message);
#endif
}
DefineEngineMethod( AIFishPlayer, getDebugLog, bool, (),,
   "@brief ")
{
#ifdef   DEBUG_HUD
   return object->getDebugLog();
#else
   return false;
#endif
}
DefineEngineMethod( AIFishPlayer, setDebugLog, void, (bool _enable),,
   "@brief When debug log is enable, text appears above fish giving debug info. Must have DEBUG_LOG defined for it to work. ")
{
#ifdef   DEBUG_HUD
   object->setDebugLog(_enable);
#endif
}

DefineEngineMethod( AIFishPlayer, setActivity, bool, (StaticShape *_staticShape, F32 _time, const char *_name),,
   "@brief Set new activity. See 'Activity'  enum")
{
   return object->setActivity(_staticShape, _time, _name);
}
DefineEngineMethod( AIFishPlayer, endActivity, void, (),,
   "@brief End current activity")
{
   return object->endActivity();
}
DefineEngineMethod( AIFishPlayer, setHerdId, void, (S32 _herdId),,
   "@brief set herd this fix belongs to. Fish in the same herd will try to follow each other")
{
   object->setHerdId(_herdId);
}
DefineEngineMethod( AIFishPlayer, activityFinished, bool, (),,
   "@brief test if most recent activity is finished")
{
   return object->mActivity.finished();
}

DefineEngineMethod( AIFishPlayer, onEnterTrigger, void, (Trigger *_trigger, int _type),,
   "@brief call back from script for when fish enters a trigger (e.g. swish,scare box")
{
   return object->onEnterTrigger(_trigger, _type);
}
DefineEngineMethod( AIFishPlayer, onLeaveTrigger, void, (Trigger *_trigger, int _type),,
   "@brief when fish leaves a trigger, this is called from script")
{
   return object->onLeaveTrigger(_trigger, _type);
}


// pg end
