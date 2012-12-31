//The AIFishPlayerScript.CS file creates a fish-style bot.
//The fish uses a simple state machine to control it's actions.
//The following are global variables used to set the fishs basic settings.

new ArrayObject(ActivityList){};
new ArrayObject(TouchCogs){};

$FISH_SPAWNCOUNT_MOD = 0.5;

// web page locations. All relative to root
//$FISH_URL = "/index_master.html?f=";  //fish name should be placed on end
$FISH_URL = "/wiki/";  //fish name should be placed on end

// dip switches
$FISH_DEBUG = 0;
$BUBBLE_BREAK = 1;   //if true, when launch bubble near existing one, old one is killed off

//bubble states
$BUBBLE_STATE_FADE_IN = 0;
$BUBBLE_STATE_FADE_OUT = 1;
$BUBBLE_STATE_IDLE = 2;

$WEBACTION_NONE=0;
$WEBACTION_CLOSEPAGE=1;
$WEBACTION_NEWPAGE=2; 

//range of movement of bubbles on gui plane
$GUIRANGE_X0 = -4.06;
$GUIRANGE_X1 = 4.06;
$GUIRANGE_Y0 = -2.12;
$GUIRANGE_Y1 = -1.655;

$CAMERA_FIELD = 79.8;   //horizontal field of view (hack, got this number out of camera in maya file)
$CAMERA_FIELD_TASKBAR = 79.8;   //this seems to be better value. One from maya file gives different offsets on different panels.

//*****************************************************
// web page launcher icon
//*****************************************************
$COG_LIFE_TIME = 1*1000;
$COG_UPDATE_TIME = 500;
$COG_CANCEL_MOVE_DISTANCE = 0.05; //when move finger by this much, web page load aborted
$COG_SCALE = 0.10;
$COG_FRAME_COUNT = 64;
$COG_FADEINTIME = 0.5*1000;
$COG_BADFADEINTIME = 1.0*1000;

$BUBBLE_SPIN_INERTIA_TIME = 1000;      //how far back to use data points in getting inertia
$BUBBLE_SPIN_DRAG_TIME = 3000;
$BUBBLE_SPINBUFFER_SIZE = 8;
$BUBBLE_SCALE  =  "0.25 0.25 0.25";    //size pop up bubble with fish in it
$BUBBLE_MAX_MOVE = 0.1;
$BUBBLE_RADIUS = 0.12;
$BUBBLE_LIFE = 30*1000;   
$BUBBLE_ANIM_FRAME_COUNT = 92;   
$BUBBLE_FADEINTIME = 0.5*1000;
$BUBBLE_MOVE_URGENCY = 1.0;   //0->1 1=snap to mouse all the time
$BUBBLE_UPDATE_TIME = 50;
$BUBBLE_PROGRESS_OFFSET = 0.15;
$BUBBLE_BACK_PLANE_DISTANCE = 0.5;        //backdrop to spinning fish is this far behind the main body of bubble  
$BUBBLE_BUTTON_DISTANCE = 0.001;            //buttons are this far in front of main bubble body
//$BUBBLE_BUTTON_DISTANCE = 0.1;            //buttons are this far in front of main bubble body
$BUBBLE_FISH_SCALE = 1.5;                //extra scale factor applied to fish in addition to their own scale.
$BUBBLE_MAX_TILT = 45;                 //how much can tilt fish up and down
$BUBBLE_MIN_TILT = -85;
$BUBBLE_DRAG_DISTANCE = -0.05;            //drag button is this far in front of main bubble body
$BUBBLE_SPIN_SPEED = 1000.0;          //how quickly fish spin in bubble

$WEBPAGE_THICKNESS = 0.05;          //layer distance for overlapping pages
$WEBPAGE_TO_CAMERA_DISTANCE = 4.9;  //distance from camera to web pages
$WEBPAGE_LIFE = 30*1000;   
$WEBPAGE_SCALE = 0.000125;
$WEBPAGE_NUMBER_ACROSS = 20;              //web pages across world. Should equal number of panels
$WEBPAGE_Y_POSITION = -1.85;               //up position of web page in gui plane
$WEBPAGE_OBJECT_SIZE = 0.625;              //size physical object web page is pasted on to
$WEBPAGE_RES = "1050 1600";      

$FISH_DEFAULT_WANDER_RANGE = "-7.5 7.6905 -5 7.5 10 -3";

$ACTIVITY_UPDATE_PERIOD = 1000;
$ACTIVITY_MAX_DISTANCE = 2.5;   //max distance fish will go for an activity

//*****************************************************
// school stuff
//*****************************************************
$DEPTH_HI = -10.0;
$DEPTH_LO = 0;

$FISH_RANDOM_START_TIME = 2.0;

$FISH_ENABLED = true;					//Whether Fish bots are loaded during mission loading.
$FISH_MARKER_HIDE = true;				//Turns marker hiding on or off - useful when editing maps.
$FISH_ENHANCED_FOV_TIME =2000;	//How long the bots field of vision is enhanced to 360 for.
$FISH_FOV			      =240;				//The fishs field of vision
$FISH_ENHANCEDFOV		=360;
$FISH_DETECT_DISTANCE = 20;			//The range at which a fishbot will start reacting to a client target
$FISH_IGNORE_DISTANCE = 60;		//The range at which the bot ignores a client and will not fire on it.
$FISH_SCANTIME       = 1000/2;			//The quickest time between think cycles.
$FISH_FOLLOWLEADERSCANTIME       = 1000/2;			//The quickest time between think cycles.
$FISH_MAX_ATTENTION = 10; 		  //This number and $FISH_SCANTIME are multiplied to set the delay in the
																		//thinking loop. Used to free up processor time on bots out of the mix.
$FISH_CREATION_DELAY_MAX = 5000 + $FISH_SCANTIME;
$FISH_CREATION_DELAY =5000;			//How long a bot waits after creation before his think cycles are controlled by
																		//his attention rate. (Used to help free up think cycles on bots while misison
																		//finishes loading.
$FISH_DEFAULTRESPAWN = true;		//Controls whether fishs respawn automatically or not if their marker does not have
																		//dynamice 'respawn' variable set in it.
$FISH_RESPAWN_DELAY = 20000;		//Determines how long a bot goes in between death and respawning.
$FISH_ENHANCEFOV_CHANCE = 10; 	//There is a 1 in x chance that fish will see 360 deg vision to prevent it
																		//from being snuck up on.
$FISH_ROAMING_RANGE = 100;

$FISH_RANDOM_OFFSET = 1.0;

$currentDelay = 10;





//The delay before respawn function is set to wait a specified duration before
//respawning an AIFishPlayerScript
function AIFishPlayerScript::DelayBeforeRespawn(%this, %name, %marker)
{
   %this.respawntrigger = %this.schedule($FISH_RESPAWN_DELAY,"respawn", %name, %marker);
}

//I dont use this function but left it in - just in case it does something I'm not privy to.
function FishPlayerClass::onEndSequence(%data,%this,%slot)
{
   if(%slot == 1)
   {
      %this.destroyThread(1);
   }
   %this.stopThread(%slot);
//   %obj.nextTask();
}

// cache lists of different available activities for fish
function AIFishPlayerScript::LoadActivities()
{
   ActivityList.empty();   //need to delete old data here if reef is rerun

   InitContainerRadiusSearch("0 0 0", 
                              10000000, 
                              $TypeMasks::StaticObjectType);
	while ( (%thing = containerSearchNext()) != 0 )
	{
      if(%thing.getclassname() $= "StaticShape")   //check its a static shape
      {
         %className = %thing.class;
         if( strpos(%className,"ActivityClass") >=0 ){   //make sure its some sort of activity
            //see if already have a list
            %index = ActivityList.getIndexFromKey(%className);
            if( %index < 0 ){
               %list = new ArrayObject();
               ActivityList.add(%className, %list);
            }else{
               %list = ActivityList.getValue(%index);           
            }

            %thing.onAdd();                        
            
            //add to list
            %count = %list.count();
            %list.add(%count, %thing);
         }
      }
   }
}
// find random activity of given name on reef
function AIFishPlayerScript::findActivity(%name, %fish)
{
   %className = %name @ ActivityClass;
   %index = ActivityList.getIndexFromKey(%className);
   if(%index<0)
   {
      return "";
   }
   else
   {
      //choose random activity from list
      %list = ActivityList.getValue(%index);
      %iActivity = getRandom(0, %list.count()-1);
      for(%i = 0 ; %i<%list.count(); %i++)
      {
         %index = (%i+%iActivity) % (%list.count());
         %activity = %list.getValue(%index);
         if( %activity.IsFree(%fish) )
         {
            return %activity;
         }
      }
      return "";
   }
   
}

//The LoadEntities function replaces the markers placed in the map with the AI bots during the
//mission loading.
function AIFishPlayerScript::LoadEntities()
{   
	//Check to see if the AIFishs are to be loaded.
	if ($FISH_ENABLED == true)
	{
		echo("Loading Fish entities...");
		//This performs a search for all items within the radius from the starting point.
		//All of the items that match "AIFishMarker" trigger a bot to be placed at the 
		//position of the marker found.
		%position = "0 0 0";
		%radius = 100000.0;
		InitContainerRadiusSearch(%position, %radius, $TypeMasks::StaticObjectType);
		%i=0;
		
		%trafficController = new TrafficController()
		{
		};
		
		%trafficController.setGrid("-20 -20 -20", "20 20 20", 4);
      		
		
		while ((%targetObject = containerSearchNext()) != 0)
		{
			if(%targetobject.getclassname() $= "StaticShape")
			{
				if (strpos(%targetobject.getDataBlock().getName(),"FishMarker") >= 0)
				{
					%i++;
					%player = AIFishPlayerScript::spawnAtMarker("Fish" @ %i, %targetobject, %trafficController);
					//schedule(getRandom(0,200), 0, "AIFishPlayerScript::spawnAtMarker", "Fish" @ %i, %targetobject);
               if ($FISH_MARKER_HIDE == true)
               {
                  %targetobject.sethidden(true);
                  %targetobject.hidden = true;  // todo: find out why
               }
				}
			}
  		}
	}
	else
	{
		echo("Fish entities disabled...");
	}
}

function AIFishPlayerScript::updateSpeed(%this)
{
   if( %this.targetPositionSet==0 ){
      %this.speed = 0;
   }else{
      %v =  VectorSub(%this.targetPosition, %this.position);
      %distanceToTarget = VectorLen(%v);
      if(%this.reachedDestination){
         if( %distanceToTarget > %this.obj.schoolFarDistance2 ){
            %this.reachedDestination = 0;
         }
      }else{
         if( %distanceToTarget < %this.obj.schoolNearDistance ){
            %this.reachedDestination = 1;
         }
      }
      %nodeSpeed = VectorLen(%this.targetVelocity);    //speed of node itself
      %minSpeed = %nodeSpeed;      
      %maxSpeed = %this.boss $= "" ? %nodeSpeed*8.0 : 8.0;
      if( %this.reachedDestination ){
         %this.speed = %nodeSpeed*0.9;     
      }else if(%distanceToTarget >= %this.obj.schoolFarDistance){
         %this.speed = %maxSpeed;
      }else{
         %f = (%distanceToTarget - %this.obj.schoolNearDistance) / (%this.obj.schoolFarDistance - %this.obj.schoolNearDistance);
         %this.speed = %nodeSpeed + (%maxSpeed-%nodeSpeed)*%f;
      }
   }
}
function InArrow(%position, %size)
{
   if( mAbs(%position.z)>(0.5*(%position.x+%size)) ){
      return false;
   }else{
      return true;
   }
}
function InCone(%position, %size)
{
   %radius = (%position.x+%size)*0.5;
   if( %radius*%radius > (%position.y*%position.y + %position.z*%position.z) ){
      return true;
   }else{
      return false;
   }
}
$LETTER_WIDTH = 0.1;

function InO(%p, %s)
{
   %y = mAbs(%p.y) / %s;
   %x = %p.x / %s;
   %z = mAbs(%p.z);
   if(%z>(0.5*$LETTER_WIDTH) ){
      return false;
   }else{
      %r = mSqrt(%x*%x + %y*%y);
      if(%r>1.0){
         return false;
      }else if(%r<(1.0-$LETTER_WIDTH)){
         return false;
      }else{
         return true;
      }
   }
}


function InX(%p, %s)
{
   %y = mAbs(%p.y) / %s;
   %x = mAbs(%p.x) / %s;
   %z = mAbs(%p.z) / %s;
   
   if(%z>(0.5*$LETTER_WIDTH) ){
      return false;
   }
   %m = (2-$LETTER_WIDTH)/2;
   %c = %m-1;   
   if( %y < (%m*%x+%c) ){
      return false;            
   }
   %c = %c + $LETTER_WIDTH;
   if( %y > (%m*%x+%c) ){
      return false;            
   }else{
      return true;
   }
}
function InV(%p, %s)
{
   %y = mAbs(%p.y) / %s;
   %x = %p.x / %s;
   %z = mAbs(%p.z) /%s;
      
   %m = ($LETTER_WIDTH*0.5-1.0)/2.0;
   %c = 1+%m;
   
         
   if(%z>(0.5*$LETTER_WIDTH) ){
      return false;
   }
   
   if( %y > (%m*%x + %c) ){
      return false;            
   }
   %c -= $LETTER_WIDTH;   
   if( %y > (%m*%x + %c) ){
      return true;            
   }else{
      return false;
   }
}

function InA(%p, %s)
{
   %y = mAbs(%p.y) / %s;
   %x = %p.x / %s;
   %z = mAbs(%p.z) /%s;
      
   %m = ($LETTER_WIDTH*0.5-1.0)/2.0;
   %c = 1+%m;
   
         
   if(%z>(0.5*$LETTER_WIDTH) ){
      return false;
   }
   
   if( %y > (%m*%x + %c) ){
      return false;            
   }
   %c -= $LETTER_WIDTH;   
   if( %y > (%m*%x + %c) ){
      return true;            
   }else{
      //interior v
      if( mAbs(%x)<(0.5*$LETTER_WIDTH) ){
         return true;
      }else{
         return false;
      }
   }
}
function InTearDrop(%position, %size)
{
   if( %position.x<(-0.5*%size.x)  ){
      %x = %position.x - (-0.5*%size.x);
      %r = mSqrt(%position.y*%position.y + %position.z*%position.z + %x*%x);      
      return %r<(0.5*%size.x);
   }else if( %position.x > (0.5*%size.x) ){
      %x = %position.x - (0.5*%size.x);
      %r = mSqrt(%position.y*%position.y + %position.z*%position.z + %x*%x);      
      return %r<(0.5*%size.x);
   }
}
function InEgg(%position, %radius)
{
   %vec = %position;
   %vec.y *= 4.0;   
   %vec.z *= 4.0;   
   
   %length = VectorLen(%vec);
   return (%length<%radius);
}
function InSphere(%position, %radius)
{
   %vec = %position;
   
   %length = VectorLen(%vec);
   return (%length<%radius);
}

//-----------------------------------------------------------------------------
// AIFishPlayerScript static functions
//-----------------------------------------------------------------------------

$fishCount = 0;

//This is the spawn function for the bot.
function AIFishPlayerScript::spawn(%name, %obj)
{   
   $fishCount++;
   
   %markerData = %obj.getDataBlock();
   %datablock = %markerData.associatedDatablock;
 
   %myName = %name;
   if( %obj.botName !$= ""){
      %myName = %obj.botName;
   }
   
   %markerScale = %obj.scale;
   if(%datablock.scaleRange)
   {
      %markerScale = getRandomFloat( %datablock.scaleRange.x, %datablock.scaleRange.y );
      %markerScale = %markerScale SPC %markerScale SPC %markerScale;
   }
     
   %player = new AIFishPlayer(%obj.botName) {
      class = "AIFishPlayerScript";
      dataBlock = %datablock;
      scale = %markerScale;
      index = $fishCount;
      time = getRandomFloat(0.0, $FISH_RANDOM_START_TIME);   
      
      //The marker is the AIFishPlayerScript marker object that the fish is associated with.
      //The marker object is kept with the player data because it's location, and
      //dynamic variable values are used in several functions. This also allows the addition
      //of future dynamic variables without having to change the spawn/respawn functions to
      //access them.
      marker = %obj;
      resource = %obj;
      movePosition = %obj.getPosition();
      moveLoop = null;
      herd = 0;
      moveReady = $AI_READY_TO_MOVE;
      //Sets whether the bot is AI or not
      isbot=true;
      //Sets the bots field of vision
      fov=$FISH_FOV;
         
      //Thinking variables
      //Firing tells whether or not we're in the midst of a firing sequence.
      firing = false;
      //The 'action' variable holds the state of the bot - which controls how it
      //thinks.
      holdcnt=$FISH_HOLDCNT_MAX-1;
      action = $FishState::Swimming;
      //The bots starting attention level is set to half of it's range.
      attentionlevel = $FISH_MAX_ATTENTION/2;
      
      //Oldpos holds the position of the bot at the end of it's last 'think' cycle
      //This is used to help determine if a bot is stuck or not.
      oldpos = %obj.getposition();
      //used to determine a bot's speed
      oldmovepos = %obj.getposition();
      readyForMoveSchedule = 0;
      depthRange = "";
      enableInputEvents = true;
      nodeSpeed = 0.0;
//      updateAiCount = 0;
      targetPosition = "0 0 0";
      targetPositionSet = 0;
      targetVelocity = "0 0 0";
      nodeDirection = "0 0 0";
      obj = %obj;
      inputEventsMethod = %datablock.inputEventsMethod;
   };
   
   if(!isObject(%player))
      return;
   MissionCleanup.add(%player);
   
   //----read data common to all types------------------------------
   if(%datablock.moveTolerance)
      %player.moveTolerance = %datablock.moveTolerance;
   if(%datablock.moveStuckTolerance)
      %player.moveStuckTolerance = %datablock.moveStuckTolerance;
   if(%datablock.moveStuckTestDelay)
      %player.moveStuckTestDelay = %datablock.moveStuckTestDelay;
   if (%obj.respawn $= "" ){
      %player.respawn=$FISH_DEFAULTRESPAWN;
   }else{
      if (%obj.respawn == true)
         %player.respawn=true;
      else
         %player.respawn=false;
   }
   if (%obj.herdId !$= ""){
      %player.setHerdId( %obj.herdId );
   }else{
      %player.setHerdId( -1 );
   }
   if(%obj.depthRange !$= ""){
      %player.depthRange = %obj.depthRange;
   }else{
      %player.depthRange = $DEPTH_LO SPC $DEPTH_HI;
   }
   %player.debugLog = (%player.index==1);
   
   //----figure out action------------------------------
   $fishType = %obj.fishType;
   if($fishType $= "" ){
      $fishType = "WANDER";
   }
   //----type specific initialization------------------------------
   %x = %player.getMoveSpeedMultiplier();
   %player.setMoveSpeedMultiplier(%player.getDataBlock().minSpeedMultiplier);
   switch$( $fishType ){
   case "SCHOOL":
      initSchool(%player, %obj);
   default:
      initSwimming(%player, %obj);   
   }
   $currentDelay += 32; //delay added to fishes schedules to even out cpu load
   return %player;
}
function AIFishPlayerScript::activityUpdate(%this)
{
   if( %this.activityFinished() )
   {
      if( isObject(%this.currentActivity) ){
         %this.currentActivity.Leave();   
         %this.currentActivity = "";
      }else{
         %this.chooseActivity();
      }
   }
  	%this.schedule($ACTIVITY_UPDATE_PERIOD,"activityUpdate");
}
function AIFishPlayerScript::chooseActivity(%this)
{
   %db = %this.getDataBlock();
   if( %db.nActivity $= "" || %db.nActivity<1 ){
      return;
   }
   %sumWeight = 0;
   for(%i=0; %i<%db.nActivity; %i++)
   {
      %sumWeight += %db.activityWeight[%i];
   }
   %roll = getRandomFloat(0,%sumWeight);
   %sum = 0;
   %iActivity = 0;
   for(%i=0; %i<%db.nActivity; %i++)
   {
      %sum += %db.activityWeight[%i];
      if( %roll<=%sum )
      {
         %iActivity = %i;
         break;      
      }   
   }
   %activityName = %db.activity[%iActivity];
   %bestActivity = AIFishPlayerScript::findActivity( %activityName, %this );
   if( isObject(%bestActivity) )
   {
      %timeLo = getWord( %db.activityTime[%iActivity], 0 );
      %timeHi = getWord( %db.activityTime[%iActivity], 1 );
      %time = getRandomFloat(%timeLo, %timeHi);
      if( %this.setActivity( %bestActivity, %time, %activityName) )
      {
         %bestActivity.Join(%this);
         %this.currentActivity = %bestActivity;
      }
   }
}
function initFollowNode(%player, %obj)
{
   //pghack, this doesnt work
   %size = %obj.getDataBlock().randomOffset;
   %offset = getRandomFloat(-%size,%size)      
               SPC getRandomFloat(-%size,%size)      
               SPC getRandomFloat(getWord(%player.depthRange,0), getWord(%player.depthRange,1));
   %player.setTransform(MatrixMultiply(%obj.gettransform(),%offset));
      
   %player.spline = %obj.spline;
   %player.followObject = %obj.spline.path1;
   %player.setAIState($FishState::FollowNode);
   %player.setFollowObject(%player.followObject, false);
   %player.setMoveSpeedMultiplier(%player.getDataBlock().minSpeedMultiplier);
}

function initSchool(%player, %obj)
{
   %player.setEnableSchool(true);
   %player.boss = %obj.boss;
   
   %player.setMoveSpeedMultiplier(%player.getDataBlock().minSpeedMultiplier);
   %player.spline = %obj.spline;
   %player.followObject = %obj.spline.path1;
   %player.oldDirection = "1 0 0";

   %player.reachedDestination = 0;
   %player.speed = 0;
   %player.oldPosition = "0 0 0";   
   %player.schoolOffset = "0 0 0";   
   
   %player.setTransform(MatrixMultiply(%obj.gettransform(),%player.schoolOffset));
   

   %player.enableCollision();
}

$initSwimmingCallCount = 0;
function initSwimming(%player, %obj)
{
   $initSwimmingCallCount = $initSwimmingCallCount + 1;
   %player.setForwardOnly(true);            
   if(%obj.wanderRange $= ""){
      %player.wanderRange = $FISH_DEFAULT_WANDER_RANGE;
   }else{
      %player.wanderRange = %obj.wanderRange;
   }
   %goodRange = 1;
   for(%index=0; %index<3; %index++){
      %lo = getWord(%player.wanderRange,%index);
      %hi = getWord(%player.wanderRange,%index+3);
      if( %lo>=%hi ){
         %goodRange = 0;
      }
   }
   if( !%goodRange ){
      echo("ERROR::" @ %player.getDataBlock().name @ " Invalid wander range " @ %player.wanderRange);         
   }
   %player.setTransform(%obj.gettransform());
   
   %nPosition = 0;
   %position[%nPosition] = %obj.position;
   %nPosition = %nPosition + 1;
   if( %obj.position2 !$= "" ){
      %position[%nPosition] = %obj.position2;
      %nPosition = %nPosition + 1;
   }
   %index =  $initSwimmingCallCount % %nPosition;
   %player.position = %position[%index];   
   %player.rotation =  "0 0 1 " @  getRandomFloat(0,360.0);
   
   //---get wandering data----------------------------------------
   %player.centerRange = %player.movePosition;
   
   %player.wanderSetRange( getWords(%player.wanderRange,0,2),
                           getWords(%player.wanderRange,3,5));
   
   %player.currentActivity = "";
  	%player.schedule($ACTIVITY_UPDATE_PERIOD + $currentDelay,"activityUpdate");
}
function getSimTimeSeconds()
{
   return getSimTime() / 1000.0;
}



//get random float in range lo -> hi
function getRandomFloat(%lo, %hi)
{
   return (getRandom()*(%hi-%lo)) + %lo;
}
//The EnhanceFOV function temporarily gives the bot a 360 degree field of vision
//This is used to emulate the bot looking around at different times. Namely when
//'Danger'.
function AIFishPlayerScript::EnhanceFOV(%this)
{
	//Is the botFOV already 360 degrees? If not then we'll set it, and set the schedule to
	//turn it back off.
	if (%this.fov != $FISH_ENHANCEDFOV)
	{
		//Sets the field of vision to 360 deg.
		%this.fov = $FISH_ENHANCEDFOV;
	}
	else
	{
		cancel(%this.fovtrigger);
	}
	//Starts the timer to disable the enhanced FOV
	%this.fovtrigger = %this.schedule($FISH_ENHANCED_FOV_TIME,"restorefov");
}

//Restore FOV sets the bot's FOV back to it's regular default setting.
function AIFishPlayerScript::restoreFOV(%this)
{
	%this.fov = $FISH_FOV;
}


//Respawn is essentially identical to 'spawn' with the exception of different calling variables.
//But the functions are basically the same, so this one is not commented. See 'spawn' for details
//if needed.
function AIFishPlayerScript::respawn(%this, %name, %obj)
{
   echo("AIFishPlayerScript::respawn");
}

//Spawn at marker is called by LoadEntities, and calls the spawn function to
//creae the bots and place them at their starting positions.
function AIFishPlayerScript::spawnAtMarker(%name,%obj,%trafficController)
{
   if (!isObject(%obj))
      return;
      
   if(%obj.spawnCount==0){
      return;
   }
   %count = %obj.spawnCount*$FISH_SPAWNCOUNT_MOD;
   if(%count<1)%count=1;
   if(%obj.fishType $= "SCHOOL"){
      %player = spawnSchool(%obj, %count, %trafficController);
   }else{
      for(%i=0; %i<%count; %i++){
         %player = AIFishPlayerScript::spawn(%name, %obj);
         %trafficController.addFish( %player );
      }
   }
      
   return %player;
}
function    spawnSchool(%obj, %count, %trafficController)
{
   //get list of positions fish can be placed at
   %nx = 2;
   %ny = 2;
   %nz = 2;
   %radius = %obj.schoolSchoolRadius;
   do{
      %nx *= 2;
      %ny *= 2;
      %nz *= 2;
      %gridSpacing = (%radius*2/(%nx-1)) SPC (%radius*2/(%ny-1)) SPC (%radius*2/(%nz-1));

      %numberInside = 0;
      %position = "0 0 0";
      for(%ix=0; %ix<%nx; %ix++){
         %x = 2*(%ix/(%nx-1))*%radius - %radius;         
         for(%iy=0; %iy<%ny; %iy++){
            %y = 2*(%iy/(%ny-1))*%radius - %radius;         
            for(%iz=0; %iz<%nz; %iz++){
               %z = 2*(%iz/(%nz-1))*%radius - %radius;         

               %p =  %x SPC %y SPC %z;
               
               //add random offsets to stop making look like grid
               %p.x += getRandomFloat(-%gridSpacing.x*0.25, %gridSpacing.x*0.25);
               %p.y += getRandomFloat(-%gridSpacing.y*0.25, %gridSpacing.y*0.25);
               %p.z += getRandomFloat(-%gridSpacing.z*0.25, %gridSpacing.z*0.25);

               %inVolume = 0;
               switch$(%obj.schoolFormation){
               case "O":
                  %inVolume = InO(%p,%radius);
               case "X":
                  %inVolume = InX(%p,%radius);
               case "ARROW":
                  %inVolume = InArrow(%p,%radius);
               case "EGG":
                  %inVolume = InEgg(%p,%radius);
               case "SPHERE":
                  %inVolume = InSphere(%p,%radius);
               case "CONE":
                  %inVolume = InCone(%p,%radius);
               case "V":
                  %inVolume = InV(%p,%radius);
               case "A":
                  %inVolume = InA(%p,%radius);
               case "TEARDROP":
                  %inVolume = InTearDrop(%p,%radius);
               default:
                  %inVolume = 1;
               }

               if(%inVolume){
                  %positionList[%numberInside] = %p;
                  %numberInside++;
               }
            }
         }
      }
   }while(%numberInside<%count)
   
   //spawn the fish
   for(%j=0; %j<%count; %j++){
      %player = AIFishPlayerScript::spawn(%name, %obj);
      %trafficController.addFish( %player );

      %index = getRandom(%numberInside-1);
      %player.setSchoolTarget(%positionList[%index], 
                              %obj.spline.path1, 
                              %obj.schoolNearDistance, 
                              %obj.schoolFarDistance);
      %player.enableSchool(true);
      %positionList[%index] = %positionList[%numberInside-1];
      
      %numberInside--;
   }
   return %player;
}
function setSchoolPosition(%player, %position)
{
   %player.schoolOffset = %position;   
}

function AIFishPlayerScript::toggleDebugHud()
{
}
function getVectorString(%v,%acc)
{
   return mFloatLength(%v.x,%acc) @ "," @ mFloatLength(%v.y,%acc) @ "," @ mFloatLength(%v.z,%acc);
}
function AIFishPlayerScript::startMove(%this, %position, %playerPosition)
{
   %this.UpdateMovePosition(%position);
}
function AIFishPlayerScript::generateSpline(%this, %count)
{
   %startPos = %this.position;
   %direction = VectorNormalize( %this.getVelocity() );
   %direction = "0 0 -1";
   %mask = $TypeMasks::StaticObjectType + $TypeMasks::TerrainObjectType;
   %endPos = VectorAdd(%startPos, VectorScale(%direction, 1000.0));
   %result = containerRayCast(%startPos, %endPos, %mask, %this);
   %hitPos = getWords(%result,1,3);
   
   %array[0] = %startPos;
   %array[1] = %hitPos;

   
}
function AIFishPlayerScript::UpdateMovePosition(%this,%position)
{
	%this.movePosition = %position;
   %this.setaimlocation(%position);
   %this.setmovedestination(%position, false);
}

function AIFishPlayerScript::stopMove(%this)
{
   %this.stop();
}


function AIFishPlayerScript::getNewSwimmingResource(%this)
{
   return AIUtilities::getRandomAIResource($AI_RESOURCE_GRASS_MASK,%this.resource,%this.getPosition(),$FISH_ROAMING_RANGE);
}

function AIFishPlayerScript::runAway(%this, %tgtPos)
{
   //The next two lines get the targets position and sets the bots destination
   //to be that position. (I tried combining this into one line and got strange
   //results. So I left it as two lines of code.)
   %pos = %this.GetPosition();
   setWord(%tgtPos,2,getWord(%pos,2)-5);// = VectorAdd(%tgtPos,"0 0 -5");
   %dest = VectorAdd(%pos,VectorScale(VectorNormalize(VectorSub(%pos,%tgtPos)),30));
   
   %this.startMove(%dest,1);
}


$selectedFish = 0;
$selectedFishWebView = 0;

singleton WebViewData(AIFishInfoWebView0) {
   class = "Movable3DWidgetWebData";
   category = "WebPage";
   canSave = "0";
   canSaveDynamicFields = "0";
   resolution = $WEBPAGE_RES;   // not for 1.7
   clientOnly = true;   // yes for 1.7

   nextAction = $WEBACTION_NONE;
   nextUrl = "";
   nextLaunchButton = "";
   nextLaunchButtonSkin = "";
   fadeDirection = 0;
   fadeState = 0;
};
$webPoolCounter = 0;

singleton WebViewData(AIFishInfoWebView1 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView2 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView3 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView4 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView5 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView6 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView7 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView8 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView9 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView10 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView11 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView12 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView13 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView14 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView15 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView16 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView17 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView18 : AIFishInfoWebView0) {};
singleton WebViewData(AIFishInfoWebView19 : AIFishInfoWebView0) {};


function debugFish(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);
   if( %this.className $= "AIFishPlayerScript" ){
      %enable = %this.getDebugLog();
      %this.setDebugLog(!%enable);
   }
}

function getWorldMouse(%camera, %pos, %vec)
{
   %hitPlane = VectorAdd(%camera.getPosition(),VectorScale(%camera.getEyeVector(), $pref::Cube::mosaicInteractionPlane));
   %upVector = "0 1 0"; // %this.getUpVector();
   %t = -(VectorDot(VectorSub(%pos, %hitPlane), %upVector) / VectorDot(%vec, %upVector));
   %destination = VectorAdd(VectorScale(%vec, %t), %pos);
   return %destination;
}
function serverCmdInfoTouchUp(%client, %shape, %pos, %vec, %mousePos, %touchId)
{
   %index = TouchCogs.getIndexFromKey(%touchId);
   if(%index>=0){
      %cog = TouchCogs.getValue(%index);
      %cog.cancel();         
      return;
   }
}

function serverCmdInfoTouchDown(%client, %shape, %pos, %vec, %mousePos, %touchId)
{
   if( $FISH_DEBUG )
   {
      debugFish(%client, %shape);
      return;
   }
   %selectedFish = %client.resolveObjectFromGhostIndex(%shape);
   
   %camera = %client.camera;
   %destination = getWorldMouse(%camera, %pos, %vec);
   %transform = MatrixCreate(%destination, "0 0 1 0");
   ServerPlay3DAngle(SonarBlipSnd, %transform);
   
   //---clamp to screen range------------------------------------------
   %cogPosition2d = transformWorldToGui(%destination, %camera);
   %bubblePosition2d = "0 0";
   %bubblePosition2d.x = mClamp(%cogPosition2d.x, $GUIRANGE_X0, $GUIRANGE_X1);
   %bubblePosition2d.y = mClamp(%cogPosition2d.y, $GUIRANGE_Y0, $GUIRANGE_Y1);
   
   //---if bubble overlaps panel boundary, move to one side of it------------   
   %loX = $GUIRANGE_X0 - $BUBBLE_RADIUS;
   %hiX = $GUIRANGE_X1 + $BUBBLE_RADIUS;
   for(%ip=1; %ip<$WEBPAGE_NUMBER_ACROSS; %ip++){
      %fraction = (%ip/$WEBPAGE_NUMBER_ACROSS);
      %x = %loX + (%hiX-%loX)*%fraction;
      %diff = (%bubblePosition2d.x-%x);
      if( %diff<$BUBBLE_RADIUS && %diff>-$BUBBLE_RADIUS ) {
         if(%diff>0){
            %bubblePosition2d.x = %x + $BUBBLE_RADIUS;
         }else{
            %bubblePosition2d.x = %x - $BUBBLE_RADIUS;
         }
         break;
      }
   }
   
   %bubblePosition = transformGuiToWorld(%bubblePosition2d, %camera);
   %cogPosition = transformGuiToWorld(%cogPosition2d, %camera);
   
   %goodPosition = 1;
   if($BUBBLE_BREAK==0){
      //---check not near launch position of imminent bubble-------------------------------------
      for(%i=0; %i<TouchCogs.count(); %i++){
         %tc = TouchCogs.getValue(%i);  
         if(%tc.fish==%selectedFish){
            %goodPosition = 0;
         }
         %sep = VectorSub(%tc.bubblePosition, %bubblePosition);
         if( VectorLen(%sep)<2*$BUBBLE_RADIUS ){
            %goodPosition = 0;
         }
      }
      //---check bubble wont be near existing one-------------------------------------
      for(%i=0; %i<InformationBubbles.count(); %i++){
         %tc = InformationBubbles.getValue(%i);  
         %sep = VectorSub(%tc.position, %bubblePosition);
         if( VectorLen(%sep)<2*$BUBBLE_RADIUS ){
            %goodPosition = 0;
         }
      }
   }
   
   //---setup cog data-------------------------------------
   if(%goodPosition==0){
      %db = BadCogSSData;
   }else{
      %db = CogSSData;
   }
   %cog = new StaticShape() {
      class = CogsClass;
      dataBlock = %db;
      position = %cogPosition;
      rotation = %camera.rotation;
      scale = $COG_SCALE SPC $COG_SCALE SPC $COG_SCALE;
      velocityMod = "1";
      gravityMod = "0";
      appliedForce = "0 0 0";
      
      //---new stuff--------------
      time = getSimTime();
      touchPosition = %mousePos;
      fish = %selectedFish;
   };
   
   %cog.goodPosition = %goodPosition;
   %cog.camera = %client.camera;
   %cog.position2d = %cogPosition2d;
   %cog.bubblePosition = %bubblePosition;
   
   if(%goodPosition==0)
   {
      //---too near existing gui, cant select fish------------------------
      %cog.cancelAppointment = %cog.schedule($COG_LIFE_TIME,"cancel");
      %cog.startFade($COG_FADEINTIME, 0, false);
   }
   else if(%cog.fish.enableInputEvents && %cog.goodPosition!=0 )
   {
      %cog.materialAnimStart("base", "c", $COG_FRAME_COUNT, $COG_LIFE_TIME);
      %cog.acceptAppointment = %cog.schedule($COG_LIFE_TIME,"accept");
      if(%cog.fish.dataBlock.interactAnim !$= "")
      {
         if( %cog.fish.className $= "AIFishPlayerScript" )
         {
            //---AI fish------------------------
            %cog.fish.enableUserTouch(true);
            %cog.fish.playThread(1, %cog.fish.dataBlock.interactAnim);
         }
         else
         {
            //---non AI fish------------------------
            %cog.fish.playThread(1, %cog.fish.dataBlock.interactAnim);
         }
      }
      else
      {
         if( %cog.fish.className $= "AIFishPlayerScript" )
         {
            %cog.fish.enableUserTouch(true);
         }
      }
      %cog.startFade($COG_BADFADEINTIME, 0, false);
   }

   %cog._client = %client;
   %cog._shape = %shape;
   %cog.updateAppointment = %cog.schedule($COG_UPDATE_TIME,"update");

   TouchCogs.add(%touchId, %cog);
}
function killBubble(%bubble)
{
   %bubble.showWebPage = 0;
   %bubble.killOff();   
}

function launchBubble(%client, %selectedFishData, %bubblePosition)
{
   if($BUBBLE_BREAK!=0){
      //---destroy bubbles that will overlap this bubbles position-------
      for(%i=0; %i<InformationBubbles.count(); %i++){
         %tc = InformationBubbles.getValue(%i);  
         if(%tc.state!=$BUBBLE_STATE_FADE_OUT){
            %sep = VectorSub(%tc.position, %bubblePosition);
            if( VectorLen(%sep)<2*$BUBBLE_RADIUS ){
               killBubble(%tc);
            }
         }
      }
   }

   %camera = %client.camera;
      
   //---setup bubble data-------------------------------------------------
   %destination = %bubblePosition;
   %bubble = new StaticShape() {
      category = "BubbleGui";
      canSave = "0";
      canSaveDynamicFields = "0";
      
      enableInputEvents = "0";
      
      dataBlock = "BubbleBodyData";
      class = "BubbleClass";
      position = %destination;
      rotation = %camera.rotation;
      scale = $BUBBLE_SCALE;
      
   };
   %bubble.master = %bubble;
   %bubble.camera = %client.camera;
   %bubble.fishData = %selectedFishData;
   %bubble.fitScale = %fitScale;
   %bubble.state = BUBBLE_STATE_FADE_IN;
   %bubble.touchEnabled = false;
   %bubble.dragEnabled = false;

   %bubble.back = addMeshPart(%bubble, "BubbleBodyBackData", %destination, %camera.rotation, $BUBBLE_SCALE, "0");
   %bubble.buttonCancel = addMeshPart(%bubble, "BubbleButtonCancelUpData", %destination, %camera.rotation, $BUBBLE_SCALE, "1");
   %bubble.buttonInformation = addMeshPart(%bubble, "BubbleButtonInformationUpData", %destination, %camera.rotation, $BUBBLE_SCALE, "1");
   %bubble.buttonTouch = addMeshPart(%bubble, "BubbleButtonTouchData", %destination, %camera.rotation, $BUBBLE_SCALE, "1");
   %bubble.buttonDrag = addMeshPart(%bubble, "BubbleButtonDragData", %destination, %camera.rotation, $BUBBLE_SCALE, "1");
   %bubble.buttonTimer = addMeshPart(%bubble, "BubbleButtonTimerData", %destination, %camera.rotation, $BUBBLE_SCALE, "0");
   %bubble.buttonTitle = addMeshPart(%bubble, "BubbleButtonTitleData", %destination, %camera.rotation, $BUBBLE_SCALE, "0");
   
   //---buffer used for mini fish inertia-----------------------------------------
   %bubble.nSpinBuffer = 0;
   for(%i=0; %i<$BUBBLE_SPINBUFFER_SIZE; %i++){
      %bubble.spinBuffer[%i] = "0 0 0 0";
   }
   %bubble.iSpinBuffer = 0;
   
   //---mini fish to put in bubble-------------------------------------
   %db = %selectedFishData;
   %bubbleDb = %db.bubbleDataBlock;
   
   %fishScale = %bubbleDb.bubbleScale * $BUBBLE_FISH_SCALE;
   %bubble.miniFish = new StaticShape() {
      category = "BubbleGui";
      enableInputEvents = "0";
      canSave = "0";
      canSaveDynamicFields = "0";

      dataBlock = %bubbleDb;
      position = %destination;
      rotation = %camera.rotation;
      scale = %fishScale SPC %fishScale SPC %fishScale;
   };
   %animName = %bubbleDb.animName;
   %bubble.minifish.playThread(0, %animName);
   %bubble.camera = %client.camera;

   %bubble.url = %db.infoUrl;
   
   %index = InformationBubbles.count();
   InformationBubbles.add(%index, %bubble);
   
   //---title text---------------------------------------------------------
   %renameString = "bubbleui_title=" @ %bubbleDb.titleMaterial;
   %bubble.buttonTitle.setSkinName(%renameString);
   
   //---initialize fish dragging stuff
   %bubble.offset = %bubbleDb.bubbleOffset;
   %bubble.fishRotationMatrix = MatrixCreate("0 0 0", "0 0 1 90");
   %bubble.resetDestroyCountdown();
   
   %bubble.lastMousePoint = "";
   %bubble.state = $BUBBLESTATE_IDLE;
   %bubble.fishAngle = "-90 0 90";
   %bubble.updateFishRotationMatrix();
   
   %bubble.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.buttonTouch.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.buttonInformation.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.buttonCancel.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.buttonTitle.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.back.startFade($BUBBLE_FADEINTIME, 0, false);
   %bubble.minifish.startFade($BUBBLE_FADEINTIME, 0, false);
   
   %bubble.targetPosition = %bubble.getPosition();
   %bubble.startTime = getSimTime();
   %bubble.reposition(%destination);
   %bubble.showWebPage = 0;
   %bubble.updateAppointment = %bubble.schedule($BUBBLE_UPDATE_TIME,update);
   %bubble.buttonTimer.materialAnimStart("timer", "ti", $BUBBLE_ANIM_FRAME_COUNT, $BUBBLE_LIFE);
   
   //make sure positioned on gui plane
   %startPosition = transformWorldToGui( %bubble.getPosition(), %bubble.camera);
   %position3d = transformGuiToWorld(%startPosition,%bubble.camera);
   %bubble.reposition(%position3d);
}
function serverCmdShowFishPage(%client, %shape, %string)
{
   echo("showing fish page");
   %this = %client.resolveObjectFromGhostIndex(%shape);
   for(%i=0; %i<$WEBPAGE_NUMBER_ACROSS; %i++)
   {
      %webData = AIFishInfoWebView @ %i;  
      if( %webData.getID() == %this.getID() ){
         launchWebPage(getFishWebRoot() @ $FISH_URL @ %string, %i, "", "", 0);
         break;
      }
   }
}
function serverCmdLaunchBubble(%client, %shape, %string)
{
   echo("launching bubble from web page");
   %this = %client.resolveObjectFromGhostIndex(%shape);
   
   // lookup fish datablock for this string
   %data = BubbleDatablocks.getValue(BubbleDatablocks.getIndexFromKey(%string));   
   
   if(isObject(%data) && isObject(%this.object))
   {
      launchBubble(%client, %data, %this.object.getPosition());
      %this.queueClose();
   }
   else
   {
      echo("ERROR: serverCmdLaunchBubble: web data not found");
   }
}

function serverCmdInfoTouchMove(%client, %shape, %pos, %vec, %mousePos, %touchId)
{
   %index = TouchCogs.getIndexFromKey(%touchId);
   if(%index>=0){
      //get projection of ray on gui plane
      %worldPos = getWorldMouse(%client.camera, %pos, %vec);
      %pos2d = transformWorldToGui(%worldPos, %client.camera);
      %pos3d = transformGuiToWorld(%pos2d, %client.camera);
      
      %cog = TouchCogs.getValue(%index);
      %move = VectorSub(%pos3d, %cog.position);   
      %distance = VectorLen(%move);
      if( %distance>$COG_CANCEL_MOVE_DISTANCE ){
         cancel(%cog.appointment);
         %cog.cancel();         
      }
   }
}

$serverCloseCount = 0;
$TIMEOUT_MINIMUM = 20*1000;

function serverCmdMovable3DWidgetDataTimeOut(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);
   if( isObject(%this) &&  (%this.getTimeViewed() > $TIMEOUT_MINIMUM) )
   {
      serverCmdMovable3DWidgetDataClose(%client, %shape);
   }
}
function serverCmdMovable3DWidgetDataClose(%client, %shape)
{
   $serverCloseCount = $serverCloseCount + 1;
   echo("webpage close request (server)" @ $serverCloseCount);
   %this = %client.resolveObjectFromGhostIndex(%shape);
   if( isObject(%this) )
   {
      %this.queueClose();
   }
}

function Movable3DWidgetWebData::onAdd()
{
   echo("here we are");
}
function Movable3DWidgetWebData::getTimeViewed(%this)
{
   if(%this.fadeState==1 && %this.currentUrl!$="")
   {
      return getSimTime() - %this.startViewTime;
   }
   else
   {
      return 0;
   }
}
function Movable3DWidgetWebData::_triggerAction(%this)
{
   switch(%this.nextAction)
   {
   case $WEBACTION_CLOSEPAGE:   
      echo("WEBACTION_CLOSEPAGE");
      if(%this.fadeState==1){
         echo("page not faded, so initiate fade");
         if(isObject(%this.object))
         {
            ServerPlay3DAngle(CloseSnd,%this.object.getTransform());
         }
         %this.startFade(-1);
      }else{
         echo("Close complete");
         %this.nextAction = $WEBACTION_NONE;
         if( %this.launchButton!$="" ){
            //change button used to launch this web page to inactive state
            %this.launchButton.setSkinName( %this.launchButtonSkin );
            %this.launchButton = "";
            %this.launchButtonSkin = "";
         }
         %this.clear();  // clear the texture ready for next use
         if(%this.object!$=""){
            %this.object.delete();
            %this.object = "";
         }
         %this.currentUrl = "";
      }
   case $WEBACTION_NEWPAGE:   
      echo("WEBACTION_NEWPAGE");
      if(%this.fadeState==1){
         if( %this.nextUrl $= %this.currentUrl ){         
            echo("page loaded! job done");
            %this.startViewTime = getSimTime();
            %this.nextAction = $WEBACTION_NONE;
         }else{
            echo("previous page up so fade it down before loading new one");
            %this.startFade(-1);
         }
      }else{
         echo("page faded so load new one");
         if(%this.object!$=""){
            %this.object.delete();
            %this.object="";            
         }
         if( %this.launchButton !$= "" ){
            %this.launchButton.setSkinName( %this.launchButtonSkin );
            %this.launchButton = "";
            %this.launchButtonSkin = "";
         }
         
         %widthPage = getWord($WEBPAGE_RES,0);
         %heightPage = getWord($WEBPAGE_RES,1);
         %webPageCenter = getWebSlotPosition(%this.slot); 
         
         %this.currentUrl = %this.nextUrl;
         %this.launchButton = %this.nextLaunchButton;
         %this.launchButtonSkin = %this.nextLaunchButtonSkin;
         
         %this.nextLaunchButton = "";
         %this.nextLaunchButtonSkin = "";
          
         // every client I'm ghosted to
         %count = ClientGroup.getCount();
         for (%i = 0; %i < %count; %i++)
         {
            %cl = ClientGroup.getObject(%i);
            %gId = %cl.getGhostID(%this.getId());
            if(%gId != -1)
               commandToClient(%cl, 'SetUrl', %gId, %this.currentUrl);
         }

         
         
 	      %cl = ClientGroup.getObject(0);
         %camera = %cl.camera;   
         %rotation = %camera.rotation;
         %shape = createWebWidget3DUseWebData(%this.currentUrl, 
                                                %webPageCenter, 
                                                %rotation, 
                                                %widthPage*$WEBPAGE_SCALE SPC %heightPage*$WEBPAGE_SCALE, 
                                                true, 
                                                %this,
                                                %camera,
                                                $WEBPAGE_OBJECT_SIZE);
                                                

         %shape.minimized = false;
         %shape.closeOnMinimize = true;
         %this.startFade(1);
      }
   }
}

function Movable3DWidgetWebData::queueClose(%this)
{
   if( %this.nextLaunchButton!$="" ){
      //Seem to be aborting page load. put back button state to unloaded
      %this.nextLaunchButton.setSkinName( %this.nextLaunchButtonSkin );
      %this.nextLaunchButton = "";
      %this.nextLaunchButtonSkin = "";
   }
   %this.nextAction = $WEBACTION_CLOSEPAGE;
   if( %this.getFadeDirection()== 0 )
   {
      %this._triggerAction();
   }
}
// %url = url to load 
// %launchButton, %launchButtonSkin = object and skin to change when close web page
function Movable3DWidgetWebData::queueOpen(%this, %url, %launchButton, %launchButtonSkin)
{
   if( %this.nextLaunchButton!$="" ){
      //Launched 2 web pages quickly one after the other.
      //put button from last one back to its inactive state
      %this.nextLaunchButton.setSkinName( %this.nextLaunchButtonSkin );
      %this.nextLaunchButton = "";
      %this.nextLaunchButtonSkin = "";
   }
   %this.nextUrl = %url;
   %this.nextLaunchButton = %launchButton;
   %this.nextLaunchButtonSkin = %launchButtonSkin;

   %this.nextAction = $WEBACTION_NEWPAGE;
   if( %this.getFadeDirection()== 0 )
   {
      %this._triggerAction();
   }
}
function Movable3DWidgetWebData::getFadeDirection(%this)
{
   return   %this.fadeDirection;
}
function Movable3DWidgetWebData::startFade(%this,%direction)
{
   %this.fadeDirection = %direction;
   if( %direction>0 ){
      echo("+start fade");
      %this.schedule($BUBBLE_FADEINTIME, fadeInEnd);
      if( isObject(%this.object) ){
         %this.object.startFade($BUBBLE_FADEINTIME, 0, false);
      }
   }else{
      echo("-start fade");
      %this.schedule($BUBBLE_FADEINTIME, fadeOutEnd);
      if( isObject(%this.object) ){
         %this.object.startFade($BUBBLE_FADEINTIME, 0, true);
      }
   }
}
function Movable3DWidgetWebData::fadeOutEnd(%this)
{
   echo("Movable3DWidgetWebData::fadeOutEnd()");
   %this.fadeState = 0;
   %this.fadeDirection = 0;
   %this._triggerAction();
}
function Movable3DWidgetWebData::fadeInEnd(%this)
{
   echo("Movable3DWidgetWebData::fadeInEnd()");
   %this.fadeState = 1;
   %this.fadeDirection = 0;
   %this._triggerAction();
}
function createWebWidget3DFromShortcut(%fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %file = new FileObject();
   %file.openForRead(%fileName);
   %line1 = %file.readLine();
   %text = %file.readLine();
   if(strIsMatchExpr("URL=*",%text,false))
   {
      %url = getSubStr(%text,4);
      createWebWidget3D(%url, %pos, %rot, %thumbWidth, %thumbHeight, %maintainAspect);
   }
}

function createWebWidget3DUseWebData(%fileName, %pos, %rot, %scale, %maintainAspect, %webData, %camera, %fitScale)
{
   echo("createWebWidget3DUseWebData() url = " @ %fileName);

//  %webView = new RigidShape() {
   %webView = new StaticShape() {
      
      class = Movable3DWidget;
      webViewData = %webData;
    //  dataBlock = Movable3DWidgetData;
      dataBlock = Static3DWidgetData;
      position = %pos;
      rotation = %rot;
      scale = %fitScale SPC %fitScale SPC %fitScale;
      canSave = "0";
      canSaveDynamicFields = "1";
      enableInputEvents = true;
      lockAspect = true;
      resizeWebViewData = false;
      allowRotation = false;
      allowScale = false;
      allowTranslation = true;
      maximizeForwardOffset = 0;
      parentGroup = MissionCleanup;
   };
   
   %webView.camera = %camera;
   %webView.position = %pos;
   %webView.rotation = %rot;
   %webData.object = %webView;
   
   %webView.distanceToCamera = $WEBPAGE_TO_CAMERA_DISTANCE;
   
   %webView.scaleObjectNative(%webData.resolution);
   %webView.position = clampToCameraPlane(%webView.camera, %webView.position, %webView.distanceToCamera);

//these functions only apply to ridid shape. used to reset physics
//   %webView.reset();
//   %webView.freezeSim(true);
   
   return %webView;
   
}

function createWebWidget3D(%fileName, %pos, %rot, %scale, %maintainAspect)
{
   
   echo("createWebWidget3D() url = " @ %fileName);
   //create web pages on the fly. Doesnt refresh pages
   %webData = new WebViewData()  {
      class = "Movable3DWidgetWebData";
      category = "WebPage";
      canSave = "0";
      canSaveDynamicFields = "0";
      url = %fileName;  
      resolution = $WEBPAGE_RES; 
      
   };
   %webData.setResolution($WEBPAGE_RES);
   %webData.setUrl(%fileName);

   %webView = new StaticShape() {
      class = Movable3DWidget;
      webViewData = %webData;
      dataBlock = "Movable3DWidgetData";
  //    position = %pos;
  //    rotation = %rot;
      scale = "0.5 0.5 0.5";
      canSave = "0";
      canSaveDynamicFields = "1";
      enableInputEvents = true;
      lockAspect = true;
      resizeWebViewData = false;
      allowRotation = false;
      allowScale = false;
      allowTranslation = false;
      maximizeForwardOffset = 0;
      parentGroup = MissionCleanup;
   };
   
   %webView.setWebViewData(%webData);
   %webView.position = %pos;
   %webView.rotation = %rot;
   
   %webView.reset();
   %webView.freezeSim(true);

   %webData.object = %webView;
   %webView.scaleObjectNative(%webData.resolution);
   
   return %webView;
}
function createImageWidget3D(%fileName, %pos, %rot, %thumbWidth, %thumbHeight, %maintainAspect)
{
   
   %webView = new StaticShape() {
      class = "Movable3DWidget";
      dataBlock = "Movable3DWidgetData";
      position = %pos;
      rotation = %rot;
      canSave = "0";
      canSaveDynamicFields = "0";
      enableInputEvents = true;
      lockAspect = true;
      resizeWebViewData = false;
      allowRotation = true;
      allowScale = true;
      allowTranslation = true;
      maximizeForwardOffset = 0;
      parentGroup = MissionCleanup;
   };
   %webView.setSkinName(%fileName);
   return %webView;
}

function AIFishPlayerScript::createMenuWidget3D(%this, %fileName, %posx, %posy)
{
   
   %frame = new StaticShape() {
      class = "MenuClass";
      dataBlock = "MenuFameData";
      position = %posx SPC 0 SPC %posy;
      rotation = %client.camera.rotation;
      scale = "0.2 0.2 0.2";
      canSave = "0";
      canSaveDynamicFields = "0";
      enableInputEvents = false;
         lockAspect = true;
         resizeWebViewData = false;
         allowRotation = true;
         allowScale = true;
         allowTranslation = true;
         parentGroup = MissionCleanup;
         
         isRenderEnabled = false;
   };
   
   %title = new StaticShape() {
      class = "MenuTitle";
      dataBlock = "MenuTitleData";
      position = %posx SPC 0 SPC %posy;
      rotation = %client.camera.rotation;
      canSave = "0";
      canSaveDynamicFields = "0";
      enableInputEvents = true;
         lockAspect = true;
         resizeWebViewData = false;
         allowRotation = true;
         allowScale = true;
         allowTranslation = true;
      menu = %frame;
         parentGroup = MissionCleanup;
         isRenderEnabled = false;
   };
   
   %closebutton = new StaticShape() {
      class = "MenuButtonClose";
      dataBlock = "MenuButtonCloseData";
      position = %posx SPC 0 SPC %posy;
      rotation = %client.camera.rotation;
      canSave = "0";
      canSaveDynamicFields = "0";
      enableInputEvents = true;
         lockAspect = true;
         resizeWebViewData = false;
         allowRotation = true;
         allowScale = true;
         allowTranslation = true;
      menu = %frame;
         parentGroup = MissionCleanup;
         isRenderEnabled = false;
   };

   %frame.title = %title;
   %frame.closebutton = %closebutton;
   
   %frame.OnAdd();
   %frame.instantMinimize();
   
   %frame.mountObject(%closebutton,0);
   %frame.mountObject(%title,1);
   
   
   %this.menu = %frame;
   %this.schedule(0, showMenu);
   
   return %frame;
}
function AIFishPlayerScript::showMenu(%this)
{
   if(isObject(%this.menu))
   {
      %this.menu.isRenderEnabled = true;
      %this.menu.title.isRenderEnabled = true;
      %this.menu.closebutton.isRenderEnabled = true;
   }
}
function AIFishPlayerScript::hideMenu(%this)
{
   if(isObject(%this.menu))
   {
      %this.menu.isRenderEnabled = false;
      %this.menu.title.isRenderEnabled = false;
      %this.menu.closebutton.isRenderEnabled = false;
   }
}
function AIFishPlayerScript::onSelect(%this)
{
}

function AIFishPlayerScript::onDeselect(%this)
{
}

function serverCmdMenu_OnCloseDown(%client, %shape)
{
}

function serverCmdMenu_OnCloseUp(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 

   %this.menu.close();
}

function MenuClass::minimize(%this)
{  
   if(%this.minimized)
      return;
      
   %this.info.minimize();
   %this.image1.minimize();
   %this.image2.minimize();
   %this.movie1.minimize();

   %this.playThread(0, "minimize");
   %this.title.playThread(0, "minimize");
   %this.closebutton.playThread(0, "minimize");
 
   %this.setObjectBox("-0.5 -0.5 -0.5 0.5 6.0 0.5");
   %this.reset();
   
   
   %this.minimized = true;
}
function MenuClass::maximize(%this)
{
   if(!%this.minimized)
      return;
   %this.minimized = false;
      
   %this.playThread(0, "maximize");
   %this.title.playThread(0, "maximize");
   %this.closebutton.playThread(0, "maximize");

   %this.schedule(1000, setThreadPosition, 0, 1);

   %this.setObjectBox("-0.5 -0.5 -0.5 6.0 0.5 0.5");
   %this.reset();
   
}

function MenuClass::instantMinimize(%this)
{
   %this.minimize();
   %this.setThreadPosition(0, 1);
   %this.title.setThreadPosition(0, 1);
   %this.closebutton.setThreadPosition(0, 1);
}
function MenuClass::close(%this)
{
   %this.selectedObject.onDeselect();
   %this.selectedObject.selected = false;
   
   %this.minimize();
   %this.schedule(1000, deleteAll);
}
function MenuClass::deleteAll(%this)
{
      %this.info.delete();
      %this.image1.delete();
      %this.image2.delete();
      %this.movie1.delete();
   
   
   %this.title.delete();
   %this.closebutton.delete();
   %this.delete();
}

function MenuClass::OnAdd(%this)
{
   %this.minimized = false;
   %this.freezeSim(true);
}


function isReallyButton(%thing)
{
   if( !isObject(%thing) ){  
      return false;      
   }
   if( %thing.className !$= "BubbleButtonClass" ){
      echo("object masquerading as button");
      return false;
   }
   return true;
}

//**********************************************************************
// button action from user
//**********************************************************************
function serverCmdBubbleButtonDown(%client,%shape, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   %button = %client.resolveObjectFromGhostIndex(%shape);
   if( !isReallyButton(%button) ){
      return;
   }
   %master = %button.master;
   %master.buttonDown(%button, %mousePoint, %pos, %vec, %mouseUVCoord);
}
function serverCmdBubbleButtonUp(%client,%shape, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   %button = %client.resolveObjectFromGhostIndex(%shape);
   if( !isReallyButton(%button) ){
      return;
   }
   %master = %button.master;
   %master.buttonUp(%button, %mousePoint, %pos, %vec, %mouseUVCoord);
}
function serverCmdBubbleButtonDragged(%client,%shape, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   %button = %client.resolveObjectFromGhostIndex(%shape);
   if( !isReallyButton(%button) ){
      return;
   }
   %master = %button.master;
   %master.buttonDragged(%button, %mousePoint, %pos, %vec, %mouseUVCoord);
}
//**********************************************************************
// Main controlling class for bubbles
//**********************************************************************
function BubbleClass::buttonUp(%this,%which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.state==$BUBBLE_STATE_FADE_IN )return;
   if( %this.state==$BUBBLE_STATE_FADE_OUT )return;

   %this.targetPosition = %this.getPosition();
   
   if(%this.buttonCancel == %which ){
      %this.killOff();
   }else if(%this.buttonDrag == %which ){
      %this.dragEnabled = false;
   }else if(%this.buttonTouch == %which ){
      %this.touchEnabled = false;
      
      //get spin inertia for fish
      %nowTime = getRealTime();
      %this.spinInertia = "0 0 0";
      for(%i=0; %i<%this.nSpinBuffer; %i++){
         %time = getWord(%this.spinBuffer[%i],0);
         %timeSince = %nowTime - %time;
         if( %timeSince < $BUBBLE_SPIN_INERTIA_TIME ){
            
            %p0 =  getWords(%this.spinBuffer[%i],1,3);
            %p1 =  getWords(%this.spinBuffer[%this.nSpinBuffer-1],1,3);
            %t0 = getWord(%this.spinBuffer[%i],0);
            %t1 = getWord(%this.spinBuffer[%this.nSpinBuffer-1],0);
            %interval = %t1 - %t0;
            if( %interval>0 ){
               %delta = VectorSub(%p1, %p0);
               %this.spinInertia = VectorScale(%delta, -1.0 / %interval );            
            }
            break;
         }
      }
      %this.spinInertia.z = 0;
   }else if(%this.buttonInformation == %which ){
      %this.showWebPage = 1;
      %this.killOff();
   }
}
function BubbleClass::buttonDown(%this,%which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.state==$BUBBLE_STATE_FADE_IN )return;
   if( %this.state==$BUBBLE_STATE_FADE_OUT )return;

   %this.resetDestroyCountdown();
   if(%this.buttonTouch == %which ){
      %this.nSpinBuffer = 0;
      %this.touchEnabled = true;
      %this.lastMousePoint = getWorldMouse(%this.camera, %pos, %vec);
   }else if(%this.buttonDrag == %which ){
      %this.moving = true;
      %this.dragEnabled = true;
      %this.dragStartTouchPosition = getWorldMouse(%this.camera, %pos, %vec);
      %this.dragStartBubblePosition = %this.getPosition();
      %this.targetPosition = %this.getPosition();
   }else if(%this.buttonCancel == %which ){
      ServerPlay3DAngle(CloseSnd,%this.getTransform());
      %this.buttonCancel.setDataBlock( "BubbleButtonCancelDownData" );
   }else if(%this.buttonInformation == %which ){
      ServerPlay3DAngle(Button1Snd,%this.getTransform());
      %this.buttonInformation.setDataBlock( "BubbleButtonInformationDownData" );
   }
}
function BubbleClass::resetDestroyCountdown(%this)
{
   %this.startTime = getSimTime();
   %this.buttonTimer.materialAnimSetTime(0);
}
function BubbleClass::reposition(%this, %destination)
{
   %rotation = MatrixCreate("0 0 0", %this.camera.rotation);

   %translation = MatrixCreate(%destination, "0 0 1 0");
   %matrix = MatrixMultiply(%translation, %rotation);   
   %this.setTransform(%matrix);

   %eyeToObject = VectorSub(%destination, %this.camera.getPosition()  );
   %eyeToObject = VectorNormalize( %eyeToObject );

   %backPos = VectorAdd(%destination, VectorScale(%eyeToObject, $BUBBLE_BACK_PLANE_DISTANCE) );
   %translation = MatrixCreate(%backPos, "0 0 1 0");
   %matrix = MatrixMultiply(%translation, %rotation);   
   %this.back.setTransform(%matrix);

   %buttonPos = VectorSub(%destination, VectorScale(%eyeToObject, $BUBBLE_BUTTON_DISTANCE) );
   %translation = MatrixCreate(%buttonPos, "0 0 1 0");
   %matrix = MatrixMultiply(%translation, %rotation);   
   %this.buttonCancel.setTransform(%matrix);
   %this.buttonInformation.setTransform(%matrix);
   %this.buttonTouch.setTransform(%matrix);
   %this.buttonTimer.setTransform(%matrix);
   %this.buttonTitle.setTransform(%matrix);

   %buttonDragPos = VectorSub(%destination, VectorScale(%eyeToObject, $BUBBLE_DRAG_DISTANCE) );
   %this.buttonDrag.position = %buttonDragPos;
   
   %this.updateMiniFishTransform();
}
function BubbleClass::updateMiniFishTransform(%this)
{
   %translation = MatrixCreate(%this.position, "0 0 1 0");
   %offsetMatrix = MatrixCreate(%this.offset, "0 0 1 0");
   %m = MatrixMultiply(%this.fishRotationMatrix, %offsetMatrix);
   %m = MatrixMultiply(%translation, %m);
   %this.miniFish.setTransform(%m);
}

function BubbleClass::updateFadeIn(%this)
{
   %timeSinceStart = getSimTime() - %this.startTime;
   %fraction = %timeSinceStart / $BUBBLE_FADEINTIME;
   if( %fraction>=1.0 ){
      %this.state = $BUBBLE_STATE_IDLE;   
   }
   %this.reposition( %this.getPosition() );
   %this.updateAppointment = %this.schedule($BUBBLE_UPDATE_TIME,update);
}
function BubbleClass::deleteBubble(%this)
{
   if( %this.showWebPage ){
      %this.showInformation();
   }
   %index = InformationBubbles.getIndexFromValue(%this);
   if(%index<0){
      echo("Failed to find bubble");
   }else{
      InformationBubbles.erase(%index);
   }
   cancel(%this.updateAppointment);
   %this.back.delete();
   %this.buttonTouch.delete();
   %this.buttonCancel.delete();
   %this.buttonInformation.delete();
   %this.buttonDrag.delete();
   %this.buttonTitle.delete();
   %this.buttonTimer.delete();
   %this.miniFish.delete();
   %this.delete();
}
function BubbleClass::updateFadeOut(%this)
{
   %timeSinceStart = getSimTime() - %this.startTime;
   %fraction = %timeSinceStart / $BUBBLE_FADEINTIME;
   %this.reposition( %this.getPosition() );
   
   if( %fraction>=1.0 )
   {
      %this.deleteBubble();
   }   
   else
   {
      %this.updateAppointment = %this.schedule($BUBBLE_UPDATE_TIME,update);
   }
}
function isBubble(%this)
{
   %flag =  (%this.class $= "BubbleClass");
   return %flag;
}
function isCog(%this)
{
   %flag =  (%this.class $= "CogsClass");
   return %flag;
}

function BubbleClass::updateIdle(%this)
{
   //mini fish inertia

   if( !%this.touchEnabled ){
      %this.spinInertia = VectorScale(%this.spinInertia, ($BUBBLE_SPIN_DRAG_TIME - $BUBBLE_UPDATE_TIME) / $BUBBLE_SPIN_DRAG_TIME);
      %delta = VectorScale(%this.spinInertia, $BUBBLE_UPDATE_TIME);
      
      
      %this.fishAngle.x = mFMod(%this.fishAngle.x + %delta.x*$BUBBLE_SPIN_SPEED, 360);
      %this.fishAngle.y += %delta.z*$BUBBLE_SPIN_SPEED;
      %this.fishAngle.y = mClamp(%this.fishAngle.y, $BUBBLE_MIN_TILT, $BUBBLE_MAX_TILT);
      %this.updateFishRotationMatrix();
      %this.updateMiniFishTransform();
   }
   
   //see if should end bubble
   %nowTime = getSimTime();
   %interval = %nowTime - %this.startTime;
   if( %interval>=  $BUBBLE_LIFE ){
      %this.killOff();
      %this.updateAppointment = %this.schedule($BUBBLE_UPDATE_TIME,update);
      return;
   }
   
   %startPosition = transformWorldToGui( %this.getPosition(), %this.camera);
   %targetPosition = transformWorldToGui( %this.targetPosition, %this.camera);
   %sep = VectorSub(%targetPosition, %startPosition);
   %distanceToGo = VectorLen(%sep)*$BUBBLE_MOVE_URGENCY;
   if( %distanceToGo<=0.001 ){
      %this.updateAppointment = %this.schedule($BUBBLE_UPDATE_TIME,update);
      return;
   }
   %move = VectorScale(%sep, $BUBBLE_MOVE_URGENCY);

   %delta = VectorLen(%move);
   if( %delta>$BUBBLE_MAX_MOVE){
      %move = VectorScale(%move,$BUBBLE_MAX_MOVE/%delta);
   }
   %distanceToGo = VectorLen( %move );
   
   %destination = VectorAdd(%startPosition, %move);
   
   %destination.x = mClamp(%destination.x, $GUIRANGE_X0, $GUIRANGE_X1);
   %destination.y = mClamp(%destination.y, $GUIRANGE_Y0, $GUIRANGE_Y1);
   
   %endPosition = %destination;
   
   %hitSomething = 0;
   %iterations = 0;   
   %lastHitObject = "";
   while( (%distanceToGo>0) && (%iterations<2) ){
      
      %centerSearch = VectorAdd( %startPosition, VectorScale(%move, 0.5) );
      %radiusSearch = %distanceToGo*0.5 + (2.5*$BUBBLE_RADIUS);
      
      %centerSearch_w = transformGuiToWorld(%centerSearch, %this.camera);
      
      InitContainerRadiusSearch(%centerSearch_w, 
                                 %radiusSearch, 
                                 $TypeMasks::StaticObjectType);
      %bestFraction = 1.0;                                 
      %bestHit = "";
      %overlap = 0;
      while ( (%other = containerSearchNext()) != 0 )
      {
         if( %this==%other )continue; 
         if( %lastHitObject ==%other )continue; //should be moving away from this object
         if(%other.state==$BUBBLE_STATE_FADE_OUT)continue;
         if( !isBubble(%other) )continue;

         %otherPosition = transformWorldToGui(%other.getPosition(), %this.camera);
         %w = VectorSub(%startPosition, %otherPosition);
         %distanceApart = VectorLen(%w);
         if( %distanceApart<2*$BUBBLE_RADIUS ){
            if( VectorDot(%w, %move)<0 ){            
               %overlap = 1;
               %bestFraction = 0;
               %bestHit = %other;
            }
         }else{
            %f = getTouchPosition(%startPosition, %endPosition, %otherPosition, $BUBBLE_RADIUS);
            if( %f < %bestFraction ){
               %bestFraction = %f;
               %bestHit = %other; 
            }
         }
      }
      if( isObject(%bestHit) ){
         if( !%bestHit.dragEnabled && isBubble(%bestHit) ){
            %bestHit.targetPosition = %bestHit.getPosition();
         }
         %hitSomething = 1;
         %lastHitObject = %bestHit;
         %hitObjectPosition = transformWorldToGui(%bestHit.getPosition(), %this.camera);
         if( %overlap ){
            %radial = VectorSub(%startPosition, %hitObjectPosition);
            %len = VectorLen(%radial);
            if( %len == 0 ){  //bubbles on top of each other
               %normal = "1 0 0";
            }else{
               %normal = VectorScale(%radial, 1/%len);
            }
            %newPosition = VectorAdd(%hitObjectPosition, VectorScale(%normal, 2*$BUBBLE_RADIUS));
         }else{
            %vv = VectorSub(%endPosition, %startPosition);
            %newPosition = VectorAdd(%startPosition, VectorScale(%vv,%bestFraction));   //move to contact position
            %radial = VectorSub(%startPosition, %hitObjectPosition);
            %len = VectorLen(%radial);
            if( %len == 0 ){  //bubbles on top of each other
               %normal = "1 0 0";
            }else{
               %normal = VectorScale(%radial, 1/%len);
            }
         }
         %delta = VectorSub(%newPosition, %startPosition);
         %distanceToGo -= VectorLen(%delta);
         %startPosition = %newPosition;
         
         //project movement along surface
         %move = VectorSub(%destination, %startPosition);
         %projection = VectorDot(%move, %normal);
         %move = VectorSub(%move, VectorScale(%normal, %projection));
         %endPosition = VectorAdd(%startPosition, %move);
      }else{
         %finished = 1;
         %startPosition = %endPosition;
         %distanceToGo = 0;         
      }
      %iterations = %iterations + 1;
   }
   

   if( isObject(%lastHitObject) ){
      %v0 = %startPosition;
      %v1 = transformWorldToGui(%lastHitObject.getPosition(), %this.camera);
      %v10 = VectorSub(%v1, %v0);
      %len = VectorLen(%v10);
      if(%len< 1.9*$BUBBLE_RADIUS){
         echo("whoops");
      }
   }

   if( %startPosition.x>=$GUIRANGE_X0  
      &&  %startPosition.x<=$GUIRANGE_X1 
      && %startPosition.y>=$GUIRANGE_Y0 
      && %startPosition.y<=$GUIRANGE_Y1 )
   {
      %position3d = transformGuiToWorld(%startPosition,%this.camera);
      %this.reposition(%position3d);
      if( %hitSomething && (!%this.dragEnabled) ){
         %this.targetPosition = %position3d;
      }
   }else{
//      echo("off screen");      
   }
   %this.updateAppointment = %this.schedule($BUBBLE_UPDATE_TIME,update);
}

$TIMER_ANIMATION_FRAMES = 128;

function BubbleClass::update(%this)
{
   if(%this.state == $BUBBLE_STATE_FADE_IN){
      %this.updateFadeIn();
   }else if(%this.state == $BUBBLE_STATE_FADE_OUT){
      %this.updateFadeOut();
   }else{
      %this.updateIdle();
   }
   
}

// get fraction along path to first collision point for two circles
// returns 1 if there are none
function getTouchPosition(%start, %end, %target, %radius)
{
   %e = VectorSub(%start, %target);
   %f = VectorSub(%end, %start);
   %A = VectorDot(%f,%f);
   %B = 2*VectorDot(%e,%f);
   %C = VectorDot(%e,%e) - (4*%radius*%radius);
   
   %fraction = 1;
   %sol = mSolveQuadratic(%A, %B, %C);
   %nSol = getWord(%sol, 0);
   for(%i=0; %i<%nSol; %i++){
      %root = getWord(%sol, %i+1);
      if(%root>=0 && %root<%fraction ){
         %fraction = %root;
      }
   }
   return %fraction;
}


function BubbleClass::buttonDragged(%this, %which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.state==$BUBBLE_STATE_FADE_IN )return;
   if( %this.state==$BUBBLE_STATE_FADE_OUT )return;
   
   %this.resetDestroyCountdown();
   %touchPosition = getWorldMouse(%this.camera, %pos, %vec);
   if(%this.buttonDrag == %which ){
      if( !%this.dragEnabled ){
         %this.moving = true;
         %this.dragEnabled = true;         
         %this.dragStartTouchPosition = %touchPosition;
         %this.dragStartBubblePosition = %this.getPosition();
         %this.targetPosition = %this.getPosition();
      }else{
         %move = VectorSub(%touchPosition, %this.dragStartTouchPosition);
         %this.targetPosition = VectorAdd(%move, %this.dragStartBubblePosition);
      }
   }else if(%this.buttonTouch == %which ){
      
      if( %this.nSpinBuffer == $BUBBLE_SPINBUFFER_SIZE ){
         %this.nSpinBuffer = $BUBBLE_SPINBUFFER_SIZE - 1;
         for(%i=0; %i< ($BUBBLE_SPINBUFFER_SIZE-1); %i++){
            %this.spinBuffer[%i] = %this.spinBuffer[%i+1];           
         }
      }
      %this.spinBuffer[%this.nSpinBuffer] = getRealTime() SPC %touchPosition;
      %this.nSpinBuffer = %this.nSpinBuffer + 1; 
      
      %delta = VectorSub(%this.lastMousePoint, %touchPosition);
         
      %this.fishAngle.x = mFMod(%this.fishAngle.x + %delta.x*$BUBBLE_SPIN_SPEED, 360);
      %this.fishAngle.y += %delta.z*$BUBBLE_SPIN_SPEED;
      %this.fishAngle.y = mClamp(%this.fishAngle.y, $BUBBLE_MIN_TILT, $BUBBLE_MAX_TILT);
      %this.updateFishRotationMatrix();
      
      %this.updateMiniFishTransform();
      %this.lastMousePoint = %touchPosition;
   }
}
function BubbleClass::updateFishRotationMatrix(%this)
{
   %db = %this.fishData.bubbleDataBlock;
   %rotY = MatrixCreate("0 0 0", "1 0 0" SPC -%this.fishAngle.y);
   %rotX = MatrixCreate("0 0 0", "0 0 1" SPC %this.fishAngle.x);
   %rotX = MatrixMultiply(%rotX, %db.bubbleRotation);    
   %this.fishRotationMatrix = MatrixMultiply(%rotY, %rotX);    
}
function BubbleClass::killOff(%this)
{
   if( %this.state==$BUBBLE_STATE_FADE_OUT ){
      return;
   }
   
   %this.state = $BUBBLE_STATE_FADE_OUT;
   %this.startTime = getSimTime();
   %this.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.buttonTouch.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.buttonInformation.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.buttonCancel.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.buttonTimer.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.buttonTitle.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.back.startFade($BUBBLE_FADEINTIME, 0, true);
   %this.minifish.startFade($BUBBLE_FADEINTIME, 0, true);
}


function BubbleClass::getWebPageSlot(%this)
{
   //get current bubble position in camera space   
   %forward = %this.camera.getForwardVector();   
   %right = %this.camera.getRightVector();   
   %up = %this.camera.getUpVector();   
   %pos = %this.camera.getPosition();   
   
   %rel = VectorSub(%this.position, %pos);
   
   %bubbleX = VectorDot(%rel, %right);
   %bubbleZ = VectorDot(%rel, %forward);
   %bubbleTan = %bubbleX / %bubbleZ;
      
   %field = $CAMERA_FIELD;
   %xRange = mTan( mDegToRad( %field * 0.5 ) );
   
   %xLeft = -%xRange;
   %xRight = %xRange;
   %pageSize = (%xRight-%xLeft)/($WEBPAGE_NUMBER_ACROSS);
   for(%i=0; %i<$WEBPAGE_NUMBER_ACROSS; %i++){
      %x0 = %xLeft + %pageSize*%i;
      %x1 = %xLeft + %pageSize*(%i+1);
      
      if( %bubbleTan>=%x0 && %bubbleTan<=%x1 ){
         return %i;
      }
   }
   return 0;  
}

function BubbleClass::showInformation(%this)
{
   %fishData = %this.fishData;
   %slot = %this.getWebPageSlot();
   launchWebPage(%fishData.infoUrl, %slot, "", "", 0);
}
/*
function serverCmdWebPageActive(%client, %shape)
{
   %webPage = %client.resolveObjectFromGhostIndex(%shape);
   if( !isObject(%webPage) )return;
   if( %webPage.className !$= "Movable3DWidget" )return;
}
*/
function Movable3DWidget::selfDestruct(%this)
{
   echo("selfDestruct");
   %wd = %this.webViewData;
   if( isObject(%wd) )
   {
      %wd.schedule($BUBBLE_FADEINTIME, fadeOutEnd);
      if( isObject(%wd.object) ){
         %wd.object.startFade($BUBBLE_FADEINTIME, 0, true);
      }
   }
}
function Movable3DWidget::restartDestructSequence(%this)
{
   cancel( %this.destructionAppointment );
   %this.destructionAppointment = %this.schedule($WEBPAGE_LIFE, selfDestruct);
}

////////////////////////////////////////////////////////////////////////////////
// Activity markers
////////////////////////////////////////////////////////////////////////////////
datablock StaticShapeData(ActivityMarker)
{
   activityName = "swimming";
   category = "AIFishPlayer Activity";
   shapeFile = "core/art/shapes/unit_cube.dts";
};
//------------------------------------------------------
datablock StaticShapeData(SwimmingMarker : ActivityMarker)
{
   activityName = "Swimming";
};
//------------------------------------------------------
datablock StaticShapeData(FeedingMarker : ActivityMarker)
{
   activityName = "Feeding";
};
//------------------------------------------------------
datablock StaticShapeData(HidingMarker : ActivityMarker)
{
   activityName = "Hiding";
};


function canReachObject(%object, %fish)
{
   %toObject = VectorSub(%object.getPosition(), %fish.getPosition());
   %distanceAway = VectorLen( %toObject );
   if( %distanceAway>$ACTIVITY_MAX_DISTANCE ){
      return false;
   }
   return true;
}

//------------------------------------------------------
function SwimmingActivityClass::OnAdd(%this)
{
   %this.currentObject = "";
}
function SwimmingActivityClass::Join(%this, %aiObject)
{
   if(!isObject(%this.currentObject))
   {
      %this.currentObject = %aiObject;
      return true;
   }
   else
      return false;
}
function SwimmingActivityClass::Leave(%this, %aiObject)
{
   %this.currentObject = "";
}
function SwimmingActivityClass::isFree(%this, %fish)
{
   return (!isObject(%this.currentObject)) && canReachObject(%this, %fish);
}
//------------------------------------------------------
function FeedingActivityClass::OnAdd(%this)
{
   %this.currentObject = "";
}
function FeedingActivityClass::Join(%this, %aiObject)
{
   if(!isObject(%this.currentObject))
   {
      %this.currentObject = %aiObject;
      return true;
   }
   else
      return false;
}
function FeedingActivityClass::Leave(%this, %aiObject)
{
   %this.currentObject = "";
}
function FeedingActivityClass::isFree(%this, %fish)
{
   return (!isObject(%this.currentObject)) && canReachObject(%this, %fish);
}
//------------------------------------------------------
function HidingActivityClass::OnAdd(%this)
{
   %this.currentObject = "";
}
function HidingActivityClass::Join(%this, %aiObject)
{
   if(!isObject(%this.currentObject))
   {
      %this.currentObject = %aiObject;
      return true;
   }
   else
      return false;
}
function HidingActivityClass::Leave(%this, %aiObject)
{
   %this.currentObject = "";
}
function HidingActivityClass::isFree(%this, %fish)
{
   return (!isObject(%this.currentObject)) && canReachObject(%this, %fish);
}

////////////////////////////////////////////////////////////////////////////////
//

datablock StaticShapeData(BubbleSData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/popup_timer.dts";
};


//*******************************************************************************************************
function BubbleButtonClass::onAdd(%this)
{
}
function BubbleButtonClass::onUnselect(%this)
{
}
function BubbleButtonClass::onSelect(%this)
{
}
function CogsClass::cancel(%this)
{
   %index = TouchCogs.getIndexFromValue(%this);
   if(%index<0){
      echo("Failed to find trigger");
   }else{
      TouchCogs.erase(%index);
   }
   cancel(%this.acceptAppointment);
   cancel(%this.updateAppointment);
   if( %this.goodPosition!=0 && %this.fish.className $= "AIFishPlayerScript" ){
      %this.fish.enableUserTouch(false);

   }
   %this.delete();
}
function CogsClass::update(%this)
{
   %this.setScale($COG_SCALE SPC $COG_SCALE SPC $COG_SCALE);
   %this.updateAppointment = %this.schedule($COG_UPDATE_TIME,"update");
}
function CogsClass::accept(%this)
{
   %index = TouchCogs.getIndexFromValue(%this);
   if(%index<0){
      echo("Failed to find cogs");
   }else{
      TouchCogs.erase(%index);
   }
   
   if( $FISH_DEBUG )
   {
      debugFish(%this._client, %this._shape);
      return;
   }
   %selectedFish = %this._client.resolveObjectFromGhostIndex(%this._shape);
   
   launchBubble(%this._client, 
               %selectedFish.getDataBlock(), 
               %this.bubblePosition);
               
   cancel(%this.updateAppointment);
   if( %this.fish.className $= "AIFishPlayerScript" ){
      %this.fish.enableUserTouch(false);
   }
   %this.delete();
}

//**********************************************************************************************
// transform between gui plane and world coordinates
//**********************************************************************************************
function transformWorldToGui(%worldPos, %camera)
{
   %rel = VectorSub(%worldPos, %camera.getPosition());
   %x = VectorDot(%rel, %camera.getRightVector());
   %y = VectorDot(%rel, %camera.getUpVector());
   %z = VectorDot(%rel, %camera.getForwardVector()) ;
   
   %x = (%x/%z) * $pref::Cube::mosaicInteractionPlane;
   %y = (%y/%z) * $pref::Cube::mosaicInteractionPlane;
   return %x SPC %y SPC "0";
}
function transformGuiToWorld(%guiPos, %camera)
{
   %pos = VectorAdd(%camera.getPosition(), VectorScale(%camera.getForwardVector(),$pref::Cube::mosaicInteractionPlane));
   %pos = VectorAdd(%pos, VectorScale(%camera.getRightVector(), %guiPos.x));
   %pos = VectorAdd(%pos, VectorScale(%camera.getUpVector(), %guiPos.y));
   return %pos;
}
//**********************************************************************
// task bar
//**********************************************************************
$TASKBAR_OFFSET = "-2.213";
$CAMERA_TILT = 7.9;
$CAMERA_POSITION = "0 0 0";


datablock StaticShapeData(TaskBarData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/taskbar_body.dae";
};
//********************************************************
datablock StaticShapeData(IconTaskBarData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/icon_taskbar.dae";
   class = "BubbleButtonData";
};

function startBackgroundSounds()
{
   echo("starting background sounds");
   
 // play on top machines only
   for(%idx = 0; %idx < ClientGroup.getCount(); %idx++)
   {
      %cl = ClientGroup.getObject(%idx);
      %clientID = %cl.clientId;
      %offset = $pref::Cube::demo1Offset[%clientId];
      %topfraction = getWord(%offset,3);

      // is it a top screen or standalone pc version
      if(%clientID==11 || %clientID==12 || %clientID==0 || %clientID=13 )
      {
         %cl.play2D(StereoUnderwaterHumSnd);
      }
   }
}
function TaskBar::spawn()
{
   schedule($pref::Cube::delayForClients, 0, startBackgroundSounds);    //without this delay, sound doesnt happen:(


   %tilt = mDegToRad($CAMERA_TILT);
   %sin = mSin(%tilt);
   %cos = mCos(%tilt);
   %cameraX = "1 0 0";
   
   %cameraZ = "0" SPC %cos SPC (%sin*-1.0);
   %cameraY = "0" SPC %sin SPC %cos;
   
   %field = $CAMERA_FIELD_TASKBAR;
   %xRange = mTan( mDegToRad( %field * 0.5 ) ) * $WEBPAGE_TO_CAMERA_DISTANCE;
   %xLeft = -%xRange;
   %xRight = %xRange;
   %barSize = (%xRight-%xLeft)/$WEBPAGE_NUMBER_ACROSS;
   
   %tbRotation = "1 0 0 " @ $CAMERA_TILT;
   if($Pref::Cube::QueenslandMuseum)
   {
      return;  //no taskbar for queensland museum
   }
   for(%i=0; %i<$WEBPAGE_NUMBER_ACROSS; %i++)
   {
      %x0 = %xLeft + %barSize*%i;
      %x1 = %xLeft + %barSize*(%i+1);
      
      %tbPosition = VectorAdd($CAMERA_POSITION, VectorScale(%cameraZ,$WEBPAGE_TO_CAMERA_DISTANCE) );
      %tbPosition = VectorAdd(%tbPosition, VectorScale(%cameraX,%x0) );
      %tbPosition = VectorAdd(%tbPosition, VectorScale(%cameraY,$TASKBAR_OFFSET) );
      %tbScale = %barSize SPC 1.0 SPC %barSize*(95/1080)*0.55;

      //---background of bar-------------------------------------
      $taskBar[%i] = new StaticShape() {
         category = "TaskBar";
         canSave = "0";
         canSaveDynamicFields = "0";
         
         enableInputEvents = "0";
         
         dataBlock = "TaskBarData";
         class = "TaskBar";
         position = %tbPosition;
         rotation = %tbRotation;
         scale = %tbScale;
      };
      $taskBar[%i].slot = %i;      
      
      %scale = %barSize*(95/1080);
      %scale = %scale * 0.5;

      %buttonPos = VectorAdd(%tbPosition, VectorScale(%cameraZ, -0.01));

      %buttonPos = VectorAdd(%buttonPos, VectorScale(%cameraY, 0.005));
      %buttonPos = VectorAdd(%buttonPos, VectorScale(%cameraX, 2.0*%scale));
            
      %buttonScale = %scale SPC %scale SPC (%scale*1.0);
      
      %buttonSpacing = 0.04;
      %buttonXOffset = (%barSize - (%buttonSpacing*3)) * 0.5;
      %buttonXOffset = %buttonXOffset - 0.01;
      %buttonXOffset = %buttonXOffset - 0.02;
      
      %iconPos = %buttonPos;
      %iconPos.x = %iconPos.x + %buttonXOffset;
      
      $taskBar[%i].species = addMeshPart($taskBar[%i], "IconTaskBarData", %iconPos, %tbRotation, %buttonScale, "1");
      $taskBar[%i].species.setSkinName("spup");
      %iconPos.x = %iconPos.x + 0.04;

      $taskBar[%i].conservation = addMeshPart($taskBar[%i], "IconTaskBarData", %iconPos, %tbRotation, %buttonScale, "1");
      $taskBar[%i].conservation.setSkinName("coup");
      %iconPos.x = %iconPos.x + 0.04;

      $taskBar[%i].future = addMeshPart($taskBar[%i], "IconTaskBarData", %iconPos, %tbRotation, %buttonScale, "1");
      $taskBar[%i].future.setSkinName("crup");
      %iconPos.x = %iconPos.x + 0.04;

      $taskBar[%i].credits = addMeshPart($taskBar[%i], "IconTaskBarData", %iconPos, %tbRotation, %buttonScale, "1");
      $taskBar[%i].credits.setSkinName("nsup");
      %iconPos.x = %iconPos.x + 0.04;

      if($Pref::Cube::demoMode){
         %buttonPos.x = %buttonPos.x + 0.15;
         $taskBar[%i].view1 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");
         
         %buttonPos.x = %buttonPos.x + 0.03;
         $taskBar[%i].view2 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");
      
         %scale = %scale * 0.5;
         %buttonPos.x = %buttonPos.x + 0.03;
         %buttonScale = %scale SPC %scale SPC %scale;
         $taskBar[%i].mini1 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");

         %buttonPos.x = %buttonPos.x + 0.02;
         %buttonScale = %scale SPC %scale SPC %scale;
         $taskBar[%i].mini2 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");

         %buttonPos.x = %buttonPos.x + 0.02;
         %buttonScale = %scale SPC %scale SPC %scale;
         $taskBar[%i].mini3 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");

         %buttonPos.x = %buttonPos.x + 0.02;
         %buttonScale = %scale SPC %scale SPC %scale;
         $taskBar[%i].mini4 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");

         %buttonPos.x = %buttonPos.x + 0.02;
         %buttonScale = %scale SPC %scale SPC %scale;
         $taskBar[%i].mini5 = addMeshPart($taskBar[%i], "TaskBarView1Data", %buttonPos, %tbRotation, %buttonScale, "1");
      }
    }
}


//*****************************************************************************************
// task bar view buttons
//*****************************************************************************************
datablock StaticShapeData( TaskBarView1Data )
{	
   category = "CubeDemo";
   shapeFile = "art/cube/interface/icon_view1.dae";
	class = "BubbleButtonData";
};

function addMeshPart(%_master, %_dataBlock, %_position, %_rotation, %_scale, %_enableInput)
{
   %button = new StaticShape() {
      category = "TaskBar";
      canSave = "0";
      canSaveDynamicFields = "0";
      
      enableInputEvents = %_enableInput;
      
      dataBlock = %_dataBlock;
      class = "BubbleButtonClass";
      position = %_position;
      rotation = %_rotation;
      scale = %_scale;
      master = %_master;
   };
   return %button;
}
function TaskBar::buttonUp(%this,%which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.species == %which ){
      %this.species.setSkinName( "spac" );
      launchWebPage(getFishWebRoot() @ "/trophic/trophic.html",%this.slot,
                  %this.species,"spup",1);      
   }else if( %this.future == %which ){
      %this.future.setSkinName( "crac" );
      launchWebPage(getFishWebRoot() @ "/index_intro.html?f=futureofthereef",%this.slot,
                  %this.future,"crup",1);      
   }else if( %this.credits == %which ){
      %this.credits.setSkinName( "nsac" );
      launchWebPage(getFishWebRoot() @ "/index_intro.html?f=credits",%this.slot,
                  %this.credits,"nsup",1);      
   }else if( %this.conservation == %which ){
      %this.conservation.setSkinName( "coac" );
      launchWebPage(getFishWebRoot() @ "/index_intro.html?f=aboutthereef",%this.slot,
                  %this.conservation,"coup",1);      
   }else if($Pref::Cube::demoMode){
      if( %this.view1 == %which ){
         echo("View1");         
         changeViewAllClients(0);
      }else if( %this.view2 == %which ){
         echo("View2");         
         changeViewAllClients(0);
      }else if( %this.mini1 == %which ){
         changeViewAllClients(1);
      }else if( %this.mini2 == %which ){
         changeViewAllClients(2);
      }else if( %this.mini3 == %which ){
         changeViewAllClients(3);
      }else if( %this.mini4 == %which ){
         changeViewAllClients(4);
      }else if( %this.mini5 == %which ){
         changeViewAllClients(5);
      }
   }
}
function TaskBar::buttonDown(%this,%which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.species == %which ){
      ServerPlay3DAngle(Button1Snd,%this.species.getTransform());
      %this.species.setSkinName( "spto" );
   }else if( %this.future == %which ){
      ServerPlay3DAngle(Button1Snd,%this.future.getTransform());
      %this.future.setSkinName( "crto" );
   }else if( %this.credits == %which ){
      ServerPlay3DAngle(Button1Snd,%this.credits.getTransform());
      %this.credits.setSkinName( "nsto" );
   }else if( %this.conservation == %which ){
      ServerPlay3DAngle(Button1Snd,%this.conservation.getTransform());
      %this.conservation.setSkinName( "coto" );
   }
}
function TaskBar::buttonDragged(%this, %which, %mousePoint, %pos, %vec, %mouseUVCoord)
{
   if( %this.species == %which ){
   }else if( %this.future == %which ){
   }else if( %this.credits == %which ){
   }else if( %this.conservation == %which ){
   }
}

$WEBPAGE_OFFSET = 0.002;   

function getWebSlotPosition(%slot)
{
	%cl = ClientGroup.getObject(0);
   %camera = %cl.camera;
   %forward = %camera.getForwardVector();   
   %right = %camera.getRightVector();   
   %up = %camera.getUpVector();   
   %pos = %camera.getPosition();   
   
   %field = $CAMERA_FIELD;
   %xRange = mTan( mDegToRad( %field * 0.5 ) );
   
   %xLeft = -%xRange;
   %xRight = %xRange;
   %pageSize = (%xRight-%xLeft)/$WEBPAGE_NUMBER_ACROSS;
   %x0 = %xLeft + %pageSize*%slot;
   %x1 = %xLeft + %pageSize*(%slot+1);
   
   %xCenter = (%x0+%x1)*0.5;
   
   %xCenter += $WEBPAGE_OFFSET; 
   
   %p = %pos;       
   %p = VectorAdd(%p, VectorScale(%forward, $WEBPAGE_TO_CAMERA_DISTANCE));       
   %p = VectorAdd(%p, VectorScale(%right, %xCenter*$WEBPAGE_TO_CAMERA_DISTANCE));       
   %p = VectorAdd(%p, VectorScale(%up, $WEBPAGE_Y_POSITION));       
   return %p;
}
function launchWebPage(%url, %slot, %launchButton, %launchButtonSkin, %closeIfLoaded)
{
   %webdata = "AIFishInfoWebView" @ %slot;
   
   
   %webdata.slot = %slot;
   if( strcmp(%url, %webdata.currentUrl)==0 )
   {
      //--already showing this url------------------------------
      if( %closeIfLoaded )
      {
         %webdata.queueClose();
      }
      else
      {
         %webdata.queueOpen(%url, %launchButton, %launchButtonSkin);
      }
      return;
   }
   else
   {
      %webdata.queueOpen(%url, %launchButton, %launchButtonSkin);
   }
}
