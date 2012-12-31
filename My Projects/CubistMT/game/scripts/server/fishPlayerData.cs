
datablock FishPlayerData(DefaultFishPlayerData)
{
   fishTypeFlags = $FishFlags::Medium;
   menu = 0;
   
   
   bubbleDataBlock = BubbleFishData;

   //schooling related parameters
   schoolWanderSizeSideLo = "0.0";         //range for random variation of heading
   schoolWanderSizeSideHi = "0.15";
   schoolWanderSizeUpLo = "0.0";         //range for random variation of heading
   schoolWanderSizeUpHi = "0.15";

   schoolWanderPeriodLo = "4.0";       //period of random heading variation
   schoolWanderPeriodHi = "8.0";
   
   noTilt = false;
   wanderTimeMin = 4.0;
   wanderTimeMax = 9.0;
   followIncreaseRate = 1.0;
   followDecreaseRate = 1.0;
   wanderYawAngleChange = mDegToRad(25.0);
   wanderPitchAngleChange = mDegToRad(15.0);
   stuckTolerance = 0.2;
   reversingTime = 5.0;

   fleeEndDistance = 1.0;
   fleeStartDistance = 0.5;
   localFishRadius = 3.0;
   obstacleWarningTime = 2.0;
   
   wanderSpeedScaleLo = 0.1;
   wanderSpeedScaleHi = 0.1;
   wanderTurnScale = 1.0;

   fleeSpeedScale = 1.0;
   fleeTurnScale = 4.0;
      
   swishSpeedScale = 0.3;
   swishTurnScale = 4.0;
   swishEffectDistance = 1.0;
   
   acceleration = 0.5;
   turnAcceleration = 1.0;

   renderFirstPerson = false;

   computeCRC = false;

   // Third person shape
   shapeFile = "art/cube/ai/orange_basselet.dts";
   cameraMaxDist = 3;
   allowImageStateAnimation = true;

   canObserve = 1;
   cmdCategory = "Clients";

   cameraDefaultFov = 55.0;
   cameraMinFov = 5.0;
   cameraMaxFov = 65.0;

   debrisShapeName = "art/shapes/actors/common/debris_player.dts";
   debris = playerDebris;
   
   throwForce = 30;

   aiAvoidThis = 1;

   //how far can look up and down
   minLookAngle = "-0.7";
   maxLookAngle = "0.7";
   maxFreelookAngle = 1.57;

   mass = 120;
   drag = 1.3;
   maxdrag = 0.4;
   density = 1.1;
   maxDamage = 100;
   maxEnergy =  60;
   repairRate = 0.33;
   energyPerDamagePoint = 75;

   rechargeRate = 0.256;

   runForce = 4320;
   runEnergyDrain = 0;
   minRunEnergy = 0;
   maxForwardSpeed = 8;
   maxBackwardSpeed = 6;
   maxSideSpeed = 6;

   sprintForce = 4320;
   sprintEnergyDrain = 0;
   minSprintEnergy = 0;
   maxSprintForwardSpeed = 14;
   maxSprintBackwardSpeed = 8;
   maxSprintSideSpeed = 6;
   sprintStrafeScale = 0.25;
   sprintYawScale = 0.05;
   sprintPitchScale = 0.05;
   sprintCanJump = true;

   crouchForce = 405;
   maxCrouchForwardSpeed = 4.0;
   maxCrouchBackwardSpeed = 2.0;
   maxCrouchSideSpeed = 2.0;

   maxUnderwaterForwardSpeed = 8.4;
   maxUnderwaterBackwardSpeed = 0;
   maxUnderwaterSideSpeed = 0;

   jumpForce = "747";
   jumpEnergyDrain = 0;
   minJumpEnergy = 0;
   jumpDelay = "15";
   airControl = 0.3;

   fallingSpeedThreshold = -6.0;

   landSequenceTime = 0.33;
   transitionToLand = false;
   recoverDelay = 0;
   recoverRunForceScale = 0;

   minImpactSpeed = 10;
   minLateralImpactSpeed = 20;
   speedDamageScale = 0.4;

   boundingBox = "0.65 0.75 1.85";
   crouchBoundingBox = "0.65 0.75 1.3";
   swimBoundingBox = "1 2 2";
   pickupRadius = 1;

   // Damage location details
   boxHeadPercentage       = 0.83;
   boxTorsoPercentage      = 0.49;
   boxHeadLeftPercentage         = 0.30;
   boxHeadRightPercentage        = 0.60;
   boxHeadBackPercentage         = 0.30;
   boxHeadFrontPercentage        = 0.60;

   // Foot Prints
   decalOffset = 0.25;

   footPuffEmitter = "LightPuffEmitter";
   footPuffNumParts = 10;
   footPuffRadius = "0.25";

   dustEmitter = "LightPuffEmitter";

   splash = PlayerSplash;
   splashVelocity = 4.0;
   splashAngle = 67.0;
   splashFreqMod = 300.0;
   splashVelEpsilon = 0.60;
   bubbleEmitTime = 0.4;
   splashEmitter[0] = PlayerWakeEmitter;
   splashEmitter[1] = PlayerFoamEmitter;
   splashEmitter[2] = PlayerBubbleEmitter;
   mediumSplashSoundVelocity = 10.0;
   hardSplashSoundVelocity = 20.0;
   exitSplashSoundVelocity = 5.0;

   // Controls over slope of runnable/jumpable surfaces
   runSurfaceAngle  = 38;
   jumpSurfaceAngle = 80;
   maxStepHeight = 0.35;  //two meters
   minJumpSpeed = 20;
   maxJumpSpeed = 30;

   horizMaxSpeed = 68;
   horizResistSpeed = 33;
   horizResistFactor = 0.35;

   upMaxSpeed = 80;
   upResistSpeed = 25;
   upResistFactor = 0.3;

   footstepSplashHeight = 0.35;

   //NOTE:  some sounds commented out until wav's are available

   // Footstep Sounds
   FootSoftSound        = FootLightSoftSound;
   FootHardSound        = FootLightHardSound;
   FootMetalSound       = FootLightMetalSound;
   FootSnowSound        = FootLightSnowSound;
   FootShallowSound     = FootLightShallowSplashSound;
   FootWadingSound      = FootLightWadingSound;
   FootUnderwaterSound  = FootLightUnderwaterSound;

   groundImpactMinSpeed    = "45";
   groundImpactShakeFreq   = "4.0 4.0 4.0";
   groundImpactShakeAmp    = "1.0 1.0 1.0";
   groundImpactShakeDuration = 0.8;
   groundImpactShakeFalloff = 10.0;

   observeParameters = "0.5 4.5 4.5";
   class = FishPlayerClass;

   cameraMinDist = "0";
   DecalData = "PlayerFootprint";

};

//This is the default datablock for the Fish.
//I changed the stock datablock name from those used in AIFishPlayerScript.CS
//I did this to allow me to create different classes of bots with their own
//thinking and reaction routines for each class.YellowBellyFishPlayer
datablock FishPlayerData(YellowBellyFishPlayer : DefaultFishPlayerData)
{
   bubbleDataBlock = YellowBellyFishBubble;
   class = "FishPlayerClass";
   className = "FishPlayerClass";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   infoUrl = "http://en.wikipedia.org/wiki/Anthiinae";  


   scaleRange = "0.7 1";

   maxForwardSpeed = 2;
   maxBackwardSpeed = 0.5;
   maxSideSpeed = 0.5;
   
   maxUnderwaterForwardSpeed = 2;

   //non-zero means fish can reverse and side step
   maxUnderwaterBackwardSpeed = 0;
   maxUnderwaterSideSpeed = 0;

   horizTurnSpeedInertia = 0;
   vertTurnSpeedInertia= 0;
   maxTurnRate = 1;
   smoothCamera = 0;
   conformToGround = true;
   
   moveTolerance = 0.1;
   moveStuckTolerance = 0.1;
   moveStuckTestDelay = 500;
   
   mass = 1.0;
   swimForce = 1.0 * 9.0;  
   runForce = 1.0 * 9.0;

   density = 1;

   splash = "";
   splashVelocity = 0;
   splashAngle = 0;
   splashFreqMod = 0;
   splashVelEpsilon = 0;
   bubbleEmitTime = 0;
   mediumSplashSoundVelocity = 0;
   hardSplashSoundVelocity = 0;
   exitSplashSoundVelocity = 0;

   boundingBox = "0.3 0.3 0.1";
   swimBoundingBox = "0.3 0.3 0.1";
   renderOffset = "0.0 0.0 0.0";
   canFly = false;
   
   maxStepHeight = 0.00000001;
   
   alwaysUnderwater = true;
   
//   availableSkins =  "base selected";   
   
   popupType = 2;
   
   inputEventsMethod = "Mesh";
   faceUser = true;

   nActivity = 3;

   activity[0] = "Feeding";
   activityTime[0] = "5 10";
   activityWeight[0] = "10";
   
   activity[1] = "Swimming";
   activityTime[1] = "10 100";
   activityWeight[1] = "70";

   activity[2] = "Hiding";
   activityTime[2] = "10 100";
   activityWeight[2] = "20";
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Flee + $FishFlags::Swish;
};

datablock FishPlayerData(HarlequinTuskFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = HarlequinTuskFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/harlequintuskfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=tuskfish";  

   maxForwardSpeed = 0.5;
   maxTurnRate = 0.5;
   maxUnderwaterForwardSpeed = 1.5;
   
   boundingBox = "0.5 0.5 0.3";
   swimBoundingBox = "0.5 0.5 0.3";
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Swish;
   
   swishTurnScale = 1.0;
   swishSpeedScale = 0.2;
   
   
};

datablock FishPlayerData(MoorishIdolFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = MoorishIdolFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/moorishidol.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=moorish";  
   
   boundingBox = "0.2 0.2 0.2";
   swimBoundingBox = "0.2 0.2 0.2";
   
   maxForwardSpeed = 1.5;
   maxUnderwaterForwardSpeed = 1.5;
   
   fleeSpeedScale = 0.5;
   fleeTurnScale = 2.0;
      
   
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Flee + $FishFlags::Swish;
};

datablock FishPlayerData(BlueTangFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = BlueTangFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/bluetang.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=bluetang";  
   
   boundingBox = "0.3 0.3 0.2";
   swimBoundingBox = "0.3 0.3 0.2";


   maxForwardSpeed = 1;
   maxTurnRate = 0.3;

   
   maxUnderwaterForwardSpeed = 1.5;
   
   availableSkins =  "base selected";   
   fishTypeFlags = $FishFlags::Large + $FishFlags::Swish;
   menu =  $FishFlags::Small +  $FishFlags::Medium ;
   
   swishTurnScale = 1.0;
   swishSpeedScale = 0.2;
};
datablock FishPlayerData(ClownFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = ClownFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/clownfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=clownfish";  
   
   boundingBox = "0.3 0.3 0.2";
   swimBoundingBox = "0.3 0.3 0.2";
   
   maxUnderwaterForwardSpeed = 1;
   fleeSpeedScale = 0.3;
   fleeTurnScale = 2.0;
   
   fishTypeFlags = $FishFlags::Small + $FishFlags::Swish;
   inputEventsMethod = "Convex";
   
   scaleRange = "0.4 0.5";
};

datablock FishPlayerData(SeahorsePlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = SeahorseBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/seahorse.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=seahorse";  
   
   boundingBox = "0.1 0.1 0.2";
   swimBoundingBox = "0.1 0.1 0.2";
   
   maxUnderwaterForwardSpeed = 0.5;
   
   noTilt = true;
   mass = 3;
   swimForce = 3 * 9.0;  
   runForce = 3 * 9.0;
   drag = 0;
   
   fishTypeFlags = $FishFlags::Small;
   inputEventsMethod = "Convex";
};
// CuttleFishPlayer.runForce
datablock FishPlayerData(CuttleFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = CuttleFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/cuttlefish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=cuttlefish";  
   
   boundingBox = "0.6 0.6 0.2";
   swimBoundingBox = "0.6 0.6 0.2";
   maxForwardSpeed = 2;
   maxUnderwaterForwardSpeed = 2;
   maxTurnRate = 0.3;
   
   mass = 3;
   swimForce = 3 * 9.0;  
   runForce = 3 * 9.0;
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Swish;
   swishTurnScale = 1.0;
   swishSpeedScale = 0.2;
   
   scaleRange = "0.5 1";
   
   interactAnim = "interact";
   faceUser = false;
};

datablock FishPlayerData(SardinellaFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = SardinellaFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/sardinella.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=sardine";  

   maxForwardSpeed = 0.2;
   maxUnderwaterForwardSpeed = 0.2;
   
   boundingBox = "0.05 0.05 0.05";
   swimBoundingBox = "0.05 0.05 0.05";
   
   maxTurnRate = 12.0;
   noAnimationScale = true;
   fishTypeFlags = $FishFlags::Medium;
};
datablock FishPlayerData(BluefinTrevallyPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = BluefinTrevallyBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/bluefin_trevally.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=trevally";  

   wanderSpeedScaleLo = 0.1;
   wanderSpeedScaleHi = 0.2;
   wanderTurnScale = 0.75;
   maxTurnRate = 0.75;

   acceleration = 0.5;
   turnAcceleration = 4.0;

   maxForwardSpeed = 3;
   maxUnderwaterForwardSpeed = 3;
   
   boundingBox = "1.095 1.095 0.431";
   swimBoundingBox = "1.095 1.095 0.431";
   fishTypeFlags = $FishFlags::Large;
   menu =  $FishFlags::Small +  $FishFlags::Medium ;
};
datablock FishPlayerData(OrangeBasseletPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = OrangeBasseletBubble;
   class = "FishPlayerClass";
   shapeFile = "art/cube/ai/orange_basselet.dts";

//   infoUrl = getFishWebRoot() @ "/index_master.html?f=basslet";

   boundingBox = "0.2 0.2 0.2";
   swimBoundingBox = "0.2 0.2 0.2";
   
   maxForwardSpeed = 1;
   maxUnderwaterForwardSpeed = 1;
   
   fishTypeFlags = $FishFlags::Small + $FishFlags::Flee + $FishFlags::Swish;
   
   inputEventsMethod = "Convex";
};
datablock FishPlayerData(MoonJellyfishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = MoonJellyfishBubble;
   class = "FishPlayerClass";
   shapeFile = "art/cube/ai/moon_jellyfish.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=jellyfish";  

   boundingBox = "0.4 0.4 0.4";
   swimBoundingBox = "0.4 0.4 0.4";
   
   fishTypeFlags = $FishFlags::JellyFish;
   
   
   maxUnderwaterForwardSpeed = 1;
   fleeSpeedScale = 0.2;
   fleeTurnScale = 1.0;
};
datablock FishPlayerData(MoonJellySmallFishPlayer : MoonJellyfishPlayer)
{
   class = "FishPlayerClass";
   scaleRange = "0.2 0.3";
};

datablock FishPlayerData(SnapperPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = SnapperBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/snapper.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=snapper";  

   wanderSpeedScaleLo = 0.2;
   wanderSpeedScaleHi = 0.5;
   wanderTurnScale = 0.5;

   acceleration = 0.5;
   turnAcceleration = 4.0;

   maxUnderwaterForwardSpeed = 0.5;
   
   boundingBox = "0.6 0.6 0.18";
   swimBoundingBox = "0.6 0.6 0.18";
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Flee  + $FishFlags::Swish;

   wanderTurnScale = 0.5;
   
   swishTurnScale = 1.0;
   swishSpeedScale = 0.2;

   fleeSpeedScale = 0.5;
   fleeTurnScale = 2.0;
};
datablock FishPlayerData(LionFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = LionFishBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/lionfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=lionfish";  
   
   maxForwardSpeed = 0.5;
   maxUnderwaterForwardSpeed = 1;
   maxTurnRate = 0.3;

   boundingBox = "0.3 0.3 0.2";
   swimBoundingBox = "0.3 0.3 0.2";
   fishTypeFlags = $FishFlags::Large + $FishFlags::Flee + $FishFlags::Swish;

   swishTurnScale = 1.0;
   swishSpeedScale = 0.2;
};
datablock FishPlayerData(BluestreakedWrassePlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = BluestreakedWrasseBubble;
   class = "FishPlayerClass";
   shapeFile = "art/cube/ai/bluestreaked_wrasse.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=cleaner";  
   boundingBox = "0.3 0.3 0.06";
   swimBoundingBox = "0.3 0.3 0.06";
   
   maxForwardSpeed = 1;
   maxUnderwaterForwardSpeed = 1;
   fishTypeFlags = $FishFlags::Small + $FishFlags::Flee + $FishFlags::Swish;
   
   interactAnim = "interact";
   faceUser = false;
   inputEventsMethod = "Convex";

};
datablock FishPlayerData(ParrotfishFemalePlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = ParrotfishFemaleBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/parrotfish_female.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=parrotfish";  

   boundingBox = "0.8 0.8 0.26";
   swimBoundingBox = "0.8 0.8 0.26";
   
   wanderSpeedScaleLo = 0.1;
   wanderSpeedScaleHi = 0.3;
   wanderTurnScale = 0.5;

   acceleration = 0.5;
   turnAcceleration = 4.0;

   maxForwardSpeed = 1;
   maxTurnRate = 1;
   maxUnderwaterForwardSpeed = 1;
   fishTypeFlags = $FishFlags::Large ;
   
   faceUser = true;

   fleeSpeedScale = 0.3;
};
datablock FishPlayerData(ParrotfishJuvenilePlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = ParrotfishJuvenileBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/parrotfish_juvenile.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=parrotfish";  

   boundingBox = "0.2 0.2 0.04";
   swimBoundingBox = "0.2 0.2 0.04";
   
   maxForwardSpeed = 1;
   maxUnderwaterForwardSpeed = 1;   
   fishTypeFlags = $FishFlags::Medium + $FishFlags::Flee;

   interactAnim = "interact";
   faceUser = false;
   inputEventsMethod = "Convex";
};
datablock FishPlayerData(ParrotfishMalePlayer : ParrotfishFemalePlayer)
{
   bubbleDataBlock = ParrotfishMaleBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/parrotfish_male.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=parrotfish";  

   boundingBox = "0.8 0.8 0.26";
   swimBoundingBox = "0.8 0.8 0.26";
   
   
   maxForwardSpeed = 1;
   maxTurnRate = 0.3;
   maxUnderwaterForwardSpeed = 1;   
   fishTypeFlags = $FishFlags::Large ;

   faceUser = true;
   fleeSpeedScale = 0.3;
};
datablock FishPlayerData(RemoraPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = RemoraBubble;
   class = "FishPlayerClass";
//   shapeFile = "art/cube/ai/remora.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=remora";  

   boundingBox = "0.8 0.8 0.12";
   swimBoundingBox = "0.8 0.8 0.12";
   
   wanderSpeedScaleLo = 0.1;
   wanderSpeedScaleHi = 0.15;
   wanderTurnScale = 0.4;

   acceleration = 0.5;
   turnAcceleration = 4.0;
   
   maxForwardSpeed = 3;
   maxUnderwaterForwardSpeed = 3;   
   fishTypeFlags = $FishFlags::Large;
};


datablock FishPlayerData(PufferFishPlayer : YellowBellyFishPlayer)
{
   bubbleDataBlock = PufferFishBubble;
   class = "FishPlayerClass";
   shapeFile = "art/cube/ai/pufferfish.dts";
//   infoUrl = getFishWebRoot() @ "/index_master.html?f=porcupinefish";  
   
   maxForwardSpeed = 0.5;
   maxUnderwaterForwardSpeed = 1;
   maxTurnRate = 0.3;

   boundingBox = "0.3 0.3 0.2";
   swimBoundingBox = "0.3 0.3 0.2";
   fishTypeFlags = $FishFlags::Flee + $FishFlags::Swish;

   swishTurnScale = 3.0;
   swishSpeedScale = 0.2;
   
   interactAnim = "interact";
   faceUser = false;
   
   scaleRange = "1.5 2.5";

};



//**************************************************************
//lookup table for tropic map to find bubble data from name
//**************************************************************
new ArrayObject(BubbleDatablocks){};
BubbleDatablocks.insert("remora",RemoraPlayer,0);
BubbleDatablocks.insert("parrotfish",ParrotfishMalePlayer,0);
BubbleDatablocks.insert("cleaner",BluestreakedWrassePlayer,0);
BubbleDatablocks.insert("lionfish",LionFishPlayer,0);
BubbleDatablocks.insert("pufferfish",PufferFishPlayer,0);
BubbleDatablocks.insert("snapper",SnapperPlayer,0);
BubbleDatablocks.insert("jellyfish",MoonJellyfishPlayer,0);
BubbleDatablocks.insert("basslet",OrangeBasseletPlayer,0);
BubbleDatablocks.insert("trevally",BluefinTrevallyPlayer,0);
BubbleDatablocks.insert("sardine",SardinellaFishPlayer,0);
BubbleDatablocks.insert("seahorse",SeahorsePlayer,0);
BubbleDatablocks.insert("cuttlefish",CuttleFishPlayer,0);
BubbleDatablocks.insert("clownfish",ClownFishPlayer,0);
BubbleDatablocks.insert("bluetang",BlueTangFishPlayer,0);
BubbleDatablocks.insert("moorish",MoorishIdolFishPlayer,0);
BubbleDatablocks.insert("tuskfish",HarlequinTuskFishPlayer,0);
//BubbleDatablocks.insert("urchin",Urchin_SSData,0);
BubbleDatablocks.insert("starfish",Starfish_SSData,0);
//BubbleDatablocks.insert("whaleshark",WhaleSharkSet1_SSData,0);
//BubbleDatablocks.insert("diver",DiverSet1_SSData,0);
BubbleDatablocks.insert("dugong",DugongSet1_SSData,0);
//BubbleDatablocks.insert("potatocod",GrouperSet1_SSData,0);
//BubbleDatablocks.insert("hermit",HermitCrab1Set1_SSData,0);
//BubbleDatablocks.insert("moray",WhitemouthMorayEelLeftSet1_SSData,0);
//BubbleDatablocks.insert("turtle",GreenSeaTurtleSet1_SSData,0);
//BubbleDatablocks.insert("ray",BluespottedStingRaySet1_SSData,0);
//BubbleDatablocks.insert("shark",GreyReefSharkSet2_SSData,0);
//BubbleDatablocks.insert("whale",WhaleSet2_SSData,0);
//BubbleDatablocks.insert("manta",MantaRaySet1_SSData,0);

//BubbleDatablocks.insert("harlequinshrimp",Shrimp_SSData,0);
//BubbleDatablocks.insert("fanworm",Fanworm_SSData,0);
//BubbleDatablocks.insert("cucumber",Seacucumber_SSData,0);
//BubbleDatablocks.insert("nudibranch",Nudibranch_SSData,0);
BubbleDatablocks.insert("clam",Clam_SSData,0);
//BubbleDatablocks.insert("porcelaincrab",PorcelainCrab_SSData,0);
//BubbleDatablocks.insert("porcupinefish",PufferFishPlayer,0);
//BubbleDatablocks.insert("sunfish",SunFishSet1_SSData,0);


