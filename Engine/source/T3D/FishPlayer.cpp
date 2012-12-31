//-----------------------------------------------------------------------------
// Torque
// Copyright GarageGames, LLC 2011
//-----------------------------------------------------------------------------

//#define   NEW_VERSION
//#define   MISSING_ANIMATIONS_WARNING

#include "platform/platform.h"
#include "T3D/fishplayer.h"

#include "platform/profiler.h"
#include "math/mMath.h"
#include "math/mathIO.h"
#include "core/resourceManager.h"
#include "core/stringTable.h"
#include "core/volume.h"
#include "core/stream/bitStream.h"
#include "console/consoleTypes.h"
#include "console/engineAPI.h"
#include "collision/extrudedPolyList.h"
#include "collision/clippedPolyList.h"
#include "collision/earlyOutPolyList.h"
#include "collision/ConcretePolyList.h"
#include "collision/OptimizedPolyList.h"
#include "ts/tsShapeInstance.h"
#include "sfx/sfxSystem.h"
#include "sfx/sfxTrack.h"
#include "sfx/sfxSource.h"
#include "sfx/sfxTypes.h"
#include "scene/sceneManager.h"
#include "scene/sceneRenderState.h"
#include "T3D/gameBase/gameConnection.h"
#include "T3D/trigger.h"
#include "T3D/physicalZone.h"
#include "T3D/item.h"
#include "T3D/missionArea.h"
#include "T3D/fx/particleEmitter.h"
#include "T3D/fx/cameraFXMgr.h"
#include "T3D/fx/splash.h"
#include "T3D/tsStatic.h"
#include "T3D/physics/physicsPlugin.h"
#include "T3D/physics/physicsPlayer.h"
#include "T3D/decal/decalManager.h"
#include "T3D/decal/decalData.h"
#include "materials/baseMatInstance.h"

// Amount of time if takes to transition to a new action sequence.
// start jc
//static F32 sAnimationTransitionTime = 0.25f;
static F32 sAnimationTransitionTime = 0.5f;
// end jc
static bool sUseAnimationTransitions = true;
static F32 sLandReverseScale = 0.25f;
static F32 sSlowStandThreshSquared = 1.69f;
static S32 sRenderMyPlayer = true;
static S32 sRenderMyItems = true;
static bool sRenderPlayerCollision = false;

// Chooses new action animations every n ticks.
static const F32 sNewAnimationTickTime = 4.0f;
static const F32 sMountPendingTickWait = 13.0f * F32(TickMs);

// Number of ticks before we pick non-contact animations
static const S32 sContactTickTime = 10;

// Movement constants
static F32 sVerticalStepDot = 0.173f;   // 80
static F32 sMinFaceDistance = 0.01f;
static F32 sTractionDistance = 0.04f;
static F32 sNormalElasticity = 0.01f;
static U32 sMoveRetryCount = 5;
static F32 sMaxImpulseVelocity = 200.0f;

// Move triggers
static S32 sJumpTrigger = 2;
static S32 sCrouchTrigger = 3;
static S32 sProneTrigger = 4;
static S32 sSprintTrigger = 5;
static S32 sImageTrigger0 = 0;
static S32 sImageTrigger1 = 1;
static S32 sJumpJetTrigger = 1;
static S32 sVehicleDismountTrigger = 2;

// Client prediction
//static F32 sMinWarpTicks = 0.5f;       // Fraction of tick at which instant warp occurs
//static S32 sMaxWarpTicks = 3;          // Max warp duration in ticks
//static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict

static F32 sMinWarpTicks = 0.0f;       // Fraction of tick at which instant warp occurs
static S32 sMaxWarpTicks = 5;          // Max warp duration in ticks
static S32 sMaxPredictionTicks = 30;   // Number of ticks to predict

// Anchor point compression
const F32 sAnchorMaxDistance = 32.0f;

//
static U32 sCollisionMoveMask =  TerrainObjectType       |
                                 InteriorObjectType      |
                                 WaterObjectType         | 
                                 PlayerObjectType        |
                                 StaticShapeObjectType   | 
                                 VehicleObjectType       |
                                 PhysicalZoneObjectType;

static U32 sServerCollisionContactMask = sCollisionMoveMask |
                                         ItemObjectType     |
                                         TriggerObjectType  |
                                         CorpseObjectType;

static U32 sClientCollisionContactMask = sCollisionMoveMask |
                                         TriggerObjectType;

enum PlayerConstants {
   JumpSkipContactsMax = 8
};


//----------------------------------------------------------------------------
// FishPlayer shape animation sequences:

// look     Used to control the upper body arm motion.  Must animate
//          vertically +-80 deg.
FishPlayer::Range FishPlayer::mArmRange(mDegToRad(-80.0f),mDegToRad(+80.0f));

// head     Used to control the direction the head is looking.  Must
//          animated vertically +-80 deg .
FishPlayer::Range FishPlayer::mHeadVRange(mDegToRad(-80.0f),mDegToRad(+80.0f));

// Action Animations:
FishPlayerData::ActionAnimationDef FishPlayerData::ActionAnimationList[NumTableActionAnims] =
{
   // *** WARNING ***
   // This array is indexed using the enum values defined in player.h

   // Root is the default animation
   { "root" },       // RootAnim,

   // These are selected in the move state based on velocity
   { "run",          { 0.0f, 1.0f, 0.0f } },       // RunForwardAnim,
   { "back",         { 0.0f,-1.0f, 0.0f } },       // BackBackwardAnim
   { "side",         {-1.0f, 0.0f, 0.0f } },       // SideLeftAnim,
   { "side_right",   { 1.0f, 0.0f, 0.0f } },       // SideRightAnim,

   { "sprint_root" },
   { "sprint_forward",  { 0.0f, 1.0f, 0.0f } },
   { "sprint_backward", { 0.0f,-1.0f, 0.0f } },
   { "sprint_side",     {-1.0f, 0.0f, 0.0f } },
   { "sprint_right",    { 1.0f, 0.0f, 0.0f } },

   { "crouch_root" },
   { "crouch_forward",  { 0.0f, 1.0f, 0.0f } },
   { "crouch_backward", { 0.0f,-1.0f, 0.0f } },
   { "crouch_side",     {-1.0f, 0.0f, 0.0f } },
   { "crouch_right",    { 1.0f, 0.0f, 0.0f } },

   { "prone_root" },
   { "prone_forward",   { 0.0f, 1.0f, 0.0f } },
   { "prone_backward",  { 0.0f,-1.0f, 0.0f } },

   { "swim_root" },
   { "swim_forward",    { 0.0f, 1.0f, 0.0f } },
   { "swim_backward",   { 0.0f,-1.0f, 0.0f }  },
   { "swim_left",       {-1.0f, 0.0f, 0.0f }  },
   { "swim_right",      { 1.0f, 0.0f, 0.0f }  },

   // These are set explicitly based on player actions
   { "fall" },       // FallAnim
   { "jump" },       // JumpAnim
   { "standjump" },  // StandJumpAnim
   { "land" },       // LandAnim
   { "jet" },        // JetAnim
   { "interact" },        // interaction when user touches fish
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(FishPlayerData);

ConsoleDocClass( FishPlayerData,
   "@brief Defines properties for a FishPlayer object.\n\n"
   "@see FishPlayer\n"
   "@ingroup gameObjects\n"
);

IMPLEMENT_CALLBACK( FishPlayerData, onPoseChange, void, ( FishPlayer* obj, const char* oldPose, const char* newPose ), ( obj, oldPose, newPose ),
   "@brief Called when the player changes poses.\n\n"
   "@param obj The FishPlayer object\n"
   "@param oldPose The pose the player is switching from.\n"
   "@param newPose The pose the player is switching to.\n");

IMPLEMENT_CALLBACK( FishPlayerData, onStartSwim, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player starts swimming.\n\n"
   "@param obj The FishPlayer object\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onStopSwim, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player stops swimming.\n\n"
   "@param obj The FishPlayer object\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onStartSprintMotion, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player starts moving while in a Sprint pose.\n\n"
   "@param obj The FishPlayer object\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onStopSprintMotion, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player stops moving while in a Sprint pose.\n\n"
   "@param obj The FishPlayer object\n" );

IMPLEMENT_CALLBACK( FishPlayerData, doDismount, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when attempting to dismount the player from a vehicle.\n\n"
   "It is up to the doDismount() method to actually perform the dismount.  Often "
   "there are some conditions that prevent this, such as the vehicle moving too fast.\n"
   "@param obj The FishPlayer object\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onEnterLiquid, void, ( FishPlayer* obj, F32 coverage, const char* type ), ( obj, coverage, type ),
   "@brief Called when the player enters a liquid.\n\n"
   "@param obj The FishPlayer object\n"
   "@param coverage Percentage of the player's bounding box covered by the liquid\n"
   "@param type The type of liquid the player has entered\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onLeaveLiquid, void, ( FishPlayer* obj, const char* type ), ( obj, type ),
   "@brief Called when the player leaves a liquid.\n\n"
   "@param obj The FishPlayer object\n"
   "@param type The type of liquid the player has left\n" );

IMPLEMENT_CALLBACK( FishPlayerData, animationDone, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called on the server when a scripted animation completes.\n\n"
   "@param obj The FishPlayer object\n"
   "@see FishPlayer::setActionThread() for setting a scripted animation and its 'hold' parameter to "
   "determine if this callback is used.\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onEnterMissionArea, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player enters the mission area.\n\n"
   "@param obj The FishPlayer object\n"
   "@see MissionArea\n" );

IMPLEMENT_CALLBACK( FishPlayerData, onLeaveMissionArea, void, ( FishPlayer* obj ), ( obj ),
   "@brief Called when the player leaves the mission area.\n"
   "@param obj The FishPlayer object\n"
   "@see MissionArea\n" );

FishPlayerData::FishPlayerData()
{
   menu = 0;
   fishTypeFlags = 0;
   noTilt = false;
   noAnimationScale = false;
   faceUser = true;
   acceleration = 1.0f;

   swishEffectDistance = 1.0f;
   swishSpeedScale = 1.0f;
   swishTurnScale = 1.0f;

   wanderSpeedScaleLo = 0.1f; 
   wanderSpeedScaleHi = 0.1f; 
   fleeSpeedScale = 2.0f; 
   wanderTurnScale = 1.0f; 
   fleeTurnScale = 4.0f; 

   shadowEnable = true;
   shadowSize = 256;
   shadowProjectionDistance = 14.0f;

   renderFirstPerson = true;

   // Used for third person image rendering
   imageAnimPrefix = StringTable->insert("");

   allowImageStateAnimation = false;

   // Used for first person image rendering
   imageAnimPrefixFP = StringTable->insert("");
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      shapeNameFP[i] = StringTable->insert("");
      mCRCFP[i] = 0;
      mValidShapeFP[i] = false;
   }

   pickupRadius = 0.0f;
   minLookAngle = -1.4f;
   maxLookAngle = 1.4f;
   maxFreelookAngle = 3.0f;
   maxTimeScale = 1.5f;

   mass = 9.0f;         // from ShapeBase
   maxEnergy = 60.0f;   // from ShapeBase
   drag = 0.3f;         // from ShapeBase
   density = 1.1f;      // from ShapeBase

   maxStepHeight = 1.0f;
   runSurfaceAngle = 80.0f;


   // Running/Walking
   runForce = 40.0f * 9.0f;
   runEnergyDrain = 0.0f;
   minRunEnergy = 0.0f;
   maxForwardSpeed = 10.0f;
   maxBackwardSpeed = 10.0f;
   maxSideSpeed = 10.0f;


   // Sprinting
   sprintForce = 50.0f * 9.0f;
   sprintEnergyDrain = 0.0f;
   minSprintEnergy = 0.0f;
   maxSprintForwardSpeed = 15.0f;
   maxSprintBackwardSpeed = 10.0f;
   maxSprintSideSpeed = 10.0f;
   sprintStrafeScale = 1.0f;
   sprintYawScale = 1.0f;
   sprintPitchScale = 1.0f;
   sprintCanJump = true;

   // Swimming
   swimForce = 55.0f * 9.0f;  
   maxUnderwaterForwardSpeed = 6.0f;
   maxUnderwaterBackwardSpeed = 6.0f;
   maxUnderwaterSideSpeed = 6.0f;

   // Crouching
   crouchForce = 45.0f * 9.0f;
   maxCrouchForwardSpeed = 4.0f;
   maxCrouchBackwardSpeed = 4.0f;
   maxCrouchSideSpeed = 4.0f;    

   // Prone
   proneForce = 45.0f * 9.0f;            
   maxProneForwardSpeed = 2.0f;  
   maxProneBackwardSpeed = 2.0f; 
   maxProneSideSpeed = 0.0f;     

   horizMaxSpeed = 80.0f;
   horizResistSpeed = 38.0f;
   horizResistFactor = 1.0f;

   upMaxSpeed = 80.0f;
   upResistSpeed = 38.0f;
   upResistFactor = 1.0f;

// strat jc
   // Flying
   canFly = false;
// end jc


   minImpactSpeed = 25.0f;
   minLateralImpactSpeed = 25.0f;

   decalData      = NULL;
   decalID        = 0;
   decalOffset      = 0.0f;

   lookAction = 0;

   // size of bounding box
   boxSize.set(1.0f, 1.0f, 2.3f);
   crouchBoxSize.set(1.0f, 1.0f, 2.0f);
   proneBoxSize.set(1.0f, 2.3f, 1.0f);
   swimBoxSize.set(1.0f, 2.3f, 1.0f);
// start ds - offset collision box and zrotation
	renderOffset.set(0.0f,0.0f,0.0f);
//	zRotationOffset = 0.0f;
// end ds

   // location of head, torso, legs
   boxHeadPercentage = 0.85f;
   boxTorsoPercentage = 0.55f;

   // damage locations
   boxHeadLeftPercentage  = 0;
   boxHeadRightPercentage = 1;
   boxHeadBackPercentage  = 0;
   boxHeadFrontPercentage = 1;

   for (S32 i = 0; i < MaxSounds; i++)
      sound[i] = NULL;

   footPuffEmitter = NULL;
   footPuffID = 0;
   footPuffNumParts = 15;
   footPuffRadius = .25f;

   dustEmitter = NULL;
   dustID = 0;

   splash = NULL;
   splashId = 0;
   splashVelocity = 1.0f;
   splashAngle = 45.0f;
   splashFreqMod = 300.0f;
   splashVelEpsilon = 0.25f;
   bubbleEmitTime = 0.4f;

   medSplashSoundVel = 2.0f;
   hardSplashSoundVel = 3.0f;
   exitSplashSoundVel = 2.0f;
   footSplashHeight = 0.1f;

   dMemset( splashEmitterList, 0, sizeof( splashEmitterList ) );
   dMemset( splashEmitterIDList, 0, sizeof( splashEmitterIDList ) );

   groundImpactMinSpeed = 10.0f;
   groundImpactShakeFreq.set( 10.0f, 10.0f, 10.0f );
   groundImpactShakeAmp.set( 20.0f, 20.0f, 20.0f );
   groundImpactShakeDuration = 1.0f;
   groundImpactShakeFalloff = 10.0f;
// start jc - Dan&rsquo;s mods (turning inertia) ->
   horizTurnSpeedInertia = 0.0f;
   vertTurnSpeedInertia = 0.0f;
//   AITurnSpeed = 1.0f;
   maxTurnRate = M_2PI_F; // default to 1 full circle per second
   conformToGround = false;
   smoothCamera = 0.0f;
// end jc - <- Dan&rsquo;s mods (turning inertia)

   // Air control
   airControl = 0.0f;

   jumpTowardsNormal = true;

   physicsPlayerType = StringTable->insert("FishController");

   dMemset( actionList, 0, sizeof(actionList) );


}

bool FishPlayerData::preload(bool server, String &errorStr)
{
   if(!Parent::preload(server, errorStr))
      return false;

   // Resolve objects transmitted from server
   if( !server )
   {
      for( U32 i = 0; i < MaxSounds; ++ i )
      {
         String errorStr;
         if( !sfxResolve( &sound[ i ], errorStr ) )
            Con::errorf( "FishPlayerData::preload: %s", errorStr.c_str() );
      }
   }

   //
   runSurfaceCos = mCos(mDegToRad(runSurfaceAngle));

   // Validate some of the data


   // If we don't have a shape don't crash out trying to
   // setup animations and sequences.
   if ( mShape )
   {
      // Go ahead a pre-load the player shape
      TSShapeInstance* si = new TSShapeInstance(mShape, false);
      TSThread* thread = si->addThread();

      // Extract ground transform velocity from animations
      // Get the named ones first so they can be indexed directly.
      ActionAnimation *dp = &actionList[0];
      for (int i = 0; i < NumTableActionAnims; i++,dp++)
      {
         ActionAnimationDef *sp = &ActionAnimationList[i];
         dp->name          = sp->name;
         dp->dir.set(sp->dir.x,sp->dir.y,sp->dir.z);
         dp->sequence      = mShape->findSequence(sp->name);

         // If this is a sprint action and is missing a sequence, attempt to use
         // the standard run ones.
         if(dp->sequence == -1 && i >= SprintRootAnim && i <= SprintRightAnim)
         {
            S32 offset = i-SprintRootAnim;
            ActionAnimationDef *standDef = &ActionAnimationList[RootAnim+offset];
            dp->sequence = mShape->findSequence(standDef->name);
         }

         dp->velocityScale = true;
         dp->death         = false;
         if (dp->sequence != -1)
            getGroundInfo(si,thread,dp);

#ifdef   MISSING_ANIMATIONS_WARNING
         // No real reason to spam the console about a missing jet animation
         if (dStricmp(sp->name, "jet") != 0)
            AssertWarn(dp->sequence != -1, avar("FishPlayerData::preload - Unable to find named animation sequence '%s'!", sp->name));
#endif
      }
      for (int b = 0; b < mShape->sequences.size(); b++)
      {
         if (!isTableSequence(b))
         {
            dp->sequence      = b;
            dp->name          = mShape->getName(mShape->sequences[b].nameIndex);
            dp->velocityScale = false;
            getGroundInfo(si,thread,dp++);
         }
      }
      actionCount = dp - actionList;
      AssertFatal(actionCount <= NumActionAnims, "Too many action animations!");
      delete si;

      // Resolve lookAction index
      dp = &actionList[0];
      String lookName("look");
      for (int c = 0; c < actionCount; c++,dp++)
         if( dStricmp( dp->name, lookName ) == 0 )
            lookAction = c;

      // Resolve spine
      spineNode[0] = mShape->findNode("Bip01 Pelvis");
      spineNode[1] = mShape->findNode("Bip01 Spine");
      spineNode[2] = mShape->findNode("Bip01 Spine1");
      spineNode[3] = mShape->findNode("Bip01 Spine2");
      spineNode[4] = mShape->findNode("Bip01 Neck");
      spineNode[5] = mShape->findNode("Bip01 Head");

      // Recoil animations
      recoilSequence[0] = mShape->findSequence("light_recoil");
      recoilSequence[1] = mShape->findSequence("medium_recoil");
      recoilSequence[2] = mShape->findSequence("heavy_recoil");
   }

   // Convert pickupRadius to a delta of boundingBox
   //
   // NOTE: it is not really correct to precalculate a pickupRadius based 
   // on boxSize since the actual player's bounds can vary by "pose".
   //
   F32 dr = (boxSize.x > boxSize.y)? boxSize.x: boxSize.y;
   if (pickupRadius < dr)
      pickupRadius = dr;
   else
      if (pickupRadius > 2.0f * dr)
         pickupRadius = 2.0f * dr;
   pickupDelta = (S32)(pickupRadius - dr);


   // Load up all the emitters
   if (!footPuffEmitter && footPuffID != 0)
      if (!Sim::findObject(footPuffID, footPuffEmitter))
         Con::errorf(ConsoleLogEntry::General, "FishPlayerData::preload - Invalid packet, bad datablockId(footPuffEmitter): 0x%x", footPuffID);

   if (!decalData && decalID != 0 )
      if (!Sim::findObject(decalID, decalData))
         Con::errorf(ConsoleLogEntry::General, "FishPlayerData::preload Invalid packet, bad datablockId(decalData): 0x%x", decalID);

   if (!dustEmitter && dustID != 0 )
      if (!Sim::findObject(dustID, dustEmitter))
         Con::errorf(ConsoleLogEntry::General, "FishPlayerData::preload - Invalid packet, bad datablockId(dustEmitter): 0x%x", dustID);

   for (int i=0; i<NUM_SPLASH_EMITTERS; i++)
      if( !splashEmitterList[i] && splashEmitterIDList[i] != 0 )
         if( Sim::findObject( splashEmitterIDList[i], splashEmitterList[i] ) == false)
            Con::errorf(ConsoleLogEntry::General, "FishPlayerData::onAdd - Invalid packet, bad datablockId(particle emitter): 0x%x", splashEmitterIDList[i]);

// start jc - Dan&rsquo;s mods (turning inertia) ->
   //Constrain these to sensible values
   horizTurnSpeedInertia = mClampF(horizTurnSpeedInertia,0.0f,1.0f);
   vertTurnSpeedInertia = mClampF(vertTurnSpeedInertia,0.0f,1.0f);
//   AITurnSpeed = mClampF(AITurnSpeed,0.01f,1.0f);
// end jc - Dan&rsquo;s mods (turning inertia)

   // First person mounted image shapes.
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      bool shapeError = false;

      if (shapeNameFP[i] && shapeNameFP[i][0])
      {
         mShapeFP[i] = ResourceManager::get().load(shapeNameFP[i]);
         if (bool(mShapeFP[i]) == false)
         {
            errorStr = String::ToString("FishPlayerData: Couldn't load mounted image %d shape \"%s\"",i,shapeNameFP[i]);
            return false;
         }

         if(!server && !mShapeFP[i]->preloadMaterialList(mShapeFP[i].getPath()) && NetConnection::filesWereDownloaded())
            shapeError = true;

         if(computeCRC)
         {
            Con::printf("Validation required for mounted image %d shape: %s", i, shapeNameFP[i]);

            Torque::FS::FileNodeRef    fileRef = Torque::FS::GetFileNode(mShapeFP[i].getPath());

            if (!fileRef)
               return false;

            if(server)
               mCRCFP[i] = fileRef->getChecksum();
            else if(mCRCFP[i] != fileRef->getChecksum())
            {
               errorStr = String::ToString("FishPlayerData: Mounted image %d shape \"%s\" does not match version on server.",i,shapeNameFP[i]);
               return false;
            }
         }

         mValidShapeFP[i] = true;
      }
   }

   return true;
}

void FishPlayerData::getGroundInfo(TSShapeInstance* si, TSThread* thread,ActionAnimation *dp)
{
   dp->death = !dStrnicmp(dp->name, "death", 5);
   if (dp->death)
   {
      // Death animations use roll frame-to-frame changes in ground transform into position
      dp->speed = 0.0f;
      dp->dir.set(0.0f, 0.0f, 0.0f);

      // Death animations MUST define ground transforms, so add dummy ones if required
      if (si->getShape()->sequences[dp->sequence].numGroundFrames == 0)
         si->getShape()->setSequenceGroundSpeed(dp->name, Point3F(0, 0, 0), Point3F(0, 0, 0));
   }
   else
   {
      VectorF save = dp->dir;
      si->setSequence(thread,dp->sequence,0);
      si->animate();
      si->advanceTime(1);
      si->animateGround();
      si->getGroundTransform().getColumn(3,&dp->dir);
      if ((dp->speed = dp->dir.len()) < 0.01f)
      {
         // No ground displacement... In this case we'll use the
         // default table entry, if there is one.
         if (save.len() > 0.01f)
         {
            dp->dir = save;
            dp->speed = 1.0f;
            dp->velocityScale = false;
         }
         else
            dp->speed = 0.0f;
      }
      else
         dp->dir *= 1.0f / dp->speed;
   }
}

bool FishPlayerData::isTableSequence(S32 seq)
{
   // The sequences from the table must already have
   // been loaded for this to work.
   for (int i = 0; i < NumTableActionAnims; i++)
      if (actionList[i].sequence == seq)
         return true;
   return false;
}

bool FishPlayerData::isJumpAction(U32 action)
{
   return (action == JumpAnim || action == StandJumpAnim);
}

void FishPlayerData::initPersistFields()
{
   Con::setIntVariable("$FishFlags::Small",      FishFlagSmall);
   Con::setIntVariable("$FishFlags::Medium",     FishFlagMedium);
   Con::setIntVariable("$FishFlags::Large",      FishFlagLarge);
   Con::setIntVariable("$FishFlags::JellyFish",  FishFlagJellyFish);
   Con::setIntVariable("$FishFlags::Swish",      FishFlagSwish);
   Con::setIntVariable("$FishFlags::Flee",       FishFlagFlee);

   addField( "wanderTimeMin", TypeF32, Offset(wanderTimeMin, FishPlayerData),
      "@brief minimum time to move in straight line before changing direction.\n");
   addField( "wanderTimeMax", TypeF32, Offset(wanderTimeMax, FishPlayerData),
      "@brief maximum time to move in straight line before changing direction .\n");
   addField( "followIncreaseRate", TypeF32, Offset(followIncreaseRate, FishPlayerData),
      "@brief .\n");
   addField( "followDecreaseRate", TypeF32, Offset(followDecreaseRate, FishPlayerData),
      "@brief .\n");
   addField( "wanderYawAngleChange", TypeF32, Offset(wanderYawAngleChange, FishPlayerData),
      "@brief Range for random change of fish yaw.\n");
   addField( "wanderPitchAngleChange", TypeF32, Offset(wanderPitchAngleChange, FishPlayerData),
      "@brief Range for random change of fish pitch .\n");
   addField( "stuckTolerance", TypeF32, Offset(stuckTolerance, FishPlayerData),
      "@brief If fish hasnt moved by this amount over a period, he is taken to be stuck .\n");
   addField( "reversingTime", TypeF32, Offset(reversingTime, FishPlayerData),
      "@brief When stuck, will allow fish to go backwards for this time.\n");
   addField( "fleeEndDistance", TypeF32, Offset(fleeEndDistance, FishPlayerData),
      "@brief when fish is within this distance from a scary fish, he will enter the flee state.\n");
   addField( "fleeStartDistance", TypeF32, Offset(fleeStartDistance, FishPlayerData),
      "@brief when fish is within this distance from a scary fish, he will end a previous flee state.\n");
   addField( "localFishRadius", TypeF32, Offset(localFishRadius, FishPlayerData),
      "@brief Radius to look for fish for who might influence this fish.\n");
   addField( "obstacleWarningTime", TypeF32, Offset(obstacleWarningTime, FishPlayerData),
      "@brief How far ahead to look for obstacles.\n");
   addField( "noTilt", TypeBool, Offset(noTilt, FishPlayerData),
      "@brief disable tilt.\n");
   addField( "noAnimationScale", TypeBool, Offset(noAnimationScale, FishPlayerData),
      "@brief dont scale animation with respect to speed.\n");
   addField( "faceUser", TypeBool, Offset(faceUser, FishPlayerData),
      "@brief stop and rotate to face user when touched.\n");

   addField( "schoolWanderSizeSideLo", TypeF32, Offset(schoolWanderSizeSideLo, FishPlayerData),
      "@brief .\n");
   addField( "schoolWanderSizeSideHi", TypeF32, Offset(schoolWanderSizeSideHi, FishPlayerData),
      "@brief .\n");
   addField( "schoolWanderSizeUpLo", TypeF32, Offset(schoolWanderSizeUpLo, FishPlayerData),
      "@brief .\n");
   addField( "schoolWanderSizeUpHi", TypeF32, Offset(schoolWanderSizeUpHi, FishPlayerData),
      "@brief .\n");
   addField( "schoolWanderPeriodLo", TypeF32, Offset(schoolWanderPeriodLo, FishPlayerData),
      "@brief .\n");
   addField( "schoolWanderPeriodHi", TypeF32, Offset(schoolWanderPeriodHi, FishPlayerData),
      "@brief .\n");




   addField( "pickupRadius", TypeF32, Offset(pickupRadius, FishPlayerData),
      "@brief Radius around the player to collide with Items in the scene (on server).\n\n"
      "Internally the pickupRadius is added to the larger side of the initial bounding box "
      "to determine the actual distance, to a maximum of 2 times the bounding box size.  The "
      "initial bounding box is that used for the root pose, and therefore doesn't take into "
      "account the change in pose.\n");
   addField( "maxTimeScale", TypeF32, Offset(maxTimeScale, FishPlayerData),
      "@brief Maximum time scale for action animations.\n\n"
      "If an action animation has a defined ground frame, it is automatically scaled to match the "
      "player's ground velocity.  This field limits the maximum time scale used even if "
      "the player's velocity exceeds it." );

   addGroup( "Camera" );

      addField( "renderFirstPerson", TypeBool, Offset(renderFirstPerson, FishPlayerData),
         "@brief Flag controlling whether to render the player shape in first person view.\n\n" );

      addField( "minLookAngle", TypeF32, Offset(minLookAngle, FishPlayerData),
         "@brief Lowest angle (in radians) the player can look.\n\n"
         "@note An angle of zero is straight ahead, with positive up and negative down." );
      addField( "maxLookAngle", TypeF32, Offset(maxLookAngle, FishPlayerData),
         "@brief Highest angle (in radians) the player can look.\n\n"
         "@note An angle of zero is straight ahead, with positive up and negative down." );
      addField( "maxFreelookAngle", TypeF32, Offset(maxFreelookAngle, FishPlayerData),
         "@brief Defines the maximum left and right angles (in radians) the player can "
         "look in freelook mode.\n\n" );

   endGroup( "Camera" );

   addGroup( "Movement" );

      //start pg

      addField( "menu", TypeS32, Offset(menu, FishPlayerData),
         "@brief flags saying type of fish this fish eats. Type of fish specified with foodGroup field.\n\n" );
      addField( "fishTypeFlags", TypeS32, Offset(fishTypeFlags, FishPlayerData),
         "@brief flags saying food group to which this fish belongs.\n\n" );

      //end pg

      addField( "maxStepHeight", TypeF32, Offset(maxStepHeight, FishPlayerData),
         "@brief Maximum height the player can step up.\n\n"
         "The player will automatically step onto changes in ground height less "
         "than maxStepHeight.  The player will collide with ground height changes "
         "greater than this." );

      addField( "runForce", TypeF32, Offset(runForce, FishPlayerData),
         "@brief Force used to accelerate the player when running.\n\n" );

      addField( "runEnergyDrain", TypeF32, Offset(runEnergyDrain, FishPlayerData),
         "@brief Energy value drained each tick that the player is moving.\n\n"
         "The player will not be able to move when his energy falls below "
         "minRunEnergy.\n"
         "@note Setting this to zero will disable any energy drain.\n"
         "@see minRunEnergy\n");
      addField( "minRunEnergy", TypeF32, Offset(minRunEnergy, FishPlayerData),
         "@brief Minimum energy level required to run or swim.\n\n"
         "@see runEnergyDrain\n");

      addField( "maxForwardSpeed", TypeF32, Offset(maxForwardSpeed, FishPlayerData),
         "@brief Maximum forward speed when running." );
      addField( "maxBackwardSpeed", TypeF32, Offset(maxBackwardSpeed, FishPlayerData),
         "@brief Maximum backward speed when running." );
      addField( "maxSideSpeed", TypeF32, Offset(maxSideSpeed, FishPlayerData),
         "@brief Maximum sideways speed when running." );

      addField( "runSurfaceAngle", TypeF32, Offset(runSurfaceAngle, FishPlayerData),
         "@brief Maximum angle from vertical (in degrees) the player can run up.\n\n" );

      addField( "minImpactSpeed", TypeF32, Offset(minImpactSpeed, FishPlayerData),
         "@brief Minimum impact speed to apply falling damage.\n\n"
         "This field also sets the minimum speed for the onImpact callback "
         "to be invoked.\n"
         "@see ShapeBaseData::onImpact()\n");
      addField( "minLateralImpactSpeed", TypeF32, Offset(minLateralImpactSpeed, FishPlayerData),
         "@brief Minimum impact speed to apply non-falling damage.\n\n"
         "This field also sets the minimum speed for the onLateralImpact callback "
         "to be invoked.\n"
         "@see ShapeBaseData::onLateralImpact()\n");

      addField( "horizMaxSpeed", TypeF32, Offset(horizMaxSpeed, FishPlayerData),
         "@brief Maximum horizontal speed.\n\n"
         "@note This limit is only enforced if the player's horizontal speed "
         "exceeds horizResistSpeed.\n"
         "@see horizResistSpeed\n"
         "@see horizResistFactor\n" );
      addField( "horizResistSpeed", TypeF32, Offset(horizResistSpeed, FishPlayerData),
         "@brief Horizontal speed at which resistence will take place.\n\n"
         "@see horizMaxSpeed\n"
         "@see horizResistFactor\n" );
      addField( "horizResistFactor", TypeF32, Offset(horizResistFactor, FishPlayerData),
         "@brief Factor of resistence once horizResistSpeed has been reached.\n\n"
         "@see horizMaxSpeed\n"
         "@see horizResistSpeed\n" );

      addField( "upMaxSpeed", TypeF32, Offset(upMaxSpeed, FishPlayerData),
         "@brief Maximum upwards speed.\n\n"
         "@note This limit is only enforced if the player's upward speed exceeds "
         "upResistSpeed.\n"
         "@see upResistSpeed\n"
         "@see upResistFactor\n" );
      addField( "upResistSpeed", TypeF32, Offset(upResistSpeed, FishPlayerData),
         "@brief Upwards speed at which resistence will take place.\n\n"
         "@see upMaxSpeed\n"
         "@see upResistFactor\n" );
      addField( "upResistFactor", TypeF32, Offset(upResistFactor, FishPlayerData),
         "@brief Factor of resistence once upResistSpeed has been reached.\n\n"
         "@see upMaxSpeed\n"
         "@see upResistSpeed\n" );

// start jc - Dan&rsquo;s mods (turning inertia) ->
   addField("horizTurnSpeedInertia",TypeF32,Offset(horizTurnSpeedInertia,FishPlayerData));
   addField("vertTurnSpeedInertia",TypeF32,Offset(vertTurnSpeedInertia,FishPlayerData));
//   addField("AITurnSpeed",TypeF32,Offset(AITurnSpeed,FishPlayerData));
   addField("maxTurnRate",    TypeF32, Offset(maxTurnRate,     FishPlayerData));
   addField("conformToGround", TypeBool, Offset(conformToGround, FishPlayerData));
   addField("smoothCamera", TypeF32, Offset(smoothCamera,FishPlayerData));
// end jc - Dan&rsquo;s mods (turning inertia)
   endGroup( "Movement" );
   
// start ds
   addGroup( "Movement: Flying" );
   addField("canFly", TypeBool, Offset(canFly, FishPlayerData));
   endGroup( "Movement: Flying" );
// end ds

   
   addGroup( "Movement: Sprinting" );

      addField( "sprintForce", TypeF32, Offset(sprintForce, FishPlayerData),
         "@brief Force used to accelerate the player when sprinting.\n\n" );

      addField( "sprintEnergyDrain", TypeF32, Offset(sprintEnergyDrain, FishPlayerData),
         "@brief Energy value drained each tick that the player is sprinting.\n\n"
         "The player will not be able to move when his energy falls below "
         "sprintEnergyDrain.\n"
         "@note Setting this to zero will disable any energy drain.\n"
         "@see minSprintEnergy\n");
      addField( "minSprintEnergy", TypeF32, Offset(minSprintEnergy, FishPlayerData),
         "@brief Minimum energy level required to sprint.\n\n"
         "@see sprintEnergyDrain\n");

      addField( "maxSprintForwardSpeed", TypeF32, Offset(maxSprintForwardSpeed, FishPlayerData),
         "@brief Maximum forward speed when sprinting." );
      addField( "maxSprintBackwardSpeed", TypeF32, Offset(maxSprintBackwardSpeed, FishPlayerData),
         "@brief Maximum backward speed when sprinting." );
      addField( "maxSprintSideSpeed", TypeF32, Offset(maxSprintSideSpeed, FishPlayerData),
         "@brief Maximum sideways speed when sprinting." );

      addField( "sprintStrafeScale", TypeF32, Offset(sprintStrafeScale, FishPlayerData),
         "@brief Amount to scale strafing motion vector while sprinting." );
      addField( "sprintYawScale", TypeF32, Offset(sprintYawScale, FishPlayerData),
         "@brief Amount to scale yaw motion while sprinting." );
      addField( "sprintPitchScale", TypeF32, Offset(sprintPitchScale, FishPlayerData),
         "@brief Amount to scale pitch motion while sprinting." );

      addField( "sprintCanJump", TypeBool, Offset(sprintCanJump, FishPlayerData),
         "@brief Can the player jump while sprinting." );

   endGroup( "Movement: Sprinting" );

   addGroup( "Movement: Swimming" );

      addField( "swimForce", TypeF32, Offset(swimForce, FishPlayerData),
         "@brief Force used to accelerate the player when swimming.\n\n" );
      addField( "maxUnderwaterForwardSpeed", TypeF32, Offset(maxUnderwaterForwardSpeed, FishPlayerData),
         "@brief Maximum forward speed when underwater.\n\n" );
      addField( "maxUnderwaterBackwardSpeed", TypeF32, Offset(maxUnderwaterBackwardSpeed, FishPlayerData),
         "@brief Maximum backward speed when underwater.\n\n" );
      addField( "maxUnderwaterSideSpeed", TypeF32, Offset(maxUnderwaterSideSpeed, FishPlayerData),
         "@brief Maximum sideways speed when underwater.\n\n" );

      addField( "wanderSpeedScaleLo", TypeF32, Offset(wanderSpeedScaleLo, FishPlayerData),
         "@brief minimum wander speed relative to cruising speed.\n\n" );
      addField( "wanderSpeedScaleHi", TypeF32, Offset(wanderSpeedScaleHi, FishPlayerData),
         "@brief maximum wander speed relative to cruising speed.\n\n" );
      addField( "wanderTurnScale", TypeF32, Offset(wanderTurnScale, FishPlayerData),
         "@brief" );

      addField( "fleeSpeedScale", TypeF32, Offset(fleeSpeedScale, FishPlayerData),
         "@brief flee speed relative to cruising speed.\n\n" );
      addField( "fleeTurnScale", TypeF32, Offset(fleeTurnScale, FishPlayerData),
         "@brief" );

      addField( "swishEffectDistance", TypeF32, Offset(swishEffectDistance, FishPlayerData),
         "@brief How near fish must be to a finger to be effected by it.\n\n" );
      addField( "swishSpeedScale", TypeF32, Offset(swishSpeedScale, FishPlayerData),
         "@brief swish speed relative to cruising speed.\n\n" );
      addField( "swishTurnScale", TypeF32, Offset(swishTurnScale, FishPlayerData),
         "@brief" );

      addField( "acceleration", TypeF32, Offset(acceleration, FishPlayerData),
         "@brief" );
      addField( "turnAcceleration", TypeF32, Offset(turnAcceleration, FishPlayerData),
         "@brief" );

   endGroup( "Movement: Swimming" );

   addGroup( "Movement: Crouching" );

      addField( "crouchForce", TypeF32, Offset(crouchForce, FishPlayerData),
         "@brief Force used to accelerate the player when crouching.\n\n" );
      addField( "maxCrouchForwardSpeed", TypeF32, Offset(maxCrouchForwardSpeed, FishPlayerData),
         "@brief Maximum forward speed when crouching.\n\n" );
      addField( "maxCrouchBackwardSpeed", TypeF32, Offset(maxCrouchBackwardSpeed, FishPlayerData),
         "@brief Maximum backward speed when crouching.\n\n" );
      addField( "maxCrouchSideSpeed", TypeF32, Offset(maxCrouchSideSpeed, FishPlayerData),
         "@brief Maximum sideways speed when crouching.\n\n" );

   endGroup( "Movement: Crouching" );

   addGroup( "Movement: Prone" );

      addField( "proneForce", TypeF32, Offset(proneForce, FishPlayerData),
         "@brief Force used to accelerate the player when prone (laying down).\n\n" );
      addField( "maxProneForwardSpeed", TypeF32, Offset(maxProneForwardSpeed, FishPlayerData),
         "@brief Maximum forward speed when prone (laying down).\n\n" );
      addField( "maxProneBackwardSpeed", TypeF32, Offset(maxProneBackwardSpeed, FishPlayerData),
         "@brief Maximum backward speed when prone (laying down).\n\n" );
      addField( "maxProneSideSpeed", TypeF32, Offset(maxProneSideSpeed, FishPlayerData),
         "@brief Maximum sideways speed when prone (laying down).\n\n" );

   endGroup( "Movement: Prone" );

   addGroup( "Collision" );

      addField( "boundingBox", TypePoint3F, Offset(boxSize, FishPlayerData),
         "@brief Size of the bounding box used by the player for collision.\n\n"
         "Dimensions are given as \"width depth height\"." );
      addField( "crouchBoundingBox", TypePoint3F, Offset(crouchBoxSize, FishPlayerData),
         "@brief Collision bounding box used when the player is crouching.\n\n"
         "@see boundingBox" );
      addField( "proneBoundingBox", TypePoint3F, Offset(proneBoxSize, FishPlayerData),
         "@brief Collision bounding box used when the player is prone (laying down).\n\n"
         "@see boundingBox" );
      addField( "swimBoundingBox", TypePoint3F, Offset(swimBoxSize, FishPlayerData),
         "@brief Collision bounding box used when the player is swimming.\n\n"
         "@see boundingBox" );

      addField( "boxHeadPercentage", TypeF32, Offset(boxHeadPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box height that represents the head.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );
      addField( "boxTorsoPercentage", TypeF32, Offset(boxTorsoPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box height that represents the torso.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );
      addField( "boxHeadLeftPercentage", TypeF32, Offset(boxHeadLeftPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box width that represents the left side of the head.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );
      addField( "boxHeadRightPercentage", TypeF32, Offset(boxHeadRightPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box width that represents the right side of the head.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );
      addField( "boxHeadBackPercentage", TypeF32, Offset(boxHeadBackPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box depth that represents the back side of the head.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );
      addField( "boxHeadFrontPercentage", TypeF32, Offset(boxHeadFrontPercentage, FishPlayerData),
         "@brief Percentage of the player's bounding box depth that represents the front side of the head.\n\n"
         "Used when computing the damage location.\n"
         "@see FishPlayer::getDamageLocation" );

   endGroup( "Collision" );
   
   addGroup( "Interaction: Footsteps" );

      addField( "footPuffEmitter", TYPEID< ParticleEmitterData >(), Offset(footPuffEmitter, FishPlayerData),
         "@brief Particle emitter used to generate footpuffs (particles created as the player "
         "walks along the ground).\n\n"
         "@note The generation of foot puffs requires the appropriate triggeres to be defined in the "
         "player's animation sequences.  Without these, no foot puffs will be generated.\n");
      addField( "footPuffNumParts", TypeS32, Offset(footPuffNumParts, FishPlayerData),
         "@brief Number of footpuff particles to generate each step.\n\n"
         "Each foot puff is randomly placed within the defined foot puff radius.  This "
         "includes having footPuffNumParts set to one.\n"
         "@see footPuffRadius\n");
      addField( "footPuffRadius", TypeF32, Offset(footPuffRadius, FishPlayerData),
         "@brief Particle creation radius for footpuff particles.\n\n"
         "This is applied to each foot puff particle, even if footPuffNumParts is set to one.  So "
         "set this value to zero if you want a single foot puff placed at exactly the same location "
         "under the player each time.\n");
      addField( "dustEmitter", TYPEID< ParticleEmitterData >(), Offset(dustEmitter, FishPlayerData),
         "@brief Emitter used to generate dust particles.\n\n"
         "@note Currently unused." );

      addField( "decalData", TYPEID< DecalData >(), Offset(decalData, FishPlayerData),
         "@brief Decal to place on the ground for player footsteps.\n\n" );
      addField( "decalOffset",TypeF32, Offset(decalOffset, FishPlayerData),
         "@brief Distance from the center of the model to the right foot.\n\n"
         "While this defines the distance to the right foot, it is also used to place "
         "the left foot decal as well.  Just on the opposite side of the player." );

   endGroup( "Interaction: Footsteps" );

   addGroup( "Interaction: Sounds" );

      addField( "FootSoftSound", TypeSFXTrackName, Offset(sound[FootSoft], FishPlayerData),
         "@brief Sound to play when walking on a surface with Material footstepSoundId 0.\n\n" );
      addField( "FootHardSound", TypeSFXTrackName, Offset(sound[FootHard], FishPlayerData),
         "@brief Sound to play when walking on a surface with Material footstepSoundId 1.\n\n" );
      addField( "FootMetalSound", TypeSFXTrackName, Offset(sound[FootMetal], FishPlayerData),
         "@brief Sound to play when walking on a surface with Material footstepSoundId 2.\n\n" );
      addField( "FootSnowSound", TypeSFXTrackName, Offset(sound[FootSnow], FishPlayerData),
         "@brief Sound to play when walking on a surface with Material footstepSoundId 3.\n\n" );

      addField( "FootShallowSound", TypeSFXTrackName, Offset(sound[FootShallowSplash], FishPlayerData),
         "@brief Sound to play when walking in water and coverage is less than "
         "footSplashHeight.\n\n"
         "@see footSplashHeight\n" );
      addField( "FootWadingSound", TypeSFXTrackName, Offset(sound[FootWading], FishPlayerData),
         "@brief Sound to play when walking in water and coverage is less than 1, "
         "but > footSplashHeight.\n\n"
         "@see footSplashHeight\n" );
      addField( "FootUnderwaterSound", TypeSFXTrackName, Offset(sound[FootUnderWater], FishPlayerData),
         "@brief Sound to play when walking in water and coverage equals 1.0 "
         "(fully underwater).\n\n" );
      addField( "FootBubblesSound", TypeSFXTrackName, Offset(sound[FootBubbles], FishPlayerData),
         "@brief Sound to play when walking in water and coverage equals 1.0 "
         "(fully underwater).\n\n" );
      addField( "movingBubblesSound", TypeSFXTrackName, Offset(sound[MoveBubbles], FishPlayerData),
         "@brief Sound to play when in water and coverage equals 1.0 (fully underwater).\n\n"
         "Note that unlike FootUnderwaterSound, this sound plays even if the "
         "player is not moving around in the water.\n" );
      addField( "waterBreathSound", TypeSFXTrackName, Offset(sound[WaterBreath], FishPlayerData),
         "@brief Sound to play when in water and coverage equals 1.0 (fully underwater).\n\n"
         "Note that unlike FootUnderwaterSound, this sound plays even if the "
         "player is not moving around in the water.\n" );

      addField( "impactSoftSound", TypeSFXTrackName, Offset(sound[ImpactSoft], FishPlayerData),
         "@brief Sound to play after falling on a surface with Material footstepSoundId 0.\n\n" );
      addField( "impactHardSound", TypeSFXTrackName, Offset(sound[ImpactHard], FishPlayerData),
         "@brief Sound to play after falling on a surface with Material footstepSoundId 1.\n\n" );
      addField( "impactMetalSound", TypeSFXTrackName, Offset(sound[ImpactMetal], FishPlayerData),
         "@brief Sound to play after falling on a surface with Material footstepSoundId 2.\n\n" );
      addField( "impactSnowSound", TypeSFXTrackName, Offset(sound[ImpactSnow], FishPlayerData),
         "@brief Sound to play after falling on a surface with Material footstepSoundId 3.\n\n" );

      addField( "impactWaterEasy", TypeSFXTrackName, Offset(sound[ImpactWaterEasy], FishPlayerData),
         "@brief Sound to play when entering the water with velocity < "
         "mediumSplashSoundVelocity.\n\n"
         "@see mediumSplashSoundVelocity\n");
      addField( "impactWaterMedium", TypeSFXTrackName, Offset(sound[ImpactWaterMedium], FishPlayerData),
         "@brief Sound to play when entering the water with velocity >= "
         "mediumSplashSoundVelocity and < hardSplashSoundVelocity.\n\n"
         "@see mediumSplashSoundVelocity\n"
         "@see hardSplashSoundVelocity\n");
      addField( "impactWaterHard", TypeSFXTrackName, Offset(sound[ImpactWaterHard], FishPlayerData),
         "@brief Sound to play when entering the water with velocity >= "
         "hardSplashSoundVelocity.\n\n"
         "@see hardSplashSoundVelocity\n");
      addField( "exitingWater", TypeSFXTrackName, Offset(sound[ExitWater], FishPlayerData),
         "@brief Sound to play when exiting the water with velocity >= exitSplashSoundVelocity.\n\n"
         "@see exitSplashSoundVelocity\n");

   endGroup( "Interaction: Sounds" );

   addGroup( "Interaction: Splashes" );

      addField( "splash", TYPEID< SplashData >(), Offset(splash, FishPlayerData),
         "@brief SplashData datablock used to create splashes when the player moves "
         "through water.\n\n" );
      addField( "splashVelocity", TypeF32, Offset(splashVelocity, FishPlayerData),
         "@brief Minimum velocity when moving through water to generate splashes.\n\n" );
      addField( "splashAngle", TypeF32, Offset(splashAngle, FishPlayerData),
         "@brief Maximum angle (in degrees) from pure vertical movement in water to "
         "generate splashes.\n\n" );

      addField( "splashFreqMod", TypeF32, Offset(splashFreqMod, FishPlayerData),
         "@brief Multipled by speed to determine the number of splash particles to generate.\n\n" );
      addField( "splashVelEpsilon", TypeF32, Offset(splashVelEpsilon, FishPlayerData),
         "@brief Minimum speed to generate splash particles.\n\n" );
      addField( "bubbleEmitTime", TypeF32, Offset(bubbleEmitTime, FishPlayerData),
         "@brief Time in seconds to generate bubble particles after entering the water.\n\n" );
      addField( "splashEmitter", TYPEID< ParticleEmitterData >(), Offset(splashEmitterList, FishPlayerData), NUM_SPLASH_EMITTERS,
         "@brief Particle emitters used to generate splash particles.\n\n" );

      addField( "footstepSplashHeight", TypeF32, Offset(footSplashHeight, FishPlayerData),
         "@brief Water coverage level to choose between FootShallowSound and FootWadingSound.\n\n"
         "@see FootShallowSound\n"
         "@see FootWadingSound\n");

      addField( "mediumSplashSoundVelocity", TypeF32, Offset(medSplashSoundVel, FishPlayerData),
         "@brief Minimum velocity when entering the water for choosing between the impactWaterEasy and "
         "impactWaterMedium sounds to play.\n\n"
         "@see impactWaterEasy\n"
         "@see impactWaterMedium\n" );
      addField( "hardSplashSoundVelocity", TypeF32, Offset(hardSplashSoundVel, FishPlayerData),
         "@brief Minimum velocity when entering the water for choosing between the impactWaterMedium and "
         "impactWaterHard sound to play.\n\n"
         "@see impactWaterMedium\n"
         "@see impactWaterHard\n" );
      addField( "exitSplashSoundVelocity", TypeF32, Offset(exitSplashSoundVel, FishPlayerData),
         "@brief Minimum velocity when leaving the water for the exitingWater sound to "
         "play.\n\n"
         "@see exitingWater");

   endGroup( "Interaction: Splashes" );

   addGroup( "Interaction: Ground Impact" );

      addField( "groundImpactMinSpeed", TypeF32, Offset(groundImpactMinSpeed, FishPlayerData),
         "@brief Minimum falling impact speed to apply damage and initiate the camera "
         "shaking effect.\n\n" );
      addField( "groundImpactShakeFreq", TypePoint3F, Offset(groundImpactShakeFreq, FishPlayerData),
         "@brief Frequency of the camera shake effect after falling.\n\n"
         "This is how fast to shake the camera.\n");
      addField( "groundImpactShakeAmp", TypePoint3F, Offset(groundImpactShakeAmp, FishPlayerData),
         "@brief Amplitude of the camera shake effect after falling.\n\n"
         "This is how much to shake the camera.\n");
      addField( "groundImpactShakeDuration", TypeF32, Offset(groundImpactShakeDuration, FishPlayerData),
         "@brief Duration (in seconds) of the camera shake effect after falling.\n\n"
         "This is how long to shake the camera.\n");
      addField( "groundImpactShakeFalloff", TypeF32, Offset(groundImpactShakeFalloff, FishPlayerData),
         "@brief Falloff factor of the camera shake effect after falling.\n\n"
         "This is how to fade the camera shake over the duration.\n");

   endGroup( "Interaction: Ground Impact" );

   addGroup( "Physics" );

      // PhysicsPlayer
      addField( "physicsPlayerType", TypeString, Offset(physicsPlayerType, FishPlayerData),
         "@brief Specifies the type of physics used by the player.\n\n"
         "This depends on the physics module used.  An example is 'Capsule'.\n"
         "@note Not current used.\n");

   endGroup( "Physics" );

   addGroup( "First Person Arms" );

      addField( "imageAnimPrefixFP", TypeCaseString, Offset(imageAnimPrefixFP, FishPlayerData),
         "@brief Optional prefix to all mounted image animation sequences in first person.\n\n"
         "This defines a prefix that will be added when looking up mounted image "
         "animation sequences while in first person.  It allows for the customization "
         "of a first person image based on the type of player.\n");

      // Mounted images arrays
      addArray( "Mounted Images", ShapeBase::MaxMountedImages );

         addField( "shapeNameFP", TypeShapeFilename, Offset(shapeNameFP, FishPlayerData), ShapeBase::MaxMountedImages,
            "@brief File name of this player's shape that will be used in conjunction with the corresponding mounted image.\n\n"
            "These optional parameters correspond to each mounted image slot to indicate a shape that is rendered "
            "in addition to the mounted image shape.  Typically these are a player's arms (or arm) that is "
            "animated along with the mounted image's state animation sequences.\n");

      endArray( "Mounted Images" );

   endGroup( "First Person Arms" );

   addGroup( "Third Person" );

      addField( "imageAnimPrefix", TypeCaseString, Offset(imageAnimPrefix, FishPlayerData),
         "@brief Optional prefix to all mounted image animation sequences in third person.\n\n"
         "This defines a prefix that will be added when looking up mounted image "
         "animation sequences while in third person.  It allows for the customization "
         "of a third person image based on the type of player.\n");

      addField( "allowImageStateAnimation", TypeBool, Offset(allowImageStateAnimation, FishPlayerData),
         "@brief Allow mounted images to request a sequence be played on the FishPlayer.\n\n"
         "When true a new thread is added to the player to allow for "
         "mounted images to request a sequence be played on the player "
         "through the image's state machine.  It is only optional so "
         "that we don't create a TSThread on the player if we don't "
         "need to.\n");

   endGroup( "Third Person" );

   Parent::initPersistFields();
}

void FishPlayerData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeFlag(renderFirstPerson);

   stream->write(minLookAngle);
   stream->write(maxLookAngle);
   stream->write(maxFreelookAngle);
   stream->write(maxTimeScale);

   stream->write(mass);
   stream->write(maxEnergy);
   stream->write(drag);
   stream->write(density);

   stream->write(maxStepHeight);

   stream->write(runForce);
   stream->write(runEnergyDrain);
   stream->write(minRunEnergy);
   stream->write(maxForwardSpeed);
   stream->write(maxBackwardSpeed);
   stream->write(maxSideSpeed);
   stream->write(runSurfaceAngle);



   // Sprinting
   stream->write(sprintForce);
   stream->write(sprintEnergyDrain);
   stream->write(minSprintEnergy);
   stream->write(maxSprintForwardSpeed);
   stream->write(maxSprintBackwardSpeed);
   stream->write(maxSprintSideSpeed);
   stream->write(sprintStrafeScale);
   stream->write(sprintYawScale);
   stream->write(sprintPitchScale);
   stream->writeFlag(sprintCanJump);

   // Swimming
   stream->write(swimForce);   
   stream->write(maxUnderwaterForwardSpeed);
   stream->write(maxUnderwaterBackwardSpeed);
   stream->write(maxUnderwaterSideSpeed);

   // Crouching
   stream->write(crouchForce);   
   stream->write(maxCrouchForwardSpeed);
   stream->write(maxCrouchBackwardSpeed);
   stream->write(maxCrouchSideSpeed);

   // Prone
   stream->write(proneForce);   
   stream->write(maxProneForwardSpeed);
   stream->write(maxProneBackwardSpeed);
   stream->write(maxProneSideSpeed);

   stream->write(horizMaxSpeed);
   stream->write(horizResistSpeed);
   stream->write(horizResistFactor);

   stream->write(upMaxSpeed);
   stream->write(upResistSpeed);
   stream->write(upResistFactor);

   stream->write(splashVelocity);
   stream->write(splashAngle);
   stream->write(splashFreqMod);
   stream->write(splashVelEpsilon);
   stream->write(bubbleEmitTime);

   stream->write(medSplashSoundVel);
   stream->write(hardSplashSoundVel);
   stream->write(exitSplashSoundVel);
   stream->write(footSplashHeight);
   // Don't need damage scale on the client
   stream->write(minImpactSpeed);
   stream->write(minLateralImpactSpeed);

   for( U32 i = 0; i < MaxSounds; i++)
      sfxWrite( stream, sound[ i ] );

   mathWrite(*stream, boxSize);
   mathWrite(*stream, crouchBoxSize);
   mathWrite(*stream, proneBoxSize);
   mathWrite(*stream, swimBoxSize);

// start ds - offset collision box and zrotation
   stream->write(renderOffset.x);
   stream->write(renderOffset.y);
   stream->write(renderOffset.z);
//   stream->write(zRotationOffset);
// end ds

   if( stream->writeFlag( footPuffEmitter ) )
   {
      stream->writeRangedU32( footPuffEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }

   stream->write( footPuffNumParts );
   stream->write( footPuffRadius );

   if( stream->writeFlag( decalData ) )
   {
      stream->writeRangedU32( decalData->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }
   stream->write(decalOffset);

   if( stream->writeFlag( dustEmitter ) )
   {
      stream->writeRangedU32( dustEmitter->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
   }


   if (stream->writeFlag( splash ))
   {
      stream->writeRangedU32(splash->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }

   for( U32 i=0; i<NUM_SPLASH_EMITTERS; i++ )
   {
      if( stream->writeFlag( splashEmitterList[i] != NULL ) )
      {
         stream->writeRangedU32( splashEmitterList[i]->getId(), DataBlockObjectIdFirst,  DataBlockObjectIdLast );
      }
   }


   stream->write(groundImpactMinSpeed);
   stream->write(groundImpactShakeFreq.x);
   stream->write(groundImpactShakeFreq.y);
   stream->write(groundImpactShakeFreq.z);
   stream->write(groundImpactShakeAmp.x);
   stream->write(groundImpactShakeAmp.y);
   stream->write(groundImpactShakeAmp.z);
   stream->write(groundImpactShakeDuration);
   stream->write(groundImpactShakeFalloff);

// start jc - Dan&rsquo;s mods (turning inertia) ->
   stream->write(horizTurnSpeedInertia);
   stream->write(vertTurnSpeedInertia);
//   stream->write(AITurnSpeed);
   stream->write(maxTurnRate);
   stream->write(conformToGround);
   stream->write(smoothCamera);
// end jc - Dan&rsquo;s mods (turning inertia)

   // Air control
   stream->write(airControl);

   // Jump off at normal
   stream->writeFlag(jumpTowardsNormal);

   stream->writeString(physicsPlayerType);

   // Third person mounted image shapes
   stream->writeString(imageAnimPrefix);

   stream->writeFlag(allowImageStateAnimation);

   // First person mounted image shapes
   stream->writeString(imageAnimPrefixFP);
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      stream->writeString(shapeNameFP[i]);

      // computeCRC is handled in ShapeBaseData
      if (computeCRC)
      {
         stream->write(mCRCFP[i]);
      }
   }
}

void FishPlayerData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   renderFirstPerson = stream->readFlag();

   stream->read(&minLookAngle);
   stream->read(&maxLookAngle);
   stream->read(&maxFreelookAngle);
   stream->read(&maxTimeScale);

   stream->read(&mass);
   stream->read(&maxEnergy);
   stream->read(&drag);
   stream->read(&density);

   stream->read(&maxStepHeight);

   stream->read(&runForce);
   stream->read(&runEnergyDrain);
   stream->read(&minRunEnergy);
   stream->read(&maxForwardSpeed);
   stream->read(&maxBackwardSpeed);
   stream->read(&maxSideSpeed);
   stream->read(&runSurfaceAngle);

   // Sprinting
   stream->read(&sprintForce);
   stream->read(&sprintEnergyDrain);
   stream->read(&minSprintEnergy);
   stream->read(&maxSprintForwardSpeed);
   stream->read(&maxSprintBackwardSpeed);
   stream->read(&maxSprintSideSpeed);
   stream->read(&sprintStrafeScale);
   stream->read(&sprintYawScale);
   stream->read(&sprintPitchScale);
   sprintCanJump = stream->readFlag();

   // Swimming
   stream->read(&swimForce);
   stream->read(&maxUnderwaterForwardSpeed);
   stream->read(&maxUnderwaterBackwardSpeed);
   stream->read(&maxUnderwaterSideSpeed);   

   // Crouching
   stream->read(&crouchForce);
   stream->read(&maxCrouchForwardSpeed);
   stream->read(&maxCrouchBackwardSpeed);
   stream->read(&maxCrouchSideSpeed);

   // Prone
   stream->read(&proneForce);
   stream->read(&maxProneForwardSpeed);
   stream->read(&maxProneBackwardSpeed);
   stream->read(&maxProneSideSpeed);

   stream->read(&horizMaxSpeed);
   stream->read(&horizResistSpeed);
   stream->read(&horizResistFactor);

   stream->read(&upMaxSpeed);
   stream->read(&upResistSpeed);
   stream->read(&upResistFactor);

   stream->read(&splashVelocity);
   stream->read(&splashAngle);
   stream->read(&splashFreqMod);
   stream->read(&splashVelEpsilon);
   stream->read(&bubbleEmitTime);

   stream->read(&medSplashSoundVel);
   stream->read(&hardSplashSoundVel);
   stream->read(&exitSplashSoundVel);
   stream->read(&footSplashHeight);

   stream->read(&minImpactSpeed);
   stream->read(&minLateralImpactSpeed);

   for( U32 i = 0; i < MaxSounds; i++)
      sfxRead( stream, &sound[ i ] );

   mathRead(*stream, &boxSize);
   mathRead(*stream, &crouchBoxSize);
   mathRead(*stream, &proneBoxSize);
   mathRead(*stream, &swimBoxSize);
// start ds - offset collision box and zrotation
   stream->read(&renderOffset.x);
   stream->read(&renderOffset.y);
   stream->read(&renderOffset.z);
//   stream->read(&zRotationOffset);
// end ds

   if( stream->readFlag() )
   {
      footPuffID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }

   stream->read(&footPuffNumParts);
   stream->read(&footPuffRadius);

   if( stream->readFlag() )
   {
      decalID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }
   stream->read(&decalOffset);

   if( stream->readFlag() )
   {
      dustID = (S32) stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   }


   if (stream->readFlag())
   {
      splashId = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
   }

   for( U32 i=0; i<NUM_SPLASH_EMITTERS; i++ )
   {
      if( stream->readFlag() )
      {
         splashEmitterIDList[i] = stream->readRangedU32( DataBlockObjectIdFirst, DataBlockObjectIdLast );
      }
   }

   stream->read(&groundImpactMinSpeed);
   stream->read(&groundImpactShakeFreq.x);
   stream->read(&groundImpactShakeFreq.y);
   stream->read(&groundImpactShakeFreq.z);
   stream->read(&groundImpactShakeAmp.x);
   stream->read(&groundImpactShakeAmp.y);
   stream->read(&groundImpactShakeAmp.z);
   stream->read(&groundImpactShakeDuration);
   stream->read(&groundImpactShakeFalloff);

// start jc - Dan&rsquo;s mods (turning inertia) ->
   stream->read(&horizTurnSpeedInertia);
   stream->read(&vertTurnSpeedInertia);
//   stream->read(&AITurnSpeed);
   stream->read(&maxTurnRate);
   stream->read(&conformToGround);
   stream->read(&smoothCamera);
// end jc - Dan&rsquo;s mods (turning inertia)

   // Air control
   stream->read(&airControl);

   // Jump off at normal
   jumpTowardsNormal = stream->readFlag();

   physicsPlayerType = stream->readSTString();

   // Third person mounted image shapes
   imageAnimPrefix = stream->readSTString();

   allowImageStateAnimation = stream->readFlag();

   // First person mounted image shapes
   imageAnimPrefixFP = stream->readSTString();
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      shapeNameFP[i] = stream->readSTString();

      // computeCRC is handled in ShapeBaseData
      if (computeCRC)
      {
         stream->read(&(mCRCFP[i]));
      }
   }
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

ImplementEnumType( FishPlayerPose,
   "@brief The pose of the FishPlayer.\n\n"
   "@ingroup gameObjects\n\n")
   { FishPlayer::StandPose,    "Stand",    "Standard movement pose.\n" },
   { FishPlayer::SprintPose,   "Sprint",   "Sprinting pose.\n" },
   { FishPlayer::CrouchPose,   "Crouch",   "Crouch pose.\n" },
   { FishPlayer::PronePose,    "Prone",    "Prone pose.\n" },
   { FishPlayer::SwimPose,     "Swim",     "Swimming pose.\n" },
EndImplementEnumType;

//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(FishPlayer);

ConsoleDocClass( FishPlayer,
   "@ingroup gameObjects\n"
);

F32 FishPlayer::mGravity = -20;

//----------------------------------------------------------------------------

FishPlayer::FishPlayer()
{
#ifdef  DEBUG_HUD
   mDebugIt=false;
#endif
   mGameTime = 0;
   mActionAnimation.forward = true;

   mLastPositionExists = false;
   mLastPositionCalls=0;
   mLastPosition=Point3F(0,0,0);
   mTrueVelocity=Point3F(0,0,0);

   mForwardOnly = false;
   mTurnRateMultiplier = 1.0f;
   mConvexMade = false;
   mTypeMask |= PlayerObjectType | DynamicShapeObjectType;
// start jc
#ifdef TORQUE_HIFI_NET
   mTypeMask |= GameBaseHiFiObjectType;
#endif
// end jc

   delta.pos = mAnchorPoint = Point3F(0,0,100);
   delta.rot = delta.head = Point3F(0,0,0);
   delta.rotOffset.set(0.0f,0.0f,0.0f);
   delta.warpOffset.set(0.0f,0.0f,0.0f);
   delta.posVec.set(0.0f,0.0f,0.0f);
   delta.rotVec.set(0.0f,0.0f,0.0f);
   delta.headVec.set(0.0f,0.0f,0.0f);
   delta.warpTicks = 0;
   delta.dt = 1.0f;
   delta.move = NullMove;
   mPredictionCount = sMaxPredictionTicks;
   mObjToWorld.setColumn(3,delta.pos);
   mRot = delta.rot;
// start jc
   mRotSmooth = delta.rot;
// end jc
   mHead = delta.head;
   mVelocity.set(0.0f, 0.0f, 0.0f);
   mDataBlock = 0;
   mHeadHThread = mHeadVThread = mRecoilThread = mImageStateThread = 0;
   mArmAnimation.action = FishPlayerData::NullAnimation;
   mArmAnimation.thread = 0;
   mActionAnimation.action = FishPlayerData::NullAnimation;
   mActionAnimation.thread = 0;
   mActionAnimation.delayTicks = 0;
   mActionAnimation.forward = true;
   mActionAnimation.firstPerson = false;
   //mActionAnimation.time = 1.0f; //ActionAnimation::Scale;
   mActionAnimation.waitForEnd = false;
   mActionAnimation.holdAtEnd = false;
   mActionAnimation.animateOnServer = false;
   mActionAnimation.atEnd = false;
   mState = MoveState;
   mSwimming = true;
   mInWater = false;
   mPose = SwimPose;
   mContactTimer = 0;
   mControlObject = 0;
   dMemset( mSplashEmitter, 0, sizeof( mSplashEmitter ) );

   mUseHeadZCalc = true;

   mImpactSound = 0;
   mRecoverTicks = 0;
   mReversePending = 0;

   mLastPos.set( 0.0f, 0.0f, 0.0f );

   mMoveBubbleSound = 0;
   mWaterBreathSound = 0;   

#ifdef   NEW_COLLIDE
#else
   mConvex.init(this);
#endif

   mWorkingQueryBox.minExtents.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.maxExtents.set(-1e9f, -1e9f, -1e9f);

   mBubbleEmitterTime = 10.0;
   mLastWaterPos.set( 0.0, 0.0, 0.0 );

   mMountPending = 0;
// start ds
   /*
   // Variable Movement -- Zshazz
   mForceMod = 0.0f; // I'm implementing it so that if it is 0, then it does nothing
   mMaxForwardSpeedMod = 0.0f;
   mMaxBackwardSpeedMod = 0.0f;
   mMaxSideSpeedMod = 0.0f;
   */
// end ds
// start jc - moved to player for variable motion.
   mMoveSpeed = 1.0f;
// end jc

// start ds
	mFrozen = false;
// end ds

   mPhysicsRep = NULL;

   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      mShapeFPInstance[i] = 0;
      mShapeFPAmbientThread[i] = 0;
      mShapeFPVisThread[i] = 0;
      mShapeFPAnimThread[i] = 0;
      mShapeFPFlashThread[i] = 0;
      mShapeFPSpinThread[i] = 0;
   }
}

FishPlayer::~FishPlayer()
{
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      delete mShapeFPInstance[i];
      mShapeFPInstance[i] = 0;
   }
}
void FishPlayer::onCollision( SceneObject *object, const VectorF &vec )
{
   Parent::onCollision(object, vec);
}


//----------------------------------------------------------------------------

// start ds
/*
// Variable Movement -- Zshazz
// Had to make the initPersistFields entirely because player doesn't normally have it.
void FishPlayer::initPersistFields()
{
   Parent::initPersistFields();
   
   // Add the Variable Movement things you want.
   addField("forwardForceMod", TypeF32, Offset(mForceMod, FishPlayer));
   addField("maxForwardSpeedMod", TypeF32, Offset(mMaxForwardSpeedMod,FishPlayer));
   addField("maxBackwardSpeedMod", TypeF32, Offset(mMaxBackwardSpeedMod,FishPlayer));
   addField("maxSideSpeedMod", TypeF32, Offset(mMaxSideSpeedMod,FishPlayer));
}
*/
// end ds
// start jc - moved to player for variable motion.
/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
void FishPlayer::setMoveSpeedMultiplier( F32 speed )
{
   mMoveSpeed = mClampF(speed, 0.0001f, 1000.0f);
   setMaskBits(MoveMask);
}
// end jc

bool FishPlayer::onAdd()
{
   mEnableUserTouch = false;

   ActionAnimation serverAnim = mActionAnimation;
   if(!Parent::onAdd() || !mDataBlock)
      return false;


   mWorkingQueryBox.minExtents.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.maxExtents.set(-1e9f, -1e9f, -1e9f);

   addToScene();

   // Make sure any state and animation passed from the server
   // in the initial update is set correctly.
   ActionState state = mState;
   mState = NullState;
   setState(state);
   setPose(StandPose);

   if (serverAnim.action != FishPlayerData::NullAnimation)
   {
      setActionThread(serverAnim.action, true, serverAnim.holdAtEnd, true, false, true);
      if (serverAnim.atEnd)
      {
         mShapeInstance->clearTransition(mActionAnimation.thread);
         mShapeInstance->setPos(mActionAnimation.thread,
                                mActionAnimation.forward ? 1.0f : 0.0f);
         if (inDeathAnim())
            mDeath.lastPos = 1.0f;
      }

      // We have to leave them sitting for a while since mounts don't come through right
      // away (and sometimes not for a while).  Still going to let this time out because
      // I'm not sure if we're guaranteed another anim will come through and cancel.
      if (!isServerObject() && inSittingAnim())
         mMountPending = (S32) sMountPendingTickWait;
      else
         mMountPending = 0;
   }
   if (mArmAnimation.action != FishPlayerData::NullAnimation)
      setArmThread(mArmAnimation.action);

   //
   if (isServerObject())
   {
      scriptOnAdd();
   }
   else
   {
      U32 i;
      for( i=0; i<FishPlayerData::NUM_SPLASH_EMITTERS; i++ )
      {
         if ( mDataBlock->splashEmitterList[i] ) 
         {
            mSplashEmitter[i] = new ParticleEmitter;
            mSplashEmitter[i]->onNewDataBlock( mDataBlock->splashEmitterList[i], false );
            if( !mSplashEmitter[i]->registerObject() )
            {
               Con::warnf( ConsoleLogEntry::General, "Could not register splash emitter for class: %s", mDataBlock->getName() );
               mSplashEmitter[i].getPointer()->destroySelf();
               mSplashEmitter[i] = NULL;
            }
         }
      }
      mLastWaterPos = getPosition();

      // clear out all camera effects
      gCamFXMgr.clear();
   }
#ifdef   USE_PHYSX
   if ( PHYSICSMGR )
   {
      PhysicsWorld *world = PHYSICSMGR->getWorld( isServerObject() ? "server" : "client" );

      mPhysicsRep = PHYSICSMGR->createPlayer();
      Point3F  capsuleSize = getCapsuleSize();
      mPhysicsRep->init(   mDataBlock->physicsPlayerType,
                           capsuleSize,
                           mDataBlock->runSurfaceCos,
                           mDataBlock->maxStepHeight,
                           this, 
                           world );
      mPhysicsRep->setTransform( getTransform() );
   }
#endif
   return true;
}
Point3F  FishPlayer::getCapsuleSize()
{
   Point3F  boxSize = mDataBlock->boxSize;
   boxSize.x *= getScale().x;
   boxSize.y *= getScale().y;
   boxSize.z *= getScale().z;

   Point3F  capsuleSize;
   //x=capsule radius, y = height (distance between bottom of caps)
   capsuleSize.x = getMax( boxSize.x, boxSize.y ) * 0.5f; 
   capsuleSize.y = 0.00001f; //cant be zero
   capsuleSize.z = 0;
   return capsuleSize;
}

void FishPlayer::onRemove()
{
   setControlObject(0);
   scriptOnRemove();
   removeFromScene();

   U32 i;
   for( i=0; i<FishPlayerData::NUM_SPLASH_EMITTERS; i++ )
   {
      if( mSplashEmitter[i] )
      {
         mSplashEmitter[i]->deleteWhenEmpty();
         mSplashEmitter[i] = NULL;
      }
   }

   mWorkingQueryBox.minExtents.set(-1e9f, -1e9f, -1e9f);
   mWorkingQueryBox.maxExtents.set(-1e9f, -1e9f, -1e9f);

   SAFE_DELETE( mPhysicsRep );		

   Parent::onRemove();
}

void FishPlayer::onScaleChanged()
{
   const Point3F& scale = getScale();
   mScaledBox = mObjBox;
   mScaledBox.minExtents.convolve( scale );
   mScaledBox.maxExtents.convolve( scale );

/*
   Point3F  lo, hi;
   lo = mPolyHedron.pointList[0];
   hi = mPolyHedron.pointList[0];
   for(int j=0; j<mPolyHedron.pointList.size(); j++){
      Point3F  p = mPolyHedron.pointList[j];
      if( lo.x > p.x ) lo.x = p.x;
      if( lo.y > p.y ) lo.y = p.y;
      if( lo.z > p.z ) lo.z = p.z;

      if( hi.x < p.x ) hi.x = p.x;
      if( hi.y < p.y ) hi.y = p.y;
      if( hi.z < p.z ) hi.z = p.z;
   }
*/

}


//----------------------------------------------------------------------------

bool FishPlayer::onNewDataBlock( GameBaseData *dptr, bool reload )
{
   FishPlayerData* prevData = mDataBlock;
   mDataBlock = dynamic_cast<FishPlayerData*>(dptr);
   if ( !mDataBlock || !Parent::onNewDataBlock( dptr, reload ) )
      return false;

   // FishPlayer requires a shape instance.
   if ( mShapeInstance == NULL )
      return false;

   // Initialize arm thread, preserve arm sequence from last datablock.
   // Arm animation can be from last datablock, or sent from the server.
   U32 prevAction = mArmAnimation.action;
   mArmAnimation.action = FishPlayerData::NullAnimation;
   if (mDataBlock->lookAction) {
      mArmAnimation.thread = mShapeInstance->addThread();
      mShapeInstance->setTimeScale(mArmAnimation.thread,0);
      if (prevData) {
         if (prevAction != prevData->lookAction && prevAction != FishPlayerData::NullAnimation)
            setArmThread(prevData->actionList[prevAction].name);
         prevAction = FishPlayerData::NullAnimation;
      }
      if (mArmAnimation.action == FishPlayerData::NullAnimation) {
         mArmAnimation.action = (prevAction != FishPlayerData::NullAnimation)?
            prevAction: mDataBlock->lookAction;
         mShapeInstance->setSequence(mArmAnimation.thread,
           mDataBlock->actionList[mArmAnimation.action].sequence,0);
      }
   }
   else
      mArmAnimation.thread = 0;

   // Initialize head look thread
   TSShape const* shape = mShapeInstance->getShape();
   S32 headSeq = shape->findSequence("head");
   if (headSeq != -1) {
      mHeadVThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mHeadVThread,headSeq,0);
      mShapeInstance->setTimeScale(mHeadVThread,0);
   }
   else
      mHeadVThread = 0;

   headSeq = shape->findSequence("headside");
   if (headSeq != -1) {
      mHeadHThread = mShapeInstance->addThread();
      mShapeInstance->setSequence(mHeadHThread,headSeq,0);
      mShapeInstance->setTimeScale(mHeadHThread,0);
   }
   else
      mHeadHThread = 0;

   // Create Recoil thread if any recoil sequences are specified.
   // Note that the server player does not play this animation.
   mRecoilThread = 0;
   if (isGhost())
      for (U32 s = 0; s < FishPlayerData::NumRecoilSequences; s++)
         if (mDataBlock->recoilSequence[s] != -1) {
            mRecoilThread = mShapeInstance->addThread();
            mShapeInstance->setSequence(mRecoilThread, mDataBlock->recoilSequence[s], 0);
            mShapeInstance->setTimeScale(mRecoilThread, 0);
            break;
         }

   // Reset the image state driven animation thread.  This will be properly built
   // in onImageStateAnimation() when needed.
   mImageStateThread = 0;

   // Initialize the primary thread, the actual sequence is
   // set later depending on player actions.
   mActionAnimation.action = FishPlayerData::NullAnimation;
   mActionAnimation.thread = mShapeInstance->addThread();
   updateAnimationTree(!isGhost());

   // First person mounted image shapes.  Only on client.
   if ( isGhost() )
   {
      for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
      {
         if (bool(mDataBlock->mShapeFP[i]))
         {
            mShapeFPInstance[i] = new TSShapeInstance(mDataBlock->mShapeFP[i], isClientObject());

            mShapeFPInstance[i]->cloneMaterialList();

            // Ambient animation
            if (mShapeFPAmbientThread[i])
            {
               S32 seq = mShapeFPInstance[i]->getShape()->findSequence("ambient");
               if (seq != -1)
               {
                  mShapeFPAmbientThread[i] = mShapeFPInstance[i]->addThread();
                  mShapeFPInstance[i]->setTimeScale(mShapeFPAmbientThread[i], 1);
                  mShapeFPInstance[i]->setSequence(mShapeFPAmbientThread[i], seq, 0);
               }
            }

            // Standard state animation
            mShapeFPAnimThread[i] = mShapeFPInstance[i]->addThread();
            if (mShapeFPAnimThread[i])
            {
               mShapeFPInstance[i]->setTimeScale(mShapeFPAnimThread[i],0);
            }
         }
      }
   }

   if ( isGhost() )
   {
      // Create the sounds ahead of time.  This reduces runtime
      // costs and makes the system easier to understand.

      SFX_DELETE( mMoveBubbleSound );
      SFX_DELETE( mWaterBreathSound );

      if ( mDataBlock->sound[FishPlayerData::MoveBubbles] )
         mMoveBubbleSound = SFX->createSource( mDataBlock->sound[FishPlayerData::MoveBubbles] );

      if ( mDataBlock->sound[FishPlayerData::WaterBreath] )
         mWaterBreathSound = SFX->createSource( mDataBlock->sound[FishPlayerData::WaterBreath] );
   }



//   mObjBox.maxExtents.x = mDataBlock->boxSize.x * 0.5f;
//   mObjBox.maxExtents.y = mDataBlock->boxSize.y * 0.5f;
//   mObjBox.maxExtents.z = mDataBlock->boxSize.z;
   Point3F  e = getObjBox().getExtents();
   mObjBox.maxExtents.x = e.x * 0.5f;
   mObjBox.maxExtents.y = e.y * 0.5f;
   mObjBox.maxExtents.z = e.z;

   mObjBox.minExtents.x = -mObjBox.maxExtents.x;
   mObjBox.minExtents.y = -mObjBox.maxExtents.y;
   mObjBox.minExtents.z = 0.0f;

   // Setup the box for our convex object...
#ifdef   NEW_COLLIDE
#else
   mObjBox.getCenter(&mConvex.mCenter);
   mConvex.mSize.x = mObjBox.len_x() / 2.0f;
   mConvex.mSize.y = mObjBox.len_y() / 2.0f;
   mConvex.mSize.z = mObjBox.len_z() / 2.0f;
#endif
   // Initialize our scaled attributes as well
   onScaleChanged();
   resetWorldBox();

   scriptOnNewDataBlock();
   return true;
}

//----------------------------------------------------------------------------

void FishPlayer::reSkin()
{
   if ( isGhost() && mShapeInstance && mSkinNameHandle.isValidString() )
   {
      Vector<String> skins;
      String(mSkinNameHandle.getString()).split( ";", skins );

      for ( int i = 0; i < skins.size(); i++ )
      {
         String oldSkin( mAppliedSkinName.c_str() );
         String newSkin( skins[i] );

         // Check if the skin handle contains an explicit "old" base string. This
         // allows all models to support skinning, even if they don't follow the 
         // "base_xxx" material naming convention.
         S32 split = newSkin.find( '=' );    // "old=new" format skin?
         if ( split != String::NPos )
         {
            oldSkin = newSkin.substr( 0, split );
            newSkin = newSkin.erase( 0, split+1 );
         }

         // Apply skin to both 3rd person and 1st person shape instances
         mShapeInstance->reSkin( newSkin, oldSkin );
         for ( int j = 0; j < ShapeBase::MaxMountedImages; j++ )
         {
            if (mShapeFPInstance[j])
               mShapeFPInstance[j]->reSkin( newSkin, oldSkin );
         }

         mAppliedSkinName = newSkin;
      }
   }
}

//----------------------------------------------------------------------------

void FishPlayer::setControllingClient(GameConnection* client)
{
   Parent::setControllingClient(client);
   if (mControlObject)
      mControlObject->setControllingClient(client);
}

void FishPlayer::setControlObject(ShapeBase* obj)
{
   if (mControlObject == obj)
      return;

   if (mControlObject) {
      mControlObject->setControllingObject(0);
      mControlObject->setControllingClient(0);
   }
   if (obj == this || obj == 0)
      mControlObject = 0;
   else {
      if (ShapeBase* coo = obj->getControllingObject())
         coo->setControlObject(0);
      if (GameConnection* con = obj->getControllingClient())
         con->setControlObject(0);

      mControlObject = obj;
      mControlObject->setControllingObject(this);
      mControlObject->setControllingClient(getControllingClient());
   }
}

void FishPlayer::onCameraScopeQuery(NetConnection *connection, CameraScopeQuery *query)
{
   // First, we are certainly in scope, and whatever we're riding is too...
   if(mControlObject.isNull() || mControlObject == mMount.object)
      Parent::onCameraScopeQuery(connection, query);
   else
   {
      connection->objectInScope(this);
      if (isMounted())
         connection->objectInScope(mMount.object);
      mControlObject->onCameraScopeQuery(connection, query);
   }
}

ShapeBase* FishPlayer::getControlObject()
{
   return mControlObject;
}

void FishPlayer::processTick(const Move* move)
{
   PROFILE_SCOPE(FishPlayer_ProcessTick);

   updateTrueVelocity();

   mGameTime += TickSec;

   bool prevMoveMotion = mMoveMotion;
   Pose prevPose = getPose();

// start ds
	if (!mFrozen)
	{
// end ds
   // If we're not being controlled by a client, let the
   // AI sub-module get a chance at producing a move.
   Move aiMove;
   if (!move && isServerObject() && getAIMove(&aiMove))
      move = &aiMove;

   // Manage the control object and filter moves for the player
   Move pMove,cMove;
   if (mControlObject) {
      if (!move)
         mControlObject->processTick(0);
      else {
         pMove = NullMove;
         cMove = *move;
         if (move->freeLook) {
            // Filter yaw/pitch/roll when freelooking.
            pMove.yaw = move->yaw;
            pMove.pitch = move->pitch;
            pMove.roll = move->roll;
            pMove.freeLook = true;
            cMove.freeLook = false;
            cMove.yaw = cMove.pitch = cMove.roll = 0.0f;
         }
         mControlObject->processTick((mDamageState == Enabled)? &cMove: &NullMove);
         move = &pMove;
      }
   }

// start ds
	}
// end ds
   Parent::processTick(move);
   // Warp to catch up to server
   if (delta.warpTicks > 0) {
      delta.warpTicks--;

      // Set new pos
      getTransform().getColumn(3, &delta.pos);
      delta.pos += delta.warpOffset;
      delta.rot += delta.rotOffset;

      delta.rot.x = makePiRange(delta.rot.x);
      delta.rot.y = makePiRange(delta.rot.y);
      delta.rot.z = makePiRange(delta.rot.z);


      setPosition(delta.pos,delta.rot);
// start jc
   //   F32 smoothFactor = mClampF(getMoveSpeedMultiplier()/10.0f, 0.0001f, 0.999f);
   //   F32 smoothFactor = mClampF(mVelocity.len()/10.0f, 0.0001f, 0.999f);
      F32 smoothFactor = mClampF(mVelocity.len(), 0.001f, 0.999f);
      if(mAbs(delta.rot.z - mRotSmooth.z) < M_PI_F)
         mRotSmooth.z = mLerp(mRotSmooth.z, delta.rot.z, smoothFactor);
      else
      {
         if(mRotSmooth.z > delta.rot.z)
            mRotSmooth.z = mLerp(mRotSmooth.z, delta.rot.z+M_2PI_F, smoothFactor);
         else
            mRotSmooth.z = mLerp(mRotSmooth.z, delta.rot.z-M_2PI_F, smoothFactor);
         mRotSmooth.z = makePiRange(mRotSmooth.z);
      }

      setRenderPosition(delta.pos,delta.rot);
// end jc
      updateDeathOffsets();
// start jc
      updateLookAnimation();
//      updateLookAnimation(TickSec);
// end jc

      // Backstepping
      delta.posVec = -delta.warpOffset;
      delta.rotVec = -delta.rotOffset;
   }
	else if (!mFrozen)
	{
      // If there is no move, the player is either an
      // unattached player on the server, or a player's
      // client ghost.
      if (!move) {
         if (isGhost()) {
            // If we haven't run out of prediction time,
            // predict using the last known move.
            if (mPredictionCount-- <= 0)
               return;

            move = &delta.move;
         }
         else
            move = &NullMove;
      }
      if (!isGhost())
         updateAnimation(TickSec);

      PROFILE_START(Player_PhysicsSection);
      if ( isServerObject() || didRenderLastRender() || getControllingClient() )
      {
         if ( !mPhysicsRep )
         {
            if ( isMounted() )
            {
               // If we're mounted then do not perform any collision checks
               // and clear our previous working list.

               mConvex.clearWorkingList();
            }
            else
            {
               updateWorkingCollisionSet();
            }
         }

         updateState();

         updateMove(move);

      // start jc
         mRotSmooth = mLerp(mRotSmooth, delta.rot, 0.01f);
         updateLookAnimation();
      //   updateLookAnimation(TickSec);
      // end jc
         updateDeathOffsets();

         updatePos();
      }
      PROFILE_END();

      if (!isGhost())
      {
         // Animations are advanced based on frame rate on the
         // client and must be ticked on the server.
         updateActionThread();
         updateAnimationTree(true);

         // Check for sprinting motion changes
         Pose currentPose = getPose();
         // FishPlayer has just switched into Sprint pose and is moving
         if (currentPose == SprintPose && prevPose != SprintPose && mMoveMotion)
         {
            mDataBlock->onStartSprintMotion_callback( this );
         }
         // FishPlayer has just switched out of Sprint pose and is moving, or was just moving
         else if (currentPose != SprintPose && prevPose == SprintPose && (mMoveMotion || prevMoveMotion))
         {
            mDataBlock->onStopSprintMotion_callback( this );
         }
         // FishPlayer is in Sprint pose and has modified their motion
         else if (currentPose == SprintPose && prevMoveMotion != mMoveMotion)
         {
            if (mMoveMotion)
            {
               mDataBlock->onStartSprintMotion_callback( this );
            }
            else
            {
               mDataBlock->onStopSprintMotion_callback( this );
            }
         }
      }
   }
}
void FishPlayer::interpolateTick(F32 dt)
{
   if (mControlObject)
      mControlObject->interpolateTick(dt);
// start ds
	if (!mFrozen)
	{
// end ds
   // Client side interpolation
   Parent::interpolateTick(dt);

   Point3F pos = delta.pos + delta.posVec * dt;
   Point3F rot = delta.rot + delta.rotVec * dt;

   updateLookAnimation(dt);

   setRenderPosition(pos,rot,dt);

/*
   // apply camera effects - is this the best place? - bramage
   GameConnection* connection = GameConnection::getConnectionToServer();
   if( connection->isFirstPerson() )
   {
      ShapeBase *obj = dynamic_cast<ShapeBase*>(connection->getControlObject());
      if( obj == this )
      {
         MatrixF curTrans = getRenderTransform();
         curTrans.mul( gCamFXMgr.getTrans() );
         Parent::setRenderTransform( curTrans );
      }
   }
*/
//   updateLookAnimation(dt);
//start ds
	}
// end ds
   delta.dt = dt;
}

void FishPlayer::advanceTime(F32 dt)
{
// start ds
	if (!mFrozen)
	{
// end ds
   // Client side animations
   Parent::advanceTime(dt);
   updateActionThread();
   updateAnimation(dt);
   updateSplash();
   updateFroth(dt);
   updateWaterSounds(dt);

   mLastPos = getPosition();
// start ds
	}
// end ds


   if (mImpactSound)
      playImpactSound();

   // update camera effects.  Definitely need to find better place for this - bramage
   if( isControlObject() )
   {
      if( mDamageState == Disabled || mDamageState == Destroyed )
      {
         // clear out all camera effects being applied to player if dead
         gCamFXMgr.clear();
      }
   }
}

bool FishPlayer::getAIMove(Move* move)
{
   return false;
}

void FishPlayer::setState(ActionState state, U32 recoverTicks)
{
   if (state != mState) {
      // Skip initialization if there is no manager, the state
      // will get reset when the object is added to a manager.
      if (isProperlyAdded()) {
         switch (state) {
            case RecoverState: {
               {
                  // Legacy recover system
                  mRecoverTicks = recoverTicks;
                  mReversePending = U32(F32(mRecoverTicks) / sLandReverseScale);
                  setActionThread(FishPlayerData::LandAnim, true, false, true, true);
               }
               break;
            }
            
            default:
               break;
         }
      }

      mState = state;
   }
}

void FishPlayer::updateState()
{
   switch (mState)
   {
      case RecoverState:
         {
            // Legacy recover system
            if (mRecoverTicks-- <= 0)
            {
               if (mReversePending && mActionAnimation.action != FishPlayerData::NullAnimation)
               {
                  // this serves and counter, and direction state
                  mRecoverTicks = mReversePending;
                  mActionAnimation.forward = false;
           // start ds - brendan animation fix
               if (mActionAnimation.action != FishPlayerData::NullAnimation)
               {
			 // end jc

                  S32 seq = mDataBlock->actionList[mActionAnimation.action].sequence;
                  S32 imageBasedSeq = convertActionToImagePrefix(mActionAnimation.action);
                  if (imageBasedSeq != -1)
                     seq = imageBasedSeq;

                  F32 pos = mShapeInstance->getPos(mActionAnimation.thread);

                  mShapeInstance->setTimeScale(mActionAnimation.thread, -sLandReverseScale);
                  mShapeInstance->transitionToSequence(mActionAnimation.thread,
                                                       seq, pos, sAnimationTransitionTime, true);
                  mReversePending = 0;
            // start ds - brendan animation fix
               }
            // end ds
               }
               else
               {
                  setState(MoveState);
               }
            }        // Stand back up slowly only if not moving much-
            else if (!mReversePending && mVelocity.lenSquared() > sSlowStandThreshSquared)
            {
               mActionAnimation.waitForEnd = false;
               setState(MoveState);
            }
         }
         break;
      
      default:
         break;
   }
}

const char* FishPlayer::getStateName()
{
   if (mDamageState != Enabled)
      return "Dead";
   if (isMounted())
      return "Mounted";
   if (mState == RecoverState)
      return "Recover";
   return "Move";
}

// start ds
/*
void FishPlayer::getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad)
{
   // TODO: This will be WRONG when player is prone or swimming!

   Point3F newPoint;
   mWorldToObj.mulP(in_rPos, &newPoint);

   Point3F boxSize = mObjBox.getExtents();
   F32 zHeight = boxSize.z;
   F32 zTorso  = mDataBlock->boxTorsoPercentage;
   F32 zHead   = mDataBlock->boxHeadPercentage;

   zTorso *= zHeight;
   zHead  *= zHeight;

   if (newPoint.z <= zTorso)
      out_rpVert = "legs";
   else if (newPoint.z <= zHead)
      out_rpVert = "torso";
   else
      out_rpVert = "head";

   if(dStrcmp(out_rpVert, "head") != 0)
   {
      if (newPoint.y >= 0.0f)
      {
         if (newPoint.x <= 0.0f)
            out_rpQuad = "front_left";
         else
            out_rpQuad = "front_right";
      }
      else
      {
         if (newPoint.x <= 0.0f)
            out_rpQuad = "back_left";
         else
            out_rpQuad = "back_right";
      }
   }
   else
   {
      F32 backToFront = boxSize.x;
      F32 leftToRight = boxSize.y;

      F32 backPoint  = backToFront * mDataBlock->boxHeadBackPercentage;
      F32 frontPoint = backToFront * mDataBlock->boxHeadFrontPercentage;
      F32 leftPoint  = leftToRight * mDataBlock->boxHeadLeftPercentage;
      F32 rightPoint = leftToRight * mDataBlock->boxHeadRightPercentage;

      S32 index = 0;
      if (newPoint.y < backPoint)
         index += 0;
      else if (newPoint.y >= frontPoint)
         index += 3;
      else
         index += 6;

      if (newPoint.x < leftPoint)
         index += 0;
      else if (newPoint.x >= rightPoint)
         index += 1;
      else
         index += 2;

      switch (index)
      {
         case 0: out_rpQuad = "left_back";      break;
         case 1: out_rpQuad = "middle_back";    break;
         case 2: out_rpQuad = "right_back";     break;
         case 3: out_rpQuad = "left_middle";    break;
         case 4: out_rpQuad = "middle_middle";  break;
         case 5: out_rpQuad = "right_middle";   break;
         case 6: out_rpQuad = "left_front";     break;
         case 7: out_rpQuad = "middle_front";   break;
         case 8: out_rpQuad = "right_front";    break;

         default:
            AssertFatal(0, "Bad non-tant index");
      };
   }
}
*/
void FishPlayer::getDamageLocation(const Point3F& in_rPos, const char *&out_rpVert, const char *&out_rpQuad)
{
   Point3F newPoint;
   mWorldToObj.mulP(in_rPos, &newPoint);
   F32 zHeight;
   if(mPose == SwimPose)
      zHeight = mObjBox.maxExtents.y;
   else
      zHeight = mObjBox.maxExtents.z;
   F32 zTorso  = mDataBlock->boxTorsoPercentage;
   F32 zHead   = mDataBlock->boxHeadPercentage;

   zTorso *= zHeight;
   zHead  *= zHeight;

   if(mPose == SwimPose) {
      if (newPoint.y <= zTorso)
         out_rpVert = "legs";
      else if (newPoint.y <= zHead)
         out_rpVert = "torso";
      else
         out_rpVert = "head";
   }
   else {
      if (newPoint.z <= zTorso)
         out_rpVert = "legs";
      else if (newPoint.z <= zHead)
         out_rpVert = "torso";
      else
         out_rpVert = "head";
   }

   out_rpQuad = "center"; // default to "center", so we're not returning nothing
}
// end ds

const char* FishPlayer::getPoseName() const
{
   return EngineMarshallData< FishPlayerPose >(getPose());
}

void FishPlayer::setPose( Pose pose )
{
#ifndef   NEW_COLLIDE
   // Already the set pose, return.
   if ( pose == mPose ) 
      return;

   Pose oldPose = mPose;

   mPose = pose;

   // Not added yet, just assign the pose and return.
   if ( !isProperlyAdded() )   
      return;
        
   Point3F boxSize(1,1,1);

   // Resize the player boxes
   switch (pose) 
   {
      case StandPose:
      case SprintPose:
//         boxSize = mDataBlock->boxSize;
         boxSize = getObjBox().getExtents();

         break;
      case CrouchPose:
         boxSize = mDataBlock->crouchBoxSize;         
         break;
      case PronePose:
         boxSize = mDataBlock->proneBoxSize;         
         break;
      case SwimPose:
         boxSize = mDataBlock->swimBoxSize;
         break;
   }

   // Object and World Boxes...
   mObjBox.maxExtents.x = boxSize.x * 0.5f;
   mObjBox.maxExtents.y = boxSize.y * 0.5f;
   mObjBox.maxExtents.z = boxSize.z;
   mObjBox.minExtents.x = -mObjBox.maxExtents.x;
   mObjBox.minExtents.y = -mObjBox.maxExtents.y;
   mObjBox.minExtents.z = 0.0f;

   resetWorldBox();

   // Setup the box for our convex object...
   mObjBox.getCenter(&mConvex.mCenter);
   mConvex.mSize.x = mObjBox.len_x() / 2.0f;
   mConvex.mSize.y = mObjBox.len_y() / 2.0f;
   mConvex.mSize.z = mObjBox.len_z() / 2.0f;
   // Initialize our scaled attributes as well...
   onScaleChanged();

   // Resize the PhysicsPlayer rep. should we have one
   if ( mPhysicsRep )
   {
      // start pg
      Point3F  capsuleSize = getCapsuleSize();
      mPhysicsRep->setSpacials( getPosition(), capsuleSize );
      // end pg
   }

   if ( isServerObject() )
      mDataBlock->onPoseChange_callback( this, EngineMarshallData< FishPlayerPose >(oldPose), EngineMarshallData< FishPlayerPose >(mPose));
#endif
}


// start jc
void FishPlayer::updateMove(const Move* move)
{
	// todo: see if this works
	if (!getControllingClient() && isGhost())
		return;

//   updateMoveOriginal(move);
   updateMoveSimple(move);
}

//----------------------------------------------------------------------------

void FishPlayer::updateMoveSimple(const Move* move)
{
   delta.move = *move;

   // Is waterCoverage high enough to be 'swimming'?

   if ( !mSwimming )
   {
      if ( !isGhost() )
      {
         mDataBlock->onStartSwim_callback( this );
      }
      mSwimming = true;
   }

   // Trigger images
   if (mDamageState == Enabled) 
   {
      setImageTriggerState( 0, move->trigger[sImageTrigger0] );

      if ( getMountedImage( 1 ) )
         setImageTriggerState( 1, move->trigger[sImageTrigger1] );
      else
         setImageAltTriggerState( 0, move->trigger[sImageTrigger1] );
   }

   // Update current orientation
   if (mDamageState == Enabled) {
      F32 prevZRot = mRot.z;

      //Need to save this before it&rsquo;s overwritten!
      Point3F prevHeadVec = delta.headVec;

      delta.headVec = mHead;

      F32 p = move->pitch;
      //If there was no movement this tick, add on some of last tick&rsquo;s movement
//      if(p == 0.0f)
//         p -= prevHeadVec.x * mDataBlock->vertTurnSpeedInertia;

   //   if (p > M_PI_F) 
   //      p -= M_2PI_F;
      while (p < -M_PI_F)  p += M_2PI_F;
      while (p >= M_PI_F)  p -= M_2PI_F;

      mHead.x = mClampF(mHead.x + p,mDataBlock->minLookAngle,
                        mDataBlock->maxLookAngle);

      F32 y = move->yaw;
//      if(y == 0.0f)
//         y -= delta.rotVec.z * mDataBlock->horizTurnSpeedInertia;

   //   if (y > M_PI_F)
   //      y -= M_2PI_F;
   //   while (y < 0.0f)      y += M_2PI_F;
   //   while (y >= M_2PI_F)  y -= M_2PI_F;

      GameConnection* con = getControllingClient();
      if (move->freeLook && ((isMounted() && getMountNode() == 0) || (con && !con->isFirstPerson())))
      {
         mHead.z = mClampF(mHead.z + y,
                           -mDataBlock->maxFreelookAngle,
                           mDataBlock->maxFreelookAngle);
      }
      else
      {
         mRot.z += y;
         // Rotate the head back to the front, center horizontal
         // as well if we're controlling another object.
         mHead.z *= 0.5f;
         if (mControlObject)
            mHead.x *= 0.5f;

         while (mRot.z < -M_PI_F)   mRot.z += M_2PI_F;
         while (mRot.z >= M_PI_F)   mRot.z -= M_2PI_F;
      }

      // constrain the range of mRot.z
      //mRot.z = make2PiRange(mRot.z);

      delta.rot = mRot;
      delta.rotVec.x = delta.rotVec.y = 0;
   //   delta.rotVec.z = makePiRange(prevZRot - mRot.z);
      delta.rotVec.z = prevZRot - mRot.z;
      while (delta.rotVec.z < -M_PI_F)    delta.rotVec.z += M_2PI_F;
      while (delta.rotVec.z >= M_PI_F)    delta.rotVec.z -= M_2PI_F;

      delta.head = mHead;
      delta.headVec -= mHead;
   }
   MatrixF zRot;
   zRot.set(EulerF(0, 0, mRot.z));

   // get speed wanted (speed in data block scaled by move input. Size of move input comes from speed multiplier)
   F32 moveSpeed;
   if (mState == MoveState  && mDamageState == Enabled)
   {
      //set speed can go at
      if (move->y > 0)     //up movement
      {
         moveSpeed = getMax(mDataBlock->maxUnderwaterForwardSpeed * move->y,
                            mDataBlock->maxUnderwaterSideSpeed * mFabs(move->x));
      }
      else
      {
         moveSpeed = getMax(mDataBlock->maxUnderwaterBackwardSpeed * mFabs(move->y),
                            mDataBlock->maxUnderwaterSideSpeed * mFabs(move->x));
      }
   }
   else
   {
      moveSpeed = 0;
   }
//   Point3F  contactNormal;
//   findContact( &contactNormal );   //this takes ~7% of cpu time!!!
   triggerCheck();

   //get acceleration
   VectorF acc(0, 0, 0);   //velocity change over current frame

   // get move direction
   VectorF moveDirection;
   MatrixF xRot;
   xRot.set(EulerF(mHead.x, 0, 0));
   zRot.set(EulerF(0, 0, mRot.z));
   MatrixF rot;
   rot.mul(zRot, xRot);
   rot.getColumn(0,&moveDirection);

   moveDirection *= move->x;
   VectorF tv;
   rot.getColumn(1,&tv);
   moveDirection += tv * move->y;
   rot.getColumn(2,&tv);
   moveDirection += tv * move->z;

   // get velocity want to move at
   VectorF wantedVelocity = moveDirection;
   F32   size = wantedVelocity.magnitudeSafe();
   if( size!=0 ){
      wantedVelocity *= (moveSpeed/size);
   }

   //get acceleration to reach velocity (assume can accelerate as much as want)
   acc = wantedVelocity - mVelocity;

   // If we are swimming, then we're not jumping
   if (mDataBlock->isJumpAction(mActionAnimation.action))
      mActionAnimation.action = FishPlayerData::NullAnimation;

   mContactTimer++;

   // Add in force from physical zones...
   acc += (mAppliedForce / getMass()) * TickSec;

   // Adjust velocity with all the move & gravity acceleration
   // TG: I forgot why doesn't the TickSec multiply happen here...
   mVelocity += acc;

   // Apply drag
   mVelocity -= mVelocity * mDrag * TickSec * ( mVelocity.len() / mDataBlock->maxUnderwaterForwardSpeed );

   // Clamp very small velocity to zero
   if ( mVelocity.isZero() )
      mVelocity = Point3F::Zero;

   
   // Enter/Leave Liquid
   if ( !mInWater && mWaterCoverage > 0.0f ) 
   {      
      mInWater = true;

      if ( !isGhost() )
         mDataBlock->onEnterLiquid_callback( this, mWaterCoverage, mLiquidType.c_str() );
   }
   else if ( mInWater && mWaterCoverage <= 0.0f ) 
   {      
      mInWater = false;

      if ( !isGhost() )
         mDataBlock->onLeaveLiquid_callback( this, mLiquidType.c_str() );
      else
      {
         // exit-water splash sound happens for client only
         if ( getSpeed() >= mDataBlock->exitSplashSoundVel && !isMounted() )         
            SFX->playOnce( mDataBlock->sound[FishPlayerData::ExitWater], &getTransform() );                     
      }
   }

   // Update the FishPlayerPose
   setPose( SwimPose );
}

//----------------------------------------------------------------------------

bool FishPlayer::checkDismountPosition(const MatrixF& oldMat, const MatrixF& mat)
{
   AssertFatal(getContainer() != NULL, "Error, must have a container!");
   AssertFatal(getObjectMount() != NULL, "Error, must be mounted!");

   Point3F pos;
   Point3F oldPos;
   mat.getColumn(3, &pos);
   oldMat.getColumn(3, &oldPos);
   RayInfo info;
   disableCollision();
   getObjectMount()->disableCollision();
   if (getContainer()->castRay(oldPos, pos, sCollisionMoveMask, &info))
   {
      enableCollision();
      getObjectMount()->enableCollision();
      return false;
   }

   Box3F wBox = mObjBox;
   wBox.minExtents += pos;
   wBox.maxExtents += pos;

   EarlyOutPolyList polyList;
   polyList.mNormal.set(0.0f, 0.0f, 0.0f);
   polyList.mPlaneList.clear();
   polyList.mPlaneList.setSize(6);
   polyList.mPlaneList[0].set(wBox.minExtents,VectorF(-1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[1].set(wBox.maxExtents,VectorF(0.0f, 1.0f, 0.0f));
   polyList.mPlaneList[2].set(wBox.maxExtents,VectorF(1.0f, 0.0f, 0.0f));
   polyList.mPlaneList[3].set(wBox.minExtents,VectorF(0.0f, -1.0f, 0.0f));
   polyList.mPlaneList[4].set(wBox.minExtents,VectorF(0.0f, 0.0f, -1.0f));
   polyList.mPlaneList[5].set(wBox.maxExtents,VectorF(0.0f, 0.0f, 1.0f));

   if (getContainer()->buildPolyList(PLC_Collision, wBox, sCollisionMoveMask, &polyList))
   {
      enableCollision();
      getObjectMount()->enableCollision();
      return false;
   }

   enableCollision();
   getObjectMount()->enableCollision();
   return true;
}


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void FishPlayer::updateLookAnimation(F32 dT)
{
   // Calculate our interpolated head position.
   Point3F renderHead = delta.head + delta.headVec * dT;

   // Adjust look pos.  This assumes that the animations match
   // the min and max look angles provided in the datablock.
   if (mArmAnimation.thread) 
   {
      // TG: Adjust arm position to avoid collision.
      F32 tp = mControlObject? 0.5:
         (renderHead.x - mArmRange.min) / mArmRange.delta;
      mShapeInstance->setPos(mArmAnimation.thread,mClampF(tp,0,1));
   }
   
   if (mHeadVThread) 
   {
      F32 tp = (renderHead.x - mHeadVRange.min) / mHeadVRange.delta;
      mShapeInstance->setPos(mHeadVThread,mClampF(tp,0,1));
   }
   
   if (mHeadHThread) 
   {
      F32 dt = 2 * mDataBlock->maxFreelookAngle;
   // start jc - just use the turn angle for now jezz im lazy
   //   F32 tp = (renderHead.z + mDataBlock->maxFreelookAngle) / dt;
   //   Point3F renderHead2 = delta.rot + delta.rotVec * dT;
   //   Point3F renderHead2 = delta.rotVec;
      

      F32 tp;
//      if (delta.warpTicks > 0)
//         tp = (((delta.rotOffset.z)*10) + mDataBlock->maxFreelookAngle) / dt;
//      else
//         tp = (((delta.rotVec.z)*10) + mDataBlock->maxFreelookAngle) / dt;
      /*
      if(mRotSmooth.z < -M_HALFPI_F && mRot.z > M_HALFPI_F)
         tp = (((mRotSmooth.z+M_2PI_F) - mRot.z) + mDataBlock->maxFreelookAngle) / dt;
      else if(mRotSmooth.z > M_HALFPI_F && mRot.z < -M_HALFPI_F)   
         tp = ((mRotSmooth.z - (mRot.z+M_2PI_F)) + mDataBlock->maxFreelookAngle) / dt;
      else
         tp = ((mRotSmooth.z - mRot.z) + mDataBlock->maxFreelookAngle) / dt;
*/
      // get the shortest distance around the circle
  //    while (mRot.z < 0.0f)      mRot.z += M_2PI_F;
  //    while (mRot.z > M_2PI_F)   mRot.z -= M_2PI_F;

      F32 rot; // = mRotSmooth.z - mRot.z;
      if(mAbs(mRot.z - mRotSmooth.z) < M_PI_F)
         rot = mRotSmooth.z - mRot.z;
      else
      {
         if(mRotSmooth.z > mRot.z)
            rot = mRotSmooth.z - (mRot.z+M_2PI_F);
         else
            rot = mRotSmooth.z - (mRot.z-M_2PI_F);
      }

    //  if(mRotSmooth.z > mRot.z )
    //     rot = -(M_PI_F-rot);
    //  if(rot < -M_PI_F )
    //     rot = -(M_PI_F+rot);


      tp = ((-rot) + mDataBlock->maxFreelookAngle) / dt;
   // end jc
      mShapeInstance->setPos(mHeadHThread,mClampF(tp,0,1));
   }
}


//----------------------------------------------------------------------------
// Methods to get delta (as amount to affect velocity by)

bool FishPlayer::inDeathAnim()
{
   if (mActionAnimation.thread && mActionAnimation.action >= 0)
      if (mActionAnimation.action < mDataBlock->actionCount)
         return mDataBlock->actionList[mActionAnimation.action].death;

   return false;
}

// Get change from mLastDeathPos - return current pos.  Assumes we're in death anim.
F32 FishPlayer::deathDelta(Point3F & delta)
{
   // Get ground delta from the last time we offset this.
   MatrixF  mat;
   F32 pos = mShapeInstance->getPos(mActionAnimation.thread);
   mShapeInstance->deltaGround1(mActionAnimation.thread, mDeath.lastPos, pos, mat);
   mat.getColumn(3, & delta);
   return pos;
}

// Called before updatePos() to prepare it's needed change to velocity, which
// must roll over.  Should be updated on tick, this is where we remember last
// position of animation that was used to roll into velocity.
void FishPlayer::updateDeathOffsets()
{
   if (inDeathAnim())
      // Get ground delta from the last time we offset this.
      mDeath.lastPos = deathDelta(mDeath.posAdd);
   else
      mDeath.clear();
}


//----------------------------------------------------------------------------

static const U32 sPlayerConformMask =  InteriorObjectType|StaticShapeObjectType|
                                       StaticObjectType|TerrainObjectType;

static void accel(F32& from, F32 to, F32 rate)
{
   if (from < to)
      from = getMin(from += rate, to);
   else
      from = getMax(from -= rate, to);
}

// if (dt == -1)
//    normal tick, so we advance.
// else
//    interpolate with dt as % of tick, don't advance
//
MatrixF * FishPlayer::Death::fallToGround(F32 dt, const Point3F& loc, F32 curZ, F32 boxRad)
{
   static const F32 sConformCheckDown = 4.0f;
   RayInfo     coll;
   bool        conformToStairs = false;
   Point3F     pos(loc.x, loc.y, loc.z + 0.1f);
   Point3F     below(pos.x, pos.y, loc.z - sConformCheckDown);
   MatrixF  *  retVal = NULL;

   PROFILE_SCOPE(ConformToGround);

   if (gClientContainer.castRay(pos, below, sPlayerConformMask, &coll))
   {
      F32      adjust, height = (loc.z - coll.point.z), sink = curSink;
      VectorF  desNormal = coll.normal;
      VectorF  normal = curNormal;

      // dt >= 0 means we're interpolating and don't accel the numbers
      if (dt >= 0.0f)
         adjust = dt * TickSec;
      else
         adjust = TickSec;

      // Try to get them to conform to stairs by doing several LOS calls.  We do this if
      // normal is within about 5 deg. of vertical.
      if (desNormal.z > 0.995f)
      {
         Point3F  corners[3], downpts[3];
         S32      c;

         for (c = 0; c < 3; c++) {    // Build 3 corners to cast down from-
            corners[c].set(loc.x - boxRad, loc.y - boxRad, loc.z + 1.0f);
            if (c)      // add (0,boxWidth) and (boxWidth,0)
               corners[c][c - 1] += (boxRad * 2.0f);
            downpts[c].set(corners[c].x, corners[c].y, loc.z - sConformCheckDown);
         }

         // Do the three casts-
         for (c = 0; c < 3; c++)
            if (gClientContainer.castRay(corners[c], downpts[c], sPlayerConformMask, &coll))
               downpts[c] = coll.point;
            else
               break;

         // Do the math if everything hit below-
         if (c == 3) {
            mCross(downpts[1] - downpts[0], downpts[2] - downpts[1], &desNormal);
            AssertFatal(desNormal.z > 0, "Abnormality in FishPlayer::Death::fallToGround()");
            downpts[2] = downpts[2] - downpts[1];
            downpts[1] = downpts[1] - downpts[0];
            desNormal.normalize();
            conformToStairs = true;
         }
      }

      // Move normal in direction we want-
      F32   * cur = normal, * des = desNormal;
      for (S32 i = 0; i < 3; i++)
         accel(*cur++, *des++, adjust * 0.25f);

      if (mFabs(height) < 2.2f && !normal.isZero() && desNormal.z > 0.01f)
      {
         VectorF  upY(0.0f, 1.0f, 0.0f), ahead;
         VectorF  sideVec;
         MatrixF  mat(true);

         normal.normalize();
         mat.set(EulerF (0.0f, 0.0f, curZ));
         mat.mulV(upY, & ahead);
	      mCross(ahead, normal, &sideVec);
         sideVec.normalize();
         mCross(normal, sideVec, &ahead);

         static MatrixF resMat(true);
         resMat.setColumn(0, sideVec);
         resMat.setColumn(1, ahead);
         resMat.setColumn(2, normal);

         // Adjust Z down to account for box offset on slope.  Figure out how
         // much we want to sink, and gradually accel to this amount.  Don't do if
         // we're conforming to stairs though
         F32   xy = mSqrt(desNormal.x * desNormal.x + desNormal.y * desNormal.y);
         F32   desiredSink = (boxRad * xy / desNormal.z);
         if (conformToStairs)
            desiredSink *= 0.5f;

         accel(sink, desiredSink, adjust * 0.15f);

         Point3F  position(pos);
         position.z -= sink;
         resMat.setColumn(3, position);

         if (dt < 0.0f)
         {  // we're moving, so update normal and sink amount
            curNormal = normal;
            curSink = sink;
         }

         retVal = &resMat;
      }
   }
   return retVal;
}


//-------------------------------------------------------------------------------------

// This is called ::onAdd() to see if we're in a sitting animation.  These then
// can use a longer tick delay for the mount to get across.
bool FishPlayer::inSittingAnim()
{
   U32   action = mActionAnimation.action;
   if (mActionAnimation.thread && action < mDataBlock->actionCount) {
      const char * name = mDataBlock->actionList[action].name;
      if (!dStricmp(name, "Sitting") || !dStricmp(name, "Scoutroot"))
         return true;
   }
   return false;
}


//----------------------------------------------------------------------------

const String& FishPlayer::getArmThread() const
{
   if (mArmAnimation.thread && mArmAnimation.thread->hasSequence())
   {
      return mArmAnimation.thread->getSequenceName();
   }

   return String::EmptyString;
}

bool FishPlayer::setArmThread(const char* sequence)
{
   // The arm sequence must be in the action list.
   for (U32 i = 1; i < mDataBlock->actionCount; i++)
      if (!dStricmp(mDataBlock->actionList[i].name,sequence))
         return setArmThread(i);
   return false;
}

bool FishPlayer::setArmThread(U32 action)
{
   FishPlayerData::ActionAnimation &anim = mDataBlock->actionList[action];
   if (anim.sequence != -1 &&
      anim.sequence != mShapeInstance->getSequence(mArmAnimation.thread))
   {
      mShapeInstance->setSequence(mArmAnimation.thread,anim.sequence,0);
      mArmAnimation.action = action;
      setMaskBits(ActionMask);
      return true;
   }
   return false;
}


//----------------------------------------------------------------------------

bool FishPlayer::setActionThread(const char* sequence,bool hold,bool wait,bool fsp)
{
   for (U32 i = 1; i < mDataBlock->actionCount; i++)
   {
      FishPlayerData::ActionAnimation &anim = mDataBlock->actionList[i];
      if (!dStricmp(anim.name,sequence))
      {
// start ds
//         setActionThread(i,true,hold,wait,fsp);
         setActionThread(i,true,hold,wait,fsp,true);
// end ds
         setMaskBits(ActionMask);
         return true;
      }
   }
   return false;
}

void FishPlayer::setActionThread(U32 action,bool forward,bool hold,bool wait,bool fsp, bool forceSet)
{
   if (!mDataBlock || (mActionAnimation.action == action && mActionAnimation.forward == forward && !forceSet))
      return;

//   Con::printf("anim->%d",action);

   if (action >= FishPlayerData::NumActionAnims)
   {
      Con::errorf("FishPlayer::setActionThread(%d): FishPlayer action out of range", action);
      return;
   }

   FishPlayerData::ActionAnimation &anim = mDataBlock->actionList[action];
// start ds 
//   if (anim.sequence == -1)
//   {
//      anim = mDataBlock->actionList[FishPlayerData::RootAnim];
//   }
// end ds
   if (anim.sequence != -1)
   {
      U32 lastAction = mActionAnimation.action;

      mActionAnimation.action          = action;
      mActionAnimation.forward         = forward;
      mActionAnimation.firstPerson     = fsp;
      mActionAnimation.holdAtEnd       = hold;
      mActionAnimation.waitForEnd      = hold? true: wait;
      mActionAnimation.animateOnServer = fsp;
      mActionAnimation.atEnd           = false;
      mActionAnimation.delayTicks      = (S32)sNewAnimationTickTime;
      mActionAnimation.atEnd           = false;

      if (sUseAnimationTransitions && (action != FishPlayerData::LandAnim ) && (isGhost()/* || mActionAnimation.animateOnServer*/))
      {
         // The transition code needs the timeScale to be set in the
         // right direction to know which way to go.
         F32   transTime = sAnimationTransitionTime;
         if (mDataBlock && mDataBlock->isJumpAction(action))
            transTime = 0.15f;

         F32 timeScale = mActionAnimation.forward ? 1.0f : -1.0f;
         if (mDataBlock && mDataBlock->isJumpAction(action))
            timeScale *= 1.5f;

         mShapeInstance->setTimeScale(mActionAnimation.thread,timeScale);

         S32 seq = anim.sequence;
         S32 imageBasedSeq = convertActionToImagePrefix(mActionAnimation.action);
         if (imageBasedSeq != -1)
            seq = imageBasedSeq;

         // If we're transitioning into the same sequence (an action may use the
         // same sequence as a previous action) then we want to start at the same
         // position.
         F32 pos = mActionAnimation.forward ? 0.0f : 1.0f;
         FishPlayerData::ActionAnimation &lastAnim = mDataBlock->actionList[lastAction];
         if (lastAnim.sequence == anim.sequence)
         {
            pos = mShapeInstance->getPos(mActionAnimation.thread);
         }

         mShapeInstance->transitionToSequence(mActionAnimation.thread,seq,
            pos, transTime, true);
      }
      else
      {
         S32 seq = anim.sequence;
         S32 imageBasedSeq = convertActionToImagePrefix(mActionAnimation.action);
         if (imageBasedSeq != -1)
            seq = imageBasedSeq;

         mShapeInstance->setSequence(mActionAnimation.thread,seq,
            mActionAnimation.forward ? 0.0f : 1.0f);
      }
   }
}

void FishPlayer::updateActionThread()
{
   PROFILE_START(UpdateActionThread);

   // Select an action animation sequence, this assumes that
   // this function is called once per tick.
   if(mActionAnimation.action != FishPlayerData::NullAnimation)
      if (mActionAnimation.forward)
         mActionAnimation.atEnd = mShapeInstance->getPos(mActionAnimation.thread) == 1;
      else
         mActionAnimation.atEnd = mShapeInstance->getPos(mActionAnimation.thread) == 0;

   // Only need to deal with triggers on the client
   if( isGhost() )
   {
      bool triggeredLeft = false;
      bool triggeredRight = false;
      
      F32 offset = 0.0f;
      if( mShapeInstance->getTriggerState( 1 ) )
      {
         triggeredLeft = true;
         offset = -mDataBlock->decalOffset * getScale().x;
      }
      else if(mShapeInstance->getTriggerState( 2 ) )
      {
         triggeredRight = true;
         offset = mDataBlock->decalOffset * getScale().x;
      }

      if( triggeredLeft || triggeredRight )
      {
         Point3F rot, pos;
         RayInfo rInfo;
         MatrixF mat = getRenderTransform();
         mat.getColumn( 1, &rot );
         mat.mulP( Point3F( offset, 0.0f, 0.0f), &pos );

         if( gClientContainer.castRay( Point3F( pos.x, pos.y, pos.z + 0.01f ),
               Point3F( pos.x, pos.y, pos.z - 2.0f ),
               STATIC_COLLISION_TYPEMASK | VehicleObjectType, &rInfo ) )
         {
            Material* material = ( rInfo.material ? dynamic_cast< Material* >( rInfo.material->getMaterial() ) : 0 );

            // Put footprints on surface, if appropriate for material.

            if( material && material->mShowFootprints
                && mDataBlock->decalData )
            {
               Point3F normal;
               Point3F tangent;
               mObjToWorld.getColumn( 0, &tangent );
               mObjToWorld.getColumn( 2, &normal );
               gDecalManager->addDecal( rInfo.point, normal, tangent, mDataBlock->decalData, getScale().y );
            }
            
            // Emit footpuffs.

            if( rInfo.t <= 0.5 && mWaterCoverage == 0.0
                && material && material->mShowDust )
            {
               // New emitter every time for visibility reasons
               ParticleEmitter * emitter = new ParticleEmitter;
               emitter->onNewDataBlock( mDataBlock->footPuffEmitter, false );

               ColorF colorList[ ParticleData::PDC_NUM_KEYS];

               for( U32 x = 0; x < getMin( Material::NUM_EFFECT_COLOR_STAGES, ParticleData::PDC_NUM_KEYS ); ++ x )
                  colorList[ x ].set( material->mEffectColor[ x ].red,
                                      material->mEffectColor[ x ].green,
                                      material->mEffectColor[ x ].blue,
                                      material->mEffectColor[ x ].alpha );
               for( U32 x = Material::NUM_EFFECT_COLOR_STAGES; x < ParticleData::PDC_NUM_KEYS; ++ x )
                  colorList[ x ].set( 1.0, 1.0, 1.0, 0.0 );

               emitter->setColors( colorList );
               if( !emitter->registerObject() )
               {
                  Con::warnf( ConsoleLogEntry::General, "Could not register emitter for particle of class: %s", mDataBlock->getName() );
                  delete emitter;
                  emitter = NULL;
               }
               else
               {
                  emitter->emitParticles( pos, Point3F( 0.0, 0.0, 1.0 ), mDataBlock->footPuffRadius,
                     Point3F( 0, 0, 0 ), mDataBlock->footPuffNumParts );
                  emitter->deleteWhenEmpty();
               }
            }

            // Play footstep sound.
            
            playFootstepSound( triggeredLeft, material, rInfo.object );
         }
      }
   }

   // Mount pending variable puts a hold on the delayTicks below so players don't
   // inadvertently stand up because their mount has not come over yet.
   if (mMountPending)
      mMountPending = (isMounted() ? 0 : (mMountPending - 1));

   if (mActionAnimation.action == FishPlayerData::NullAnimation ||
       ((!mActionAnimation.waitForEnd || mActionAnimation.atEnd)) &&
// start ds - call script callback when finished even if holding
/*
       !mActionAnimation.holdAtEnd && (mActionAnimation.delayTicks -= !mMountPending) <= 0)
   {
      //The scripting language will get a call back when a script animation has finished...
      //  example: When the chat menu animations are done playing...
      if ( isServerObject() && mActionAnimation.action >= FishPlayerData::NumTableActionAnims )
         mDataBlock->animationDone_callback( this );
      pickActionAnimation();
*/
       (mActionAnimation.delayTicks -= !mMountPending) <= 0)
   {
      // pick an animation first so that script may override it
      if (!mActionAnimation.holdAtEnd)
         pickActionAnimation();
      //The scripting language will get a call back when a script animation has finished...
      //  example: When the chat menu animations are done playing...
      if ( isServerObject() && mActionAnimation.action >= FishPlayerData::NumTableActionAnims )
         mDataBlock->animationDone_callback( this );
      
// end ds
   }

   if ( (mActionAnimation.action != FishPlayerData::LandAnim) &&
        (mActionAnimation.action != FishPlayerData::NullAnimation) )
   {
      // Update action animation time scale to match ground velocity
      F32 scale = 1;
      //-----start pg---------------
/*
      FishPlayerData::ActionAnimation &anim =
         mDataBlock->actionList[mActionAnimation.action];
      if (anim.velocityScale && anim.speed) {
         VectorF vel;
         mWorldToObj.mulV(mVelocity,&vel);
         scale = mFabs(mDot(vel, anim.dir) / anim.speed);

         if (scale > mDataBlock->maxTimeScale)
            scale = mDataBlock->maxTimeScale;
      }
      */
      if( !scaleAnimationSpeed() ){
         scale = 1.0f;
      }else{
//      if( !mDataBlock->noAnimationScale ){
         F32   speed = mVelocity.magnitudeSafe();
         if(speed){
            scale = speed/mDataBlock->maxUnderwaterForwardSpeed;
            scale *= 10.0f;
         }
         scale = mClampF(scale, 1.0f, 5.0f);
      }
      //-----end pg---------------
      mShapeInstance->setTimeScale(mActionAnimation.thread,
                                   mActionAnimation.forward? scale: -scale);
   }
   PROFILE_END();
}

void FishPlayer::pickBestMoveAction(U32 startAnim, U32 endAnim, U32 * action, bool * forward) const
{
   *action = startAnim;
   *forward = false;

   VectorF vel;
   mWorldToObj.mulV(mVelocity,&vel);

   // Note:: used to be test for small speed here. If speed was too small just assumed defaults above used
   
   // Bias the velocity towards picking the forward/backward anims over
   // the sideways ones to prevent oscillation between anims.
   vel *= VectorF(0.5f, 1.0f, 0.5f);

   // Pick animation that is the best fit for our current (local) velocity.
   // Assumes that the root (stationary) animation is at startAnim.
   F32 curMax = -0.01f;
   for (U32 i = startAnim+1; i <= endAnim; i++)
   {
      const FishPlayerData::ActionAnimation &anim = mDataBlock->actionList[i];
      if (anim.sequence != -1 && anim.speed) 
      {
         F32 d = mDot(vel, anim.dir);
         if (d > curMax)
         {
            curMax = d;
            *action = i;
            *forward = true;
         }
         else
         {
            // Check if reversing this animation would fit (bias against this
            // so that when moving right, the real right anim is still chosen,
            // but if not present, the reversed left anim will be used instead)
            d *= -0.75f;
            if (d > curMax)
            {
               curMax = d;
               *action = i;
               *forward = false;
            }
         }
      }
   }
}

void FishPlayer::pickActionAnimation()
{
   // Only select animations in our normal move state.
   if (mState != MoveState || mDamageState != Enabled)
      return;

   if (isMounted() || mMountPending)
   {
      // Go into root position unless something was set explicitly
      // from a script.
      if (mActionAnimation.action != FishPlayerData::RootAnim &&
          mActionAnimation.action < FishPlayerData::NumTableActionAnims)
         setActionThread(FishPlayerData::RootAnim,true,false,false);
      return;
   }

   bool forward = true;
   U32 action = FishPlayerData::RootAnim;
   bool fsp = false;
   
//   if( mEnableUserTouch && mDataBlock->actionList[FishPlayerData::InteractAnim].sequence!=-1 )
//   {
//      action = FishPlayerData::InteractAnim;
//   }
   if ( mSwimming )
   {
      pickBestMoveAction(FishPlayerData::SwimRootAnim, FishPlayerData::SwimRightAnim, &action, &forward);
   }
   else if ( mPose == StandPose )
   {
      if (mContactTimer >= sContactTickTime)
      {
         // Nothing under our feet
         action = FishPlayerData::RootAnim;
      }
      else
      {
         // Our feet are on something
         pickBestMoveAction(FishPlayerData::RootAnim, FishPlayerData::SideRightAnim, &action, &forward);
      }
   }
   else if ( mPose == CrouchPose )
   {
      pickBestMoveAction(FishPlayerData::CrouchRootAnim, FishPlayerData::CrouchRightAnim, &action, &forward);
   }
   else if ( mPose == PronePose )
   {
      pickBestMoveAction(FishPlayerData::ProneRootAnim, FishPlayerData::ProneBackwardAnim, &action, &forward);
   }
   else if ( mPose == SprintPose )
   {
      pickBestMoveAction(FishPlayerData::SprintRootAnim, FishPlayerData::SprintRightAnim, &action, &forward);
   }
   setActionThread(action,forward,false,false,fsp);
}

void FishPlayer::onImage(U32 imageSlot, bool unmount)
{
   // Update 3rd person sequences based on images used.  Start be getting a
   // list of all possible image prefix sequences.
   String prefixPaths[ShapeBase::MaxMountedImages];
   buildImagePrefixPaths(prefixPaths);

   // Clear out any previous image state animation
   if (mImageStateThread)
   {
      mShapeInstance->destroyThread(mImageStateThread);
      mImageStateThread = 0;
   }

   // Attempt to update the action thread
   U32 action = mActionAnimation.action;
   if (action != FishPlayerData::NullAnimation)
   {
      String actionSeq = mDataBlock->actionList[action].name;
      if (actionSeq.isNotEmpty())
      {
         S32 seqIndex = mDataBlock->actionList[action].sequence;
         S32 prefixIndex = findPrefixSequence(prefixPaths, actionSeq);
         if (prefixIndex != -1)
         {
            seqIndex = prefixIndex;
         }

         // Only change the sequence if it isn't already playing.
         if (seqIndex != mShapeInstance->getSequence(mActionAnimation.thread))
         {
            F32 pos = mShapeInstance->getPos(mActionAnimation.thread);
            mShapeInstance->setSequence(mActionAnimation.thread, seqIndex, pos);
         }
      }
   }

   // Attempt to update the arm thread
   U32 armAction = getArmAction();
   if (armAction != FishPlayerData::NullAnimation)
   {
      String armSeq = mDataBlock->actionList[armAction].name;
      if (armSeq.isNotEmpty())
      {
         S32 seqIndex = mDataBlock->actionList[armAction].sequence;
         S32 prefixIndex = findPrefixSequence(prefixPaths, armSeq);
         if (prefixIndex != -1)
         {
            seqIndex = prefixIndex;
         }

         // Only change the sequence if it isn't already playing.
         if (seqIndex != mShapeInstance->getSequence(mArmAnimation.thread))
         {
            F32 pos = mShapeInstance->getPos(mArmAnimation.thread);
            mShapeInstance->setSequence(mArmAnimation.thread, seqIndex, pos);
         }
      }
   }

   // Attempt to update the head threads
   if (mHeadVThread)
   {
      TSShape const* shape = mShapeInstance->getShape();
      S32 seqIndex = shape->findSequence("head");
      S32 prefixIndex = findPrefixSequence(prefixPaths, "head");
      if (prefixIndex != -1)
      {
         seqIndex = prefixIndex;
      }

      // Only change the sequence if it isn't already playing.
      if (seqIndex != mShapeInstance->getSequence(mHeadVThread))
      {
         F32 pos = mShapeInstance->getPos(mHeadVThread);
         mShapeInstance->setSequence(mHeadVThread, seqIndex, pos);
      }
   }

   if (mHeadHThread)
   {
      TSShape const* shape = mShapeInstance->getShape();
      S32 seqIndex = shape->findSequence("headside");
      S32 prefixIndex = findPrefixSequence(prefixPaths, "headside");
      if (prefixIndex != -1)
      {
         seqIndex = prefixIndex;
      }

      // Only change the sequence if it isn't already playing.
      if (seqIndex != mShapeInstance->getSequence(mHeadHThread))
      {
         F32 pos = mShapeInstance->getPos(mHeadHThread);
         mShapeInstance->setSequence(mHeadHThread, seqIndex, pos);
      }
   }
}

void FishPlayer::buildImagePrefixPaths(String* prefixPaths)
{
   // We begin obtaining the anim prefix for each image.
   String prefix[ShapeBase::MaxMountedImages];
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      MountedImage& image = mMountedImageList[i];
      if (image.dataBlock && image.dataBlock->imageAnimPrefix && image.dataBlock->imageAnimPrefix[0])
      {
         prefix[i] = String(image.dataBlock->imageAnimPrefix);
      }
   }

   // Build out the full prefix names we will be searching for.
   S32 counter = ShapeBase::MaxMountedImages-1;
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      // Only build out the prefix path for images that have a defined prefix.
      if (prefix[i].isNotEmpty())
      {
         bool start = true;
         for (U32 j=0; j<=i; ++j)
         {
            if (prefix[j].isNotEmpty())
            {
               if (!start)
               {
                  prefixPaths[counter] += "_";
               }
               else
               {
                  start = false;
               }
               prefixPaths[counter] += prefix[j];
            }
         }
      }

      -- counter;
   }
}
S32 FishPlayer::findPrefixSequence(String* prefixPaths, const String& baseSeq)
{
   // Go through the prefix list.  If we find a match then return the sequence
   // index.
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      if (prefixPaths[i].isNotEmpty())
      {
         String seq = prefixPaths[i] + "_" + baseSeq;
         S32 seqIndex = mShapeInstance->getShape()->findSequence(seq);
         if (seqIndex != -1)
         {
            return seqIndex;
         }
      }
   }

   return -1;
}

S32 FishPlayer::convertActionToImagePrefix(U32 action)
{
   String prefixPaths[ShapeBase::MaxMountedImages];
   buildImagePrefixPaths(prefixPaths);

   if (action != FishPlayerData::NullAnimation)
   {
      String actionSeq;
      S32 seq = -1;

      // We'll first attempt to find the action sequence by name
      // as defined within the action list.
      actionSeq = mDataBlock->actionList[action].name;
      if (actionSeq.isNotEmpty())
      {
         seq = findPrefixSequence(prefixPaths, actionSeq);
      }

      if (seq == -1)
      {
         // Couldn't find a valid sequence.  If this is a sprint action
         // then we also need to search through the standard movement
         // sequences.
         if (action >= FishPlayerData::SprintRootAnim && action <= FishPlayerData::SprintRightAnim)
         {
            U32 standardAction = action - FishPlayerData::SprintRootAnim;
            actionSeq = mDataBlock->actionList[standardAction].name;
            if (actionSeq.isNotEmpty())
            {
               seq = findPrefixSequence(prefixPaths, actionSeq);
            }
         }
      }

      return seq;
   }

   return -1;
}

void FishPlayer::onImageRecoil( U32, ShapeBaseImageData::StateData::RecoilState state )
{
   if ( mRecoilThread )
   {
      if ( state != ShapeBaseImageData::StateData::NoRecoil )
      {
         S32 stateIndex = state - ShapeBaseImageData::StateData::LightRecoil;
         if ( mDataBlock->recoilSequence[stateIndex] != -1 )
         {
            mShapeInstance->setSequence( mRecoilThread, mDataBlock->recoilSequence[stateIndex], 0 );
            mShapeInstance->setTimeScale( mRecoilThread, 1 );
         }
      }
   }
}

void FishPlayer::onImageStateAnimation(U32 imageSlot, const char* seqName, bool direction, bool scaleToState, F32 stateTimeOutValue)
{
   if (mDataBlock->allowImageStateAnimation && isGhost())
   {
      MountedImage& image = mMountedImageList[imageSlot];

      // Just as with onImageAnimThreadChange we're going to apply various prefixes to determine the final sequence to use.
      // Here is the order:
      // imageBasePrefix_scriptPrefix_baseAnimName
      // imageBasePrefix_baseAnimName
      // scriptPrefix_baseAnimName
      // baseAnimName

      // Collect the prefixes
      const char* imageBasePrefix = "";
      bool hasImageBasePrefix = image.dataBlock && image.dataBlock->imageAnimPrefix && image.dataBlock->imageAnimPrefix[0];
      if (hasImageBasePrefix)
         imageBasePrefix = image.dataBlock->imageAnimPrefix;
      const char* scriptPrefix = getImageScriptAnimPrefix(imageSlot).getString();
      bool hasScriptPrefix = scriptPrefix && scriptPrefix[0];

      S32 seqIndex = mShapeInstance->getShape()->findSequence(seqName);

      // Find the final sequence based on the prefix combinations
      if (hasImageBasePrefix || hasScriptPrefix)
      {
         bool found = false;
         String baseSeqName(seqName);

         if (!found && hasImageBasePrefix && hasScriptPrefix)
         {
            String seqName = String(imageBasePrefix) + String("_") + String(scriptPrefix) + String("_") + baseSeqName;
            S32 index = mShapeInstance->getShape()->findSequence(seqName);
            if (index != -1)
            {
               seqIndex = index;
               found = true;
            }
         }

         if (!found && hasImageBasePrefix)
         {
            String seqName = String(imageBasePrefix) + String("_") + baseSeqName;
            S32 index = mShapeInstance->getShape()->findSequence(seqName);
            if (index != -1)
            {
               seqIndex = index;
               found = true;
            }
         }

         if (!found && hasScriptPrefix)
         {
            String seqName = String(scriptPrefix) + String("_") + baseSeqName;
            S32 index = mShapeInstance->getShape()->findSequence(seqName);
            if (index != -1)
            {
               seqIndex = index;
               found = true;
            }
         }
      }

      if (seqIndex != -1)
      {
         if (!mImageStateThread)
         {
            mImageStateThread = mShapeInstance->addThread();
         }

         mShapeInstance->setSequence( mImageStateThread, seqIndex, 0 );

         F32 timeScale = (scaleToState && stateTimeOutValue) ?
            mShapeInstance->getDuration(mImageStateThread) / stateTimeOutValue : 1.0f;

         mShapeInstance->setTimeScale( mImageStateThread, direction ? timeScale : -timeScale );
      }
   }
}

const char* FishPlayer::getImageAnimPrefix(U32 imageSlot, S32 imageShapeIndex)
{
   if (!mDataBlock)
      return "";

   switch (imageShapeIndex)
   {
      case ShapeBaseImageData::StandardImageShape:
      {
         return mDataBlock->imageAnimPrefix;
      }

      case ShapeBaseImageData::FirstPersonImageShape:
      {
         return mDataBlock->imageAnimPrefixFP;
      }

      default:
      {
         return "";
      }
   }
}

void FishPlayer::onImageAnimThreadChange(U32 imageSlot, S32 imageShapeIndex, ShapeBaseImageData::StateData* lastState, const char* anim, F32 pos, F32 timeScale, bool reset)
{
   if (!mShapeFPInstance[imageSlot] || !mShapeFPAnimThread[imageSlot])
      return;

   MountedImage& image = mMountedImageList[imageSlot];
   ShapeBaseImageData::StateData& stateData = *image.state;

   if (reset)
   {
      // Reset cyclic sequences back to the first frame to turn it off
      // (the first key frame should be it's off state).
      if (mShapeFPAnimThread[imageSlot]->getSequence()->isCyclic() && (stateData.sequenceNeverTransition || !(stateData.sequenceTransitionIn || (lastState && lastState->sequenceTransitionOut))) )
      {
         mShapeFPInstance[imageSlot]->setPos(mShapeFPAnimThread[imageSlot],0);
         mShapeFPInstance[imageSlot]->setTimeScale(mShapeFPAnimThread[imageSlot],0);
      }

      return;
   }

   // Just as with ShapeBase::udpateAnimThread we're going to apply various prefixes to determine the final sequence to use.
   // Here is the order:
   // imageBasePrefix_scriptPrefix_baseAnimName
   // imageBasePrefix_baseAnimName
   // scriptPrefix_baseAnimName
   // baseAnimName

   // Collect the prefixes
   const char* imageBasePrefix = "";
   bool hasImageBasePrefix = image.dataBlock && image.dataBlock->imageAnimPrefixFP && image.dataBlock->imageAnimPrefixFP[0];
   if (hasImageBasePrefix)
      imageBasePrefix = image.dataBlock->imageAnimPrefixFP;
   const char* scriptPrefix = getImageScriptAnimPrefix(imageSlot).getString();
   bool hasScriptPrefix = scriptPrefix && scriptPrefix[0];

   S32 seqIndex = mShapeFPInstance[imageSlot]->getShape()->findSequence(anim);

   // Find the final sequence based on the prefix combinations
   if (hasImageBasePrefix || hasScriptPrefix)
   {
      bool found = false;
      String baseSeqName(anim);

      if (!found && hasImageBasePrefix && hasScriptPrefix)
      {
         String seqName = String(imageBasePrefix) + String("_") + String(scriptPrefix) + String("_") + baseSeqName;
         S32 index = mShapeFPInstance[imageSlot]->getShape()->findSequence(seqName);
         if (index != -1)
         {
            seqIndex = index;
            found = true;
         }
      }

      if (!found && hasImageBasePrefix)
      {
         String seqName = String(imageBasePrefix) + String("_") + baseSeqName;
         S32 index = mShapeFPInstance[imageSlot]->getShape()->findSequence(seqName);
         if (index != -1)
         {
            seqIndex = index;
            found = true;
         }
      }

      if (!found && hasScriptPrefix)
      {
         String seqName = String(scriptPrefix) + String("_") + baseSeqName;
         S32 index = mShapeFPInstance[imageSlot]->getShape()->findSequence(seqName);
         if (index != -1)
         {
            seqIndex = index;
            found = true;
         }
      }
   }

   if (seqIndex != -1)
   {
      if (!lastState)
      {
         // No lastState indicates that we are just switching animation sequences, not states.  Transition into this new sequence, but only
         // if it is different than what we're currently playing.
         S32 prevSeq = -1;
         if (mShapeFPAnimThread[imageSlot]->hasSequence())
         {
            prevSeq = mShapeFPInstance[imageSlot]->getSequence(mShapeFPAnimThread[imageSlot]);
         }
         if (seqIndex != prevSeq)
         {
            mShapeFPInstance[imageSlot]->transitionToSequence(mShapeFPAnimThread[imageSlot], seqIndex, pos, image.dataBlock->scriptAnimTransitionTime, true);
         }
      }
      else if (!stateData.sequenceNeverTransition && stateData.sequenceTransitionTime && (stateData.sequenceTransitionIn || (lastState && lastState->sequenceTransitionOut)) )
      {
         mShapeFPInstance[imageSlot]->transitionToSequence(mShapeFPAnimThread[imageSlot], seqIndex, pos, stateData.sequenceTransitionTime, true);
      }
      else
      {
         mShapeFPInstance[imageSlot]->setSequence(mShapeFPAnimThread[imageSlot], seqIndex, pos);
      }
      mShapeFPInstance[imageSlot]->setTimeScale(mShapeFPAnimThread[imageSlot], timeScale);
   }
}

void FishPlayer::onImageAnimThreadUpdate(U32 imageSlot, S32 imageShapeIndex, F32 dt)
{
   if (!mShapeFPInstance[imageSlot])
      return;

   if (mShapeFPAmbientThread[imageSlot] && mShapeFPAmbientThread[imageSlot]->hasSequence())
   {
      mShapeFPInstance[imageSlot]->advanceTime(dt,mShapeFPAmbientThread[imageSlot]);
   }

   if (mShapeFPAnimThread[imageSlot] && mShapeFPAnimThread[imageSlot]->hasSequence())
   {
      mShapeFPInstance[imageSlot]->advanceTime(dt,mShapeFPAnimThread[imageSlot]);
   }
}

void FishPlayer::onUnmount( ShapeBase *obj, S32 node )
{
   // Reset back to root position during dismount.
   setActionThread(FishPlayerData::RootAnim,true,false,false);

   // Re-orient the player straight up
   Point3F pos,vec;
   getTransform().getColumn(1,&vec);
   getTransform().getColumn(3,&pos);
   Point3F rot(0.0f,0.0f,-mAtan2(-vec.x,vec.y));
   setPosition(pos,rot);

   // Parent function will call script
   Parent::onUnmount( obj, node );
}

void FishPlayer::unmount()
{
   // Reset back to root position during dismount.  This copies what is
   // done on the server and corrects the fact that the RootAnim change
   // is not sent across to the client using the standard ActionMask.
   setActionThread(FishPlayerData::RootAnim,true,false,false);

   Parent::unmount();
}


//----------------------------------------------------------------------------

void FishPlayer::updateAnimation(F32 dt)
{
   // If dead then remove any image animations
   if ((mDamageState == Disabled || mDamageState == Destroyed) && mImageStateThread)
   {
      // Remove the image state animation
      mShapeInstance->destroyThread(mImageStateThread);
      mImageStateThread = 0;
   }

   if ((isGhost() || mActionAnimation.animateOnServer) && mActionAnimation.thread)
      mShapeInstance->advanceTime(dt,mActionAnimation.thread);
   if (mRecoilThread)
      mShapeInstance->advanceTime(dt,mRecoilThread);
   if (mImageStateThread)
      mShapeInstance->advanceTime(dt,mImageStateThread);

   // If we are the client's player on this machine, then we need
   // to make sure the transforms are up to date as they are used
   // to setup the camera.
   if (isGhost())
   {
      if (getControllingClient())
      {
         updateAnimationTree(isFirstPerson());
         mShapeInstance->animate();
      }
      else
      {
         updateAnimationTree(false);
      }
   }
}

void FishPlayer::updateAnimationTree(bool firstPerson)
{
   S32 mode = 0;
   if (firstPerson)
      if (mActionAnimation.firstPerson)
         mode = 0;
//            TSShapeInstance::MaskNodeRotation;
//            TSShapeInstance::MaskNodePosX |
//            TSShapeInstance::MaskNodePosY;
      else
         mode = TSShapeInstance::MaskNodeAllButBlend;

   for (U32 i = 0; i < FishPlayerData::NumSpineNodes; i++)
      if (mDataBlock->spineNode[i] != -1)
         mShapeInstance->setNodeAnimationState(mDataBlock->spineNode[i],mode);
}



//----------------------------------------------------------------------------
inline Point3F createInterpPos(const Point3F& s, const Point3F& e, const F32 t, const F32 d)
{
   Point3F ret;
   ret.interpolate(s, e, t/d);
   return ret;
}
#ifdef   NEW_COLLIDE
Polyhedron  *FishPlayer::getPolyHedron(const MatrixF& transform,const Box3F& box)
{
   F32   sizeX = box.len_x();
   F32   sizeY = box.len_y();
   F32   sizeZ = box.len_z();

   F32   scaleX = sizeX / (mBox.maxExtents.x - mBox.minExtents.x);
   F32   scaleY = sizeY / (mBox.maxExtents.y - mBox.minExtents.y);
   F32   scaleZ = sizeZ / (mBox.maxExtents.z - mBox.minExtents.z);

   Point3F xvec,yvec,zvec,min;
   transform.getColumn(0,&xvec);
   xvec *= box.len_x();
   transform.getColumn(1,&yvec);
   yvec *= box.len_y();
   transform.getColumn(2,&zvec);
   zvec *= box.len_z();
   transform.mulP(box.minExtents,&min);

   Polyhedron  *from = &mPolyHedron;
   Polyhedron  *to = &mPolyHedron2;
   for(int j=0; j<from->pointList.size(); j++){

      Point3F  &p = from->pointList[j];

//    to->pointList[j] = min + (xvec*p.x) + (yvec*p.y) + (zvec*p.z);

      Point3F  v = p - mBox.minExtents;

      v.x /= (mBox.maxExtents.x - mBox.minExtents.x);
      v.y /= (mBox.maxExtents.y - mBox.minExtents.y);
      v.z /= (mBox.maxExtents.z - mBox.minExtents.z);
      to->pointList[j] = min + (xvec*v.x) + (yvec*v.y) + (zvec*v.z);
   }
   for(int j=0; j<from->planeList.size(); j++){

      PlaneF  &fromPlane = from->planeList[j];
      PlaneF  &toPlane = to->planeList[j];

      Point3F  vec = fromPlane.d * fromPlane;
//      Point3F  newVec = (xvec*vec.x) + (yvec*vec.y) + (zvec*vec.z);
      Point3F  newVec;
      newVec.x = (scaleX*vec.x);
      newVec.y = (scaleY*vec.y);
      newVec.z = (scaleZ*vec.z);

      toPlane.d = newVec.magnitudeSafe();
      newVec.normalizeSafe();
      toPlane.x = newVec.x;
      toPlane.y = newVec.y;
      toPlane.z = newVec.z;
   }
   return to;
}
#endif
Point3F FishPlayer::_move( const F32 travelTime, Collision *outCol )
{
   // Try and move to new pos
   F32 totalMotion  = 0.0f;
   
   // TODO: not used?
   //F32 initialSpeed = mVelocity.len();

   Point3F start;
   Point3F initialPosition;
   getTransform().getColumn(3,&start);
   initialPosition = start;

   static CollisionList collisionList;
   static CollisionList physZoneCollisionList;

   collisionList.clear();
   physZoneCollisionList.clear();

   MatrixF collisionMatrix(true);
   collisionMatrix.setColumn(3, start);

   VectorF firstNormal(0.0f, 0.0f, 0.0f);
   F32 time = travelTime;
   U32 count = 0;

   static Polyhedron sBoxPolyhedron;
   static ExtrudedPolyList sExtrudedPolyList;
   static ExtrudedPolyList sPhysZonePolyList;

   for (; count < sMoveRetryCount; count++) {
      F32 speed = mVelocity.len();
      if (!speed && !mDeath.haveVelocity())
         break;

      Point3F end = start + mVelocity * time;
      if (mDeath.haveVelocity()) {
         // Add in death movement-
         VectorF  deathVel = mDeath.getPosAdd();
         VectorF  resVel;
         getTransform().mulV(deathVel, & resVel);
         end += resVel;
      }
      Point3F distance = end - start;

      if (mFabs(distance.x) < mObjBox.len_x() &&
          mFabs(distance.y) < mObjBox.len_y() &&
          mFabs(distance.z) < mObjBox.len_z())
      {
         // We can potentially early out of this.  If there are no polys in the clipped polylist at our
         //  end position, then we can bail, and just set start = end;
         Box3F wBox = mScaledBox;
         wBox.minExtents += end;
         wBox.maxExtents += end;

         static EarlyOutPolyList eaPolyList;
         eaPolyList.clear();
         eaPolyList.mNormal.set(0.0f, 0.0f, 0.0f);
         eaPolyList.mPlaneList.clear();
         eaPolyList.mPlaneList.setSize(6);
         eaPolyList.mPlaneList[0].set(wBox.minExtents,VectorF(-1.0f, 0.0f, 0.0f));
         eaPolyList.mPlaneList[1].set(wBox.maxExtents,VectorF(0.0f, 1.0f, 0.0f));
         eaPolyList.mPlaneList[2].set(wBox.maxExtents,VectorF(1.0f, 0.0f, 0.0f));
         eaPolyList.mPlaneList[3].set(wBox.minExtents,VectorF(0.0f, -1.0f, 0.0f));
         eaPolyList.mPlaneList[4].set(wBox.minExtents,VectorF(0.0f, 0.0f, -1.0f));
         eaPolyList.mPlaneList[5].set(wBox.maxExtents,VectorF(0.0f, 0.0f, 1.0f));

         // Build list from convex states here...

         CollisionWorkingList& rList = mConvex.getWorkingList();
         CollisionWorkingList* pList = rList.wLink.mNext;
         while (pList != &rList) {
            Convex* pConvex = pList->mConvex;
            if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
               Box3F convexBox = pConvex->getBoundingBox();
               if (wBox.isOverlapped(convexBox))
               {
                  // No need to separate out the physical zones here, we want those
                  //  to cause a fallthrough as well...
                  pConvex->getPolyList(&eaPolyList);
               }
            }
            pList = pList->wLink.mNext;
         }

         if (eaPolyList.isEmpty())
         {
            totalMotion += (end - start).len();
            start = end;
            break;
         }
      }
      collisionMatrix.setColumn(3, start);
      Polyhedron  *pl;

#ifdef   NEW_COLLIDE
      pl = getPolyHedron(collisionMatrix, mScaledBox);
#else
      sBoxPolyhedron.buildBox(collisionMatrix, mScaledBox, true);
      pl = &sBoxPolyhedron;
#endif
      // Setup the bounding box for the extrudedPolyList
      Box3F plistBox = mScaledBox;
      collisionMatrix.mul(plistBox);
      Point3F oldMin = plistBox.minExtents;
      Point3F oldMax = plistBox.maxExtents;
      plistBox.minExtents.setMin(oldMin + (mVelocity * time) - Point3F(0.1f, 0.1f, 0.1f));
      plistBox.maxExtents.setMax(oldMax + (mVelocity * time) + Point3F(0.1f, 0.1f, 0.1f));

      // Build extruded polyList...
      VectorF vector = end - start;
      sExtrudedPolyList.extrude(*pl,vector);
      sExtrudedPolyList.setVelocity(mVelocity);
      sExtrudedPolyList.setCollisionList(&collisionList);

      sPhysZonePolyList.extrude(*pl,vector);
      sPhysZonePolyList.setVelocity(mVelocity);
      sPhysZonePolyList.setCollisionList(&physZoneCollisionList);

      //test
      Box3F    box = mConvex.getBoundingBox();
      Point3F   extents = box.getExtents();



      // Build list from convex states here...
      CollisionWorkingList& rList = mConvex.getWorkingList();


      CollisionWorkingList* pList = rList.wLink.mNext;
      while (pList != &rList) {
         Convex* pConvex = pList->mConvex;
         if (pConvex->getObject()->getTypeMask() & sCollisionMoveMask) {
            Box3F convexBox = pConvex->getBoundingBox();
            if (plistBox.isOverlapped(convexBox))
            {
               if (pConvex->getObject()->getTypeMask() & PhysicalZoneObjectType){
                  pConvex->getPolyList(&sPhysZonePolyList);
               }else{
                  pConvex->getPolyList(&sExtrudedPolyList);
               }
            }
         }
         pList = pList->wLink.mNext;
      }
      // Take into account any physical zones...
      for (U32 j = 0; j < physZoneCollisionList.getCount(); j++) 
      {
         AssertFatal(dynamic_cast<PhysicalZone*>(physZoneCollisionList[j].object), "Bad phys zone!");
         const PhysicalZone* pZone = (PhysicalZone*)physZoneCollisionList[j].object;
         if (pZone->isActive())
            mVelocity *= pZone->getVelocityMod();
      }

      if (collisionList.getCount() != 0 && collisionList.getTime() < 1.0f) 
      {
         // Set to collision point
         F32 velLen = mVelocity.len();

         F32 dt = time * getMin(collisionList.getTime(), 1.0f);
         start += mVelocity * dt;
         time -= dt;

         totalMotion += velLen * dt;


         // Back off...
         if ( velLen > 0.f ) {
            F32 newT = getMin(0.01f / velLen, dt);
            start -= mVelocity * newT;
            totalMotion -= velLen * newT;
         }

         // Pick the surface most parallel to the face that was hit.
         const Collision *collision = &collisionList[0];
         const Collision *cp = collision + 1;
         const Collision *ep = collision + collisionList.getCount();
         for (; cp != ep; cp++)
         {
            if (cp->faceDot > collision->faceDot)
               collision = cp;
         }

         F32 bd = _doCollisionImpact( collision );

         // Copy this collision out so
         // we can use it to do impacts
         // and query collision.
         *outCol = *collision;

         // Subtract out velocity
         VectorF dv = collision->normal * (bd + sNormalElasticity);
         mVelocity += dv;
         if (count == 0)
         {
            firstNormal = collision->normal;
         }
         else
         {
            if (count == 1)
            {
               // Re-orient velocity along the crease.
               if (mDot(dv,firstNormal) < 0.0f &&
                   mDot(collision->normal,firstNormal) < 0.0f)
               {
                  VectorF nv;
                  mCross(collision->normal,firstNormal,&nv);
                  F32 nvl = nv.len();
                  if (nvl)
                  {
                     if (mDot(nv,mVelocity) < 0.0f)
                        nvl = -nvl;
                     nv *= mVelocity.len() / nvl;
                     mVelocity = nv;
                  }
               }
            }
         }
      }
      else
      {
         totalMotion += (end - start).len();
         start = end;
         break;
      }
   }

   if (count == sMoveRetryCount)
   {
      // Failed to move
      start = initialPosition;
      mVelocity.set(0.0f, 0.0f, 0.0f);
   }

   return start;
}

F32 FishPlayer::_doCollisionImpact( const Collision *collision)
{
   F32 bd = -mDot( mVelocity, collision->normal);

   // shake camera on ground impact
   if( bd > mDataBlock->groundImpactMinSpeed && isControlObject() )
   {
      F32 ampScale = (bd - mDataBlock->groundImpactMinSpeed) / mDataBlock->minImpactSpeed;

      CameraShake *groundImpactShake = new CameraShake;
      groundImpactShake->setDuration( mDataBlock->groundImpactShakeDuration );
      groundImpactShake->setFrequency( mDataBlock->groundImpactShakeFreq );

      VectorF shakeAmp = mDataBlock->groundImpactShakeAmp * ampScale;
      groundImpactShake->setAmplitude( shakeAmp );
      groundImpactShake->setFalloff( mDataBlock->groundImpactShakeFalloff );
      groundImpactShake->init();
      gCamFXMgr.addFX( groundImpactShake );
   }

   if ( isServerObject() && 
      (bd > (mDataBlock->minImpactSpeed / 3.0f) || bd > (mDataBlock->minLateralImpactSpeed / 3.0f )) ) 
   {
      mImpactSound = FishPlayerData::ImpactNormal;
      setMaskBits(ImpactMask);
   }
   return bd;
}

void FishPlayer::_handleCollision( const Collision &collision )
{
   // Track collisions
   if (  !isGhost() && collision.object  /*&& collision.object != mContactInfo.contactObject*/){
      

      queueCollision( collision.object, mVelocity - collision.object->getVelocity() );
      if(collision.object->getTypeMask() & PlayerObjectType ){
         onHitFish(collision);
      }else{
         onHitReef(collision);
      }
   }
}
#ifdef   NEW_VERSION
#include "T3d/physics/physicsworld.h"
#include "T3d/physics/physicsplugin.h"
bool FishPlayer::updatePos(const F32 travelTime)
{
   Point3F  newPos = getBoxCenter();
   if(!isGhost()){
      PhysicsWorld *pw = PHYSICSMGR->getWorld("server");
	   Point3F start = getBoxCenter();
      Point3F  finish = start + mVelocity*travelTime;

      SweepResult res;
      if( pw->sweepSphere(res, start, finish, mCapsuleRadius) ){
         mVelocity = mVelocity - (mDot(mVelocity,res.normal)*res.normal);
         newPos = start + (finish-start)*res.pathFraction;
      }else{
         newPos = finish;
      }
   }else{

   }
   if (isClientObject())
   {
      delta.pos = newPos;
      delta.posVec = delta.posVec - delta.pos;
      delta.dt = 1.0f;
   }

   setPosition( newPos, mRot );
   setMaskBits( MoveMask );
   updateContainer();
/*
   if (!isGhost())  
   {
      notifyCollision();
   }
   */
   return true;
}
#else



bool FishPlayer::updatePos(const F32 travelTime)
{
   PROFILE_SCOPE(Player_UpdatePos);
   getTransform().getColumn(3,&delta.posVec);

   // When mounted to another object, only Z rotation used.
   if (isMounted()) {
      mVelocity = mMount.object->getVelocity();
      setPosition(Point3F(0.0f, 0.0f, 0.0f), mRot);
      setMaskBits(MoveMask);
      return true;
   }

   Point3F newPos;

   Collision col;
   dMemset( &col, 0, sizeof( col ) );

   // DEBUG:
   //Point3F savedVelocity = mVelocity;

   if ( mPhysicsRep )
   {
      static CollisionList collisionList;
      collisionList.clear();

// start jc
//      newPos = mPhysicsRep->move( mVelocity * travelTime, collisionList );
	  VectorF disp(mVelocity * travelTime);
     if (!mSwimming && mIsZero(disp.z)){
	    disp.z = 0.0f;
     }
      newPos = mPhysicsRep->move( disp, collisionList );
// end jc

      bool haveCollisions = false;
      if (collisionList.getCount() > 0)
      {
         haveCollisions = true;
      }

      if (haveCollisions)
      {
         // Pick the collision that most closely matches our direction
         VectorF velNormal = mVelocity;
         velNormal.normalizeSafe();
         const Collision *collision = &collisionList[0];
         F32 collisionDot = mDot(velNormal, collision->normal);
         const Collision *cp = collision + 1;
         const Collision *ep = collision + collisionList.getCount();
         for (; cp != ep; cp++)
         {
            F32 dp = mDot(velNormal, cp->normal);
            if (dp < collisionDot)
            {
               collisionDot = dp;
               collision = cp;
            }
         }

         _doCollisionImpact( collision );

         // Modify our velocity based on collisions
         for (U32 i=0; i<collisionList.getCount(); ++i)
         {
            F32 bd = -mDot( mVelocity, collisionList[i].normal );
            VectorF dv = collisionList[i].normal * (bd + sNormalElasticity);
            mVelocity += dv;
         }

//start pg
/*
         // Store the last collision for use later on.  The handle collision
         // code only expects a single collision object.
         if (collisionList.getCount() > 0)
            col = collisionList[collisionList.getCount() - 1];

         // We'll handle any player-to-player collision, and the last collision
         // with other obejct types.
         for (U32 i=0; i<collisionList.getCount(); ++i)
         {
            Collision& colCheck = collisionList[i];
            if (colCheck.object)
            {
               SceneObject* obj = static_cast<SceneObject*>(col.object);
               if (obj->getTypeMask() & PlayerObjectType)
               {
                  _handleCollision( colCheck );
               }
               else
               {
                  col = colCheck;
               }
            }
         }
         _handleCollision( col );
*/
         static   int   maxCount = 0;
         maxCount = maxCount<collisionList.getCount()? collisionList.getCount() : maxCount;

         _handleCollision( collisionList[ collisionList.getCount()-1 ] );

//         for (U32 i=0; i<collisionList.getCount(); ++i)
//         {
//            Collision& colCheck = collisionList[i];
//            _handleCollision( colCheck );
//         }
         //end pg


      }
   }
   else
   {
      if ( mVelocity.isZero() )
         newPos = delta.posVec;
      else
         newPos = _move( travelTime, &col );
   
      _handleCollision( col );
   }

   // DEBUG:
   //if ( isClientObject() )
   //   Con::printf( "(client) vel: %g %g %g", mVelocity.x, mVelocity.y, mVelocity.z );
   //else
   //   Con::printf( "(server) vel: %g %g %g", mVelocity.x, mVelocity.y, mVelocity.z );

   // Set new position
   // If on the client, calc delta for backstepping
   if (isClientObject())
   {
      delta.pos = newPos;
      delta.posVec = delta.posVec - delta.pos;
      delta.dt = 1.0f;
   }

   setPosition( newPos, mRot );
   setMaskBits( MoveMask );
   updateContainer();

   if (!isGhost())  
   {
      // Collisions are only queued on the server and can be
      // generated by either updateMove or updatePos
      notifyCollision();

   }

   // Check the total distance moved.  If it is more than 1000th of the velocity, then
   //  we moved a fair amount...
   //if (totalMotion >= (0.001f * initialSpeed))
      return true;
  // else
   //   return false;
}
#endif


//----------------------------------------------------------------------------
 
void FishPlayer::_findContact( SceneObject **contactObject, 
                           VectorF *contactNormal, 
                           Vector<SceneObject*> *outOverlapObjects )
{
   Point3F pos;
   getTransform().getColumn(3,&pos);

   Box3F wBox;
   Point3F exp(0,0,sTractionDistance);
   wBox.minExtents = pos + mScaledBox.minExtents - exp;
   wBox.maxExtents.x = pos.x + mScaledBox.maxExtents.x;
   wBox.maxExtents.y = pos.y + mScaledBox.maxExtents.y;
   wBox.maxExtents.z = pos.z + mScaledBox.minExtents.z + sTractionDistance;

   static ClippedPolyList polyList;
   polyList.clear();
   polyList.doConstruct();
   polyList.mNormal.set(0.0f, 0.0f, 0.0f);
   polyList.setInterestNormal(Point3F(0.0f, 0.0f, -1.0f));

   polyList.mPlaneList.setSize(6);
   polyList.mPlaneList[0].setYZ(wBox.minExtents, -1.0f);
   polyList.mPlaneList[1].setXZ(wBox.maxExtents, 1.0f);
   polyList.mPlaneList[2].setYZ(wBox.maxExtents, 1.0f);
   polyList.mPlaneList[3].setXZ(wBox.minExtents, -1.0f);
   polyList.mPlaneList[4].setXY(wBox.minExtents, -1.0f);
   polyList.mPlaneList[5].setXY(wBox.maxExtents, 1.0f);
   Box3F plistBox = wBox;

   // Expand build box as it will be used to collide with items.
   // PickupRadius will be at least the size of the box.
   F32 pd = (F32)mDataBlock->pickupDelta;
   wBox.minExtents.x -= pd; wBox.minExtents.y -= pd;
   wBox.maxExtents.x += pd; wBox.maxExtents.y += pd;
   wBox.maxExtents.z = pos.z + mScaledBox.maxExtents.z;

   // Build list from convex states here...
   CollisionWorkingList& rList = mConvex.getWorkingList();
   CollisionWorkingList* pList = rList.wLink.mNext;
   while (pList != &rList)
   {
      Convex* pConvex = pList->mConvex;

      U32 objectMask = pConvex->getObject()->getTypeMask();
      
      if (  ( objectMask & sCollisionMoveMask ) &&
            !( objectMask & PhysicalZoneObjectType ) )
      {
         Box3F convexBox = pConvex->getBoundingBox();
         if (plistBox.isOverlapped(convexBox))
            pConvex->getPolyList(&polyList);
      }
      else
         outOverlapObjects->push_back( pConvex->getObject() );

      pList = pList->wLink.mNext;
   }

   if (!polyList.isEmpty())
   {
      // Pick flattest surface
      F32 bestVd = -1.0f;
      ClippedPolyList::Poly* poly = polyList.mPolyList.begin();
      ClippedPolyList::Poly* end = polyList.mPolyList.end();
      for (; poly != end; poly++)
      {
         F32 vd = poly->plane.z;       // i.e.  mDot(Point3F(0,0,1), poly->plane);
         if (vd > bestVd)
         {
            bestVd = vd;
            *contactObject = poly->object;
            *contactNormal = poly->plane;
         }
      }
   }
}
//********************************************************************************
// substitute for findContact() if only interested in trigger overlaps
//********************************************************************************
void FishPlayer::triggerCheck()
{
   Vector<SceneObject *>   objectList;
   gServerContainer.findObjectList(mWorldBox, TriggerObjectType, &objectList);
   for(Vector<SceneObject *>::iterator it = objectList.begin(); 
      it != objectList.end(); 
      ++it) 
   {
      Trigger* pTrigger = static_cast<Trigger*>( *it );
      pTrigger->potentialEnterObject(this);
   }
}
void FishPlayer::findContact( VectorF *contactNormal )
{
   SceneObject *contactObject = NULL;

   Vector<SceneObject*> overlapObjects;
   if ( mPhysicsRep )
      mPhysicsRep->findContact( &contactObject, contactNormal, &overlapObjects );
   else
      _findContact( &contactObject, contactNormal, &overlapObjects );

   // Check for triggers, corpses and items.
   const U32 filterMask = isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask;
   for ( U32 i=0; i < overlapObjects.size(); i++ )
   {
      SceneObject *obj = overlapObjects[i];
      U32 objectMask = obj->getTypeMask();

      if ( !( objectMask & filterMask ) )
         continue;

      // Check: triggers, corpses and items...
      //
      if (objectMask & TriggerObjectType)
      {
         Trigger* pTrigger = static_cast<Trigger*>( obj );
         pTrigger->potentialEnterObject(this);
      }
	  /*
      else if (objectMask & CorpseObjectType)
      {
         // If we've overlapped the worldbounding boxes, then that's it...
         if ( getWorldBox().isOverlapped( obj->getWorldBox() ) )
         {
            ShapeBase* col = static_cast<ShapeBase*>( obj );
            queueCollision(col,getVelocity() - col->getVelocity());
         }
      }
	  */
      else if (objectMask & ItemObjectType)
      {
         // If we've overlapped the worldbounding boxes, then that's it...
         Item* item = static_cast<Item*>( obj );
         if (  getWorldBox().isOverlapped(item->getWorldBox()) &&
               item->getCollisionObject() != this && 
               !item->isHidden() )
            queueCollision(item,getVelocity() - item->getVelocity());
      }
   }

//   F32 vd = (*contactNormal).z;
//   *run = vd > mDataBlock->runSurfaceCos;
//   *jump = vd > mDataBlock->jumpSurfaceCos;

   mContactInfo.clear();
  
   mContactInfo.contacted = contactObject != NULL;
   mContactInfo.contactObject = contactObject;

   if ( mContactInfo.contacted )
      mContactInfo.contactNormal = *contactNormal;

//   mContactInfo.run = *run;
//   mContactInfo.jump = *jump;
}

//----------------------------------------------------------------------------

bool FishPlayer::isDisplacable() const
{
   return true;
}

Point3F FishPlayer::getMomentum() const
{
   return mVelocity * getMass();
}

void FishPlayer::setMomentum(const Point3F& newMomentum)
{
   Point3F newVelocity = newMomentum / getMass();
   mVelocity = newVelocity;
}

#define  LH_HACK   1
// Hack for short-term soln to Training crash -
#if   LH_HACK
static U32  sBalance;

bool FishPlayer::displaceObject(const Point3F& displacement)
{
   F32 vellen = mVelocity.len();
   if (vellen < 0.001f || sBalance > 16) {
      mVelocity.set(0.0f, 0.0f, 0.0f);
      return false;
   }

   F32 dt = displacement.len() / vellen;

   sBalance++;

   bool result = updatePos(dt);

   sBalance--;

   getTransform().getColumn(3, &delta.pos);
   delta.posVec.set(0.0f, 0.0f, 0.0f);

   return result;
}

#else

bool FishPlayer::displaceObject(const Point3F& displacement)
{
   F32 vellen = mVelocity.len();
   if (vellen < 0.001f) {
      mVelocity.set(0.0f, 0.0f, 0.0f);
      return false;
   }

   F32 dt = displacement.len() / vellen;

   bool result = updatePos(dt);

   mObjToWorld.getColumn(3, &delta.pos);
   delta.posVec.set(0.0f, 0.0f, 0.0f);

   return result;
}

#endif

//----------------------------------------------------------------------------

void FishPlayer::setPosition(const Point3F& pos,const Point3F& _rot)
{
   MatrixF mat;

   Point3F rot = _rot;
   if (isMounted()) {
      // Use transform from mounted object
      MatrixF nmat,zrot;
      mMount.object->getMountTransform( mMount.node, mMount.xfm, &nmat );
      zrot.set(EulerF(0.0f, 0.0f, rot.z));
      mat.mul(nmat,zrot);
   }else{
      if(mDataBlock && mDataBlock->noTilt){
         rot.x = 0;
      }
      if (mDataBlock && mDataBlock->conformToGround)
      {
         if(mSwimming)
         {
            mat.set(EulerF(0.0f, 0.0f, rot.z));
            mat.setColumn(3,pos);
         }
         else
         {
            mat.set(EulerF(0.0f, 0.0f, rot.z));
            mat.setColumn(3,pos);

            RayInfo surfaceInfo;
            Point3F above = Point3F(pos.x,pos.y,pos.z + 0.5f);
            Point3F below = Point3F(pos.x,pos.y,pos.z - 100.0f);
            if (gClientContainer.castRay(above,below,sCollisionMoveMask,&surfaceInfo))
            {
               Point3F x,y,z;
               z = surfaceInfo.normal;
               z.normalize();
               mat.getColumn(1,&y);
               mCross(y,z,&x);
               x.normalize();
               mCross(z,x,&y);
               mat.setColumn(0,x);
               mat.setColumn(1,y);
               mat.setColumn(2,z);
            }
         }
      }
      else
      {
         mat.set(EulerF(0.0f, 0.0f, rot.z));
         mat.setColumn(3,pos);
      }
   }
   Parent::setTransform(mat);
   mRot = rot;

   if ( mPhysicsRep )
      mPhysicsRep->setTransform( mat );
}


void FishPlayer::setRenderPosition(const Point3F& cpos, const Point3F& rot, F32 dt)
{
   MatrixF mat;

// start ds - offset collision box
	Point3F pos = cpos + mDataBlock->renderOffset;
// end ds

   if (isMounted()) {
      // Use transform from mounted object
      MatrixF nmat,zrot;
      mMount.object->getRenderMountTransform( dt, mMount.node, mMount.xfm, &nmat );
      zrot.set(EulerF(0.0f, 0.0f, rot.z));
      mat.mul(nmat,zrot);
   }
   else {
      EulerF   orient(0.0f, 0.0f, rot.z);

// start ds - conform to ground
//      mat.set(orient);
//      mat.setColumn(3, pos);
// end ds

      if (inDeathAnim()) {
//         F32   boxRad = (mDataBlock->boxSize.x * 0.5f);
         F32   boxRad = (getObjBox().getExtents().x * 0.5f);

         if (MatrixF * fallMat = mDeath.fallToGround(dt, pos, rot.z, boxRad))
            mat = * fallMat;
      }
      else
         mDeath.initFall();
// start ds - conform to ground
      if (mDataBlock && mDataBlock->conformToGround)
      {
         if(mSwimming)
         {
//            mat.set(rot);
//            mat.setColumn(3, pos);
            MatrixF xRot, zRot;
            xRot.set(EulerF(mHead.x, 0, 0));
            zRot.set(EulerF(0, 0, rot.z));

            mat.mul(zRot, xRot);
            mat.setColumn(3,pos);
         }
         else
         {
            mat.set(orient);
            mat.setColumn(3, pos);

            Point3F x,y,z;
            RayInfo surfaceInfo;
            Point3F above = Point3F(pos.x,pos.y,pos.z + 0.5f);
            Point3F below = Point3F(pos.x,pos.y,pos.z - 100.0f);
            if (gClientContainer.castRay(above,below,sCollisionMoveMask,&surfaceInfo))
            {
               z = surfaceInfo.normal;
               z.normalize();
               mat.getColumn(1,&y);
               mCross(y,z,&x);
               x.normalize();
               mCross(z,x,&y);
               mat.setColumn(0,x);
               mat.setColumn(1,y);
               mat.setColumn(2,z);
            }
         }
      }
      else
      {
         mat.set(orient);
         mat.setColumn(3, pos);
      }
// end ds
   }

// start ds - conform to ground
   //Parent::setRenderTransform(mat);
   setRenderTransform(mat);

// end ds
}

//----------------------------------------------------------------------------

void FishPlayer::setTransform(const MatrixF& mat)
{
   // This method should never be called on the client.

   // This currently converts all rotation in the mat into
   // rotations around the z axis.
   Point3F pos,vec;
   mat.getColumn(1,&vec);
   mat.getColumn(3,&pos);
   Point3F rot(0.0f, 0.0f, -mAtan2(-vec.x,vec.y));
   setPosition(pos,rot);
   setMaskBits(MoveMask | NoWarpMask);
}
// start ds - conform to ground
void FishPlayer::setRenderTransform(const MatrixF & mat)
{
   if (mDataBlock && mDataBlock->smoothCamera>0.0001f)
   {
      Point3F pos, x,y,z;
      mat.getColumn(3,&pos);

      // keep compass direction (assume not looking straight up)
      mat.getColumn(1,&y);
      y.z = 0;
      y.normalize();

      // smooth out camera
      MatrixF mat2 = getRenderTransform();
      QuatF q1(mat);
      QuatF q2(mat2);
      F32 k = mDataBlock->smoothCamera;

      q2.x = q2.x * k + (1.0f-k) * q1.x;
      q2.y = q2.y * k + (1.0f-k) * q1.y;
      q2.z = q2.z * k + (1.0f-k) * q1.z;
      q2.w = q2.w * k + (1.0f-k) * q1.w;
      q2.normalize();
      q2.setMatrix(&mat2);
      mat2.setColumn(3,pos);
   
 
      // keep up-vector, refigure y & z based on saved off y
      mat2.getColumn(2,&z);
      mCross(y,z,&x);
      x.normalize();
      mCross(z,x,&y);
      mat2.setColumn(0,x);
      mat2.setColumn(1,y);
   

      Parent::setRenderTransform(mat2);
   }
   else
      Parent::setRenderTransform(mat);
}
// end ds

void FishPlayer::getEyeTransform(MatrixF* mat)
{
   getEyeBaseTransform(mat);

   // The shape instance is animated in getEyeBaseTransform() so we're
   // good here when attempting to get the eye node position on the server.
   S32 imageIndex = -1;
   S32 shapeIndex = -1;
   MountedImage* image = NULL;
   ShapeBaseImageData* data = NULL;
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      image = &(mMountedImageList[i]);
      if (image->dataBlock) 
      {
         data = image->dataBlock;
         shapeIndex = getImageShapeIndex(*image);
         if ( data->useEyeNode && (data->animateOnServer || isGhost()) && isFirstPerson() && data->eyeMountNode[shapeIndex] != -1 && data->eyeNode[shapeIndex] != -1 )
         {
            imageIndex = i;
            break;
         }
      }
   }

   if (imageIndex >= 0)
   {
      // Get the image's eye node's position relative to the eye mount node
      MatrixF mountTransform = image->shapeInstance[shapeIndex]->mNodeTransforms[data->eyeMountNode[shapeIndex]];
      Point3F eyeMountNodePos = mountTransform.getPosition();
      mountTransform = image->shapeInstance[shapeIndex]->mNodeTransforms[data->eyeNode[shapeIndex]];
      Point3F eyeNodePos = mountTransform.getPosition() - eyeMountNodePos;

      // Now transform to the image's eye node (position only)
      MatrixF xfm(true);
      xfm.setPosition(eyeNodePos);
      mat->mul(xfm);
   }
}

void FishPlayer::getEyeBaseTransform(MatrixF* mat)
{
   // Eye transform in world space.  We only use the eye position
   // from the animation and supply our own rotation.
   MatrixF pmat,xmat,zmat;

   if(!isGhost()) 
      mShapeInstance->animate();

   xmat.set(EulerF(mHead.x, 0.0f, 0.0f));

   if (mUseHeadZCalc)
      zmat.set(EulerF(0.0f, 0.0f, mHead.z));
   else
      zmat.identity();

   pmat.mul(zmat,xmat);

   F32 *dp = pmat;

   F32* sp;
   MatrixF eyeMat(true);
   if (mDataBlock->eyeNode != -1)
   {
      sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   }
   else
   {
      Point3F center;
      mObjBox.getCenter(&center);
      eyeMat.setPosition(center);
      sp = eyeMat;
   }

   const Point3F& scale = getScale();
   dp[3] = sp[3] * scale.x;
   dp[7] = sp[7] * scale.y;
   dp[11] = sp[11] * scale.z;
   mat->mul(getTransform(),pmat);
}

void FishPlayer::getRenderEyeTransform(MatrixF* mat)
{
   getRenderEyeBaseTransform(mat);

   // Use the first image that is set to use the eye node
   for (U32 i=0; i<ShapeBase::MaxMountedImages; ++i)
   {
      MountedImage& image = mMountedImageList[i];
      if (image.dataBlock) 
      {
         ShapeBaseImageData& data = *image.dataBlock;
         U32 shapeIndex = getImageShapeIndex(image);
         if ( data.useEyeNode && isFirstPerson() && data.eyeMountNode[shapeIndex] != -1 && data.eyeNode[shapeIndex] != -1 )
         {
            // Get the eye node's position relative to the eye mount node
            MatrixF mountTransform = image.shapeInstance[shapeIndex]->mNodeTransforms[data.eyeMountNode[shapeIndex]];
            Point3F eyeMountNodePos = mountTransform.getPosition();
            mountTransform = image.shapeInstance[shapeIndex]->mNodeTransforms[data.eyeNode[shapeIndex]];
            Point3F eyeNodePos = mountTransform.getPosition() - eyeMountNodePos;

            // Now transform to the image's eye node (position only)
            MatrixF xfm(true);
            xfm.setPosition(eyeNodePos);
            mat->mul(xfm);

            return;
         }
      }
   }
}

void FishPlayer::getRenderEyeBaseTransform(MatrixF* mat)
{
   // Eye transform in world space.  We only use the eye position
   // from the animation and supply our own rotation.
   MatrixF pmat,xmat,zmat;
   xmat.set(EulerF(delta.head.x + delta.headVec.x * delta.dt, 0.0f, 0.0f));

   if (mUseHeadZCalc)
      zmat.set(EulerF(0.0f, 0.0f, delta.head.z + delta.headVec.z * delta.dt));
   else
      zmat.identity();

   pmat.mul(zmat,xmat);

   F32 *dp = pmat;

   F32* sp;
   MatrixF eyeMat(true);
   if (mDataBlock->eyeNode != -1)
   {
      sp = mShapeInstance->mNodeTransforms[mDataBlock->eyeNode];
   }
   else
   {
      // Use the center of the FishPlayer's bounding box for the eye position.
      Point3F center;
      mObjBox.getCenter(&center);
      eyeMat.setPosition(center);
      sp = eyeMat;
   }

   // Only use position of eye node, and take FishPlayer's scale
   // into account.
   const Point3F& scale = getScale();
   dp[3] = sp[3] * scale.x;
   dp[7] = sp[7] * scale.y;
   dp[11] = sp[11] * scale.z;

   mat->mul(getRenderTransform(), pmat);
}

void FishPlayer::renderMountedImage( U32 imageSlot, TSRenderState &rstate, SceneRenderState *state )
{
   GFX->pushWorldMatrix();

   MatrixF world;

   MountedImage& image = mMountedImageList[imageSlot];
   ShapeBaseImageData& data = *image.dataBlock;
   U32 imageShapeIndex;
   if ( state->isShadowPass() )
   {
      // Force the standard image shapes for the shadow pass.
      imageShapeIndex = ShapeBaseImageData::StandardImageShape;
   }
   else
   {
      imageShapeIndex = getImageShapeIndex(image);
   }

   if ( !state->isShadowPass() && isFirstPerson() && (data.useEyeOffset || (data.useEyeNode && data.eyeMountNode[imageShapeIndex] != -1)) ) 
   {
      if (data.useEyeNode && data.eyeMountNode[imageShapeIndex] != -1)
      {
         MatrixF nmat;
         getRenderEyeBaseTransform(&nmat);
         MatrixF offsetMat = image.shapeInstance[imageShapeIndex]->mNodeTransforms[data.eyeMountNode[imageShapeIndex]];
         offsetMat.affineInverse();
         world.mul(nmat,offsetMat);
      }
      else
      {
         MatrixF nmat;
         getRenderEyeBaseTransform(&nmat);
         world.mul(nmat,data.eyeOffset);
      }

      if ( imageSlot == 0 )
      {
         MatrixF nmat;
         MatrixF smat;

         getRenderRetractionTransform(0,&nmat);         
         getRenderImageTransform(0,&smat);

         // See if we are pushed into a wall...
         Point3F start, end;
         smat.getColumn(3, &start);
         nmat.getColumn(3, &end);

         world.setPosition( world.getPosition() );
      }
   }
   else 
   {
      MatrixF nmat;
      getRenderMountTransform( 0.0f, data.mountPoint, MatrixF::Identity, &nmat);
      world.mul(nmat,data.mountTransform[imageShapeIndex]);
   }

   GFX->setWorldMatrix( world );

   image.shapeInstance[imageShapeIndex]->animate();
   image.shapeInstance[imageShapeIndex]->render( rstate );

   // Render the first person mount image shape?
   if (!state->isShadowPass() && imageShapeIndex == ShapeBaseImageData::FirstPersonImageShape && mShapeFPInstance[imageSlot])
   {
      mShapeFPInstance[imageSlot]->animate();
      mShapeFPInstance[imageSlot]->render( rstate );
   }

   GFX->popWorldMatrix();
}
void FishPlayer::getCameraParameters(F32 *min,F32* max,Point3F* off,MatrixF* rot)
{
   if (!mControlObject.isNull() && mControlObject == getObjectMount()) {
      mControlObject->getCameraParameters(min,max,off,rot);
      return;
   }
   const Point3F& scale = getScale();
   *min = mDataBlock->cameraMinDist * scale.y;
   *max = mDataBlock->cameraMaxDist * scale.y;
   off->set(0.0f, 0.0f, 0.0f);
   rot->identity();
}


//----------------------------------------------------------------------------

Point3F FishPlayer::getVelocity() const
{
   return mVelocity;
}

F32 FishPlayer::getSpeed() const
{
   return mVelocity.len();
}

void FishPlayer::setVelocity(const VectorF& vel)
{
	AssertFatal( !mIsNaN( vel ), "FishPlayer::setVelocity() - The velocity is NaN!" );

   mVelocity = vel;
   setMaskBits(MoveMask);
}

void FishPlayer::applyImpulse(const Point3F&,const VectorF& vec)
{
	AssertFatal( !mIsNaN( vec ), "FishPlayer::applyImpulse() - The vector is NaN!" );

   // Players ignore angular velocity
   VectorF vel;
   vel.x = vec.x / getMass();
   vel.y = vec.y / getMass();
   vel.z = vec.z / getMass();

   // Make sure the impulse isn't too big
   F32 len = vel.magnitudeSafe();
   if (len > sMaxImpulseVelocity)
   {
      Point3F excess = vel * ( 1.0f - (sMaxImpulseVelocity / len ) );
      vel -= excess;
   }

   setVelocity(mVelocity + vel);
}


//----------------------------------------------------------------------------

bool FishPlayer::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
   if (getDamageState() != Enabled)
      return false;

   // Collide against bounding box. Need at least this for the editor.
   F32 st,et,fst = 0.0f,fet = 1.0f;
   F32 *bmin = &mObjBox.minExtents.x;
   F32 *bmax = &mObjBox.maxExtents.x;
   F32 const *si = &start.x;
   F32 const *ei = &end.x;

   for (int i = 0; i < 3; i++) {
      if (*si < *ei) {
         if (*si > *bmax || *ei < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si < *bmin)? (*bmin - *si) / di: 0.0f;
         et = (*ei > *bmax)? (*bmax - *si) / di: 1.0f;
      }
      else {
         if (*ei > *bmax || *si < *bmin)
            return false;
         F32 di = *ei - *si;
         st = (*si > *bmax)? (*bmax - *si) / di: 0.0f;
         et = (*ei < *bmin)? (*bmin - *si) / di: 1.0f;
      }
      if (st > fst) fst = st;
      if (et < fet) fet = et;
      if (fet < fst)
         return false;
      bmin++; bmax++;
      si++; ei++;
   }

   info->normal = start - end;
   info->normal.normalizeSafe();
   getTransform().mulV( info->normal );

   info->t = fst;
   info->object = this;
   info->point.interpolate(start,end,fst);
   info->material = 0;
   return true;
}


//----------------------------------------------------------------------------

static MatrixF IMat(1);

bool FishPlayer::buildPolyList(PolyListContext, AbstractPolyList* polyList, const Box3F&, const SphereF&)
{
   // Collision with the player is always against the player's object
   // space bounding box axis aligned in world space.
   Point3F pos;
   getTransform().getColumn(3,&pos);
   IMat.setColumn(3,pos);
   polyList->setTransform(&IMat, Point3F(1.0f,1.0f,1.0f));
   polyList->setObject(this);
   polyList->addBox(mObjBox);
   return true;
}


void FishPlayer::buildConvex(const Box3F& box, Convex* convex)
{
#ifdef   NEW_COLLIDE
   Parent::buildConvex(box, convex);
#else
   if (mShapeInstance == NULL)
      return;


   // These should really come out of a pool
   mConvexList->collectGarbage();

   Box3F realBox = box;
   mWorldToObj.mul(realBox);
   realBox.minExtents.convolveInverse(mObjScale);
   realBox.maxExtents.convolveInverse(mObjScale);

   if (realBox.isOverlapped(getObjBox()) == false)
      return;

   Convex* cc = 0;
   CollisionWorkingList& wl = convex->getWorkingList();
   for (CollisionWorkingList* itr = wl.wLink.mNext; itr != &wl; itr = itr->wLink.mNext) {
      if (itr->mConvex->getType() == BoxConvexType &&
          itr->mConvex->getObject() == this) {
         cc = itr->mConvex;
         break;
      }
   }
   if (cc)
      return;

   // Create a new convex.
   BoxConvex* cp = new OrthoBoxConvex;
   mConvexList->registerObject(cp);
   convex->addToWorkingList(cp);
   cp->init(this);

   mObjBox.getCenter(&cp->mCenter);
   cp->mSize.x = mObjBox.len_x() / 2.0f;
   cp->mSize.y = mObjBox.len_y() / 2.0f;
   cp->mSize.z = mObjBox.len_z() / 2.0f;
#endif
}


//----------------------------------------------------------------------------

void FishPlayer::updateWorkingCollisionSet()
{
#ifdef   NEW_COLLIDE
   makeConvex();
#endif
   // First, we need to adjust our velocity for possible acceleration.  It is assumed
   // that we will never accelerate more than 20 m/s for gravity, plus 10 m/s for
   // jetting, and an equivalent 10 m/s for jumping.  We also assume that the
   // working list is updated on a Tick basis, which means we only expand our
   // box by the possible movement in that tick.
   Point3F scaledVelocity = mVelocity * TickSec;
   F32 len    = scaledVelocity.len();
   F32 newLen = len + (10.0f * TickSec);

   // Check to see if it is actually necessary to construct the new working list,
   // or if we can use the cached version from the last query.  We use the x
   // component of the min member of the mWorkingQueryBox, which is lame, but
   // it works ok.
   bool updateSet = false;

   Box3F convexBox = mConvex.getBoundingBox(getTransform(), getScale());

   F32 l = (newLen * 1.1f) + 0.1f;  // from Convex::updateWorkingList
   const Point3F  lPoint( l, l, l );
   convexBox.minExtents -= lPoint;
   convexBox.maxExtents += lPoint;

   // Check containment
   if (mWorkingQueryBox.minExtents.x != -1e9f)
   {
      if (mWorkingQueryBox.isContained(convexBox) == false)
         // Needed region is outside the cached region.  Update it.
         updateSet = true;
   }
   else
   {
      // Must update
      updateSet = true;
   }
   // Actually perform the query, if necessary
   if (updateSet == true) {
      const Point3F  twolPoint( 2.0f * l, 2.0f * l, 2.0f * l );
      mWorkingQueryBox = convexBox;
      mWorkingQueryBox.minExtents -= twolPoint;
      mWorkingQueryBox.maxExtents += twolPoint;

      disableCollision();
      mConvex.updateWorkingList(mWorkingQueryBox,
         isGhost() ? sClientCollisionContactMask : sServerCollisionContactMask);
      enableCollision();
   }
}


//----------------------------------------------------------------------------

void FishPlayer::writePacketData(GameConnection *connection, BitStream *stream)
{
   Parent::writePacketData(connection, stream);

   stream->writeInt(mState,NumStateBits);
// start jc
//   stream->writeInt(mPose, NumPoseBits);   // Mquaker :: add net logic for mPose  
// end jc
   if (stream->writeFlag(mState == RecoverState))
      stream->writeInt(mRecoverTicks,FishPlayerData::RecoverDelayBits);

   Point3F pos;
   getTransform().getColumn(3,&pos);
   if (stream->writeFlag(!isMounted())) {
      // Will get position from mount
      stream->setCompressionPoint(pos);
      stream->write(pos.x);
      stream->write(pos.y);
      stream->write(pos.z);
      stream->write(mVelocity.x);
      stream->write(mVelocity.y);
      stream->write(mVelocity.z);

   }
   stream->write(mHead.x);
   stream->write(mHead.z);
   stream->write(mRot.z);

   if (mControlObject) {
      S32 gIndex = connection->getGhostIndex(mControlObject);
      if (stream->writeFlag(gIndex != -1)) {
         stream->writeInt(gIndex,NetConnection::GhostIdBitSize);
         mControlObject->writePacketData(connection, stream);
      }
   }
   else
      stream->writeFlag(false);
}


void FishPlayer::readPacketData(GameConnection *connection, BitStream *stream)
{
   Parent::readPacketData(connection, stream);

   mState = (ActionState)stream->readInt(NumStateBits);
// start jc
//   mPose = (Pose)stream->readInt(NumPoseBits);   // Mquaker :: add net logic for mPose  
//	  Pose pose = (Pose)stream->readInt(NumPoseBits);
//      setPose(pose);
// end jc
   if (stream->readFlag())
      mRecoverTicks = stream->readInt(FishPlayerData::RecoverDelayBits);

   Point3F pos,rot;
   if (stream->readFlag()) {
      // Only written if we are not mounted
      stream->read(&pos.x);
      stream->read(&pos.y);
      stream->read(&pos.z);
      stream->read(&mVelocity.x);
      stream->read(&mVelocity.y);
      stream->read(&mVelocity.z);
      stream->setCompressionPoint(pos);
      delta.pos = pos;

   }
   else
      pos = delta.pos;
   stream->read(&mHead.x);
   
   if(mDataBlock->noTilt){
      mHead.x = 0;   //pghack
   }


   stream->read(&mHead.z);
   stream->read(&rot.z);
   rot.x = rot.y = 0;
   setPosition(pos,rot);
   delta.head = mHead;
   delta.rot = rot;

   if (stream->readFlag()) {
      S32 gIndex = stream->readInt(NetConnection::GhostIdBitSize);
      ShapeBase* obj = static_cast<ShapeBase*>(connection->resolveGhost(gIndex));
      setControlObject(obj);
      obj->readPacketData(connection, stream);
   }
   else
      setControlObject(0);
}

#define FishPlayerNetOptimize
U32 FishPlayer::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
#ifndef FishPlayerNetOptimize
   U32 retMask = Parent::packUpdate(con, mask, stream);
#else
   U32 retMask = Parent::Parent::packUpdate(con, mask, stream);

   if (mask & InitialUpdateMask) {
      // mask off sounds that aren't playing
      S32 i;
      for (i = 0; i < MaxSoundThreads; i++)
         if (!mSoundThread[i].play)
            mask &= ~(SoundMaskN << i);

      // mask off threads that aren't running
      for (i = 0; i < MaxScriptThreads; i++)
         if (mScriptThread[i].sequence == -1)
            mask &= ~(ThreadMaskN << i);

      // mask off images that aren't updated
      for(i = 0; i < MaxMountedImages; i++)
         if(!mMountedImageList[i].dataBlock)
            mask &= ~(ImageMaskN << i);

   }

// start jc
//   if(!stream->writeFlag(mask & (NameMask | DamageMask | SoundMask | MeshHiddenMask |
//         ThreadMask | ImageMask | CloakMask | InvincibleMask |
//         ShieldMask | SkinMask)))
   if(stream->writeFlag(mask & (NameMask | DamageMask | SoundMask | MeshHiddenMask |
         ThreadMask | ImageMask | CloakMask | InvincibleMask |
         ShieldMask | SkinMask | GravityMask)))
   {
// end jc


//   if (stream->writeFlag(mask & DamageMask)) {
//      stream->writeFloat(mClampF(mDamage / mDataBlock->maxDamage, 0.f, 1.f), DamageLevelBits);
//      stream->writeInt(mDamageState,NumDamageStateBits);
//      stream->writeNormalVector( damageDir, 8 );
//   }

// start jc
#ifndef TORQUE_RESYNC_ANIM
   if (stream->writeFlag(mask & ThreadMask)) {
#else
   bool needsReSync = false;
   for (int i = 0; i < MaxScriptThreads; i++)
   {
      Thread& st = mScriptThread[i];
      if(st.timescale > 0.0f)
         needsReSync = bool(st.position >= st.reSync);
      else
         needsReSync = bool(st.position <= st.reSync);
      
      if(needsReSync)
         break; 
   }
   if (stream->writeFlag(mask & ThreadMask) || needsReSync) {
#endif
// end jc
      for (int i = 0; i < MaxScriptThreads; i++) {
         Thread& st = mScriptThread[i];
      // start jc
#ifndef TORQUE_RESYNC_ANIM
         if (stream->writeFlag( (st.sequence != -1 || st.state == Thread::Destroy) && (mask & (ThreadMaskN << i)) ) ) {
#else
         needsReSync = false;
         if(st.timescale > 0.0f)
            needsReSync = bool(st.position >= st.reSync);
         else
            needsReSync = bool(st.position <= st.reSync);

         if (stream->writeFlag( (st.sequence != -1 || st.state == Thread::Destroy) && ((mask & (ThreadMaskN << i)) || needsReSync) ) ) {
#endif
      // end jc
            stream->writeInt(st.sequence,ThreadSequenceBits);
            stream->writeInt(st.state,2);
            stream->write(st.timescale);
            stream->write(st.position);
            stream->writeFlag(st.atEnd);
         // start jc
#ifdef TORQUE_RESYNC_ANIM
            // update for the next sync point
            st.reSync = st.position + st.reSyncFactor;
            if(st.timescale > 0.0f)
               st.reSync = mClampF(st.position + st.reSyncFactor, 0.0f, 1.0f);
            else
               st.reSync = mClampF(st.position - st.reSyncFactor, 0.0f, 1.0f);
#endif
         // edn jc
         }
      }
   }

   if (stream->writeFlag(mask & SoundMask)) {
      for (int i = 0; i < MaxSoundThreads; i++) {
         Sound& st = mSoundThread[i];
         if (stream->writeFlag(mask & (SoundMaskN << i)))
            if (stream->writeFlag(st.play))
               stream->writeRangedU32(st.profile->getId(),DataBlockObjectIdFirst,
                                      DataBlockObjectIdLast);
      }
   }

/*
   if (stream->writeFlag(mask & ImageMask)) {
      for (int i = 0; i < MaxMountedImages; i++)
         if (stream->writeFlag(mask & (ImageMaskN << i))) {
            MountedImage& image = mMountedImageList[i];
            if (stream->writeFlag(image.dataBlock))
               stream->writeInt(image.dataBlock->getId() - DataBlockObjectIdFirst,
                                DataBlockObjectIdBitSize);
            con->packNetStringHandleU(stream, image.skinNameHandle);
            con->packNetStringHandleU(stream, image.scriptAnimPrefix);

            // Used to force the 1st person rendering on the client.  This is required
            // as this object could be ghosted to the client prior to its controlling client
            // being set.  Therefore there is a network tick when the object is in limbo...
            stream->writeFlag(image.dataBlock && image.dataBlock->animateAllShapes && getControllingClient() == con);

            stream->writeFlag(image.wet);
            stream->writeFlag(image.motion);
            stream->writeFlag(image.ammo);
            stream->writeFlag(image.loaded);
            stream->writeFlag(image.target);
            stream->writeFlag(image.triggerDown);
            stream->writeFlag(image.altTriggerDown);

            for (U32 i=0; i<ShapeBaseImageData::MaxGenericTriggers; ++i)
            {
               stream->writeFlag(image.genericTrigger[i]);
            }

            stream->writeInt(image.fireCount,3);            
            stream->writeInt(image.altFireCount,3);
            stream->writeInt(image.reloadCount,3);
            stream->writeFlag(isImageFiring(i));
            stream->writeFlag(isImageAltFiring(i));
            stream->writeFlag(isImageReloading(i));
         }
   }
*/

   // Group some of the uncommon stuff together.
// start jc
   //if (stream->writeFlag(mask & (NameMask | ShieldMask | CloakMask | InvincibleMask | SkinMask | MeshHiddenMask ))) {
   if (stream->writeFlag(mask & (NameMask | ShieldMask | CloakMask | InvincibleMask | SkinMask | MeshHiddenMask | GravityMask )))
   {
// end jc         
/*
      if (stream->writeFlag(mask & CloakMask))
      {
         // cloaking
         stream->writeFlag( mCloaked );

         // piggyback control update
*/
         stream->writeFlag(bool(getControllingClient()));
/*
         // fading
         if(stream->writeFlag(mFading && mFadeElapsedTime >= mFadeDelay)) {
            stream->writeFlag(mFadeOut);
            stream->write(mFadeTime);
         }
         else
            stream->writeFlag(mFadeVal == 1.0f);
      }
*/
      if (stream->writeFlag(mask & NameMask)) {
         con->packNetStringHandleU(stream, mShapeNameHandle);
      }
/*
      if (stream->writeFlag(mask & ShieldMask)) {
         stream->writeNormalVector(mShieldNormal, ShieldNormalBits);
         stream->writeFloat( getEnergyValue(), EnergyLevelBits );
      }
      if (stream->writeFlag(mask & InvincibleMask)) {
         stream->write(mInvincibleTime);
         stream->write(mInvincibleSpeed);
      }
*/
      if ( stream->writeFlag( mask & MeshHiddenMask ) )
         stream->writeBits( mMeshHidden );

// start jc
   //   if (stream->writeFlag(mask & SkinMask))
   //      con->packNetStringHandleU(stream, mSkinNameHandle);

      if (stream->writeFlag(mask & SkinMask))
      {
         con->packNetStringHandleU(stream, mSkinNameHandle);
      //   stream->writeFlag();
           
//         if(stream->writeFlag(bool(mWebViewData)))
//         {
//            S32 gIndex = con->getGhostIndex(mWebViewData);
//            if(stream->writeFlag(bool(gIndex != -1)))
//               stream->writeRangedU32( U32(gIndex), 0, NetConnection::MaxGhostCount );
//         }
         
         stream->writeFlag(mEnableInputEvents);
	  }
      if (stream->writeFlag(mask & GravityMask))
      {
	      stream->write(mGravityMod);
	  }
	  
// end jc
   }
	}
#endif



//   if (stream->writeFlag((mask & ImpactMask) && !(mask & InitialUpdateMask)))
//      stream->writeInt(mImpactSound, FishPlayerData::ImpactBits);

   if (stream->writeFlag(mask & ActionMask &&
         mActionAnimation.action != FishPlayerData::NullAnimation &&
         mActionAnimation.action >= FishPlayerData::NumTableActionAnims)) {
      stream->writeInt(mActionAnimation.action,FishPlayerData::ActionAnimBits);
      stream->writeFlag(mActionAnimation.holdAtEnd);
      stream->writeFlag(mActionAnimation.atEnd);
      stream->writeFlag(mActionAnimation.firstPerson);
      if (!mActionAnimation.atEnd) {
         // If somewhere in middle on initial update, must send position-
         F32   where = mShapeInstance->getPos(mActionAnimation.thread);
         if (stream->writeFlag((mask & InitialUpdateMask) != 0 && where > 0))
      // start jc
         //   stream->writeSignedFloat(where, 6);
            stream->writeSignedFloat(where, 32);
      // end jc
      }
   }

//   if (stream->writeFlag(mask & ActionMask &&
//         mArmAnimation.action != FishPlayerData::NullAnimation &&
//         (!(mask & InitialUpdateMask) ||
//         mArmAnimation.action != mDataBlock->lookAction))) {
//      stream->writeInt(mArmAnimation.action,FishPlayerData::ActionAnimBits);
//   }

   // The rest of the data is part of the control object packet update.
   // If we're controlled by this client, we don't need to send it.
   // we only need to send it if this is the initial update - in that case,
   // the client won't know this is the control object yet.
   if(stream->writeFlag(getControllingClient() == con && !(mask & InitialUpdateMask)))
      return(retMask);


   if (stream->writeFlag(mask & MoveMask))
   {
//      stream->writeFlag(mFalling);
//// start ds
//      stream->writeFlag(mSwimming);
//      stream->writeFlag(mFlying);
////   	  stream->writeInt(getPose(),5);
//      stream->writeFlag(mFrozen);
// end ds

   // start ds
	/*
      // Variable Movement -- Zshazz
      // Makes it so that everyone is synced up correctly
      stream->write(mForceMod);
      stream->write(mMaxForwardSpeedMod);
      stream->write(mMaxBackwardSpeedMod);
      stream->write(mMaxSideSpeedMod);
	*/

   stream->write(mMoveSpeed);
   // end ds

      stream->writeInt(mState,NumStateBits);
   // start jc
   //   stream->writeInt(mPose, NumPoseBits);   // Mquaker :: add net logic for mPose  
   // end jc
//      if (stream->writeFlag(mState == RecoverState))
//         stream->writeInt(mRecoverTicks,FishPlayerData::RecoverDelayBits);

      Point3F pos;
      getTransform().getColumn(3,&pos);
      stream->writeCompressedPoint(pos);

      writeVelocity(stream);
  // start jc
    //  stream->writeFloat(mRot.z / M_2PI_F, 7);
    //  stream->writeSignedFloat(mHead.x / mDataBlock->maxLookAngle, 6);
    //  stream->writeSignedFloat(mHead.z / mDataBlock->maxFreelookAngle, 6);
      stream->writeFloat(mRot.z / M_2PI_F, 12);
      stream->writeSignedFloat(mHead.x / mDataBlock->maxLookAngle, 10);
      stream->writeSignedFloat(mHead.z / mDataBlock->maxFreelookAngle, 10);
  // end jc
      delta.move.pack(stream);
      stream->writeFlag(!(mask & NoWarpMask));
   }
   // Ghost need energy to predict reliably
//   stream->writeFloat(getEnergyLevel() / mDataBlock->maxEnergy,EnergyLevelBits);
   return retMask;
}

//old
//static   const U32   speedFractionBits=5;
//static   const U32   speedIntegerBits=8;
//new
const U32   FishPlayer::sSpeedFractionBits=8;
const U32   FishPlayer::sSpeedIntegerBits=2;
const U32   FishPlayer::sSpeedTotalBits=sSpeedFractionBits + sSpeedIntegerBits;
const F32   FishPlayer::sSpeedScale=(F32)(1<<sSpeedFractionBits);
const U32   FishPlayer::sSpeedMaxSendValue=((1<<(sSpeedTotalBits)) - 1);

void FishPlayer::writeVelocity(BitStream *stream)
{
   Point3F  vel = mVelocity;
//   Point3F  vel = mTrueVelocity;

   F32 len = vel.len();
   if(stream->writeFlag(len > 0.0001f))
   {
      Point3F outVel = vel;
      outVel *= 1.0f/len;
      stream->writeNormalVector(outVel, 10);
      len *= sSpeedScale;  
      if(len > sSpeedMaxSendValue)
         len = sSpeedMaxSendValue;
      stream->writeInt((S32)len, sSpeedTotalBits);
   }
}
void FishPlayer::readVelocity(BitStream *stream)
{
   if(stream->readFlag())
   {
      stream->readNormalVector(&mVelocity, 10);
      mVelocity *= ((F32)stream->readInt(sSpeedTotalBits)) / sSpeedScale;
      if ((mActionAnimation.action >= FishPlayerData::NumTableActionAnims
            || mActionAnimation.action == FishPlayerData::LandAnim))
      {
         mActionAnimation.action = FishPlayerData::NullAnimation;
      }
   }
   else
   {
      mVelocity.set(0.0f, 0.0f, 0.0f);
   }
}
void FishPlayer::unpackUpdate(NetConnection *con, BitStream *stream)
{
#ifndef FishPlayerNetOptimize
   Parent::unpackUpdate(con,stream);
#else
   Parent::Parent::unpackUpdate(con, stream);
   mLastRenderFrame = sLastRenderFrame; // make sure we get a process after the event...

   if(stream->readFlag())
   {

//   if (stream->readFlag()) {
//      mDamage = mClampF(stream->readFloat(DamageLevelBits) * mDataBlock->maxDamage, 0.f, mDataBlock->maxDamage);
//      DamageState prevState = mDamageState;
//      mDamageState = DamageState(stream->readInt(NumDamageStateBits));
//      stream->readNormalVector( &damageDir, 8 );
//      if (prevState != Destroyed && mDamageState == Destroyed && isProperlyAdded())
//         blowUp();
//      updateDamageLevel();
//      updateDamageState();
//   }

   if (stream->readFlag()) {
      for (S32 i = 0; i < MaxScriptThreads; i++) {
         if (stream->readFlag()) {
            Thread& st = mScriptThread[i];
            U32 seq = stream->readInt(ThreadSequenceBits);
            st.state = stream->readInt(2);
			   stream->read( &st.timescale );
			   stream->read( &st.position );
            st.atEnd = stream->readFlag();
            if (st.sequence != seq && st.state != Thread::Destroy)
               setThreadSequence(i,seq,false);
            else
               updateThread(st);
         }
      }
   }

   if ( stream->readFlag() ) 
   {
      for ( S32 i = 0; i < MaxSoundThreads; i++ ) 
      {
         if ( stream->readFlag() ) 
         {
            Sound& st = mSoundThread[i];
            st.play = stream->readFlag();
            if ( st.play ) 
            {
               st.profile = (SFXTrack*) stream->readRangedU32(  DataBlockObjectIdFirst,
                                                                DataBlockObjectIdLast );
            }

            if ( isProperlyAdded() )
               updateAudioState( st );
         }
      }
   }
/*
   // Mounted Images
   if (stream->readFlag()) {
      for (int i = 0; i < MaxMountedImages; i++) {
         if (stream->readFlag()) {
            MountedImage& image = mMountedImageList[i];
            ShapeBaseImageData* imageData = 0;
            if (stream->readFlag()) {
               SimObjectId id = stream->readInt(DataBlockObjectIdBitSize) +
                  DataBlockObjectIdFirst;
               if (!Sim::findObject(id,imageData)) {
                  con->setLastError("Invalid packet (mounted images).");
                  return;
               }
            }

            NetStringHandle skinDesiredNameHandle = con->unpackNetStringHandleU(stream);

            NetStringHandle scriptDesiredAnimPrefix = con->unpackNetStringHandleU(stream);

            image.forceAnimateAllShapes = stream->readFlag();

            image.wet = stream->readFlag();

            image.motion = stream->readFlag();

            image.ammo = stream->readFlag();

            image.loaded = stream->readFlag();

            image.target = stream->readFlag();

            image.triggerDown = stream->readFlag();
            image.altTriggerDown = stream->readFlag();

            for (U32 i=0; i<ShapeBaseImageData::MaxGenericTriggers; ++i)
            {
               image.genericTrigger[i] = stream->readFlag();
            }

            int count = stream->readInt(3);
            int altCount = stream->readInt(3);
            int reloadCount = stream->readInt(3);

            bool datablockChange = image.dataBlock != imageData;
            if (datablockChange || (image.skinNameHandle != skinDesiredNameHandle))
            {
               MountedImage& image = mMountedImageList[i];
               image.scriptAnimPrefix = scriptDesiredAnimPrefix;

               setImage(   i, imageData, 
                           skinDesiredNameHandle, image.loaded, 
                           image.ammo, image.triggerDown, image.altTriggerDown,
                           image.motion, image.genericTrigger[0], image.genericTrigger[1], image.genericTrigger[2], image.genericTrigger[3],
                           image.target);
            }
            
            if (!datablockChange && image.scriptAnimPrefix != scriptDesiredAnimPrefix)
            {
               // We don't have a new image, but we do have a new script anim prefix to work with.
               // Notify the image of this change.
               MountedImage& image = mMountedImageList[i];
               image.scriptAnimPrefix = scriptDesiredAnimPrefix;
               updateAnimThread(i, getImageShapeIndex(image));
            }

            bool isFiring = stream->readFlag();
            bool isAltFiring = stream->readFlag();
            bool isReloading = stream->readFlag();

            if (isProperlyAdded()) {
               // Normal processing
               bool processFiring = false;
               if (count != image.fireCount)
               {
                  image.fireCount = count;
                  setImageState(i,getImageFireState(i),true);
                  processFiring = true;
               }
               else if (altCount != image.altFireCount)
               {
                  image.altFireCount = altCount;
                  setImageState(i,getImageAltFireState(i),true);
                  processFiring = true;
               }
               else if (reloadCount != image.reloadCount)
               {
                  image.reloadCount = reloadCount;
                  setImageState(i,getImageReloadState(i),true);
               }

               if (processFiring && imageData)
               {
                  if ( imageData->lightType == ShapeBaseImageData::WeaponFireLight )                     
                     image.lightStart = Sim::getCurrentTime();                     
                  
                  // HACK: Only works properly if you are in control
                  // of the one and only shapeBase object in the scene
                  // which fires an image that uses camera shake.
                  if ( imageData->shakeCamera )
                  {
                     if ( !mWeaponCamShake )
                     {
                        mWeaponCamShake = new CameraShake();
                        mWeaponCamShake->remoteControlled = true;
                     }

                     mWeaponCamShake->init();
                     mWeaponCamShake->setFrequency( imageData->camShakeFreq );
                     mWeaponCamShake->setAmplitude( imageData->camShakeAmp );  
                     
                     if ( !mWeaponCamShake->isAdded )
                     {
                        gCamFXMgr.addFX( mWeaponCamShake );
                        mWeaponCamShake->isAdded = true;
                     }
                  }
               }
               
               updateImageState(i,0);

               if ( !image.triggerDown && !image.altTriggerDown )
               {
                  if ( mWeaponCamShake && mWeaponCamShake->isAdded )
                  {
                     gCamFXMgr.removeFX( mWeaponCamShake );
                     mWeaponCamShake->isAdded = false;
                  }
               }
            }
            else
            {               
               if(imageData)
               {
                  // Initial state
                  image.fireCount = count;
                  image.altFireCount = altCount;
                  image.reloadCount = reloadCount;
                  if (isFiring)
                     setImageState(i,getImageFireState(i),true);
                  else if (isAltFiring)
                     setImageState(i,getImageAltFireState(i),true);
                  else if (isReloading)
                     setImageState(i,getImageReloadState(i),true);
               }
            }
         }
      }
   }
*/
   if (stream->readFlag())
   {

/*
      if(stream->readFlag())     // CloakMask and control
      {
         // Read cloaking state.
         
         setCloakedState(stream->readFlag());
*/
         mIsControlled = stream->readFlag();
/*
         if (( mFading = stream->readFlag()) == true) {
            mFadeOut = stream->readFlag();
            if(mFadeOut)
               mFadeVal = 1.0f;
            else
               mFadeVal = 0;
            stream->read(&mFadeTime);
            mFadeDelay = 0;
            mFadeElapsedTime = 0;
         }
         else
            mFadeVal = F32(stream->readFlag());
      }
*/
      if (stream->readFlag())  { // NameMask
         mShapeNameHandle = con->unpackNetStringHandleU(stream);
      }

//      if(stream->readFlag())     // ShieldMask
//      {
//         // Cloaking, Shield, and invul masking
//         Point3F shieldNormal;
//         stream->readNormalVector(&shieldNormal, ShieldNormalBits);
//         
//         // CodeReview [bjg 4/6/07] This is our energy level - why aren't we storing it? Was in a
//         // local variable called energyPercent.
//         stream->readFloat(EnergyLevelBits);
//      }
//
//      if (stream->readFlag()) 
//      {
//         // InvincibleMask
//         F32 time, speed;
//         stream->read(&time);
//         stream->read(&speed);
//         setupInvincibleEffect(time, speed);
//      }
     
      if ( stream->readFlag() ) // MeshHiddenMask
      {
         stream->readBits( &mMeshHidden );
         _updateHiddenMeshes();
      }

      if (stream->readFlag())    // SkinMask
      {
         NetStringHandle skinDesiredNameHandle = con->unpackNetStringHandleU(stream);;
         if (mSkinNameHandle != skinDesiredNameHandle)
         {
            mSkinNameHandle = skinDesiredNameHandle;
         // start jc
         //   reSkin();

            reSkinNewPath();
         // end jc
         }
      // start jc
//         if( stream->readFlag() )
//         {
//            if(stream->readFlag())
//            {
//               mWebViewID = stream->readRangedU32( 0, NetConnection::MaxGhostCount );
//               webSkin();
//            }
//         }
         mEnableInputEvents = stream->readFlag();

     // end jc

      }
  // start jc
	if (stream->readFlag())
	{
		stream->read(&mGravityMod);
	}
  // end jc

   }
	}
#endif


//   if (stream->readFlag())
//      mImpactSound = stream->readInt(FishPlayerData::ImpactBits);

   // Server specified action animation
   if (stream->readFlag()) {
      U32 action = stream->readInt(FishPlayerData::ActionAnimBits);
      bool hold = stream->readFlag();
      bool atEnd = stream->readFlag();
      bool fsp = stream->readFlag();

      F32   animPos = -1.0f;
      if (!atEnd && stream->readFlag())
      // start jc
      //   animPos = stream->readSignedFloat(6);
         animPos = stream->readSignedFloat(32);
      // end jc

      if (isProperlyAdded()) {
// start ds
/*
         setActionThread(action,true,hold,true,fsp);
*/
         setActionThread(action,true,hold,true,fsp,true);
// end ds
         bool  inDeath = inDeathAnim();
         if (atEnd)
         {
            mShapeInstance->clearTransition(mActionAnimation.thread);
            mShapeInstance->setPos(mActionAnimation.thread,
                                   mActionAnimation.forward? 1: 0);
            if (inDeath)
               mDeath.lastPos = 1.0f;
         }
         else if (animPos > 0) {
            mShapeInstance->setPos(mActionAnimation.thread, animPos);
            if (inDeath)
               mDeath.lastPos = animPos;
         }

         // mMountPending suppresses tickDelay countdown so players will sit until
         // their mount, or another animation, comes through (or 13 seconds elapses).
         mMountPending = (S32) (inSittingAnim() ? sMountPendingTickWait : 0);
      }
      else {
         mActionAnimation.action = action;
         mActionAnimation.holdAtEnd = hold;
         mActionAnimation.atEnd = atEnd;
         mActionAnimation.firstPerson = fsp;
      }
   }

   // Server specified arm animation
//   if (stream->readFlag()) {
//      U32 action = stream->readInt(FishPlayerData::ActionAnimBits);
//      if (isProperlyAdded())
//         setArmThread(action);
//      else
//         mArmAnimation.action = action;
//   }

   // Done if controlled by client ( and not initial update )
   if(stream->readFlag())
      return;

   // MoveMask
   if (stream->readFlag()) {
      mPredictionCount = sMaxPredictionTicks;
//      mFalling = stream->readFlag();
//// start ds
//	  mSwimming = stream->readFlag(); // fafhrd, swim code
//	  mFlying = stream->readFlag();
////     setPose((FishPlayer::Pose)stream->readInt(5));
//// end ds
//// start ds
//		mFrozen = stream->readFlag();
//// end ds
// start ds
   // Variable Movement -- Zshazz
      /*
		stream->read(&mRunForceMod);
		stream->read(&mMaxForwardSpeedMod);
		stream->read(&mMaxBackwardSpeedMod);
		stream->read(&mMaxWalkForwardSpeedMod);
		stream->read(&mMaxWalkBackwardSpeedMod);
		stream->read(&mMaxSideSpeedMod);
      */
   stream->read(&mMoveSpeed);

// end ds

      stream->readInt(NumStateBits);
   // start jc - Mquaker :: add net logic for mPose >>>
   //   Pose pose = (Pose)stream->readInt(NumPoseBits);
   //   setPose(pose);
   // end jc - Mquaker :: add net logic for mPose <<<
//      if (stream->readFlag()) {
//         mRecoverTicks = stream->readInt(FishPlayerData::RecoverDelayBits);
//         setState(actionState, mRecoverTicks);
//      }
//      else
//         setState(actionState);

      Point3F pos,rot;
      stream->readCompressedPoint(&pos);

      F32 speed = mVelocity.len();  //this is the speed before latest update
      readVelocity(stream);
      
      rot.y = rot.x = 0.0f;
  // start jc
  //    rot.z = stream->readFloat(7) * M_2PI_F;
  //    mHead.x = stream->readSignedFloat(6) * mDataBlock->maxLookAngle;
  //    mHead.z = stream->readSignedFloat(6) * mDataBlock->maxFreelookAngle;
      rot.z = stream->readFloat(12) * M_2PI_F;
      mHead.x = stream->readSignedFloat(10) * mDataBlock->maxLookAngle;
      if(mDataBlock->noTilt){
         mHead.x = 0;   //pghack
      }
      mHead.z = stream->readSignedFloat(10) * mDataBlock->maxFreelookAngle;
  // end jc
      delta.move.unpack(stream);

      delta.head = mHead;
      delta.headVec.set(0.0f, 0.0f, 0.0f);


      if (stream->readFlag() && isProperlyAdded())
      {
         // Determine number of ticks to warp based on the average
         // of the client and server velocities.
         delta.warpOffset = pos - delta.pos;
         F32 as = (speed + mVelocity.len()) * 0.5f * TickSec;
         F32 dt = (as > 0.00001f) ? delta.warpOffset.len() / as: sMaxWarpTicks;
         delta.warpTicks = (S32)((dt > sMinWarpTicks) ? getMax(mFloor(dt + 0.5f), 1.0f) : 0.0f);

         
         if (delta.warpTicks)
         {
            // Setup the warp to start on the next tick.
            if (delta.warpTicks > sMaxWarpTicks)
               delta.warpTicks = sMaxWarpTicks;
            delta.warpOffset /= (F32)delta.warpTicks;

            delta.rotOffset = rot - delta.rot;
		// start jc - http://www.garagegames.com/community/forums/viewthread/123597
            delta.rotOffset.x = makePiRange(delta.rotOffset.x);
            delta.rotOffset.y = makePiRange(delta.rotOffset.y);
            delta.rotOffset.z = makePiRange(delta.rotOffset.z);
		// end jc
            delta.rotOffset /= (F32)delta.warpTicks;
         }
         else
         {
            // Going to skip the warp, server and client are real close.
            // Adjust the frame interpolation to move smoothly to the
            // new position within the current tick.
            Point3F cp = delta.pos + delta.posVec * delta.dt;
            if (delta.dt == 0) 
            {
               delta.posVec.set(0.0f, 0.0f, 0.0f);
               delta.rotVec.set(0.0f, 0.0f, 0.0f);
            }
            else
            {
               F32 dti = 1.0f / delta.dt;
               delta.posVec = (cp - pos) * dti;
		    // start jc
               delta.rotVec = mRot - rot;

               delta.rotVec.x = makePiRange(delta.rotVec.x);
               delta.rotVec.y = makePiRange(delta.rotVec.y);
               delta.rotVec.z = makePiRange(delta.rotVec.z);

               delta.rotVec *= dti;
		    // end jc
            }
            delta.pos = pos;
            delta.rot = rot;
            setPosition(pos,rot);
         }
      }
      else 
      {
         // Set the player to the server position
         delta.pos = pos;
         delta.rot = rot;
         delta.posVec.set(0.0f, 0.0f, 0.0f);
         delta.rotVec.set(0.0f, 0.0f, 0.0f);
         delta.warpTicks = 0;
         delta.dt = 0.0f;
         setPosition(pos,rot);
      }
   }
}


//----------------------------------------------------------------------------
DefineEngineMethod( FishPlayer, getPose, const char*, (),,
   "@brief Get the name of the player's current pose.\n\n"

   "The pose is one of the following:\n\n<ul>"
   "<li>Stand - Standard movement pose.</li>"
   "<li>Sprint - Sprinting pose.</li>"
   "<li>Crouch - Crouch pose.</li>"
   "<li>Prone - Prone pose.</li>"
   "<li>Swim - Swimming pose.</li></ul>\n"

   "@return The current pose; one of: \"Stand\", \"Sprint\", \"Crouch\", \"Prone\", \"Swim\"\n" )
{
   return object->getPoseName();
}

//----------------------------------------------------------------------------

DefineEngineMethod( FishPlayer, getState, const char*, (),,
   "@brief Get the name of the player's current state.\n\n"

   "The state is one of the following:\n\n<ul>"
   "<li>Dead - The FishPlayer is dead.</li>"
   "<li>Mounted - The FishPlayer is mounted to an object such as a vehicle.</li>"
   "<li>Move - The FishPlayer is free to move.  The usual state.</li>"
   "<li>Recover - The FishPlayer is recovering from a fall.  See FishPlayerData::recoverDelay.</li></ul>\n"

   "@return The current state; one of: \"Dead\", \"Mounted\", \"Move\", \"Recover\"\n" )
{
   return object->getStateName();
}

DefineEngineMethod( FishPlayer, getDamageLocation, const char*, ( Point3F pos ),,
   "@brief Get the named damage location and modifier for a given world position.\n\n"

   "the FishPlayer object can simulate different hit locations based on a pre-defined set "
   "of FishPlayerData defined percentages.  These hit percentages divide up the FishPlayer's "
   "bounding box into different regions.  The diagram below demonstrates how the various "
   "FishPlayerData properties split up the bounding volume:\n\n"

   "<img src=\"images/player_damageloc.png\">\n\n"

   "While you may pass in any world position and getDamageLocation() will provide a best-fit "
   "location, you should be aware that this can produce some interesting results.  For example, "
   "any position that is above FishPlayerData::boxHeadPercentage will be considered a 'head' hit, even "
   "if the world position is high in the sky.  Therefore it may be wise to keep the passed in point "
   "to somewhere on the surface of, or within, the FishPlayer's bounding volume.\n\n"

   "@note This method will not return an accurate location when the player is "
   "prone or swimming.\n\n"

   "@param pos A world position for which to retrieve a body region on this player.\n"

   "@return a string containing two words (space separated strings), where the "
   "first is a location and the second is a modifier.\n\n"

   "Posible locations:<ul>"
   "<li>head</li>"
   "<li>torso</li>"
   "<li>legs</li></ul>\n"

   "Head modifiers:<ul>"
   "<li>left_back</li>"
   "<li>middle_back</li>"
   "<li>right_back</li>"
   "<li>left_middle</li>"
   "<li>middle_middle</li>"
   "<li>right_middle</li>"
   "<li>left_front</li>"
   "<li>middle_front</li>"
   "<li>right_front</li></ul>\n"

   "Legs/Torso modifiers:<ul>"
   "<li>front_left</li>"
   "<li>front_right</li>"
   "<li>back_left</li>"
   "<li>back_right</li></ul>\n"

   "@see FishPlayerData::boxHeadPercentage\n"
   "@see FishPlayerData::boxHeadFrontPercentage\n"
   "@see FishPlayerData::boxHeadBackPercentage\n"
   "@see FishPlayerData::boxHeadLeftPercentage\n"
   "@see FishPlayerData::boxHeadRightPercentage\n"
   "@see FishPlayerData::boxTorsoPercentage\n"
   )
{
   const char *buffer1;
   const char *buffer2;

   object->getDamageLocation(pos, buffer1, buffer2);

   char *buff = Con::getReturnBuffer(128);
   dSprintf(buff, 128, "%s %s", buffer1, buffer2);
   return buff;
}

DefineEngineMethod( FishPlayer, setArmThread, bool, ( const char* name ),,
   "@brief Set the sequence that controls the player's arms (dynamically adjusted "
   "to match look direction).\n\n"
   "@param name Name of the sequence to play on the player's arms.\n"
   "@return true if successful, false if failed.\n"
   "@note By default the 'look' sequence is used, if available.\n")
{
   return object->setArmThread( name );
}

//----------------------------------------------------------------------------
DefineEngineMethod( FishPlayer, setTurnRateMultiplier, void, (F32 multiplier),,
   "@brief Set max rotation speed multiplier. This multiplies the maxTurnRate in the fishes data block"
   "to get the current turn rate\n\n" )
{
   object->setTurnRateMultiplier(multiplier);
}
//----------------------------------------------------------------------------
DefineEngineMethod( FishPlayer, getTurnRateMultiplier, F32, (),,
   "@brief Get max rotation speed multiplier. This multiplies the maxTurnRate in the fishes data block"
   "to get the current turn rate\n\n" )
{
   return object->getTurnRateMultiplier();
}
//----------------------------------------------------------------------------
DefineEngineMethod( FishPlayer, setForwardOnly, void, (bool a),,
   "@brief Set max rotation speed multiplier. This multiplies the maxTurnRate in the fishes data block"
   "to get the current turn rate\n\n" )
{
   object->setForwardOnly(a);
}
//----------------------------------------------------------------------------
DefineEngineMethod( FishPlayer, getForwardOnly, bool, (),,
   "@brief Get max rotation speed multiplier. This multiplies the maxTurnRate in the fishes data block"
   "to get the current turn rate\n\n" )
{
   return object->getForwardOnly();
}



DefineEngineMethod( FishPlayer, setActionThread, bool, ( const char* name, bool hold, bool fsp ), ( false, true ),
   "@brief Set the main action sequence to play for this player.\n\n"
   "@param name Name of the action sequence to set\n"
   "@param hold Set to false to get a callback on the datablock when the sequence ends (FishPlayerData::animationDone()).  "
   "When set to true no callback is made.\n"
   "@param fsp True if first person and none of the spine nodes in the shape should animate.  False will allow the shape's "
   "spine nodes to animate.\n"
   "@return True if succesful, false if failed\n"
   
   "@note The spine nodes for the FishPlayer's shape are named as follows:\n\n<ul>"
   "<li>Bip01 Pelvis</li>"
   "<li>Bip01 Spine</li>"
   "<li>Bip01 Spine1</li>"
   "<li>Bip01 Spine2</li>"
   "<li>Bip01 Neck</li>"
   "<li>Bip01 Head</li></ul>\n\n"
   
   "You cannot use setActionThread() to have the FishPlayer play one of the motion "
   "determined action animation sequences.  These sequences are chosen based on how "
   "the FishPlayer moves and the FishPlayer's current pose.  The names of these sequences are:\n\n<ul>"
   "<li>root</li>"
   "<li>run</li>"
   "<li>side</li>"
   "<li>side_right</li>"
   "<li>crouch_root</li>"
   "<li>crouch_forward</li>"
   "<li>crouch_backward</li>"
   "<li>crouch_side</li>"
   "<li>crouch_right</li>"
   "<li>prone_root</li>"
   "<li>prone_forward</li>"
   "<li>prone_backward</li>"
   "<li>swim_root</li>"
   "<li>swim_forward</li>"
   "<li>swim_backward</li>"
   "<li>swim_left</li>"
   "<li>swim_right</li>"
   "<li>fall</li>"
   "<li>jump</li>"
   "<li>standjump</li>"
   "<li>land</li>"
   "<li>jet</li></ul>\n\n"
   
   "If the player moves in any direction then the animation sequence set using this "
   "method will be cancelled and the chosen mation-based sequence will take over.  This makes "
   "great for times when the FishPlayer cannot move, such as when mounted, or when it doesn't matter "
   "if the action sequence changes, such as waving and saluting.\n"
   
   "@tsexample\n"
      "// Place the player in a sitting position after being mounted\n"
      "%player.setActionThread( \"sitting\", true, true );\n"
	"@endtsexample\n")
{
   return object->setActionThread( name, hold, true, fsp);
}

DefineEngineMethod( FishPlayer, setControlObject, bool, ( ShapeBase* obj ),,
   "@brief Set the object to be controlled by this player\n\n"

   "It is possible to have the moves sent to the FishPlayer object from the "
   "GameConnection to be passed along to another object.  This happens, for example "
   "when a player is mounted to a vehicle.  The move commands pass through the FishPlayer "
   "and on to the vehicle (while the player remains stationary within the vehicle).  "
   "With setControlObject() you can have the FishPlayer pass along its moves to any object.  "
   "One possible use is for a player to move a remote controlled vehicle.  In this case "
   "the player does not mount the vehicle directly, but still wants to be able to control it.\n"

   "@param obj Object to control with this player\n"
   "@return True if the object is valid, false if not\n"

   "@see getControlObject()\n"
   "@see clearControlObject()\n"
   "@see GameConnection::setControlObject()")
{
   if (obj) {
      object->setControlObject(obj);
      return true;
   }
   else
      object->setControlObject(0);
   return false;
}

DefineEngineMethod( FishPlayer, getControlObject, S32, (),,
   "@brief Get the current object we are controlling.\n\n"
   "@return ID of the ShapeBase object we control, or 0 if not controlling an "
   "object.\n"
   "@see setControlObject()\n"
   "@see clearControlObject()")
{
   ShapeBase* controlObject = object->getControlObject();
   return controlObject ? controlObject->getId(): 0;
}

DefineEngineMethod( FishPlayer, clearControlObject, void, (),,
   "@brief Clears the player's current control object.\n\n"
   "Returns control to the player. This internally calls "
   "FishPlayer::setControlObject(0).\n"
   "@tsexample\n"
		"%player.clearControlObject();\n"
      "echo(%player.getControlObject()); //<-- Returns 0, player assumes control\n"
      "%player.setControlObject(%vehicle);\n"
      "echo(%player.getControlObject()); //<-- Returns %vehicle, player controls the vehicle now.\n"
	"@endtsexample\n"
   "@note If the player does not have a control object, the player will receive all moves "
   "from its GameConnection.  If you're looking to remove control from the player itself "
   "(i.e. stop sending moves to the player) use GameConnection::setControlObject() to transfer "
   "control to another object, such as a camera.\n"
   "@see setControlObject()\n"
   "@see getControlObject()\n"
   "@see GameConnection::setControlObject()\n")
{
   object->setControlObject(0);
}

DefineEngineMethod( FishPlayer, checkDismountPoint, bool, ( Point3F oldPos, Point3F pos ),,
   "@brief Check if it is safe to dismount at this position.\n\n"

   "Internally this method casts a ray from oldPos to pos to determine if it hits the "
   "terrain, an interior object, a water object, another player, a static shape, "
   "a vehicle (exluding the one currently mounted), or physical zone.  If this ray "
   "is in the clear, then the player's bounding box is also checked for a collision at "
   "the pos position.  If this displaced bounding box is also in the clear, then "
   "checkDismountPoint() returns true.\n"

   "@param oldPos The player's current position\n"
   "@param pos The dismount position to check\n"
   "@return True if the dismount position is clear, false if not\n"
   
   "@note The player must be already mounted for this method to not assert.\n")
{
   MatrixF oldPosMat(true);
   oldPosMat.setColumn(3, oldPos);
   MatrixF posMat(true);
   posMat.setColumn(3, pos);
   return object->checkDismountPosition(oldPosMat, posMat);
}

// start ds
DefineEngineMethod( FishPlayer, setFrozen, void, (bool frozen),, "setFrozen(frozen)" )
{
   object->setFrozen(frozen);
}
// end ds
DefineEngineMethod( FishPlayer, getNumDeathAnimations, S32, ( ),,
   "@brief Get the number of death animations available to this player.\n\n"
   "Death animations are assumed to be named death1-N using consecutive indices." )
{
   S32 count = 0;
   const FishPlayerData * db = dynamic_cast<FishPlayerData*>( object->getDataBlock() );
   if ( db )
   {

      for ( S32 i = 0; i < db->actionCount; i++ )
         if ( db->actionList[i].death )
            count++;
   }
   return count;
}

//----------------------------------------------------------------------------
void FishPlayer::consoleInit()
{
   Con::addVariable("$player::renderMyPlayer",TypeBool, &sRenderMyPlayer,
      "@brief Determines if the player is rendered or not.\n\n"
      "Used on the client side to disable the rendering of all FishPlayer objects.  This is "
      "mainly for the tools or debugging.\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::renderMyItems",TypeBool, &sRenderMyItems,
      "@brief Determines if mounted shapes are rendered or not.\n\n"
      "Used on the client side to disable the rendering of all FishPlayer mounted objects.  This is "
      "mainly used for the tools or debugging.\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::renderCollision", TypeBool, &sRenderPlayerCollision, 
      "@brief Determines if the player's collision mesh should be rendered.\n\n"
      "This is mainly used for the tools and debugging.\n"
	   "@ingroup GameObjects\n");

   Con::addVariable("$player::minWarpTicks",TypeF32,&sMinWarpTicks, 
      "@brief Fraction of tick at which instant warp occures on the client.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::maxWarpTicks",TypeS32,&sMaxWarpTicks, 
      "@brief When a warp needs to occur due to the client being too far off from the server, this is the "
      "maximum number of ticks we'll allow the client to warp to catch up.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::maxPredictionTicks",TypeS32,&sMaxPredictionTicks, 
      "@brief Maximum number of ticks to predict on the client from the last known move obtained from the server.\n\n"
	   "@ingroup GameObjects\n");

   Con::addVariable("$player::maxImpulseVelocity", TypeF32, &sMaxImpulseVelocity, 
      "@brief The maximum velocity allowed due to a single impulse.\n\n"
	   "@ingroup GameObjects\n");

   // Move triggers
   Con::addVariable("$player::jumpTrigger", TypeS32, &sJumpTrigger, 
      "@brief The move trigger index used for player jumping.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::crouchTrigger", TypeS32, &sCrouchTrigger, 
      "@brief The move trigger index used for player crouching.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::proneTrigger", TypeS32, &sProneTrigger, 
      "@brief The move trigger index used for player prone pose.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::sprintTrigger", TypeS32, &sSprintTrigger, 
      "@brief The move trigger index used for player sprinting.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::imageTrigger0", TypeS32, &sImageTrigger0, 
      "@brief The move trigger index used to trigger mounted image 0.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::imageTrigger1", TypeS32, &sImageTrigger1, 
      "@brief The move trigger index used to trigger mounted image 1 or alternate fire "
      "on mounted image 0.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::jumpJetTrigger", TypeS32, &sJumpJetTrigger, 
      "@brief The move trigger index used for player jump jetting.\n\n"
	   "@ingroup GameObjects\n");
   Con::addVariable("$player::vehicleDismountTrigger", TypeS32, &sVehicleDismountTrigger, 
      "@brief The move trigger index used to dismount player.\n\n"
	   "@ingroup GameObjects\n");
}

//--------------------------------------------------------------------------
void FishPlayer::calcClassRenderData()
{
   Parent::calcClassRenderData();

}

//-----------------------------------------------------------------------------

void FishPlayer::playFootstepSound( bool triggeredLeft, Material* contactMaterial, SceneObject* contactObject )
{
   MatrixF footMat = getTransform();
   if( mWaterCoverage > 0.0 )
   {
      // Treading water.

      if ( mWaterCoverage < mDataBlock->footSplashHeight )
         SFX->playOnce( mDataBlock->sound[ FishPlayerData::FootShallowSplash ], &footMat );
      else
      {
         if ( mWaterCoverage < 1.0 )
            SFX->playOnce( mDataBlock->sound[ FishPlayerData::FootWading ], &footMat );
         else
         {
            if ( triggeredLeft )
            {
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::FootUnderWater ], &footMat );
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::FootBubbles ], &footMat );
            }
         }
      }
   }
   else if( contactMaterial && contactMaterial->mFootstepSoundCustom )
   {
      // Footstep sound defined on material.

      SFX->playOnce( contactMaterial->mFootstepSoundCustom, &footMat );
   }
   else
   {
      // Play default sound.

      S32 sound = -1;
      if( contactMaterial && contactMaterial->mFootstepSoundId != -1 )
         sound = contactMaterial->mFootstepSoundId;
      else if( contactObject && contactObject->getTypeMask() & VehicleObjectType )
         sound = 2;

      switch ( sound )
      {
      case 0: // Soft
         SFX->playOnce( mDataBlock->sound[FishPlayerData::FootSoft], &footMat );
         break;
      case 1: // Hard
         SFX->playOnce( mDataBlock->sound[FishPlayerData::FootHard], &footMat );
         break;
      case 2: // Metal
         SFX->playOnce( mDataBlock->sound[FishPlayerData::FootMetal], &footMat );
         break;
      case 3: // Snow
         SFX->playOnce( mDataBlock->sound[FishPlayerData::FootSnow], &footMat );
         break;
      /*
      default: //Hard
         SFX->playOnce( mDataBlock->sound[FishPlayerData::FootHard], &footMat );
         break;
      */
      }
   }
}

void FishPlayer:: playImpactSound()
{
   if( mWaterCoverage == 0.0f )
   {
      Point3F pos;
      RayInfo rInfo;
      MatrixF mat = getTransform();
      mat.mulP(Point3F(mDataBlock->decalOffset,0.0f,0.0f), &pos);

      if( gClientContainer.castRay( Point3F( pos.x, pos.y, pos.z + 0.01f ),
                                    Point3F( pos.x, pos.y, pos.z - 2.0f ),
                                    STATIC_COLLISION_TYPEMASK | VehicleObjectType,
                                    &rInfo ) )
      {
         Material* material = ( rInfo.material ? dynamic_cast< Material* >( rInfo.material->getMaterial() ) : 0 );

         if( material && material->mImpactSoundCustom )
            SFX->playOnce( material->mImpactSoundCustom, &getTransform() );
         else
         {
            S32 sound = -1;
            if( material && material->mImpactSoundId )
               sound = material->mImpactSoundId;
            else if( rInfo.object->getTypeMask() & VehicleObjectType )
               sound = 2; // Play metal;

            switch( sound )
            {
            case 0:
               //Soft
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::ImpactSoft ], &getTransform() );
               break;
            case 1:
               //Hard
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::ImpactHard ], &getTransform() );
               break;
            case 2:
               //Metal
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::ImpactMetal ], &getTransform() );
               break;
            case 3:
               //Snow
               SFX->playOnce( mDataBlock->sound[ FishPlayerData::ImpactSnow ], &getTransform() );
               break;
               /*
            default:
               //Hard
               alxPlay(mDataBlock->sound[FishPlayerData::ImpactHard], &getTransform());
               break;
               */
            }
         }
      }
   }

   mImpactSound = 0;
}
// fix angle to range 0->2*pi
float FishPlayer::make2PiRange(float a)
{
   if(a>=0){
      return fmodf(a,M_2PI_F);
   }else{
      return M_2PI_F - fmodf(-a,M_2PI_F);
   }
}
// fix angle to range -pi -> +pi
float FishPlayer::makePiRange(float a)
{
   a = make2PiRange(a);
   return a>M_PI? -(M_2PI_F - a) : a;
}

//--------------------------------------------------------------------------
// Update splash
//--------------------------------------------------------------------------

void FishPlayer::updateSplash()
{
   F32 speed = getVelocity().len();
   if( speed < mDataBlock->splashVelocity || isMounted() ) return;

   Point3F curPos = getPosition();

   if ( curPos.equal( mLastPos ) )
      return;

   if (pointInWater( curPos )) {
      if (!pointInWater( mLastPos )) {
         Point3F norm = getVelocity();
         norm.normalize();

         // make sure player is moving vertically at good pace before playing splash
         F32 splashAng = mDataBlock->splashAngle / 360.0;
         if( mDot( norm, Point3F(0.0, 0.0, -1.0) ) < splashAng )
            return;


         RayInfo rInfo;
         if (gClientContainer.castRay(mLastPos, curPos,
               WaterObjectType, &rInfo)) {
            createSplash( rInfo.point, speed );
            mBubbleEmitterTime = 0.0;
         }

      }
   }
}


//--------------------------------------------------------------------------

void FishPlayer::updateFroth( F32 dt )
{
   // update bubbles
   Point3F moveDir = getVelocity();
   mBubbleEmitterTime += dt;

   if (mBubbleEmitterTime < mDataBlock->bubbleEmitTime) {
      if (mSplashEmitter[FishPlayerData::BUBBLE_EMITTER]) {
         Point3F emissionPoint = getRenderPosition();
         U32 emitNum = FishPlayerData::BUBBLE_EMITTER;
         mSplashEmitter[emitNum]->emitParticles(mLastPos, emissionPoint,
            Point3F( 0.0, 0.0, 1.0 ), moveDir, (U32)(dt * 1000.0));
      }
   }

   Point3F contactPoint;
   if (!collidingWithWater(contactPoint)) {
      mLastWaterPos = mLastPos;
      return;
   }

   F32 speed = moveDir.len();
   if ( speed < mDataBlock->splashVelEpsilon ) 
      speed = 0.0;

   U32 emitRate = (U32) (speed * mDataBlock->splashFreqMod * dt);

   // If we're in the water, swimming, but not
   // moving, then lets emit some particles because
   // we're treading water.  
   if ( mSwimming && speed == 0.0 )
   {
      emitRate = (U32) (2.0f * mDataBlock->splashFreqMod * dt);
   }   

   U32 i;
   for ( i=0; i<FishPlayerData::BUBBLE_EMITTER; i++ ) {
      if (mSplashEmitter[i] )
         mSplashEmitter[i]->emitParticles( mLastWaterPos,
            contactPoint, Point3F( 0.0, 0.0, 1.0 ),
            moveDir, emitRate );
   }
   mLastWaterPos = contactPoint;
}

void FishPlayer::updateWaterSounds(F32 dt)
{
   if ( mWaterCoverage < 1.0f || mDamageState != Enabled )
   {
      // Stop everything
      if ( mMoveBubbleSound )
         mMoveBubbleSound->stop();
      if ( mWaterBreathSound )
         mWaterBreathSound->stop();
      return;
   }

   if ( mMoveBubbleSound )
   {
      // We're under water and still alive, so let's play something
      if ( mVelocity.len() > 1.0f )
      {
         if ( !mMoveBubbleSound->isPlaying() )
            mMoveBubbleSound->play();

         mMoveBubbleSound->setTransform( getTransform() );
      }
      else
         mMoveBubbleSound->stop();
   }

   if ( mWaterBreathSound )
   {
      if ( !mWaterBreathSound->isPlaying() )
         mWaterBreathSound->play();

      mWaterBreathSound->setTransform( getTransform() );
   }
}


//--------------------------------------------------------------------------
// Returns true if player is intersecting a water surface
//--------------------------------------------------------------------------
bool FishPlayer::collidingWithWater( Point3F &waterHeight )
{
   if ( !mCurrentWaterObject )
      return false;
   
   Point3F curPos = getPosition();

   if ( mWorldBox.maxExtents.z < mLiquidHeight )
      return false;

   curPos.z = mLiquidHeight;

   waterHeight = getPosition();
   waterHeight.z = mLiquidHeight;

   return true;
}

//--------------------------------------------------------------------------

void FishPlayer::createSplash( Point3F &pos, F32 speed )
{
   if ( speed >= mDataBlock->hardSplashSoundVel )
      SFX->playOnce( mDataBlock->sound[FishPlayerData::ImpactWaterHard], &getTransform() );
   else if ( speed >= mDataBlock->medSplashSoundVel )
      SFX->playOnce( mDataBlock->sound[FishPlayerData::ImpactWaterMedium], &getTransform() );
   else
      SFX->playOnce( mDataBlock->sound[FishPlayerData::ImpactWaterEasy], &getTransform() );

   if( mDataBlock->splash )
   {
      MatrixF trans = getTransform();
      trans.setPosition( pos );
      Splash *splash = new Splash;
      splash->onNewDataBlock( mDataBlock->splash, false );
      splash->setTransform( trans );
      splash->setInitialState( trans.getPosition(), Point3F( 0.0, 0.0, 1.0 ) );
      if (!splash->registerObject())
         delete splash;
   }
}


bool FishPlayer::isControlObject()
{
   GameConnection* connection = GameConnection::getConnectionToServer();
   if( !connection ) return false;
   ShapeBase *obj = dynamic_cast<ShapeBase*>(connection->getControlObject());
   return ( obj == this );
}


void FishPlayer::prepRenderImage( SceneRenderState* state )
{
   bool renderPlayer = true;
   bool renderItems = true;


   /*
   if ( mPhysicsRep && Con::getBoolVariable("$PhysicsPlayer::DebugRender",false) )
   {
      ObjectRenderInst *ri = state->getRenderPass()->allocInst<ObjectRenderInst>();
      ri->renderDelegate.bind( mPhysicsRep, &PhysicsPlayer::renderDebug );
      ri->objectIndex = -1;
      ri->type = RenderPassManager::RIT_Editor;
      state->getRenderPass()->addInst( ri );
   }
   */

   // Debug rendering for all convexes in the Players working list.
   ObjectRenderInst *ri = state->getRenderPass()->allocInst<ObjectRenderInst>();
   if ( sRenderPlayerCollision )
   {
      ri->renderDelegate.bind( this, &FishPlayer::renderConvex );
      ri->objectIndex = -1;
      ri->type = RenderPassManager::RIT_Editor;
      state->getRenderPass()->addInst( ri );
   }

   GameConnection* connection = GameConnection::getConnectionToServer();
   if( connection && connection->getControlObject() == this && connection->isFirstPerson() )
   {
      renderPlayer = mDataBlock->renderFirstPerson || !state->isDiffusePass();

      if( !sRenderMyPlayer )
         renderPlayer = false;
      if( !sRenderMyItems )
         renderItems = false;
   }

   // Call the protected base class to do the work 
   // now that we know if we're rendering the player
   // and mounted shapes.
   return ShapeBase::_prepRenderImage( state, 
                                       renderPlayer, 
                                       renderItems );
}

void FishPlayer::renderConvex( ObjectRenderInst *ri, SceneRenderState *state, BaseMatInstance *overrideMat )
{
   GFX->enterDebugEvent( ColorI(255,0,255), "Player_renderConvex" );
   mConvex.renderWorkingList();
   GFX->leaveDebugEvent();
}


#ifdef   NEW_COLLIDE
void  FishPlayer::makeConvex()
{
   if(mConvexMade){
      return;
   }
   mConvexMade = true;


   for(int hullId=0; hullId<mDataBlock->collisionBounds.size(); hullId++){
      ShapeBaseConvex   *c;
      if(hullId ==0){
         c = &mConvex;
      }else{
         c = new ShapeBaseConvex();
      }
      c->mObject    = this;
      c->pShapeBase = this;
      c->hullId     = hullId;
      c->box        = mDataBlock->collisionBounds[hullId];
      c->transform = 0;
      c->findNodeTransform();
      if(hullId!=0){
         mConvex.registerObject(c);
      }
   }

   OptimizedPolyList  pl;
   mConvex.getPolyList(&pl);

   //big time hack
   for(int j=0; j<pl.mPolyList.size(); j++){
      OptimizedPolyList::Poly& poly = pl.mPolyList[ j ];
      poly.type = OptimizedPolyList::TriangleList;   //hack!!!!
   }
   mPolyHedron = pl.toPolyhedron();
   mPolyHedron2 = pl.toPolyhedron();

   mBox.minExtents = mPolyHedron.pointList[0];
   mBox.maxExtents = mPolyHedron.pointList[0];
   for(int j=0; j<mPolyHedron.pointList.size(); j++){
      Point3F  &v = mPolyHedron.pointList[j];
      mBox.minExtents.x = getMin(v.x, mBox.minExtents.x);
      mBox.minExtents.y = getMin(v.y, mBox.minExtents.y);
      mBox.minExtents.z = getMin(v.z, mBox.minExtents.z);

      mBox.maxExtents.x = getMax(v.x, mBox.maxExtents.x);
      mBox.maxExtents.y = getMax(v.y, mBox.maxExtents.y);
      mBox.maxExtents.z = getMax(v.z, mBox.maxExtents.z);
   }
}
#endif

//*************************************************************
const static int  UPDATE_FREQUENCY = 4;
void  FishPlayer::updateTrueVelocity()
{
   if(!mLastPositionExists){
      //no initted yet
      mLastPositionExists = true;
      mLastPosition = getPosition(); 
      mTrueVelocity = Point3F::Zero;
   }else if( !(mLastPositionCalls%UPDATE_FREQUENCY) ){
      Point3F  dp = getPosition() - mLastPosition;
      F32      interval = ((F32)UPDATE_FREQUENCY)*TickSec;
      mTrueVelocity = dp*(1.0f/interval);
      mLastPosition = getPosition();      
   }
   mLastPositionCalls = (mLastPositionCalls+1)%UPDATE_FREQUENCY;
}
