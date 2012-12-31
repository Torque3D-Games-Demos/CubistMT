//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#ifndef _AIFISHPLAYER_H_
#define _AIFISHPLAYER_H_



#ifndef _FISHPLAYER_H_
#include "T3D/fishplayer.h"
#endif

// start ds
#ifndef _NODEINTERPOLATOR_H_
#include "T3D/nodeInterpolator.h"
#endif

#include "T3D/staticshape.h"
#include "T3D/influence.h"
#include "T3D/influenceWander.h"
#include "T3D/influenceSwish.h"
#include "T3D/influenceSchool.h"
#include "T3D/influenceFleeFish.h"
#include "T3D/influenceFollowLeader.h"
#include "T3D/influenceFleeFish.h"
#include "T3D/influenceFleeTrigger.h"
#include "T3D/influenceFeed.h"
#include "T3D/influenceBounds.h"
#include "T3D/influenceAvoidCollision.h"
#include "T3D/influenceFleeTrigger.h"
#include "T3D/influenceUserTouch.h"
#include "T3D/influenceCrashRecover.h"

#include "T3D/alarm.h"

enum InfluenceTypes{
   influenceWander,
   influenceSwish,
   influenceSchool,
   influenceFleeFish,
   influenceFollowLeader,
   influenceFeed,
   influenceAvoidCollision,
   influenceBounds,
   influenceFleeTrigger,
   influenceUserTouch,
   influenceCrashRecover,
   influenceCount,
};

enum Activity{
   ActivityNone,
   ActivitySwim,
   ActivityEat,
   ActivityHide,
   ActivityCount,
};
struct Obstacle
{
   F32         time;
   SceneObject *object;
   Point3F     normal;
   Point3F     position;
};

class TrafficController;

struct   FishActivity
{
   StaticShape *shape;
   F32         time;
   Activity    activity;
   bool        finished(){ return time<=0;}
};

class AIFishPlayer : public FishPlayer {

	typedef FishPlayer Parent;

public:
	enum MoveState {
		ModeStop,                       // AI has stopped moving.
		ModeMove,                       // AI is currently moving.
		ModeStuck,                      // AI is stuck, but wants to move.
      ModeSlowing,                    // AI is slowing down as it reaches it's destination.
	};
   virtual void onCollision( SceneObject *object, const VectorF &vec );

   //new ai stuff
   bool  scaleAnimationSpeed();
   Point3F  getHeading();
   F32      getPassingDistance();

   F32    getYaw();
   F32    getPitch();

   void     wanderSetRange(Point3F _lo, Point3F _hi);
   void     wanderGetRange(Point3F &_lo, Point3F &_hi);
   bool     justCrashed();
   void     think();
   void     endActivity();
   bool     isFleeing();
   void     addCollisionAvoidance(Influence *_influence);

   bool     setActivity(StaticShape *_shape, F32 _time, const char *_name);
   void     setHerdId(S32 _herdId);

   void  onEnterTrigger(Trigger *_trigger, int _type);
   void  onLeaveTrigger(Trigger *_trigger, int _type);



   virtual void   processTick(const Move *move);
   InfluenceSchool         *getSchoolInfluence(){ return &mInfluenceSchool;}

#ifdef   DEBUG_HUD
#define  FISH_MAXLOG 4     //number of lines used for logging fish events on hud

   char*       mLog[FISH_MAXLOG];
   int         mLogCount;
   char        *mLogBuffer;
   bool        mDebugLog;
   void     addLog(const char *_message);
   void     addLogFormatted(const char * format, ...);
   void     showLog();
   bool     getDebugLog(){return mDebugLog;}
   void     setDebugLog(bool _debugLog){mDebugLog=_debugLog;}
   void     addText(char *base, int *iBase, char *add);
#endif
#ifdef   DEBUG_GFX
   void     debugDrawing();
   void     drawRelationship(ShapeBase *who, ColorI _color);
   void     drawRelationship(Point3F _where, ColorI _color);
   void     drawInfluence();
   void     drawCapsule();
   void     printInfluence();
   void     drawWanderBox();
#endif
   FishActivity mActivity;


   bool           ignoreCollision();
   void     setEnableSchool(bool a);
   void           enableUserTouch(bool _enable);
   void     setTrafficController(TrafficController *_trafficController){mTrafficController = _trafficController;}
private:

   enum AlarmList{
      AlarmScaryFishSearch,
      AlarmInfluence,
      AlarmObstacleCheck,
      AlarmLocalFishCheck,
      AlarmStuckCheck,
      AlarmCount,
   };
   enum  TriggerType{
      TriggerTypeSwish,
      TriggerTypeScare,
   };
   //---
   static   AIFishPlayer   *sTestFish;
   static   bool  castRayCallback( RayInfo* ri );
   //---
   U32            mRadarPhase;
   AIFishPlayer   *mLeader;
   F32            getCollisionWarningDistance();

   void           updateLeader();
   ShapeBase      *getScaryFish(){return mInfluenceFleeFish.getScaryObject();}
   void           processTickAi();
   void           processTickBubble();
   void           initBubble();
   F32            getLeaderAttraction(AIFishPlayer *who);

   void           updateActivity();
   bool           validLeader(AIFishPlayer *_who);
   AIFishPlayer   *getScaryObject();
   AIFishPlayer   *getLeader();
   bool           missedPlayerCollision(Obstacle *c);
   bool           missedReefCollision(Obstacle *c);
   void           onMoveStuck();
   void           obstacleCheck();
   void           stuckCheck();
   void           localFishCheck();
   void           applyInfluences();
   virtual  void  onHitReef(const Collision &collision );
   virtual  void  onHitFish(const Collision &collision );
   void           searchForScaryFish();
   //---

#define  MAX_LOCAL_FISH (8)
   AIFishPlayer   *mLocalFish[MAX_LOCAL_FISH];
   int            mLocalFishCount;
   TrafficController *mTrafficController;

   Influence      *mInfluence[influenceCount];

   InfluenceSchool         mInfluenceSchool;
   InfluenceWander         mInfluenceWander;
   InfluenceSwish          mInfluenceSwish;
   InfluenceFleeFish       mInfluenceFleeFish;
   InfluenceFollowLeader   mInfluenceFollowLeader;
   InfluenceFeed           mInfluenceFeed;
   InfluenceAvoidCollision mInfluenceAvoidCollision;
   InfluenceBounds         mInfluenceBounds;
   InfluenceFleeTrigger    mInfluenceFleeTrigger;
   InfluenceUserTouch      mInfluenceUserTouch;
   InfluenceCrashRecover   mInfluenceCrashRecover;


   Point3F     mBadPosition;              //average of previous collision points
   F32         mBadPositionTime;          //time last got a collision point
   bool        mSchooling;
   S32         herdId;


   Alarm       mAlarm[AlarmCount];

   Point3F     mWanderLo, mWanderHi;

   Point3F       mLastStuckCheckPosition;

   MoveState mMoveState;
// start jc - moved to player for variable motion.
//   F32 mMoveSpeed;
// end jc
   F32 mMoveTolerance;                 // Distance from destination before we stop
   Point3F mMoveDestination;           // Destination for movement
   bool mMoveSlowdown;                 // Slowdown as we near the destination

   SimObjectPtr<GameBase> mAimObject; // Object to point at, overrides location
   bool mAimLocationSet;               // Has an aim location been set?
   Point3F mAimLocation;               // Point to look at
   bool mTargetInLOS;                  // Is target object visible?
   F32   mBadLeadershipTime;

// start ds
// Added by Brendan Ledwich 2005-11-9
//		NodeInterpolator *mInterpolator;
		//SceneObject *mAvoidObject;

      //object avoidance data
		Point3F      mAvoidPoint;

      ShapeBase * mFollowObject;
	  S32 mFollowNodeIndex;
	  //S32 mFollowSlotIndex;
      //Point3F mCollisionPoint;
		//Point3F mMoveDestination;           // Destination for movement
// end ds

     
   //float       mFutureCollisionTime;
   //Collision   mFutureCollision;

   Obstacle    mObstacle;

   F32         mLastHardCollisionTime;

   Point3F mAimOffset;

   // Utility Methods
   void throwCallback( const char *name );

public:
//   Collision   *getFutureCollision(){ return mFutureCollisionTime>getTimeSeconds()? &mFutureCollision : 0;}
   Obstacle   *getFutureCollision();
   float       getFutureCollisionStrength();
   DECLARE_CONOBJECT( AIFishPlayer );

   AIFishPlayer();
   ~AIFishPlayer();

   bool  fullSpeed();

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
   bool  reachedTarget();

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

   Point3F getTargetPoint();

   void updateMove(const Move* move)
   {
		// todo: see if this works
		if (!getControllingClient() && isGhost())
			return;

//	   updateMoveOriginal(move);
      updateMoveSimple(move);
   }
// end jc

	void stopMove();
};

#endif
