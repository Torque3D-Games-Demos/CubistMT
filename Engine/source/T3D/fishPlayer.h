//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

#ifndef _FISHPLAYER_H_
#define _FISHPLAYER_H_

#ifndef _SHAPEBASE_H_
#include "T3D/shapeBase.h"
#endif
#ifndef _BOXCONVEX_H_
#include "collision/boxConvex.h"
#endif

#include "T3D/gameBase/gameProcess.h"

//#define  DEBUG_HUD       //bring up debug information about fish when clicked on rather than web page (must set $FISH_DEBUG=1 in aiFishplayerscript.cs as well)
//#define  DEBUG_GFX       //print letter above each fish showing his current main influence. Also draws arrow from each fish point at objects that are
                           //affecting it

class Material;
class ParticleEmitter;
class ParticleEmitterData;
class DecalData;
class SplashData;
class PhysicsPlayer;
class FishPlayer;


#define  USE_PHYSX    //if defined, use physx character controller (capsule), otherwise use 
//#define  NEW_COLLIDE    //when not using physx, this determines the type of collision mesh. If defined, collision comes from mesh data, otherwise, a box


//----------------------------------------------------------------------------

enum FishTypeFlags{
   FishFlagSmall        =0x01,
   FishFlagMedium       =0x02,
   FishFlagLarge        =0x04,
   FishFlagJellyFish    =0x08,
   FishFlagSwish        =0x010,
   FishFlagFlee         =0x020,
};

struct FishPlayerData: public ShapeBaseData {
   typedef ShapeBaseData Parent;
   enum Constants {
      RecoverDelayBits = 7,
      NumSpineNodes = 6,
      ImpactBits = 3,
      NUM_SPLASH_EMITTERS = 3,
      BUBBLE_EMITTER = 2,
   };
   bool renderFirstPerson;    ///< Render the player shape in first person

   StringTableEntry  imageAnimPrefix;                             ///< Passed along to mounted images to modify
                                                                  ///  animation sequences played in third person. [optional]
   bool              allowImageStateAnimation;                    ///< When true a new thread is added to the player to allow for
                                                                  ///  mounted images to request a sequence be played on the player
                                                                  ///  through the image's state machine.  It is only optional so
                                                                  ///  that we don't create a TSThread on the player if we don't
                                                                  ///  need to.

   StringTableEntry  shapeNameFP[ShapeBase::MaxMountedImages];    ///< Used to render with mounted images in first person [optional]
   StringTableEntry  imageAnimPrefixFP;                           ///< Passed along to mounted images to modify
                                                                  ///  animation sequences played in first person. [optional]
   Resource<TSShape> mShapeFP[ShapeBase::MaxMountedImages];       ///< First person mounted image shape resources [optional]
   U32               mCRCFP[ShapeBase::MaxMountedImages];         ///< Computed CRC values for the first person mounted image shapes
                                                                  ///  Depends on the ShapeBaseData computeCRC field.
   bool              mValidShapeFP[ShapeBase::MaxMountedImages];  ///< Indicates that there is a valid first person mounted image shape

   F32 pickupRadius;          ///< Radius around player for items (on server)
   F32 maxTimeScale;          ///< Max timeScale for action animations

   F32 minLookAngle;          ///< Lowest angle (radians) the player can look
   F32 maxLookAngle;          ///< Highest angle (radians) the player can look
   F32 maxFreelookAngle;      ///< Max left/right angle the player can look
   F32 maxUnderwaterForwardSpeed;   ///< pg: made public so influence code can read it

   F32  wanderSpeedScaleLo; 
   F32  wanderSpeedScaleHi; 
   F32  wanderTurnScale; 

   F32  fleeSpeedScale; 
   F32  fleeTurnScale; 

   F32   acceleration;
   F32   turnAcceleration;

   F32   swishSpeedScale;
   F32   swishTurnScale;

   F32   swishEffectDistance;

   /// @name Physics constants
   /// @{


   // new ai
   F32   wanderTimeMin;
   F32   wanderTimeMax;
   F32   followIncreaseRate;
   F32   followDecreaseRate;
   F32   wanderYawAngleChange;
   F32   wanderPitchAngleChange;
   F32   stuckTolerance;
   F32   reversingTime;
   F32   fleeEndDistance;
   F32   fleeStartDistance;
   F32   localFishRadius;
   F32   obstacleWarningTime;
   bool  noTilt;
   bool  noAnimationScale;             //dont scale animation with respect to speed
   bool  faceUser;                     //stop & rotate to face user when touched

   // schooling related stuff
   F32   schoolWanderSizeSideLo;         //range for random variation of heading
   F32   schoolWanderSizeSideHi;
   F32   schoolWanderSizeUpLo;         //range for random variation of heading
   F32   schoolWanderSizeUpHi;
   F32   schoolWanderPeriodLo;       //period of random heading variation
   F32   schoolWanderPeriodHi;





// start ds
   F32 walkForce;                   ///< Force used to accelerate player 
   F32 walkEnergyDrain;             ///< Energy drain/tick
   F32 maxWalkForwardSpeed;            ///< Maximum forward speed when running
   F32 maxWalkBackwardSpeed;           ///< Maximum backward speed when running
   F32 maxWalkSideSpeed;          ///< Maximum side speed when running
// end ds

   F32 maxStepHeight;         ///< Maximum height the player can step up
   F32 runSurfaceAngle;       ///< Maximum angle from vertical in degrees the player can run up

   F32 horizMaxSpeed;         ///< Max speed attainable in the horizontal
   F32 horizResistSpeed;      ///< Speed at which resistance will take place
   F32 horizResistFactor;     ///< Factor of resistance once horizResistSpeed has been reached

   F32 upMaxSpeed;            ///< Max vertical speed attainable
   F32 upResistSpeed;         ///< Speed at which resistance will take place
   F32 upResistFactor;        ///< Factor of resistance once upResistSpeed has been reached

   S32   fishTypeFlags;       ///< flags giving fish size etc....
   S32   menu;                ///< what this fish eats in terms of flags above

   // Running/Walking
   F32 runForce;              ///< Force used to accelerate player
   F32 runEnergyDrain;        ///< Energy drain/tick
   F32 minRunEnergy;          ///< Minimum energy required to run
   F32 maxForwardSpeed;       ///< Maximum forward speed when running
   F32 maxBackwardSpeed;      ///< Maximum backward speed when running
   F32 maxSideSpeed;          ///< Maximum side speed when running

   // Sprinting
   F32 sprintForce;                 ///< Force used to accelerate player
   F32 sprintEnergyDrain;           ///< Energy drain/tick
   F32 minSprintEnergy;             ///< Minimum energy required to sprint
   F32 maxSprintForwardSpeed;       ///< Maximum forward speed when sprinting
   F32 maxSprintBackwardSpeed;      ///< Maximum backward speed when sprinting
   F32 maxSprintSideSpeed;          ///< Maximum side speed when sprinting
   F32 sprintStrafeScale;           ///< Amount to scale strafing motion vector while sprinting
   F32 sprintYawScale;              ///< Amount to scale yaw motion while sprinting
   F32 sprintPitchScale;            ///< Amount to scale pitch motion while sprinting
   bool sprintCanJump;              ///< Can the player jump while sprinting

   // Swimming
   F32 swimForce;                   ///< Force used to accelerate player while swimming
   F32 maxUnderwaterBackwardSpeed;  ///< Maximum underwater backward speed when running
   F32 maxUnderwaterSideSpeed;      ///< Maximum underwater side speed when running
// start ds
   F32 swimDepth;
// end ds
   // Crouching
   F32 crouchForce;                 ///< Force used to accelerate player while crouching
   F32 maxCrouchForwardSpeed;       ///< Maximum forward speed when crouching
   F32 maxCrouchBackwardSpeed;      ///< Maximum backward speed when crouching
   F32 maxCrouchSideSpeed;          ///< Maximum side speed when crouching

   // Prone
   F32 proneForce;                  ///< Force used to accelerate player while prone
   F32 maxProneForwardSpeed;        ///< Maximum forward speed when prone
   F32 maxProneBackwardSpeed;       ///< Maximum backward speed when prone
   F32 maxProneSideSpeed;           ///< Maximum side speed when prone


// strat jc
   // Flying
   bool canFly;
// end jc

   /// @}

   /// @name Hitboxes
   /// @{

   F32 boxHeadPercentage;
   F32 boxTorsoPercentage;

   F32 boxHeadLeftPercentage;
   F32 boxHeadRightPercentage;
   F32 boxHeadBackPercentage;
   F32 boxHeadFrontPercentage;
   /// @}

   F32 minImpactSpeed;        ///< Minimum impact speed required to apply fall damage
   F32 minLateralImpactSpeed; ///< Minimum impact speed required to apply non-falling damage.

   F32 decalOffset;

   F32 groundImpactMinSpeed;      ///< Minimum impact speed required to apply fall damage with the ground
   VectorF groundImpactShakeFreq; ///< Frequency in each direction for the camera to shake
   VectorF groundImpactShakeAmp;  ///< How much to shake
   F32 groundImpactShakeDuration; ///< How long to shake
   F32 groundImpactShakeFalloff;  ///< How fast the shake disapates
// start jc - Dan&rsquo;s mods (turning intertia) ->  
   F32 horizTurnSpeedInertia; ///< Amount of z-axis rotation to carry over from last tick  
   F32 vertTurnSpeedInertia; ///< Amount of x-axis rotation to carry over from last tick  
//   F32 AITurnSpeed; ///< Fraction of actual distance for AIPlayers to turn  
   
   F32 maxTurnRate;
   bool conformToGround;
   F32 smoothCamera;
// end jc - <- Dan&rsquo;s mods (turning inertia)  

   /// Zounds!
   enum Sounds {
      FootSoft,
      FootHard,
      FootMetal,
      FootSnow,
      FootShallowSplash,
      FootWading,
      FootUnderWater,
      FootBubbles,
      MoveBubbles,
      WaterBreath,
      ImpactSoft,
      ImpactHard,
      ImpactMetal,
      ImpactSnow,
      ImpactWaterEasy,
      ImpactWaterMedium,
      ImpactWaterHard,
      ExitWater,
      MaxSounds
   };
   SFXTrack* sound[MaxSounds];

   Point3F boxSize;           ///< Width, depth, height
   Point3F crouchBoxSize;
   Point3F proneBoxSize;
   Point3F swimBoxSize;
// start ds - offset collision box and zrotation
	Point3F renderOffset;
// end ds

   /// Animation and other data initialized in onAdd
   struct ActionAnimationDef {
      const char* name;       ///< Sequence name
      struct Vector {
         F32 x,y,z;
      } dir;                  ///< Default direction
   };
   struct ActionAnimation {
      const char* name;       ///< Sequence name
      S32      sequence;      ///< Sequence index
      VectorF  dir;           ///< Dir of animation ground transform
      F32      speed;         ///< Speed in m/s
      bool     velocityScale; ///< Scale animation by velocity
      bool     death;         ///< Are we dying?
   };
   enum {
      // *** WARNING ***
      // These enum values are used to index the ActionAnimationList
      // array instantiated in player.cc
      // The first several are selected in the move state based on velocity
      RootAnim,
      RunForwardAnim,
      BackBackwardAnim,
      SideLeftAnim,
      SideRightAnim,

      SprintRootAnim,
      SprintForwardAnim,
      SprintBackwardAnim,
      SprintLeftAnim,
      SprintRightAnim,

      CrouchRootAnim,
      CrouchForwardAnim,
      CrouchBackwardAnim,
      CrouchLeftAnim,
      CrouchRightAnim,

      ProneRootAnim,
      ProneForwardAnim,
      ProneBackwardAnim,

      SwimRootAnim,
      SwimForwardAnim,
      SwimBackwardAnim,
      SwimLeftAnim,
      SwimRightAnim,

      // These are set explicitly based on player actions
      FallAnim,
      JumpAnim,
      StandJumpAnim,
      LandAnim,
      JetAnim,
      InteractAnim,

      // 
      NumTableActionAnims = InteractAnim + 1,

      NumExtraActionAnims = 512 - NumTableActionAnims,
      NumActionAnims = NumTableActionAnims + NumExtraActionAnims,
      ActionAnimBits = 9,
      NullAnimation = (1 << ActionAnimBits) - 1
   };

   static ActionAnimationDef ActionAnimationList[NumTableActionAnims];
   ActionAnimation actionList[NumActionAnims];
   U32 actionCount;
   U32 lookAction;
   S32 spineNode[NumSpineNodes];
   S32 pickupDelta;           ///< Base off of pcikupRadius
   F32 runSurfaceCos;         ///< Angle from vertical in cos(runSurfaceAngle)
   F32 jumpSurfaceCos;        ///< Angle from vertical in cos(jumpSurfaceAngle)

   enum Impacts {
      ImpactNone,
      ImpactNormal,
   };

   enum Recoil {
      LightRecoil,
      MediumRecoil,
      HeavyRecoil,
      NumRecoilSequences
   };
   S32 recoilSequence[NumRecoilSequences];

   /// @name Particles
   /// All of the data relating to environmental effects
   /// @{

   ParticleEmitterData * footPuffEmitter;
   S32 footPuffID;
   S32 footPuffNumParts;
   F32 footPuffRadius;

   DecalData* decalData;
   S32 decalID;

   ParticleEmitterData * dustEmitter;
   S32 dustID;

   SplashData* splash;
   S32 splashId;
   F32 splashVelocity;
   F32 splashAngle;
   F32 splashFreqMod;
   F32 splashVelEpsilon;
   F32 bubbleEmitTime;

   F32 medSplashSoundVel;
   F32 hardSplashSoundVel;
   F32 exitSplashSoundVel;
   F32 footSplashHeight;


   // Air control
   F32 airControl;

   // Jump off surfaces at their normal rather than straight up
   bool jumpTowardsNormal;

   // For use if/when mPhysicsPlayer is created
   StringTableEntry physicsPlayerType;

   ParticleEmitterData* splashEmitterList[NUM_SPLASH_EMITTERS];
   S32 splashEmitterIDList[NUM_SPLASH_EMITTERS];
   /// @}

   //
   DECLARE_CONOBJECT(FishPlayerData);
   FishPlayerData();
   bool preload(bool server, String &errorStr);
   void getGroundInfo(TSShapeInstance*,TSThread*,ActionAnimation*);
   bool isTableSequence(S32 seq);
   bool isJumpAction(U32 action);

   static void initPersistFields();
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);

   /// @name Callbacks
   /// @{
   DECLARE_CALLBACK( void, onPoseChange, ( FishPlayer* obj, const char* oldPose, const char* newPose ) );
   DECLARE_CALLBACK( void, onStartSwim, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onStopSwim, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onStartSprintMotion, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onStopSprintMotion, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, doDismount, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onEnterLiquid, ( FishPlayer* obj, F32 coverage, const char* type ) );
   DECLARE_CALLBACK( void, onLeaveLiquid, ( FishPlayer* obj, const char* type ) );
   DECLARE_CALLBACK( void, animationDone, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onEnterMissionArea, ( FishPlayer* obj ) );
   DECLARE_CALLBACK( void, onLeaveMissionArea, ( FishPlayer* obj ) );
   /// @}
};


//----------------------------------------------------------------------------

class FishPlayer: public ShapeBase
{
   typedef ShapeBase Parent;

public:
   enum Pose {
      StandPose = 0,
      SprintPose,
      CrouchPose,
      PronePose,
      SwimPose,
      NumPoseBits = 4
   };

   FishPlayerData* mDataBlock;    ///< MMmmmmm...datablock...
   bool        mEnableUserTouch;
   F32 mGameTime;
#ifdef  DEBUG_HUD
   bool  mDebugIt;
#endif

protected:
   static   const U32   sSpeedFractionBits;
   static   const U32   sSpeedIntegerBits;
   static   const U32   sSpeedTotalBits;
   static   const F32   sSpeedScale;
   static   const U32   sSpeedMaxSendValue;

   void  readVelocity(BitStream *stream);
   void  writeVelocity(BitStream *stream);

   virtual  bool  scaleAnimationSpeed(){ return !mDataBlock->noAnimationScale;}

   /// Bit masks for different types of events
   enum MaskBits {
      ActionMask   = Parent::NextFreeMask << 0,
      MoveMask     = Parent::NextFreeMask << 1,
      ImpactMask   = Parent::NextFreeMask << 2,
      NextFreeMask = Parent::NextFreeMask << 3
   };

   struct Range {
      Range(F32 _min,F32 _max) {
         min = _min;
         max = _max;
         delta = _max - _min;
      };
      F32 min,max;
      F32 delta;
   };
   PlaneSetF  *mPlaneSet;
   bool  mConvexMade;
   Polyhedron mPolyHedron;
   Polyhedron mPolyHedron2;
   SimObjectPtr<ParticleEmitter> mSplashEmitter[FishPlayerData::NUM_SPLASH_EMITTERS];
   F32 mBubbleEmitterTime;


   /// Client interpolation/warp data
   struct StateDelta {
      Move move;                    ///< Last move from server
      F32 dt;                       ///< Last interpolation time
      /// @name Interpolation data
     /// @{

      Point3F pos;
      Point3F rot;
      Point3F head;
      VectorF posVec;
      VectorF rotVec;
      VectorF headVec;
     /// @}

     /// @name Warp data
     /// @{

      S32 warpTicks;
      Point3F warpOffset;
      Point3F rotOffset;
     /// @}
   };
   StateDelta delta;                ///< Used for interpolation on the client.  @see StateDelta
   S32 mPredictionCount;            ///< Number of ticks to predict

   // Current pos, vel etc.
   Point3F mHead;                   ///< Head rotation, uses only x & z
   Point3F mRot;                    ///< Body rotation, uses only z
   Point3F mRotSmooth;     // start jc
   VectorF mVelocity;               ///< Velocity
   Point3F mAnchorPoint;            ///< Pos compression anchor
   static F32 mGravity;             ///< Gravity
   S32 mImpactSound;

   bool mUseHeadZCalc;              ///< Including mHead.z in transform calculations

   S32 mMountPending;               ///< mMountPending suppresses tickDelay countdown so players will sit until
                                    ///< their mount, or another animation, comes through (or 13 seconds elapses).

   F32 mMoveSpeed;                  ///< actually a speed scale value for maxUnderwaterSideSpeed, maxUnderwaterBackwardSpeed, maxUnderwaterForwardSpeed

   /// Main player state
   enum ActionState {
      NullState,
      MoveState,
      RecoverState,
      NumStateBits = 3
   };
   ActionState mState;              ///< What is the player doing? @see ActionState
   
   Pose  mPose;
   
   S32 mContactTimer;               ///< Ticks since last contact

   SFXSource* mMoveBubbleSound;   ///< Sound for moving bubbles
   SFXSource* mWaterBreathSound;  ///< Sound for underwater breath

   SimObjectPtr<ShapeBase> mControlObject; ///< Controlling object

   /// @name Animation threads & data
   /// @{

   struct ActionAnimation {
      U32 action;
      TSThread* thread;
      S32 delayTicks;               // before picking another.
      bool forward;
      bool firstPerson;
      bool waitForEnd;
      bool holdAtEnd;
      bool animateOnServer;
      bool atEnd;
   } mActionAnimation;

   struct ArmAnimation {
      U32 action;
      TSThread* thread;
   } mArmAnimation;
   TSThread* mArmThread;

   TSThread* mHeadVThread;
   TSThread* mHeadHThread;
   TSThread* mRecoilThread;
   TSThread* mImageStateThread;
   static Range mArmRange;
   static Range mHeadVRange;
   static Range mHeadHRange;
   /// @}

   //
   S32 mRecoverTicks;         ///< same as recoverTicks in the player datablock
   U32 mReversePending;
   F32 mRecoverDelay;         ///< When bypassing the legacy recover system and only using the land sequence,
                              ///  this is how long the player will be in the land sequence.

   bool mInWater;            ///< Is true if WaterCoverage is greater than zero
   bool mSwimming;            ///< Is true if WaterCoverage is above the swimming threshold

   Point3F mLastPos;          ///< Holds the last position for physics updates
   Point3F mLastWaterPos;     ///< Same as mLastPos, but for water

   F32     mTurnRateMultiplier;
   bool    mForwardOnly;

   struct ContactInfo 
   {
      bool contacted, jump, run;
      SceneObject *contactObject;
      VectorF  contactNormal;

      void clear()
      {
         contacted=jump=run=false; 
         contactObject = NULL; 
         contactNormal.set(1,1,1);
      }

      ContactInfo() { clear(); }

   } mContactInfo;

   struct Death {
      F32      lastPos;
      Point3F  posAdd;
      VectorF  rotate;
      VectorF  curNormal;
      F32      curSink;
      void     clear()           {dMemset(this, 0, sizeof(*this)); initFall();}
      VectorF  getPosAdd()       {VectorF ret(posAdd); posAdd.set(0,0,0); return ret;}
      bool     haveVelocity()    {return posAdd.x != 0 || posAdd.y != 0;}
      void     initFall()        {curNormal.set(0,0,1); curSink = 0;}
      Death()                    {clear();}
      MatrixF* fallToGround(F32 adjust, const Point3F& pos, F32 zrot, F32 boxRad);
   } mDeath;

   PhysicsPlayer *mPhysicsRep;
   Point3F  getCapsuleSize();

   // First person mounted image shapes
   TSShapeInstance*  mShapeFPInstance[ShapeBase::MaxMountedImages];
   TSThread *mShapeFPAmbientThread[ShapeBase::MaxMountedImages];
   TSThread *mShapeFPVisThread[ShapeBase::MaxMountedImages];
   TSThread *mShapeFPAnimThread[ShapeBase::MaxMountedImages];
   TSThread *mShapeFPFlashThread[ShapeBase::MaxMountedImages];
   TSThread *mShapeFPSpinThread[ShapeBase::MaxMountedImages];

   //---
   void           updateTrueVelocity();
   bool           mLastPositionExists;
   U32            mLastPositionCalls;
   Point3F        mLastPosition;
   Point3F        mTrueVelocity;
   
public:
   void  setTurnRateMultiplier(F32  a){ mTurnRateMultiplier = a;}
   F32   getTurnRateMultiplier(){ return mTurnRateMultiplier;}
   F32   getMaxTurnRate(){ return mDataBlock->maxTurnRate*mTurnRateMultiplier; }
   
   void  setForwardOnly(bool a){mForwardOnly = a;}
   bool  getForwardOnly(){return mForwardOnly;}


   // New collision
#ifdef   NEW_COLLIDE
   void  makeConvex();
   ShapeBaseConvex   mConvex;
   Box3F             mBox;
#else
   OrthoBoxConvex mConvex;
#endif
   Box3F          mWorkingQueryBox;

   /// Standing / Crouched / Prone or Swimming   
   Pose getPose() const { return mPose; }
   virtual const char* getPoseName() const;

   
   /// Setting this from script directly might not actually work,
   /// This is really just a helper for the player class so that its bounding box
   /// will get resized appropriately when the pose changes
   void setPose( Pose pose );

   float make2PiRange(float a);
   float makePiRange(float a);


   PhysicsPlayer* getPhysicsRep() const { return mPhysicsRep; }

protected:
#ifdef   NEW_COLLIDE
   Polyhedron  *getPolyHedron(const MatrixF& transform,const Box3F& box);
#endif
   virtual  void           onHitReef(const Collision &collision ){}
   virtual  void           onHitFish(const Collision &collision ){}

   virtual void reSkin();

   void setState(ActionState state, U32 ticks=0);
   void updateState();


   ///Update the movement
   virtual void updateMove(const Move *move);
   void updateMoveSimple(const Move* move);

   ///Interpolate movement
   Point3F _move( const F32 travelTime, Collision *outCol );
   F32 _doCollisionImpact( const Collision *collision);
   void _handleCollision( const Collision &collision );
   virtual bool updatePos(const F32 travelTime = TickSec);

   ///Update head animation
   void updateLookAnimation(F32 dT = 0.f);

   ///Update other animations
   void updateAnimation(F32 dt);
   void updateAnimationTree(bool firstPerson);

   virtual U32 getArmAction() const { return mArmAnimation.action; }
   virtual bool setArmThread(U32 action);
   virtual void setActionThread(U32 action,bool forward,bool hold = false,bool wait = false,bool fsp = false, bool forceSet = false);
   virtual void updateActionThread();
   virtual void pickBestMoveAction(U32 startAnim, U32 endAnim, U32 * action, bool * forward) const;
   virtual void pickActionAnimation();

   /// @name Mounted objects
   /// @{
   virtual void onUnmount( ShapeBase *obj, S32 node );
   virtual void unmount();
   /// @}

   void setPosition(const Point3F& pos,const Point3F& viewRot);
// start ds - conform to ground
   void setRenderTransform(const MatrixF &);
// end ds
   void setRenderPosition(const Point3F& pos,const Point3F& viewRot,F32 dt=-1);
   void _findContact( SceneObject **contactObject, VectorF *contactNormal, Vector<SceneObject*> *outOverlapObjects );
  // void findContact( bool *run, bool *jump, VectorF *contactNormal );
   void findContact( VectorF *contactNormal );
   void triggerCheck();

   void buildImagePrefixPaths(String* prefixPaths);
   S32 findPrefixSequence(String* prefixPaths, const String& baseSeq);
   S32 convertActionToImagePrefix(U32 action);

   virtual void onImage(U32 imageSlot, bool unmount);
   virtual void onImageRecoil(U32 imageSlot,ShapeBaseImageData::StateData::RecoilState);
   virtual void onImageStateAnimation(U32 imageSlot, const char* seqName, bool direction, bool scaleToState, F32 stateTimeOutValue);
   virtual const char* getImageAnimPrefix(U32 imageSlot, S32 imageShapeIndex);
   virtual void onImageAnimThreadChange(U32 imageSlot, S32 imageShapeIndex, ShapeBaseImageData::StateData* lastState, const char* anim, F32 pos, F32 timeScale, bool reset=false);
   virtual void onImageAnimThreadUpdate(U32 imageSlot, S32 imageShapeIndex, F32 dt);

   /// Set which client is controlling this player
   void setControllingClient(GameConnection* client);

   void calcClassRenderData();
   
   /// Play sound for foot contact.
   ///
   /// @param triggeredLeft If true, left foot hit; right otherwise.
   /// @param contactMaterial Material onto which the player stepped; may be NULL.
   /// @param contactObject Object onto which the player stepped; may be NULL.
   void playFootstepSound( bool triggeredLeft, Material* contactMaterial, SceneObject* contactObject );
   
   /// Play an impact sound.
   void playImpactSound();

   /// Are we in the process of dying?
   bool inDeathAnim();
   F32  deathDelta(Point3F &delta);
   void updateDeathOffsets();
   bool inSittingAnim();

   /// @name Water
   /// @{

   void updateSplash();                             ///< Update the splash effect
   void updateFroth( F32 dt );                      ///< Update any froth
   void updateWaterSounds( F32 dt );                ///< Update water sounds
   void createSplash( Point3F &pos, F32 speed );    ///< Creates a splash
   bool collidingWithWater( Point3F &waterHeight ); ///< Are we colliding with water?
   /// @}

   void disableHeadZCalc() { mUseHeadZCalc = false; }
   void enableHeadZCalc() { mUseHeadZCalc = true; }

   void renderDebug( ObjectRenderInst *ri,
                               SceneRenderState *state,
                               BaseMatInstance *overrideMat );

public:
   DECLARE_CONOBJECT(FishPlayer);

   FishPlayer();
   ~FishPlayer();
   virtual void onCollision( SceneObject *object, const VectorF &vec );
   static void consoleInit();

   /// @name Transforms
   /// @{

   void setTransform(const MatrixF &mat);
   void getEyeTransform(MatrixF* mat);
   void getEyeBaseTransform(MatrixF* mat);
   void getRenderEyeTransform(MatrixF* mat);
   void getRenderEyeBaseTransform(MatrixF* mat);
   void getCameraParameters(F32 *min, F32 *max, Point3F *offset, MatrixF *rot);

   /// @}

   F32 getSpeed() const;
   Point3F getVelocity() const;
   void setVelocity(const VectorF& vel);
   /// Apply an impulse at the given point, with magnitude/direction of vec
   void applyImpulse(const Point3F& pos,const VectorF& vec);
   /// Get the rotation of the player
   const Point3F& getRotation() { return mRot; }
   /// Get the rotation of the head of the player
   const Point3F& getHeadRotation() { return mHead; }
   void getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad);

   bool haveContact() const { return !mContactTimer; }         ///< Is it in contact with something
   F32 getMaxForwardVelocity() const { return (mDataBlock != NULL ? mDataBlock->maxForwardSpeed : 0); }

   virtual bool    isDisplacable() const;
   virtual Point3F getMomentum() const;
   virtual void    setMomentum(const Point3F &momentum);
   virtual bool    displaceObject(const Point3F& displaceVector);
   virtual bool    getAIMove(Move*);

   bool checkDismountPosition(const MatrixF& oldPos, const MatrixF& newPos);  ///< Is it safe to dismount here?

   //
   bool onAdd();
   void onRemove();
   bool onNewDataBlock( GameBaseData *dptr, bool reload );
   void onScaleChanged();
   Box3F mScaledBox;

   // Animation
   const char* getStateName();
   bool setActionThread(const char* sequence,bool hold,bool wait,bool fsp = false);
   const String& getArmThread() const;
   bool setArmThread(const char* sequence);

   // Object control
   void setControlObject(ShapeBase *obj);
   ShapeBase* getControlObject();
   
// start ds
   // Variable Movement -- Zshazz
//	static void initPersistFields(); // Has to be declared so that the engine knows to use my persist fields 
// end ds
   //
   void updateWorkingCollisionSet();
   virtual void processTick(const Move *move);
   void interpolateTick(F32 delta);
   void advanceTime(F32 dt);
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool buildPolyList(PolyListContext context, AbstractPolyList* polyList, const Box3F &box, const SphereF &sphere);
   void buildConvex(const Box3F& box, Convex* convex);
   bool isControlObject();
   float getTimeSeconds(){return mGameTime;}

   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *);
   void writePacketData(GameConnection *conn, BitStream *stream);
   void readPacketData (GameConnection *conn, BitStream *stream);
   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);

   virtual void prepRenderImage( SceneRenderState* state );
   virtual void renderConvex( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *overrideMat );   
   virtual void renderMountedImage( U32 imageSlot, TSRenderState &rstate, SceneRenderState *state );
// start ds
	bool mFrozen;
	void setFrozen(bool frozen) {if (frozen != mFrozen && isServerObject()) {setMaskBits(MoveMask);} mFrozen = frozen;}
// end ds
// start jc - moved to player for variable motion.
   void setMoveSpeedMultiplier( const F32 speed );
   F32 getMoveSpeedMultiplier() const { return mMoveSpeed; }
// end jc
};

typedef FishPlayer::Pose FishPlayerPose;

DefineEnumType( FishPlayerPose );

#endif
