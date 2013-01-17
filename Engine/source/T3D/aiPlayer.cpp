//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "T3D/aiPlayer.h"

#include "console/consoleInternal.h"
#include "math/mMatrix.h"
#include "T3D/gameBase/moveManager.h"
#include "console/engineAPI.h"
// start jc
#include "terrain/terrData.h"
//end jc

// start jc
#include "core/stream/bitStream.h"

/*
// AIPlayerData datablock functions
IMPLEMENT_CO_DATABLOCK_V1(AIPlayerData);

AIPlayerData::AIPlayerData()
{
//   maxTurnRate = M_2PI_F; // default to 1 full circle per second
}

void AIPlayerData::initPersistFields()
{
   Parent::initPersistFields();
//   addField("maxTurnRate",    TypeF32, Offset(maxTurnRate,     AIPlayerData));
}

void AIPlayerData::packData(BitStream* stream)
{
   Parent::packData(stream);
//   stream->write(maxTurnRate);
} 

void AIPlayerData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
//   stream->read(&maxTurnRate);
}
*/
// end jc


IMPLEMENT_CO_NETOBJECT_V1(AIPlayer);

ConsoleDocClass( AIPlayer,
	"@brief A Player object not controlled by conventional input, but by an AI engine.\n\n"

	"The AIPlayer provides a Player object that may be controlled from script.  You control "
   "where the player moves and how fast.  You may also set where the AIPlayer is aiming at "
   "-- either a location or another game object.\n\n"

   "The AIPlayer class does not have a datablock of its own.  It makes use of the PlayerData "
   "datablock to define how it looks, etc.  As the AIPlayer is an extension of the Player class "
   "it can mount objects and fire weapons, or mount vehicles and drive them.\n\n"

   "While the PlayerData datablock is used, there are a number of additional callbacks that are "
   "implemented by AIPlayer on the datablock.  These are listed here:\n\n"

   "void onReachDestination(AIPlayer obj) \n"
   "Called when the player has reached its set destination using the setMoveDestination() method.  "
   "The actual point at which this callback is called is when the AIPlayer is within the mMoveTolerance "
   "of the defined destination.\n\n"

   "void onMoveStuck(AIPlayer obj) \n"
   "While in motion, if an AIPlayer has moved less than moveStuckTolerance within a single tick, this "
   "callback is called.  From here you could choose an alternate destination to get the AIPlayer moving "
   "again.\n\n"

   "void onTargetEnterLOS(AIPlayer obj) \n"
   "When an object is being aimed at (following a call to setAimObject()) and the targeted object enters "
   "the AIPlayer's line of sight, this callback is called.  The LOS test is a ray from the AIPlayer's eye "
   "position to the center of the target's bounding box.  The LOS ray test only checks against interiors, "
   "statis shapes, and terrain.\n\n"

   "void onTargetExitLOS(AIPlayer obj) \n"
   "When an object is being aimed at (following a call to setAimObject()) and the targeted object leaves "
   "the AIPlayer's line of sight, this callback is called.  The LOS test is a ray from the AIPlayer's eye "
   "position to the center of the target's bounding box.  The LOS ray test only checks against interiors, "
   "statis shapes, and terrain.\n\n"

	"@tsexample\n"
	"// Create the demo player object\n"
	"%player = new AiPlayer()\n"
	"{\n"
	"	dataBlock = DemoPlayer;\n"
	"	path = \"\";\n"
	"};\n"
	"@endtsexample\n\n"

	"@see Player for a list of all inherited functions, variables, and base description\n"

	"@ingroup AI\n"
	"@ingroup gameObjects\n");
/**
 * Constructor
 */
AIPlayer::AIPlayer()
{
   mMoveDestination.set( 0.0f, 0.0f, 0.0f );
// start jc - moved to player for variable motion.
//   mMoveSpeed = 1.0f;
// end jc
   mMoveTolerance = 0.25f;
   mMoveStuckTolerance = 0.01f;
   mMoveStuckTestDelay = 30;
   mMoveStuckTestCountdown = 0;
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
   mWidth = -1;
   mTurnDirection = 0;
//   mMaxTurnAngle = 0.2;
   mFollowObject = NULL;
   mFollowNodeIndex = -1;
//   mFollowSlotIndex = 0;
   mTurnRateMod = 1;
//end ds
}

/**
 * Destructor
 */
AIPlayer::~AIPlayer()
{
// start ds
//	delete mInterpolator;
// end ds
}

void AIPlayer::initPersistFields()
{
   addGroup( "AI" );

      addField( "moveTolerance", TypeF32, Offset( mMoveTolerance, AIPlayer ), 
         "@brief Distance from destination before stopping.\n\n"
         "When the AIPlayer is moving to a given destination it will move to within "
         "this distance of the destination and then stop.  By providing this tolerance "
         "it helps the AIPlayer from never reaching its destination due to minor obstacles, "
         "rounding errors on its position calculation, etc.  By default it is set to 0.25.\n");

      addField( "moveStuckTolerance", TypeF32, Offset( mMoveStuckTolerance, AIPlayer ), 
         "@brief Distance tolerance on stuck check.\n\n"
         "When the AIPlayer is moving to a given destination, if it ever moves less than "
         "this tolerance during a single tick, the AIPlayer is considered stuck.  At this point "
         "the onMoveStuck() callback is called on the datablock.\n");

      addField( "moveStuckTestDelay", TypeS32, Offset( mMoveStuckTestDelay, AIPlayer ), 
         "@brief The number of ticks to wait before testing if the AIPlayer is stuck.\n\n"
         "When the AIPlayer is asked to move, this property is the number of ticks to wait "
         "before the AIPlayer starts to check if it is stuck.  This delay allows the AIPlayer "
         "to accelerate to full speed without its initial slow start being considered as stuck.\n"
         "@note Set to zero to have the stuck test start immediately.\n");

   endGroup( "AI" );

   Parent::initPersistFields();
}

bool AIPlayer::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // Use the eye as the current position (see getAIMove)
   MatrixF eye;
   getEyeTransform(&eye);
   mLastLocation = eye.getPosition();

   return true;
}

// start jc - moved to player for variable motion.
/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
/*
void AIPlayer::setMoveSpeed( F32 speed )
{
   mMoveSpeed = getMax(0.0f, getMin( 1.0f, speed ));
}
*/
// end jc

/**
 * Stops movement for this AI
 */
void AIPlayer::stopMove()
{
   mMoveState = ModeStop;
}

/**
 * Sets how far away from the move location is considered
 * "on target"
 *
 * @param tolerance Movement tolerance for error
 */
void AIPlayer::setMoveTolerance( const F32 tolerance )
{
   mMoveTolerance = getMax( 0.1f, tolerance );
}

/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void AIPlayer::setMoveDestination( const Point3F &location, bool slowdown )
{
// start ds
/*
   mMoveDestination = location;
*/
   clearFollowObject();
	// Use the eye as the current position.
//	MatrixF eye;
//	getEyeTransform(&eye);
//	Point3F points[1];
/*
	points[0].set(eye.getPosition());

	eye.getColumn(1,&points[1]);
//	points[1] *= 5;
	points[1] *= 2.0f/mDataBlock->maxTurnRate;
	points[1] += points[0];
	points[2].set(location);
	mInterpolator->submitPoints(points,3);
	//Con::printf("going from (%f,%f,%f) to (%f,%f,%f)",points[0].x,points[0].y,points[0].z,points[1].x,points[1].y,points[1].z);
	
	//Con::printf("point 0 is (%f,%f,%f)",points[0].x,points[0].y,points[0].z);
	//Con::printf("point 1 is (%f,%f,%f)",points[1].x,points[1].y,points[1].z);
	//Con::printf("point 2 is (%f,%f,%f)",points[2].x,points[2].y,points[2].z);

*/
/*
   points[1].set(location);
   mInterpolator->submitPoints(points,2);
	*/

  // clearFollowObject();
   mMoveDestination = location;
/*
   points[0].set(location);
   mInterpolator->submitPoints(points,1);
*/

// end ds

   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
// start ds
/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 * @param approachVector direction to run to location from
 */
void AIPlayer::setMoveDestination( const Point3F &location, const Point3F &approachVector, bool slowdown )
{
setMoveDestination(location, slowdown);
/*

	// Use the eye as the current position.
   clearFollowObject();
	MatrixF eye;
	getEyeTransform(&eye);
	Point3F points[4];
	points[0].set(eye.getPosition());
	eye.getColumn(1,&points[1]);

//	points[1] *= 50;
//	points[1] *= 100;
//	points[1] *= 10 * mDataBlock->AITurnSpeed;
//	points[1] *= 10 * mDataBlock->maxTurnRate;
	points[1] *= 2.0f/mDataBlock->maxTurnRate;
//	points[1] *= (1.0f - (mDataBlock->maxTurnRate*mTurnRateMod));
// todo: fix for diffent move speeds
//	points[1] *= (mDataBlock->maxForwardSpeed*100) * mMoveSpeed;
	
	points[1] += points[0];
//	points[2].set(location-(approachVector * (1.0f - (mDataBlock->maxTurnRate*mTurnRateMod))));
//	points[2].set(location-(approachVector * ((mDataBlock->maxForwardSpeed*100) * mMoveSpeed)));
   if(approachVector.isZero())
   {
	   points[2].set(location);
	   mInterpolator->submitPoints(points,3);
   }
   else
   {
//	points[2].set(location-(approachVector*100));
	points[2].set(location-(approachVector*(2.0f/mDataBlock->maxTurnRate)));
	points[3].set(location);
	//Con::printf("going from (%f,%f,%f) to (%f,%f,%f)",points[0].x,points[0].y,points[0].z,points[3].x,points[3].y,points[3].z);
	
	//Con::printf("point 0 is (%f,%f,%f)",points[0].x,points[0].y,points[0].z);
	//Con::printf("point 1 is (%f,%f,%f)",points[1].x,points[1].y,points[1].z);
	//Con::printf("point 2 is (%f,%f,%f)",points[2].x,points[2].y,points[2].z);
	//Con::printf("point 3 is (%f,%f,%f)",points[3].x,points[3].y,points[3].z);
	
	mInterpolator->submitPoints(points,4);
   }

//	points[1].set(location);
//	mInterpolator->submitPoints(points,2);
   

   //mMoveDestination = location;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
*/
}
// end ds

/**
 * Sets the object the bot is targeting
 *
 * @param targetObject The object to target
 */
void AIPlayer::setAimObject( GameBase *targetObject )
{
   mAimObject = targetObject;
   mTargetInLOS = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

// start ds
void AIPlayer::setFollowObject( ShapeBase *targetObject, bool slowdown )
{
   mFollowObject = targetObject;
   mFollowNodeIndex = -1;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
void AIPlayer::setFollowObjectNode( ShapeBase *targetObject, S32 nodeIndex, bool slowdown )
{
   mFollowObject = targetObject;
   mFollowNodeIndex = nodeIndex;
//   mFollowSlotIndex = slotIndex;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
   mMoveStuckTestCountdown = mMoveStuckTestDelay;
}
void AIPlayer::clearFollowObject( )
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
void AIPlayer::setAimObject( GameBase *targetObject, Point3F offset )
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
void AIPlayer::setAimLocation( const Point3F &location )
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
void AIPlayer::clearAim()
{
   mAimObject = 0;
   mAimLocationSet = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}
// start ds
/**
 * This method recalculates the move vector based on planned path and world influences
 */
	
void AIPlayer::updateMoveDestination(const Point3F &location, const Point3F &forwardVector, const bool is3D)
{
/*
	Point3F dest = getMoveDestination();
	Point3F moveVec = dest-location;
	moveVec.z = 0;
	//RayInfo rInfo;

	if (mTurnDirection != 0 && mDot(moveVec, mAvoidPoint-location) <=0)
	{
		// we were avoiding something, but we've past it
		mTurnDirection = 0;
	}
	bool destSet = false;
   disableCollision();
   if (mFollowObject) mFollowObject->disableCollision();

	// calculate search length
   F32 searchLen = moveVec.len();
   F32 velLen = (mVelocity.len()) / 4 + 1.5;
   if (searchLen > velLen) searchLen = velLen;

   //find the closest object to avoid
   
   Collision cInfo;
// todo: do we need this is it worth it
	if (findClosestObjectInFront(searchLen, &cInfo))
   {
      if(mFlying && static_cast<TerrainBlock*>(cInfo.object))
      {
		   Point3F pointToDest = dest - cInfo.point;
		//   pointToDest.z = 0;
		   Point3F oldPointToDest = dest - mAvoidPoint;
		//   oldPointToDest.z = 0;
			mAvoidPoint = cInfo.point;
         mTurnDirection = 0.0f;

		   Point3F pointVec = cInfo.point - getBoxCenter();
		//   pointVec.z = 0;
		   F32 urgency = 1.0f;
		   
		//   mMoveDestination.set(location+rotateVec(forwardVector*20,mDataBlock->AITurnSpeed*urgency,1));
		   mMoveDestination.set(location+rotateVec(forwardVector*20,mDataBlock->maxTurnRate*urgency,1));
		   destSet = true;
      }
      else
      {
		Point3F pointToDest = dest - cInfo.point;
		pointToDest.z = 0;
		Point3F oldPointToDest = dest - mAvoidPoint;
		oldPointToDest.z = 0;
		if (mTurnDirection == 0 ||  (dest - cInfo.point).lenSquared() < (dest - mAvoidPoint).lenSquared())
		{
			mAvoidPoint = cInfo.point;

			if (mTurnDirection == 0)
			{
				Point3F dir(cInfo.object->getBoxCenter() - location);
				F32 temp = dir.x;
				dir.x = -dir.y;
				dir.y = temp;
				if (mDot(forwardVector,dir) > 0)
				{
					//left
					mTurnDirection = 1;
				}
				else
				{
					//right
					mTurnDirection = -1;
				}
			}
		}
		Point3F pointVec = cInfo.point - getBoxCenter();
		pointVec.z = 0;
	//	F32 pointLenSquared =  pointVec.lenSquared();
		F32 urgency = 1.0f;
		
	//	if (pointLenSquared > 0)
	//	{
	//		pointVec.z = 0;
	//		// dot direction to point then divide by length
	//		// lenSquared normalises the point vector and divides at same time
	//		urgency = getMin(getMax(mDot(pointVec,forwardVector)/pointLenSquared,0.05f), 1.0f);
	//	}
		
	//	mMoveDestination.set(location+rotateVec(forwardVector*20,mTurnDirection*mDataBlock->AITurnSpeed*urgency,3));
		mMoveDestination.set(location+rotateVec(forwardVector*20,mTurnDirection*mDataBlock->maxTurnRate*urgency,3));
		destSet = true;
      }
	}
   
   enableCollision();
   if (mFollowObject) mFollowObject->enableCollision();
	if (!destSet)
	{
*/
      if (mFollowObject)
      {
		 if(mFollowNodeIndex != -1)
		 {
			 MatrixF mat;
			 mFollowObject->getNodeWorldTransform(mFollowNodeIndex, &mat);
			 mMoveDestination = mat.getPosition();
		 }
		 else
			mMoveDestination = mFollowObject->getPosition();
/*
	  }
      else
      {
		   //Con::printf("getWayPoint((%f,%f,%f),(%f,%f,%f),%f)",location.x,location.y,location.z,v2.x,v2.y,v2.z,mMoveSpeed);
         //Con::printf("prediction scale = %f",mVelocity.len()/8+0.5);
//         mMoveDestination.set(((mInterpolator)->getWaypoint(location, forwardVector, mVelocity.len()/8+0.5)));
		   //Con::printf(" = (%f,%f,%f)",mMoveDestination.x,mMoveDestination.y,mMoveDestination.z);
      }

      mMoveDestination -= mVelocity*0.05f;

		//get angle of vec around z axis
		F32 angle = -getVectorAngle(forwardVector, mMoveDestination-location,3);
		F32 maxTurnRate = mDataBlock->maxTurnRate*0.01;
		// if we're avoiding an object - and turning away - turn back towards
		if (angle < -0.01f && mTurnDirection < 0.0f)
		{
		//	angle = mDataBlock->AITurnSpeed;
			angle = -maxTurnRate;
			mMoveDestination.set(location+(rotateVec(forwardVector,angle,3) * 20.0f));
		}
		else if (angle > 0.01f && mTurnDirection > 0.0f)
		{
		//	angle = mDataBlock->AITurnSpeed;
			angle = maxTurnRate;
			mMoveDestination.set(location+(rotateVec(forwardVector,angle,3) * 20.0f));
		}
		//else if (angle > mDataBlock->AITurnSpeed || angle < -mDataBlock->AITurnSpeed)
		else if (angle > maxTurnRate || angle < -maxTurnRate)
		{
			//Con::printf("angle=%f",angle);
			// cap to approximately 5.7 degrees movement
		//	angle = (angle>0)?mDataBlock->AITurnSpeed:-mDataBlock->AITurnSpeed; 
			angle = (angle>0)?maxTurnRate:-maxTurnRate; 
			mMoveDestination.set(location+(rotateVec(forwardVector,angle,3) * 20.0f));
			//Con::printf("new dest=(%f,%f,%f)",mMoveDestination.x,mMoveDestination.y,mMoveDestination.z);
		}	
		
		//get angle of vec around z axis
		if (is3D)
      {
         Point3F xyVec(mMoveDestination.x-location.x,mMoveDestination.y-location.y,0.0);
         F32 xyDist = xyVec.len();
         if (xyDist > 0.0)
         {
            F32 ratio = (mMoveDestination.z-location.z)/xyDist;
            if (ratio > 0.5f || ratio < -0.5f)
            {
               Point3F save = mMoveDestination;
            //   F32 originalLength = (mMoveDestination-location).len();
               xyVec.z = (mMoveDestination.z > location.z )?xyDist:-xyDist;
               //xyVec *= originalLength/xyDist;
               mMoveDestination += xyVec;
            }
         }
      }
	  */
	}
}

// end ds

/**
 * This method calculates the moves for the AI player
 *
 * @param movePtr Pointer to move the move list into
 */
bool AIPlayer::getAIMove(Move *movePtr)
{
   *movePtr = NullMove;

   // Use the eye as the current position.
   MatrixF eye;
   getEyeTransform(&eye);
   Point3F location = eye.getPosition();
   Point3F rotation = getRotation();
// start ds
   VectorF v2;
   eye.getColumn(1,&v2);

   bool is3D = mFlying || mSwimming;
   
   if (mWidth < 0.0) //un-initialised
   {
	   mWidth = getObjBox().maxExtents.x - getObjBox().minExtents.x;
      mWidth = mWidth * 2.0f;
   }
   VectorF crosseyed(-v2.y,v2.x,0.0);
   crosseyed*= mWidth;
// end ds

   // Orient towards the aim point, aim object, or towards
   // our destination.
   if (mAimObject || mAimLocationSet || mMoveState != ModeStop) 
   {
// start ds
		updateMoveDestination(getBoxCenter(), v2, is3D);
// end ds

      // Update the aim position if we're aiming for an object
      if (mAimObject)
         mAimLocation = mAimObject->getPosition() + mAimOffset;
      else
         if (!mAimLocationSet)
            mAimLocation = mMoveDestination;

      F32 xDiff = mAimLocation.x - location.x;
      F32 yDiff = mAimLocation.y - location.y;
// start ds
/*
      if (!mIsZero(xDiff) || !mIsZero(yDiff)) 
      {
*/
      F32 zDiff = mAimLocation.z - location.z;
      if (!mIsZero(xDiff) || !mIsZero(yDiff) || (is3D && !mIsZero(zDiff) )) {
// end ds


         // First do Yaw
         // use the cur yaw between -Pi and Pi
         F32 curYaw = rotation.z;
         while (curYaw > M_2PI_F)
            curYaw -= M_2PI_F;
         while (curYaw < -M_2PI_F)
            curYaw += M_2PI_F;

         // find the yaw offset
         F32 newYaw = mAtan2( xDiff, yDiff );
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
	// start jc
    // todo: do we need this anymore
		 // don't look too fast
        // if (yawDiff < 0.01 && yawDiff > -0.01)
        // if (yawDiff < 0.0001 && yawDiff > -0.0001)
         if (yawDiff < 0.001 && yawDiff > -0.001)
         {
            if (mMoveState == ModeStop)
            {
               throwCallback("onReachDestination");
            }
         }
	// end jc

         movePtr->yaw = yawDiff;
// start jc - Dan&rsquo;s mods (turning inertia) ->
//         movePtr->yaw *= mDataBlock->AITurnSpeed;
// end jc - Dan&rsquo;s mods (turning inertia)

// start jc
		 // control the rotation of the AIPlayer
   
         // get the maximum amount we can yaw this tick
         F32 maxTurnRate = mDataBlock->maxTurnRate * TickSec;

         // limit the yaw if it exceeds the maxYaw
         if(mFabs(yawDiff) > maxTurnRate)
            movePtr->yaw = maxTurnRate * mSign(yawDiff);
         else
            movePtr->yaw = yawDiff;
// end jc

         // Next do pitch.
// start ds
/*
         if (!mAimObject && !mAimLocationSet) 
         {
*/
        Point3F headRotation = getHeadRotation();
         if (!mAimObject && !mAimLocationSet && !is3D) {
// end ds
            // Level out if were just looking at our next way point.
            movePtr->pitch = -headRotation.x;

         // start jc - Dan&rsquo;s mods (turning inertia) ->
         //   movePtr->pitch *= mDataBlock->AITurnSpeed;
         //   movePtr->pitch *= mDataBlock->AITurnSpeed;
         // end jc - Dan&rsquo;s mods (turning inertia)

         }
         else {
            // This should be adjusted to run from the
            // eye point to the object's center position. Though this
            // works well enough for now.
// start ds
/*
            F32 vertDist = mAimLocation.z - location.z;
            F32 horzDist = mSqrt(xDiff * xDiff + yDiff * yDiff);
            F32 newPitch = mAtan2( horzDist, vertDist ) - ( M_PI_F / 2.0f );
*/
         //   F32 horzDist = mSqrt(xDiff * xDiff + yDiff * yDiff);
         //   F32 newPitch = mAtan2( horzDist, zDiff ) - ( M_PI_F / 2.0f );

		 // First do Yaw
			 // use the cur yaw between -Pi and Pi
			 F32 curPitch = headRotation.x;
		//	 while (curPitch > M_2PI_F)
		//		curPitch -= M_2PI_F;
		//	 while (curPitch < -M_2PI_F)
		//		curPitch += M_2PI_F;
			
			 // find the yaw offset
			 //F32 newPitch = mAtan2( xDiff, yDiff );
			 F32 horzDist = mSqrt(xDiff * xDiff + yDiff * yDiff);
             F32 newPitch = mAtan2( horzDist, zDiff ) - ( M_PI_F / 2.0f );

			 F32 pitchDiff = newPitch - curPitch;
		//	 F32 pitchDiff = newPitch - headRotation.x;

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

// end ds
// start jc
/*
            if (mFabs(newPitch) > 0.001f) {
               Point3F headRotation = getHeadRotation();
               movePtr->pitch = newPitch - headRotation.x;
			}
*/
            if (mFabs(pitchDiff) > 0.001f) {
		//	if(!is3D)
		//	{
          //  if (mFabs(newPitch) > 0.01f) {
            //   Point3F headRotation = getHeadRotation();
               movePtr->pitch = pitchDiff;
		//	}
		//	else
		//	{
		//		movePtr->pitch = pitchDiff;
				/*
	   			 // get the maximum amount we can yaw this tick
				 F32 maxPitch = mDataBlock->maxTurnRate * TickSec;

				 // limit the yaw if it exceeds the maxYaw
				 if(mFabs(pitchDiff) > maxPitch)
					movePtr->pitch = (maxPitch * mSign(pitchDiff));
				 else
					movePtr->pitch = pitchDiff;
*/
        //    }

			}

			// never move more than we are aloud
			if(mFabs(movePtr->pitch) > maxTurnRate)
				movePtr->pitch = maxTurnRate * mSign(movePtr->pitch);
// end jc
         }

// start jc - todo fix max turn speed for pitch 
		 /*
		 // control the rotation of the AIPlayer
   
		 // get the maximum amount we can yaw this tick
		 F32 maxPitch = mDataBlock->maxTurnRate * TickSec;

		 // limit the yaw if it exceeds the maxYaw
		 if(mFabs(pitchDiff) > maxPitch)
			movePtr->pitch = maxPitch * mSign(pitchDiff);
		 else
			movePtr->pitch = pitchDiff;
			*/
// end jc

      }
   }
// start ds
//   else
   else if (!is3D || mMoveState == ModeStop) {
// end ds
      // Level out if we're not doing anything else
      Point3F headRotation = getHeadRotation();
      movePtr->pitch = -headRotation.x;
   }

   // Move towards the destination
   if (mMoveState != ModeStop) 
   {
      F32 xDiff = mMoveDestination.x - location.x;
      F32 yDiff = mMoveDestination.y - location.y;

//start ds
/*
      // Check if we should mMove, or if we are 'close enough'
      if (mFabs(xDiff) < mMoveTolerance && mFabs(yDiff) < mMoveTolerance) 
      {
*/
      F32 zDiff = (is3D)?mMoveDestination.z - location.z:0;
      
      const Point3F finish = (mFollowObject) ? (mFollowObject->getPosition()) : mMoveDestination;
//      const Point3F * finish = (mFollowObject)?&(mFollowObject->getPosition()):mInterpolator->getPoint(mInterpolator->getNumPoints()-1);
      F32 xFinDiff = finish.x - location.x;
      F32 yFinDiff = finish.y - location.y;
      F32 zFinDiff = (is3D)?finish.z - location.z:0;

      // Check if we should mMove, or if we are 'close enough'
      if (mFabs(xFinDiff) < mMoveTolerance && mFabs(yFinDiff) < mMoveTolerance && (!is3D || mFabs(zFinDiff) < mMoveTolerance))
      {
      
//end ds
         mMoveState = ModeStop;
         throwCallback("onReachDestination");
      }
      else
      {
         // Build move direction in world space
// start ds
/*
         if (mIsZero(xDiff))
            movePtr->y = (location.y > mMoveDestination.y) ? -1.0f : 1.0f;
         else
            if (mIsZero(yDiff))
               movePtr->x = (location.x > mMoveDestination.x) ? -1.0f : 1.0f;
            else
               if (mFabs(xDiff) > mFabs(yDiff)) 
               {
                  F32 value = mFabs(yDiff / xDiff);
                  movePtr->y = (location.y > mMoveDestination.y) ? -value : value;
                  movePtr->x = (location.x > mMoveDestination.x) ? -1.0f : 1.0f;
               }
               else 
               {
                  F32 value = mFabs(xDiff / yDiff);
                  movePtr->x = (location.x > mMoveDestination.x) ? -value : value;
                  movePtr->y = (location.y > mMoveDestination.y) ? -1.0f : 1.0f;
               }

*/
         if (mIsZero(xDiff) && mIsZero(zDiff))
         {
            movePtr->y = (location.y > mMoveDestination.y)? -1 : 1;
         }
         else if (mIsZero(yDiff) && mIsZero(zDiff))
         {
         	movePtr->x = (location.x > mMoveDestination.x)? -1 : 1;
         }
         else if (mIsZero(xDiff) && mIsZero(yDiff)&&is3D)
		 {
		 	movePtr->z = (location.z > mMoveDestination.z)? -1 : 1;
		 }
		 else
	     {
	        
	        F32 total = mSqrt(xDiff*xDiff+yDiff*yDiff+zDiff*zDiff);
	        movePtr->x = xDiff/total;
	        movePtr->y = yDiff/total;
	        movePtr->z = zDiff/total;
	    }
// end ds

         // Rotate the move into object space (this really only needs
         // a 2D matrix)
         Point3F newMove;
         MatrixF moveMatrix;
// start ds
/*
         moveMatrix.set(EulerF(0.0f, 0.0f, -(rotation.z + movePtr->yaw)));
         moveMatrix.mulV( Point3F( movePtr->x, movePtr->y, 0.0f ), &newMove );
         movePtr->x = newMove.x;
         movePtr->y = newMove.y;
*/
         moveMatrix.set(EulerF(0.0f, 0.0f, -(rotation.z + movePtr->yaw)));
         moveMatrix.mulV( Point3F( movePtr->x, movePtr->y, movePtr->z ), &newMove );
         movePtr->x = newMove.x;
         movePtr->y = newMove.y;
         movePtr->z = newMove.z;
// end ds
         // Set movement speed.  We'll slow down once we get close
         // to try and stop on the spot...
// start jc
         if (mMoveSlowdown) {
//         if (mMoveState == ModeSlowing) {
// end jc		 

            F32 speed = mMoveSpeed;
// start ds
/*
            F32 dist = mSqrt(xDiff*xDiff + yDiff*yDiff);
*/
            F32 dist = mSqrt(xFinDiff*xFinDiff + yFinDiff*yFinDiff+ ((is3D)?zFinDiff*zFinDiff:0.0f));
// end ds
// start ds
//            F32 maxDist = 5.0f;
//            F32 maxDist = 2.0f;
//            if (dist < maxDist)
//               speed *= dist / maxDist;

            F32 maxDist = 2.0f / mMoveSpeed;
            if (dist < maxDist)
			{
               speed *= dist / maxDist;
			   mMoveState = ModeSlowing;
			}
			else
               mMoveState = ModeMove;
// end jc
// start jc - stay animating to avoid animation freak out
            if(speed < 0.002f)
               speed = 0.002f;
// end jc
            movePtr->x *= speed;
            movePtr->y *= speed;
            if(is3D) movePtr->z *= speed;

         //   mMoveState = ModeSlowing; // start jc
         }
         else 
         {
            movePtr->x *= mMoveSpeed;
            movePtr->y *= mMoveSpeed;
            if(is3D) movePtr->z *= mMoveSpeed;

            mMoveState = ModeMove;
         }

         // Reset a previous stuck mode
         mMoveState = ModeMove;

         if (mMoveStuckTestCountdown > 0)
         {
            --mMoveStuckTestCountdown;
         }
         else
         {
            // We should check to see if we are stuck...
            F32 locationDelta = (location - mLastLocation).len();
            if (locationDelta < mMoveStuckTolerance && mDamageState == Enabled) 
            {
               // If we are slowing down, then it's likely that our location delta will be less than
               // our move stuck tolerance. Because we can be both slowing and stuck
               // we should TRY to check if we've moved. This could use better detection.
               if ( mMoveState != ModeSlowing || locationDelta == 0 )
               {
                  mMoveState = ModeStuck;
                  throwCallback("onMoveStuck");
				  mMoveStuckTestCountdown = mMoveStuckTestDelay;
               }
            }
         }
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
      }
      else
         if (!mTargetInLOS) {
            throwCallback( "onTargetEnterLOS" );
            mTargetInLOS = true;
         }
   }

   // Replicate the trigger state into the move so that
   // triggers can be controlled from scripts.
   for( int i = 0; i < MaxTriggerKeys; i++ )
      movePtr->trigger[i] = getImageTriggerState(i);

   mLastLocation = location;

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
void AIPlayer::throwCallback( const char *name )
{
   Con::executef(getDataBlock(), name, getIdString());
}

/**
 * Calculates the angle between two Point3F vectors
 * @param v1 the first vector
 * @param v2 the second vector
 * @param discardDimension the dimension (if any) to discard for calculation (1->x, 2->y, 3->z)
 */
F32 AIPlayer::getVectorAngle(const Point3F &v1, const Point3F &v2, U32 discardDimension)
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
		Con::printf("AIPlayer::getAngle - unsupported discardDimension %d", discardDimension);
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
		Con::printf("AIPlayer::getAngle - unsupported discardDimension %d", discardDimension);
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
 inline Point3F AIPlayer::rotateVec(const Point3F &v,F32 angle, U32 axis)
*/
Point3F AIPlayer::rotateVec(const Point3F &v,F32 angle, U32 axis)
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



bool AIPlayer::findClosestObjectInFront(F32 range, Collision *closest, U32 mask)
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


// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------

DefineEngineMethod( AIPlayer, stop, void, ( ),,
   "@brief Tells the AIPlayer to stop moving.\n\n")
{
   object->stopMove();
}

DefineEngineMethod( AIPlayer, clearAim, void, ( ),,
   "@brief Use this to stop aiming at an object or a point.\n\n"
   
   "@see setAimLocation()\n"
   "@see setAimObject()\n")
{
   object->clearAim();
}

DefineEngineMethod( AIPlayer, setMoveSpeed, void, ( F32 speed ),,
   "@brief Sets the move speed for an AI object.\n\n"

   "@param speed A speed multiplier between 0.0 and 1.0.  "
   "This is multiplied by the AIPlayer's base movement rates (as defined in "
   "its PlayerData datablock)\n\n"
   
   "@see getMoveDestination()\n")
{
	object->setMoveSpeed(speed);
}

DefineEngineMethod( AIPlayer, getMoveSpeed, F32, ( ),,
   "@brief Gets the move speed of an AI object.\n\n"

   "@return A speed multiplier between 0.0 and 1.0.\n\n"

   "@see setMoveSpeed()\n")
{
   return object->getMoveSpeed();
}


DefineEngineMethod( AIPlayer, setMoveDestination, void, ( Point3F goal, bool slowDown ), ( true ),
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

DefineEngineMethod( AIPlayer, getMoveDestination, Point3F, (),,
   "@brief Get the AIPlayer's current destination.\n\n"

   "@return Returns a point containing the \"x y z\" position "
   "of the AIPlayer's current move destination. If no move destination "
   "has yet been set, this returns \"0 0 0\"."
   
   "@see setMoveDestination()\n")
{
	return object->getMoveDestination();
}

DefineEngineMethod( AIPlayer, setAimLocation, void, ( Point3F target ),,
   "@brief Tells the AIPlayer to aim at the location provided.\n\n"

   "@param target An \"x y z\" position in the game world to target.\n\n"
   
   "@see getAimLocation()\n")
{
	object->setAimLocation(target);
}

DefineEngineMethod( AIPlayer, getAimLocation, Point3F, (),,
   "@brief Returns the point the AIPlayer is aiming at.\n\n"

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

ConsoleDocFragment _setAimObject(
   "@brief Sets the AIPlayer's target object.  May optionally set an offset from target location\n\n"

   "@param targetObject The object to target\n"
   "@param offset Optional three-element offset vector which will be added to the position of the aim object.\n\n"

   "@tsexample\n"
   "// Without an offset\n"
   "%ai.setAimObject(%target);\n\n"
   "// With an offset\n"
   "// Cause our AI object to aim at the target\n"
   "// offset (0, 0, 1) so you don't aim at the target's feet\n"
   "%ai.setAimObject(%target, \"0 0 1\");\n"
   "@endtsexample\n\n"
   
   "@see getAimLocation()\n"
   "@see getAimObject()\n"
   "@see clearAim()\n",

   "AIPlayer",
   "void setAimObject(GameBase targetObject, Point3F offset);"
);
ConsoleMethod( AIPlayer, setAimObject, void, 3, 4, "( GameBase obj, [Point3F offset] )"
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

DefineEngineMethod( AIPlayer, getAimObject, S32, (),,
   "@brief Gets the object the AIPlayer is targeting.\n\n"

   "@return Returns -1 if no object is being aimed at, "
   "or the SimObjectID of the object the AIPlayer is aiming at.\n\n"
   
   "@see setAimObject()\n")
{
	GameBase* obj = object->getAimObject();
   return obj? obj->getId(): -1;
}



// start jc
ConsoleMethod( AIPlayer, setMoveDestAndAppVec, void, 4, 5, "(Point3F goal, Point3F appVec, bool slowDown=true)"
              "Tells the AI to move to the location provided.")				
{																			
   Point3F v( 0.0f, 0.0f, 0.0f );											
   Point3F v2( 0.0f, 0.0f, 0.0f );											
   dSscanf( argv[2], "%f %f %f", &v.x, &v.y, &v.z );						
   dSscanf( argv[3], "%f %f %f", &v2.x, &v2.y, &v2.z );						
   bool slowdown = (argc > 4)? dAtob(argv[4]): true;						
   object->setMoveDestination( v, v2, slowdown);							
}

ConsoleMethod( AIPlayer, clearFollowObject, void, 2, 2, "()"							
              "Stop following.")									
{																			
   object->clearFollowObject();														
}	
ConsoleMethod( AIPlayer, setFollowObject, void, 3, 4, "( ShapeBase obj, bool slowDown=true)"	\
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
ConsoleMethod( AIPlayer, setFollowObjectNode, void, 3, 6, "( ShapeBase obj, char* nodeName, bool slowDown=true)"	\
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
void AIPlayer::changePose(S32 poseNumber)
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
DefineEngineMethod( AIPlayer, setPose, void, (U32 pose),,
			  "(int pose) StandPose=0,CrouchPose=1,PronePose=2")
{	
   object->changePose(pose);
}//yorks end
// end jc


// start jc
bool AIPlayer::onNewDataBlock(GameBaseData* dptr, bool reload)
{
 //  mDataBlock = dynamic_cast<AIPlayerData*>(dptr);
   mDataBlock = dynamic_cast<PlayerData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr, reload))
      return false;
   
//	mDataBlock->mStuckT

   scriptOnNewDataBlock();
   return true;
}
// end jc
