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

#ifndef _AIPLAYER_H_
#define _AIPLAYER_H_

#ifndef _PLAYER_H_
#include "T3D/player.h"
#endif

// start ds
#ifndef _NODEINTERPOLATOR_H_
#include "T3D/nodeInterpolator.h"
#endif
// end jc

// start jc
/*
class AIPlayerData : public PlayerData {
   typedef PlayerData Parent;

public:
   AIPlayerData(); // CTor

   // maximum yaw rate
   //F32 maxTurnRate;
   
   // standard datablock stuff
   DECLARE_CONOBJECT(AIPlayerData);
   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
};
*/
// end jc

class AIPlayer : public Player {

	typedef Player Parent;

// start jc
//	AIPlayerData *mDataBlock;        // our new datablock type
// end jc

public:
	enum MoveState {
		ModeStop,                       // AI has stopped moving.
		ModeMove,                       // AI is currently moving.
		ModeStuck,                      // AI is stuck, but wants to move.
      ModeSlowing,                    // AI is slowing down as it reaches it's destination.
	};

private:
   MoveState mMoveState;
// start jc - moved to player for variable motion.
//   F32 mMoveSpeed;
// end jc
   F32 mMoveTolerance;                 // Distance from destination before we stop
   Point3F mMoveDestination;           // Destination for movement
   Point3F mLastLocation;              // For stuck check
   F32 mMoveStuckTolerance;            // Distance tolerance on stuck check
   S32 mMoveStuckTestDelay;            // The number of ticks to wait before checking if the AI is stuck
   S32 mMoveStuckTestCountdown;        // The current countdown until at AI starts to check if it is stuck
   bool mMoveSlowdown;                 // Slowdown as we near the destination

   SimObjectPtr<GameBase> mAimObject; // Object to point at, overrides location
   bool mAimLocationSet;               // Has an aim location been set?
   Point3F mAimLocation;               // Point to look at
   bool mTargetInLOS;                  // Is target object visible?

// start ds
// Added by Brendan Ledwich 2005-11-9
//		NodeInterpolator *mInterpolator;
		Point3F mAvoidPoint;
		F32 mWidth;
		F32 mTurnDirection;
		F32 mTurnRateMod;
//      F32 mMaxTurnAngle;

      ShapeBase * mFollowObject;
	  S32 mFollowNodeIndex;
	  //S32 mFollowSlotIndex;
      //Point3F mCollisionPoint;
		//Point3F mMoveDestination;           // Destination for movement
// end ds



   Point3F mAimOffset;

   // Utility Methods
   void throwCallback( const char *name );

public:
   DECLARE_CONOBJECT( AIPlayer );

   AIPlayer();
   ~AIPlayer();

// start jc
   bool onNewDataBlock(GameBaseData* dptr, bool reload);  // new datablock assigned
// end jc

   static void initPersistFields();

   bool onAdd();

   virtual bool getAIMove( Move *move );

   // Targeting and aiming sets/gets
   void setAimObject( GameBase *targetObject );
   void setAimObject( GameBase *targetObject, Point3F offset );
   GameBase* getAimObject() const  { return mAimObject; }
   void setAimLocation( const Point3F &location );
   Point3F getAimLocation() const { return mAimLocation; }
   void clearAim();

   // Movement sets/gets
// start jc - moved to player for variable motion.
//   void setMoveSpeed( const F32 speed );
//   F32 getMoveSpeed() const { return mMoveSpeed; }
// end jc
   void setMoveTolerance( const F32 tolerance );
   F32 getMoveTolerance() const { return mMoveTolerance; }
   void setMoveDestination( const Point3F &location, bool slowdown );

// start jc
//   Point3F getMoveDestination() const { return mMoveDestination; }

	Point3F getMoveDestination() const
	{
		if (mFollowObject)
			if(mFollowNodeIndex != -1)
			 {
				MatrixF mat;
				mFollowObject->getNodeWorldTransform(mFollowNodeIndex, &mat);
				return mat.getPosition();
			 }
			 else
			 	return mFollowObject->getPosition();
		else
			return mMoveDestination;
		//	return (mInterpolator->getNumPoints())?*(mInterpolator->getPoint(mInterpolator->getNumPoints()-1)):Point3F(0,0,0);
	}
	void changePose(S32 poseNumber);

	F32 getVectorAngle(const Point3F &v1, const Point3F &v2, U32 discardDimension);
	Point3F rotateVec(const Point3F &v,F32 angle, U32 axis);
	bool findClosestObjectInFront(F32 range, Collision *closest, U32 mask = 0);

	void setMoveDestination( const Point3F &location, const Point3F &approachVector, bool slowdown = false );
	void setFollowObject( ShapeBase *targetObject, bool slowdown );
	void setFollowObjectNode( ShapeBase *targetObject, S32 nodeIndex = -1, bool slowdown = false );
	void clearFollowObject();

	virtual void updateMoveDestination(const Point3F &location, const Point3F &forwardVector, const bool is3D);

   F32 getTurnRateMod()          { return mTurnRateMod;  }
   void setTurnRateMod(F32 mod)  { mTurnRateMod = mod;   }

   void updateMove(const Move* move)
   {
		// todo: see if this works
		if (!getControllingClient() && isGhost())
			return;

	   if(mFlying)
		  updateMoveFlight(move);
	   else updateMoveOriginal(move);
	}
// end jc

	void stopMove();
};

#endif
