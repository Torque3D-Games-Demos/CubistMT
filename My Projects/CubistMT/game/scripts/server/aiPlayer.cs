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


//The aiPlayer.cs file creates a guard-style bot that can also be pathed.
//The guard uses a simple state machine to control it's actions.

//The bots actions are as follows:
//Dead:				The bot stops all activity and is dead.
//Guarding: 		When guarding the bot scans for new targets, and when one is found it switches to 'Attacking'
//Attacking: 		The guard tries to close with the target while firing and checking for target updates.
//Holding:			When the bot loses a target it will go into a holding pattern. While holding the bot's FOV
//							is enhanced. The bot holds for a set number of cycles before changing it's action state to
//							'Returning'
//Returning:		The bot tries to return to it's original position. While returning the guard looks for new targets
//							and checks it motion relattive to it's last movement to determine if it is stuck.
//							If it is stuck the bot tries to move is a random direction to try and clear the obstacle.
//							(Not always a foolproof solution, but in a simple environment it works well enough.)
//Defending:		When a bot takes damage it's status is set to defending. The bot sidesteps and then
//							goes into a holding pattern. This does two things. It enhances the bots FOV and it scans for
//							targets. Plus it will have the bot return to it's original position if there is no
//							perceivable threat in range.
//NoTarget			This is set when the bot loses sight or perception of it's targets. This is used to help
//							clear the bots aim and other housekeeping functions

$botCounter = 0;

//The following are global variables used to set the guards basic settings.

//Generic
$AI_GUARD_ENABLED = true;						   //Whether Guard bots are loaded during mission loading.
$AI_GUARD_CHAR_TYPE = Bot1;                  //This is the default datablock that is spawned as the bot unless another is specified on the node
$AI_GUARD_SPAWN_GROUP = "AISpawnMarkers";    //The mission group that contains all of the ai spawn markers
$AI_GUARD_MARKER_HIDE = true;			   	   //Turns marker hiding on or off - useful when editing maps.
$AI_GUARD_DISPLAY_SHAPE_NAME = false;			//Whether the Guard bots' shape names and health are displayed in guiShapeNameHudCtrl
$AI_GAURD_TEAM = 1;                          //The default Gaurd bot team. This is used for detecting which players are enemies.
                                                      //Bots won't attack other players on their same team. Team 0 is the client team.
                                                      //Team -1 is no team.
//Weapon
$AI_GUARD_WEAPON = "RocketLauncher";			            //Which weapon do you want the guard to use
$AI_GUARD_ENDLESS_AMMO = false;		   	   //When set to true the guard will replenish its ammo perpetually
$AI_GUARD_WEAPON_USES_AMMO = true;           //Set this to false for energy weapons that do not use ammo
$AI_GUARD_AMMO = 100;			                  //The ammount of ammo the guard spawns with
$AI_GUARD_FIREDELAY = 10;					      //How long the bot waits between firing bursts.
$AI_GUARD_TRIGGER_DOWN = 100;			         //How long the bot holds down the trigger when firing. Use longer pulses for
																		//spray and pray type weapons.
//Senses
$AI_GUARD_FOV = 200;				               //The guards field of vision
$AI_GUARD_ENHANCED_DEFENDING_TIME = 5000;    //How long the bot gets a 360 FOV and a longer detect distance for after being sniped.
$AI_GUARD_ENHANCED_DEFENDING_DISTANCE = 50;  //Bonuse detect distance after being sniped.
$AI_GUARD_DETECT_DISTANCE = 70;			      //The bot's sight range (used for detecting players in FOV and ammo)
$AI_GUARD_MAX_FIRE_DISTANCE = 50;		      //The bot will fire at the target once it is within this distance.
$AI_GUARD_PREFERED_DISTANCE = 30;		      //The bot will try to stay this distance away from their target.
$AI_GUARD_LOS_TIME = 100;                    //The amount of time after the bot loses sight of player that it will get their position.
                                                      //This helps the bot turn sharp corners. Set it to 1 or 0 if you don't want the bot to cheat.
$AI_GUARD_LOS_BYPASS = 3;                    //The distance at which positions will not have line of sight tests done on them.
                                                      //This is needed because the bot can not see the area around its feet.
$AI_GUARD_HEARING = 5;                      //This represents the Guard bots hearing. It's used to sense enemies who aren't in the bot's
                                                      //FOV. A random number between 0 and $AI_GUARD_HEARING_VARIANCE is added to this number
                                                      //and compared with the enemy's stealth.
$AI_GUARD_HEARING_VARIANCE = 12;             //A number between 0 and this is added to $AI_GUARD_HEARING.
$AI_GAURD_STEALTH = 15;                      //The Guard bot's stealth.
$AI_GUARD_MOVE_SILENTLY = 1.0;               //This number is multiplied by the bot's velocity and the result is subtracted from the
                                                      //bot's stealth.
$AI_GUARD_DEFAULT_PLAYER_STEALTH = 15;       //The stealth to give a player if not already declared.
$AI_GUARD_DEFAULT_PLAYER_MOVE_SILENTLY = 1.0;//The value to give a player if not already declared.

//Movement
$AI_GAURD_DOES_SIDESTEP = 0;                 //If the guard bot sidesteps and paces
$AI_GUARD_MAX_PACE = 12;                     //The maximum range the mobs pace away from their guard point. (works like AI_GUARD_SIDESTEP)
$AI_GUARD_MIN_PACE = 1;                      //The minimum range the mobs pace away from their guard point.
$AI_GUARD_PACE_SPEED = 1.0;                  //Set the speed of the mob while pacing (0.0 to 1.0)
$AI_GUARD_PACE_TIME = 4;                     //Sets how many think cycles the bot has to travel to it's location (or stand at
                                                      //it's location if it's already there) before getting another one to move to, random between 1 and this number.
$AI_GUARD_SIDESTEP = 20;						   //This value helps determine how far a bot sidesteps when he is stuck.
																		//The computer picks a random number between 1 and $AI_GUARD_SIDESTEP
																		//The value is then subtracted by half it's value to create a left/right
																		//and forward/back component. So the effective range is really -10 to +10
																		//with the default setting of 20
$AI_GUARD_CORNERING = 0.8;			            //How close the bot will attempt to take corners. If the bot is having problems with corners,
                                                      //adjust this value, $AI_GUARD_LOS_TIME and $AI_GUARD_LOS_BYPASS as needed (will vary based on run speed).
$AI_GUARD_CAN_COVER = true;                  //If Gaurd bots can hide behind cover
$AI_GUARD_COVER_TIME = 2500;                 //The amount of time a Guard bot will hide behind cover
$AI_GUARD_COVER_TIME_VARIANCE = 3000;        //A random number between 0 and this is added to $AI_GUARD_COVER_TIME

//Thinking
$AI_GUARD_SCANTIME = 500;			            //The quickest time between think cycles.
$AI_GUARD_MAX_ATTENTION = 10; 		         //This number and $AI_GUARD_SCANTIME are multiplied to set the delay in the
																		//thinking loop. Used to free up processor time on bots out of the mix.
$AI_GUARD_CREATION_DELAY = 100;			      //How long a bot waits after creation before his think cycles are controlled by
																		//his attention rate. (Used to help free up think cycles on bots while misison
																		//finishes loading.
$AI_GUARD_HOLDCNT_MAX = 10;					   //The number of think cycles that the bot will 'hold' for before trying to
																		//return to his post.
//Health
$AI_GUARD_LIFE_OVER_AMMO_PERCENT = 0.15;    //The percent of health remaining where a bot values its life over ammo. (0.0 to 1.0)
                                                      //eg. If the bot is out of ammo and it sees both ammo and an enemy, it will go for the ammo if above this
                                                      //health percent, or go to the "NoAmmo" action if at or below it.
$AI_GUARD_DEFAULTRESPAWN = true;		         //Controls whether guards respawn automatically or not if their marker does not have
																		//dynamic 'respawn' variable set in it.
$AI_GUARD_RESPAWN_DELAY = 6000;		         //Determines how long a bot goes in between death and respawning.

//The AIPlayer marker is placed in the map during edit mode. When the map is loaded the
//marker is replaced by a guard player. (Assuming that the $AI_GUARD_ENABLED variable is set
//to true.) The marker is hidden or not depending on the value set in $AI_GUARD_MARKER_HIDE.
//The AIPlayer marker can use a dynamic variable - set during map creation - called 'respawn'
//Creating and setting the 'respawn' variable will override the default value set in
//$AI_GUARD_DEFAULTRESPAWN. This allows more freedom in determining which bots respawn and which do not.

datablock StaticShapeData(AIPlayerMarker)
{
   // Mission editor category, this datablock will show up in the
   // specified category under the "shapes" root category.
   category = "AIMarker";

   // Basic Item properties
   shapeFile = "art/shapes/actors/Gideon/gideon.dts";
};

//The onReachDestination function is responsible for setting the bots 'action'
//state to the appropriate setting depending on what action the bot was following
//to reach the destination.
function DefaultPlayer::onReachDestination(%this, %obj)
{
   %obj.justCovered = false;
  	//Picks an appropriate set of actions based on the bots 'action' variable
  	switch$(%obj.action)
  	{
  		//If the bot is attacking when it reaches it's target it will go into a hold.
  		case "Attacking":
  			%obj.action="Holding";
  		//If the bot is returning it has two possible scenarios for reaching a destination
  		//The first case is the the bot sidestepped and has reached it's sidestep location.
  		//If that is the case, then the bot goes into a quick hold. (Which sets the bot to
  		//only hold for 1 cycle before returning to his post.)
  		//The other alternative is that the bot has returned as is back at it's original position.
  		//If this is the case, then the bot's transform is set to match that of it's marker's
  		//transformation.
  		//This will cause a snapping into position - but it ensures that your guard always faces the
  		//direction you want it to when it returns to it's post.
  		//(It also helps to make sure that your markers are set as close to the ground as possible.
  		//Otherwise your bots will hop up and drop from the sky when they return to post.)
  		case "Returning":
  		  //If the bot is pathed have it move to the next node on its path
  		  if (%obj.path !$= "")
  		  {
  			   //Check if the bot's guarding
  			   if (%obj.doesGuard $= "guard")
  			   {
  			        if (%obj.returningPos == %obj.marker.getposition())
  			        {
  			            %obj.moveToNextNode(%this.returningPath);
  			        }
  			        else
  			        {
  			            %obj.path = "";
  			            %obj.doesGuard = "";
  			        }
  			   }
  			   else
  			      %obj.moveToNextNode(%this.returningPath);
  		  }
  		  else
  		  {
  			if (%obj.doesGuard $= "guard")
  			   %basedist = vectorDist(%obj.getposition(), %obj.marker.getposition());
  			else
  			   %basedist = vectorDist(%obj.getposition(), %obj.returningPos);
  			//if the bot is close to his original position then set it's action to
  			//Guarding and set it to it's original facing and position.
  			if(%basedist < 1.0)
  				{
  					%obj.action = "Guarding";
  					//Set the bots returning position to its marker if it's guarding
  					if (%obj.doesGuard $= "guard")
  					   %obj.settransform(%obj.marker.gettransform());
  					else
  					   %obj.settransform(%obj.returningTrans);
  					%obj.clearaim();
  				}
  				//if the bot is away from his post, then he must have gotten here
  				//as a result of sidestepping so set him to do a quick hold to scan
  				//for targets then return to post.
  				else
  				{
  					//Sets holdcnt to 1 less than the max. Ensures that the bot only holds for 1 cycle.
  					//before trying to return.
  					%obj.holdcnt=$AI_GUARD_HOLDCNT_MAX-1;
  					%obj.action="Holding";
  				}
  		  }
  		//The bot was defending and sidestepped. So set him to 'hold' to check for targets
  		//and to prepare to return to post if no targets are found.
  		case "Defending":
  			%obj.action = "Holding";

      //The was defending and just reached cover to hide behind
      case "Covering":
         %obj.setAimLocation(%obj.coverFrom.getPosition());
         %obj.setPose(1);
         %obj.setTransform(%obj.getTransform());
         %obj.justCovered = true;
         %coverTime = $AI_GUARD_COVER_TIME + getRandom(0, $AI_GUARD_COVER_TIME_VARIANCE);
         %obj.uncovertrigger = %obj.schedule(%coverTime, "uncover", %obj);
         %obj.ailoop = %obj.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel, "Think", %obj);

      case "RetrievingItem":
			%obj.holdcnt=$AI_GUARD_HOLDCNT_MAX-1;
  			%obj.action="Holding";

      case "NoAmmo" :
         if (%obj.path !$= "")
            %obj.moveToNextNode(%obj.returningPath);
  	}
}

//The OnDamage function sets the bots action to 'Dead' and starts the respawn process
//if called for.
function DefaultPlayer::OnDamage(%this, %obj, %delta)
{
   if (%obj.action !$= "Attacking" && %obj.action !$= "Defending" && %obj.getstate() !$="Dead")
   {
      %obj.enhancedefending(%obj);
   }
   if (%obj.action !$= "Covering")
      %obj.action = "Defending";

   if(%obj.getstate() $= "Dead")
      %obj.action="Dead";

   if(%obj.getState() $= "Dead" && %obj.respawn == true)
   {
      //%obj.delaybeforerespawn(%obj.botname, %obj.marker);
      %this.player = 0;
   }
}

function AIPlayer::updateHealth(%this)
{
   //Avoids console errors
}
function AIPlayer::playDeathAnimation(%this)
{
   %this.setActionThread("Death" @ getRandom(1, 11));
}
function AIPlayer::playDeathCry(%this)
{
   %this.playAudio(0, DeathCrySound);
}

//The bot stands up after hiding behind cover. It then calls a think cycle.
function AIPlayer::uncover(%this, %obj)
{
   %obj.setPose(0);
   %obj.setTransform(%obj.getTransform());
   %obj.coverPoint.used = false;
   %obj.action = "Attacking";
   %this.Think(%obj);
}

//The delay before respawn function is set to wait a specified duration before
//respawning an AIPlayer
function AIPlayer::DelayBeforeRespawn(%this, %name, %marker)
{
   eval("$AI_GAURD_RESPAWN_" @ %this @ " = schedule($AI_GUARD_RESPAWN_DELAY, 0, \"AIPlayer::spawn\", 0, %name, %marker);");
}

//The LoadEntities function replaces the markers placed in the map with the AI bots during the
//mission loading.
function AIPlayer::LoadEntities()
{
	//Check to see if the AIPlayers are to be loaded.
	if ($AI_GUARD_ENABLED == true)
	{
      echo("Loading Guard entities...");

      for (%i = 0; %i < $AI_GUARD_SPAWN_GROUP.getCount(); %i++)
      {
         %spawnPoint = $AI_GUARD_SPAWN_GROUP.getObject(%i);

         if(%spawnPoint.getclassname() $= "StaticShape" && %spawnPoint.getDataBlock().getName() $= "AIPlayerMarker")
         {

            // Let's check to see if the marker specifies a datablock.
            // if so, we spawn that datablock model instead of the default
            if (%spawnPoint.block $= "")
            {
               %block = $AI_GUARD_CHAR_TYPE;
            }
            else
            {
               %block = %targetObject.block;
            }
            %player = AIPlayer::spawnAtMarker("Guard" @ %i, %spawnPoint, %block);

            // Hide the marker
            if ($AI_GUARD_MARKER_HIDE == true)
               %spawnPoint.sethidden(true);
         }
      }
	}
   else
   {
      echo("Guard entities disabled...");
   }
}
function AIPlayer::spawnByGroup(%spawnGroup)
{
   echo ("spawning group " @ %spawnGroup);

   for (%i = 0; %i < $AI_GUARD_SPAWN_GROUP.getCount(); %i++)
   {
      %spawnPoint = $AI_GUARD_SPAWN_GROUP.getObject(%i);

      if(%spawnPoint.getclassname() $= "StaticShape" && %spawnPoint.getDataBlock().getName() $= "AIPlayerMarker")
      {
         echo("%spawnPoint's spawn is " @ %spawnPoint.spawnGroup);
         if (%spawnPoint.spawnGroup $= %spawnGroup)
         {
            // we're in the correct spawn group!

            // Let's check to see if the marker specifies a datablock.
            // if so, we spawn that datablock model instead of the default
            if (%spawnPoint.block $= "")
            {
               %block = $AI_GUARD_CHAR_TYPE;
            }
            else
            {
               %block = %spawnPoint.block;
            }

            // let's spawn some bad guys!
            %player = AIPlayer::spawnAtMarker("Guard" @ %i, %spawnPoint, %block);

            // Hide the marker
            if ($AI_GUARD_MARKER_HIDE == true)
               %spawnPoint.sethidden(true);
         }
      }
   }
}

//This function sets the bots aim to the current target, and 'pulls' the trigger
//of the weapon of the bot to begin the firing sequence.
function AIPlayer::openfire(%this, %obj, %tgt)
{
	//If the bot is dead or the target is dead then let's bail out of here.
	if (%obj.getState() $= "Dead" || %tgt.getState() $="Dead")
	{
	 	%obj.firing = false;
	 	%obj.NoTarget(%obj);
	}
	else
	{
      //We've got two live ones. So let's kill something.
      //The firing variable is set while firing and is cleared at the end of the delay cycle.
      //This is done to allow the use of a firing delay - and prevent a bot from firing again
      //prematurely.
      if(!%obj.firing)
      {
         //Gets the range to target - rtt
         %rtt = vectorDist(%obj.getposition(), %tgt.getposition());

         //If the target is within our ignore distance then we will attack.
         if(%rtt < %obj.weapRange)
         {
            //if(%obj.getAimLocation() != %tgt.getposition())    //Fix for premature firing
            //   return;

            //Sets the firing variable to true
            %obj.firing = true;

            if($AI_GUARD_WEAPON_USES_AMMO)
            {
               if($AI_GUARD_ENDLESS_AMMO == true)
               {
                  %obj.incinventory(%obj.Ammo, 100);
               }
            }

            //'Pulls' the trigger on the bot gun.
            %obj.setImageTrigger(0,true);
            //This sets a delay of $AI_GUARD_TRIGGER_DOWN length to hold the trigger down for.
            %this.trigger = %this.schedule($AI_GUARD_TRIGGER_DOWN,"ceasefire", %obj);
         }
         else
         {
            //There was a target when openfire was called, but now they're out of range so
            //we have no target. Call NoTarget to clear the bots aim.
            %obj.NoTarget(%obj);
         }
      }
   }
}

//This simply clears the bots aim to have it look forward relative to it's movement.
function AIPlayer::NoTarget(%this, %obj)
{
	%obj.clearaim();
}

//Ceasefire is called by the openfire function after the set delay to
//hold the trigger down is met.
function AIPlayer::ceasefire(%this, %obj)
{
   //Turns off the trigger, or lets off of it.
   %obj.setImageTrigger(0,false);
   //This sets the delay between when we let off the trigger and how soon it will
   //be before we allow the bot to fire again.
   %this.ceasefiretrigger = %this.schedule($AI_GUARD_FIREDELAY,"delayfire", %obj);
}

//delayfire is called to clear the firing variable. Clearing this allows
//the bot to fire again in the openfire function.
function AIPlayer::delayfire(%this, %obj)
{
	//this is the end of the firing cycle
	%obj.firing = false;
}

//-----------------------------------------------------------------------------
// AIPlayer static functions
//-----------------------------------------------------------------------------

//This is the spawn function for the bot.
function AIPlayer::spawn(%this, %name, %obj, %block)
{
   %block = %obj.block $= "" ? $AI_GUARD_CHAR_TYPE : %obj.block;

   // Create the demo player object
   %player = new AIPlayer() {
      dataBlock = %block;

      //The marker is the AIPlayer marker object that the guard is associated with.
      //The marker object is kept with the player data because it's location, and
      //dynamic variable values are used in several functions. This also allows the addition
      //of future dynamic variables without having to change the spawn/respawn functions to
      //access them.
      marker = %obj;
      botname = %name;
      //Sets the bot's team
      team = %obj.team $= "" ? $AI_GAURD_TEAM : %obj.team;
      //Sets the bot's field of vision
      fov = $AI_GUARD_FOV;
      //Sets the bot's range distance
      detdis = %obj.rangeSight $= "" ? $AI_GUARD_DETECT_DISTANCE : %obj.rangeSight;
      enhancedDis = %obj.rangeEnhancedBonus $= "" ? $AI_GUARD_ENHANCED_DEFENDING_DISTANCE : %obj.rangeEnhancedBonus;
      weapRange = %obj.rangeFire $= "" ? $AI_GUARD_MAX_FIRE_DISTANCE : %obj.rangeFire;
      prefDis = %obj.rangePref $= "" ? $AI_GUARD_PREFERED_DISTANCE : %obj.rangePref;
      //Sets the bot's hearing
      hearing = %obj.hearing $= "" ? $AI_GUARD_HEARING : %obj.hearing;
      hearingVariance = %obj.hearingVariance $= "" ? $AI_GUARD_HEARING_VARIANCE : %obj.hearingVariance;
      //Sets the bot's stealth
      stealth = %obj.stealth $= "" ? $AI_GAURD_STEALTH : %obj.stealth;
      moveSilently = %obj.moveSilently $= "" ? $AI_GUARD_MOVE_SILENTLY : %obj.moveSilently;
      //Sets the bot's returning position and transform
      returningPos = %obj.getposition();
      returningTrans = %obj.gettransform();
      //Sets the bot not to return to a path as soon as it is loaded
      //The pathed bots will go to there paths at another point
      returningPath = 0;
      doesSidestep = %obj.doesSidestep $= "" ? $AI_GAURD_DOES_SIDESTEP : %obj.doesSidestep;
      //Sets the bot's pacing
      pace = $AI_GUARD_PACE_TIME;
      //The pathname variable is a dynamic variable set during map editing.
      //This allows the designer to attach each bot to a seperate path
      path = %obj.pathname;
      //Does the bot return to its spawn point
      doesGuard = %obj.doesReturn;
      //Can the bot hide behind cover
      canCover = %obj.canCover $= "" ? $AI_GUARD_CAN_COVER : %obj.canCover;
      //Sets whether the bot is AI or not
      isBot = true;
      //Thinking variables
      //Firing tells whether or not we're in the midst of a firing sequence.
      firing = false;
      //The 'action' variable holds the state of the bot - which controls how it
      //thinks.
      holdcnt = $AI_GUARD_HOLDCNT_MAX-1;
      action = "Guarding";
      //The bots starting attention level is set to half of it's range.
      attentionlevel = $AI_GUARD_MAX_ATTENTION/2;
      //Oldpos holds the position of the bot at the end of it's last 'think' cycle
      //This is used to help determine if a bot is stuck or not.
      oldpos = %obj.getposition();
      //Added for bots use different weapons
      botWeapon = %obj.weapon $= "" ? $AI_GUARD_WEAPON : %obj.weapon;
   };
   //%player.disableCollision();

   $botIDs[$botCounter] = %player;
   $botCounter++;

   MissionCleanup.add(%player);

   // Set the type of ammo that the bot uses
   switch$ (%player.botWeapon)
   {
      case "": %player.Ammo = "";
      default:
         %weaponImage = %player.botWeapon @ "Image";
         %player.Ammo = %weaponImage.Ammo;
   }

   if ($AI_GUARD_DISPLAY_SHAPE_NAME)
      //Sets the name displayed in the hud above the bot.
      %player.setShapeName(%name);
   //Equipbot is called to set the bots beginning inventory.
   %player.EquipBot(%player);
   //Sets the bot's initial position to that of it's marker.
   %player.setTransform(%obj.gettransform());

  	//The following cluster of if-thens sets whether the bot will respawn or not
  	//based on it's markers dynamic variable - or the default respawn variable setting.
   if (%obj.respawn $= "" )
   {
      %player.respawn=$AI_GUARD_DEFAULTRESPAWN;
   }
   else
   {
      if (%obj.respawn == true)
         %player.respawn=true;
      else
         %player.respawn=false;
   }

   if (%obj.pathname !$= "")
   {
      %player.schedule($AI_GUARD_CREATION_DELAY,"followPath", %obj.pathname, -1);
   }

   //Sets the bot to begin thinking after waiting the length of $AI_GUARD_CREATION_DELAY
   %player.schedule($AI_GUARD_CREATION_DELAY,"Think", %player);

   return %player;
}

//This sets the bots beginning equipment and inventory
function AIPlayer::EquipBot(%this, %obj)
{
   //This adds a weapon to the bots inventory.
   %obj.incinventory(%obj.botWeapon, 1);
   //This mounts the weapon on the bot.
   %obj.mountImage(%obj.botWeapon @ "Image", 0);

   if($AI_GUARD_WEAPON_USES_AMMO == true)
   {
   	//This sets the bots beginning inventory of ammo.
   	%ammo = %obj.marker.ammo $= "" ? $AI_GUARD_AMMO : %obj.marker.ammo;
 	 	%obj.setInventory(%obj.Ammo, %ammo);
   }
}

//Enhances the defending mob's FOV and detect distance after being hit.
function AIPlayer::EnhanceDefending(%this, %obj)
{
   %obj.detdis += %obj.enhancedDis;
   %this.distancetrigger = %this.schedule($AI_GUARD_ENHANCED_DEFENDING_TIME, "restoreDefending", %obj);
}

//Restores the defending mob's detect distance.
function AIPlayer::restoreDefending(%this, %obj)
{
	%obj.detdis -= %obj.enhancedDis;
}

//Spawn at marker is called by LoadEntities, and calls the spawn function to
//create the bots and place them at their starting positions.
function AIPlayer::spawnAtMarker(%name, %obj, %block)
{
   if (!isObject(%obj))
      return;
   %player = AIPlayer::spawn(%this, %name, %obj, %block);
   return %player;
}

//-----------------------------------------------------------------------------
// AITargeting
//-----------------------------------------------------------------------------

//Return the angle of a vector in relation to world origin
function AIPlayer::getAngleofVector(%this, %vec)
{
	%vector = VectorNormalize(%vec);
	%vecx = getWord(%vector,0);
	%vecy = getWord(%vector,1);
	if(%vecx >= 0 && %vecy >= 0)
		%quad = 1;
	else
		if(%vecx >= 0 && %vecy < 0)
	  	%quad = 2;
	  else
	  	if(%vecx < 0 && %vecy < 0)
	    	%quad = 3;
	    else
	    	%quad = 4;
	%angle = mATan(%vecy/%vecx, -1);
	%degangle = mRadToDeg(%angle);
	switch(%quad)
	{
		case 1:
	  	%angle = %degangle-90;
	  case 2:
	  	%angle = %degangle+270;
	  case 3:
	  	%angle = %degangle+90;
	  case 4:
	  	%angle = %degangle+450;
	}
  if (%angle < 0)  %angle = %angle + 360;
  return %angle;
}

//This is another function taken from code off of garagegames.
//The only mods I made to it was to add the extra check to ensure that the
//angle is within the 0-360 range.
function AIPlayer::check2DAngletoItem(%this, %obj, %item)
{
	%eyeVec = VectorNormalize(%this.getEyeVector());
	%eyeangle = %this.getAngleofVector(%eyeVec);
	%posVec = VectorSub(%item.getPosition(), %obj.getPosition());
	%posangle = %this.getAngleofVector(%posVec);
	%angle = %posangle - %eyeAngle;
	%angle = %angle ? %angle : %angle * -1;
  if (%angle < 0)  %angle = %angle + 360;
	return %angle;
}

//This is another function taken from code off of garagegames.
//The only mods I made to it was to add the extra check to ensure that the
//angle is within the 0-360 range.
function AIPlayer::check2DAngletoTarget(%this, %obj, %tgt)
{
	%eyeVec = VectorNormalize(%this.getEyeVector());
	%eyeangle = %this.getAngleofVector(%eyeVec);
	%posVec = VectorSub(%tgt.getPosition(), %obj.getPosition());
	%posangle = %this.getAngleofVector(%posVec);
	%angle = %posangle - %eyeAngle;
	%angle = %angle ? %angle : %angle * -1;
  if (%angle < 0)  %angle = %angle + 360;
	return %angle;
}

//The 'Think' function is the brains of the bot.
//The bot performs certain actions based on what it's current 'action' state is.
//The bot thinks on a scheduled basis. How fast the bot 'thinks' is determined by
//the bots attention level and its default scan time. (There are a few cases in the think
//function below where the schedule is shortened - but only to make the 'thinking' more
//realistic and to cut down on duplicating chunks of code.
function AIPlayer::Think(%this, %obj)
{
		//This cancels the current schedule - just to make sure that things are kept neat and tidy.
		cancel(%this.ailoop);
		//If the bot is dead, then there's no need to think or do anything. So let's bail out.
		if (!%obj || %obj.getstate() $="Dead")
			return;

		%prevaction = %obj.action;

		if (%obj.action !$="RetrievingItem" && %obj.action !$="Dead")
		{
			if ($AI_GUARD_WEAPON_USES_AMMO == true)
			{
			   if (%obj.getInventory(%obj.Ammo) <= 0)
				{
					%i_ammo = %this.getclosestiteminsightandrange(%obj, %obj.Ammo );
  				   if (%i_ammo > 0)
					{
					   //This ensures that if the ammo was dropped, the bot waits one thought process
					      //so that he goes where the ammo lands, not from where it was thrown
					   if (%obj.sawAmmo)
						   %obj.action = "GetAmmo";
                  %obj.sawAmmo = true;
					}
					else
					{
					   %obj.sawAmmo = false;
                  %obj.action = "NoAmmo";
					}
				}
				else
				{
               %obj.sawAmmo = false;
				}
			}
		}

		//The switch$ takes the value of the bots action variable and then chooses what code to run
		//according to what value it is.
		switch$(%obj.action)
		{
         //The bot is 'dead' so lets clear his aim, and turn off his firing variable.
         case "Dead":
            %obj.clearaim();
            %obj.firing = false;

         //This is the bots default position. While guarding the bot will only do 2 things.
         //This checks for nearby targets. If found the bot goes into attack mode.
         case "Guarding":
            %obj.lostest = 0;

            //The bot checks for the nearest valid target if any.
            %tgt = %this.GetClosestPlayerInSightandRange(%obj);
            //If %tgt >= 0 then a target is in sight and range.
            if(%tgt >= 0 && %obj.getInventory(%obj.Ammo) > 0)
            {
               //Set the bots action to 'Attacking' and set it to attack quickly.
               %obj.action = "Attacking";
               //This is one instance where the bots thinking is sped up to enable the bot
               //to react more quickly as seems appropriate.
               %this.ailoop = %this.schedule(100,"Think" , %obj);
            }
            else
            {
               //Check if the bot's pathed and if not, pace if it's time to pace
               if (%obj.path $= "" && %obj.doesSidestep)
               {
                  if(%obj.pace == 0)
                  {
                     %obj.pace = getRandom(($AI_GUARD_PACE_TIME-1)) +1;
                     %this.pacing(%obj);
                  }
                  else
                  {
                     %obj.pace--;
                  }
               }
               //There are no targets so continue guarding and call the scheduler to have the bot think
               //at it's regular interval
               %this.ailoop=%this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel  ,"Think" , %obj);
            }

         //The bot has been told that there is a target in sight and range and is set to attack it.
         //While attacking the bot's attention level is kept at it's lowest value (Quickest thinking)
         //The bot looks for the nearest target in sight. If the target is found the bot will aim at the
         //target, set it's move destination to the position of the target, and then openfire on the target.
         case "Attacking":
            //Set the bot's move speed back to normal
            %obj.setMoveSpeed(1.0);
            //Maintain a low attention value to keep the bot thinking quickly while attacking.
            %obj.attentionlevel = 1;
            //Get the id of the nearest valid target
            %tgt = %this.GetClosestPlayerInSightandRange(%obj);
            //If %tgt >= 0 then there is a valid target
            if(%tgt >=0)
            {
               //Make sure that we keep ourself in attack mode since we have a target in sight.
               %obj.action = "Attacking";
               //Set the bot to aim at the target.
               //(The code uses the VectorAdd to adjust the aim of the bot to correct for the
               //bot trying to shoot at the targets feet.)
               %obj.setAimObject(%tgt, "0 0 1");

               %dest = %tgt.getposition();

               %objPos = %obj.getposition();
               %basedist = VectorDist(%objPos, %dest);

               //Check if the bot is too far or too close
               if((%basedist > %obj.prefDis + 1 || %basedist < %obj.prefDis - 1) && (!%obj.justCovered || %obj.testCover(%obj)))
               {
                  %obj.justCovered = false;
                  //Gets a vector with length of prefDis in the direction of the bot from the player
                  %vec = VectorScale(VectorNormalize(VectorSub(%objPos, %dest)), %obj.prefDis);
                  %dest = VectorAdd(%dest, %vec);
                  %this.moveDestinationA = %dest;
                  %this.dontMoveAlongTheWall(%obj);
               }
               //Tells the bot to start shooting the target.
               %obj.openfire(%obj, %tgt);
               //Tells the scheduler to have us think again
               %this.ailoop=%this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel  ,"Think" , %obj);
            }
            else
            {
               //There was no target found, so set our action to NoTarget.
               %obj.action="NoTarget";
               //Again this sets the scheduler to have us think quickly to have the bot
               //react to the loss of it's attack target
               %this.ailoop=%this.schedule(100, "Think", %obj);
            }

         //When a bot loses it's target, or when the bot reaches it's destination as the result of
         //a sidestep the bot will go into a 'hold'
         //The bot will look for targets in range and attack if found.
         //If no target is found the bot will increase it's holdcnt by 1. When the bot reaches it's
         //maximum holdcnt value it will attempt to return to it's base position.
         case "Holding":
            //Set the bot's move speed back to normal
            %obj.setMoveSpeed(1.0);
            //Checks for targets - (See the above code for full details of this section of code)
            %tgtid = %this.GetClosestPlayerInSightandRange(%obj);
            if(%tgtid >=0)
            {
               %obj.holdcnt=0;
               %obj.action = "Attacking";
               %this.ailoop=%this.schedule(100,"Think" , %obj);
            }
            else
            {
               //There was no target found, so we need to have the bot continue to 'hold'
               //for a little bit before doing anything else.

               //Increase the holdcnt variable by one
               %obj.holdcnt++;
               %obj.fireLater = 0;

               %basedist = vectorDist(%this.getposition(), %this.moveDestinationA);
               if (%basedist > 0.5)
                  %this.dontMoveAlongTheWall(%obj);

               //Check to see if we've passed our threshold of waiting
               if (%obj.holdcnt > $AI_GUARD_HOLDCNT_MAX)
               {
                  //Set holdcnt back to 0 for the next time we need it.
                  %obj.holdcnt = 0;

                  //Set the bot to return to where it last saw the player if it's not pathed
                  if (%obj.path $= "")
                  {
                       //Reset returning positions for guard bots
                       if (%obj.doesGuard $= "guard")
                       {
                           %obj.returningPos = %obj.marker.getposition();
                           %obj.returningTrans = %obj.marker.gettransform();
                       }

                       %this.moveDestinationA = %obj.returningPos;
                       %this.dontMoveAlongTheWall(%obj);
                  }
                  //Set the bot to return to its path since it is pathed
                  else
                  {
                     if (%obj.returningPath != 0)
                     {
                        if (%obj.doesGuard $= "guard")
                        {
                           %this.moveDestinationA = %obj.returningPos;
                           %this.dontMoveAlongTheWall(%obj);
                        }
                        else
                           %this.movtrigg = %this.schedule(100, "followPath", %obj.path, -1);
                     }
                     else
                        %obj.returningPath = 1;
                  }

                  //Set the bot action to 'Returning'
                  %obj.action="Returning";
                  //Sets the bots oldpos to that of the position it's returning to
                  //This is done this way due to the fact that we've been holding
                  //and our position hasn't been changing. So we want to be sure that
                  //our bot doesn't think that it's stuck as soon as it tries to return.
                  %obj.oldpos = %obj.returningPos;
                  //We've waited long enough, so let's quickthink and go into 'Return' mode
                  %this.ailoop=%this.schedule(100, "Think" , %obj);
               }
               else
               {
                  //Start the bot moving to its return point while it's still in holding mode
                  %this.moveDestinationA = %obj.returningPos;
                  %this.dontMoveAlongTheWall(%obj);

                  %obj.clearaim();
                  %this.ailoop = %this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel  ,"Think" , %obj);
               }
            }

         //In Return mode the bot will do the following.
         //It looks for the nearest target in sight and will attack it.
         //If a target is found the bot will attack.
         //If no target is found, the bot is still in the process of returning so we check to see
         //if the bot is stuck. Stuck in the case means that the bot hase moved a distance of less than
         //1 unit since the last time it thought.
         //If the bot is stuck, sidestep is called to have the bot try to move a different direction
         //The bot is then set to go into 'Holding' but with it's holdcnt set to 1 less than it's maximum.
         //This essentially means that the bot will sidestep, and go into hold for one cycle in which to check
         //targets and then try to return again if there is nothing to attack.
         //If the bot is not stuck and there are no targets, then the bots aim is set to point towards it's
         //destination of it's spawn point. (This is done to prevent the bot from pointing to the position
         //of it's last sidestep while returning.)
		   case "Returning":
				//Set the bot's move speed back to normal
				%obj.setMoveSpeed(1.0);
				//The next line can be commented out if desired. I chose to put it in so that the
				//bots would try to return in a timely manner rather than having them wait too long
				//between thinks to see if they were stuck.
				%obj.attentionlevel = $AI_GUARD_MAX_ATTENTION/2;

				//The next few lines again have the bot check for a target and attack if need be.
				%tgt = %this.GetClosestPlayerInSightandRange(%obj);
				if(%tgt >= 0 && %obj.getInventory(%obj.Ammo) > 0)
				{
			 		%obj.action = "Attacking";
			 		%this.ailoop = %this.schedule(100, "Think", %obj);
				}
				else
				{
				   //There was no target or the bot's out of ammo so we're still returning. So now check for a pathed or stuck bot
					//This gets a value depicting the distance from the bots last known move point
					%movedist = vectorDist(%obj.getposition(), %obj.oldpos);
					//If the bot hasn't moved more than 1 unit we're probably stuck.
					//Remember - this is only checked for while returning - not guarding
					if (%movedist < 1.0)
					{
						//Set our holdcnt to 1 less than the maximum so we only hold for 1 cycle
						%obj.holdcnt = $AI_GUARD_HOLDCNT_MAX-1;
						//Call sidestep to pick a new move destination near the bot
                  if (%obj.doesSidestep)
						   %this.sidestep(%obj);
					}
					else
					{
						//Check to make sure the bot is not pathed
						if (%obj.path $= "")
						{
						   //We're returning and we're not stuck. So make sure we're looking the direction we're running.
						   //Check if the bot is guarding
						   if (%obj.doesGuard $= "guard")
						   {
						      %obj.setAimLocation(%obj.marker.getposition());
						   }
						   else
						   {
						      %obj.setAimLocation(%obj.returningPos);
						   }
                  }
					}
				}
            //Set our oldpos to match our current position so that next time we cycle through
            //we'll know if we're going anywhere or not
            %obj.oldpos = %obj.getposition();
            //Scedhule ourselves to think at our regular interval
            %this.ailoop = %this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel, "Think", %obj);

         //When a bot takes damage his state is set to defending.
         //A bot that is defending will have it's attention set to it's lowest level
         //It will first try to hide behind cover. If it can't, then
         //it will sidestep to try to avoid the danger, and to throw some randomness into it's
         //movement. The bot will then go into a quick hold of 1 count.
         case "Defending":
            //Set the bot's move speed back to normal
            %obj.setMoveSpeed(1.0);
            //Set the hldcnt to 1 less than the max
            %obj.holdcnt = $AI_GUARD_HOLDCNT_MAX-1;
            //Set the bot to it's highest awareness
            %obj.attentionlevel = 1;
            //Try to find cover
            %cover = %obj.canCover ? %this.findCover(%obj) : -1;
            if (%cover == -1)
            {
               //Sidestep to a random position
               if (%obj.doesSidestep)
                  %this.sidestep(%obj);
               //Set our action to 'Holding'
               %obj.action = "Holding";
               //Set a quick think schedule to start us looking for targets quickly.
               %this.ailoop = %this.schedule(100, "Think", %obj);
            }
            else
            {
               //Move to cover
               %cover.used = true;
               %obj.setAimLocation(%cover.getPosition());
               %obj.setMoveDestination(%cover.getPosition());
               %obj.coverPoint = %cover;
               %obj.action = "Covering";
            }

         //While a bot is hiding behind cover, the only thing it will do is check to see
         //if the enemy it is hiding from can see it. If the enemy is too far for the bot
         // to see, then it will assume that it is safe to come out.
         case "Covering":
            if (%this.testCover(%obj))
            {
               //The bot is still safe
               %this.ailoop = %this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel, "Think", %obj);
            }
            else
            {
               //The enemy can see the bot
               %obj.justCovered = false;
               %this.uncover(%obj);
            }

         //NoTarget is set when a bot loses it's target while attacking.
         //It causes a bot's firing variable to be reset, sets the holdcnt to 0
         //so that when we go into a hold we will do so for the full duration
         case "NoTarget":
            //Clear the firing variable
            %obj.firing = false;
            //Clear holdcnt
            %obj.holdcnt=0;
            //Set our action to 'Holding'
            %obj.action = "Holding";
            //Quick think to start us looking for our lost target.
            %this.ailoop=%this.schedule(100 , "Think", %obj);

         case "GetAmmo":
            %i_ammo = %this.getclosestiteminsightandrange(%obj, %obj.Ammo );
            if (%i_ammo > 0)
            {
               %tgt = %this.GetClosestPlayerInSightandRange(%obj);
               if (%tgt >= 0 && %obj.getInventory(%obj.Ammo) <= 0 && 1 - %obj.getDamagePercent() <= $AI_GUARD_LIFE_OVER_AMMO_PERCENT)
               {
                  %obj.action = "NoAmmo";
               }
               else
               {
                  %obj.action = "RetrievingItem";
                  %dest = %i_ammo.getposition();
                  %obj.setmovedestination(%dest);
               }
            }
            else
            {
               %obj.action = %prevaction;
            }

            %this.ailoop = %this.schedule(100, "Think", %obj);

			case "RetrievingItem":
				%obj.setMoveSpeed(1.0);
				%obj.attentionlevel = $AI_GUARD_MAX_ATTENTION/2;
				%tgt = %this.GetClosestPlayerInSightandRange(%obj);
				if (%tgt >= 0)
				{
               %obj.action = "RetrievingItem";
               %obj.attentionlevel = 1;
               %tgt = %this.GetClosestPlayerInSightandRange(%obj);
               if (%tgt >= 0)
               {
                  %obj.setAimObject(%tgt, "0 0 1");
                  %obj.openfire(%obj, %tgt);
               }
               else
               {
                  %obj.firing = false;
               }
				}
				else
				{
					%movedist = vectorDist(%obj.getposition(), %obj.oldpos);
					if (%movedist < 1.0)
               {
						%obj.holdcnt = $AI_GUARD_HOLDCNT_MAX-1;
						if (%obj.doesSidestep)
						   %this.sidestep(%obj);
               }
               else
               {
                  %obj.setaimlocation(%obj.getmovedestination());
               }
            }
            %obj.oldpos = %obj.getposition();
            %this.ailoop = %this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel  ,"Think" , %obj);

		   case "NoAmmo":
				//Set the bot's move speed back to normal
				%obj.setMoveSpeed(1.0);
            //This gets a value depicting the distance from the bots last known move point
            %movedist = vectorDist(%obj.getposition(), %obj.oldpos);
            //If the bot hasn't moved more than 1 unit we're probably stuck.
            //Remember - this is only checked for while returning - not guarding
            if (%movedist < 1.0)
            {
               if (%obj.path !$= "")
                  %this.moveToNode(%this.currentNode);
               else
               {
                  //Set our holdcnt to 1 less than the maximum so we only hold for 1 cycle
                  %obj.holdcnt = $AI_GUARD_HOLDCNT_MAX-1;
                  //Call sidestep to pick a new move destination near the bot
                  if (%obj.doesSidestep)
                     %this.sidestep(%obj);
               }
            }
            else
            {
               //Check to make sure the bot is not pathed
               if (%obj.path $= "")
               {
                  //We're returning and we're not stuck. So make sure we're looking the direction we're running.
                  %obj.setAimLocation(%obj.marker.getposition());
               }
            }
            //Set our oldpos to match our current position so that next time we cycle through
            //we'll know if we're going anywhere or not
            %obj.oldpos = %obj.getposition();
            //Scedhule ourselves to think at our regular interval
            %this.ailoop = %this.schedule($AI_GUARD_SCANTIME * %obj.attentionlevel, "Think", %obj);

         default:
            %obj.action = "Holding";
            %this.ailoop = %this.schedule(100  ,"Think" , %obj);
		}

   //If you want to see the bots thinking processes in action then uncomment the
   //lines below. It will then set the hud above the bot to show it's current
   //action/attention level/damage/ammo
   //(Used during testing, but kind of fun to watch when you have
   //several bots on the map at a time to see how things are working out.)

   //%objname= %obj.action @ ":"@ %this.attentionlevel @ ":" @ %obj.getdamagelevel() @ ":" @ %obj.getInventory(%obj.Ammo) ;
   //%obj.setshapename(%objname);

   //Clear aim if attention hits max.
   if (%this.attentionlevel == $AI_GUARD_MAX_ATTENTION)
      %obj.clearaim();
}

//Causes AIPlayer to slowly pace around their current location
function AIPlayer::Pacing(%this, %obj)
{
   //%xrand and %yrand are set to be a random number that is equal to -1/2$AI_GUARD_MAX_PACE and +1/2$AI_GUARD_MAX_PACE
   %xrand = getRandom(1,$AI_GUARD_MAX_PACE)-$AI_GUARD_MAX_PACE/2;
   %yrand = getRandom(1,$AI_GUARD_MAX_PACE)-$AI_GUARD_MAX_PACE/2;

   while(%xrand > -$AI_GUARD_MIN_PACE && %xrand < $AI_GUARD_MIN_PACE)
   {
      %xrand = getRandom(1,$AI_GUARD_MAX_PACE)-$AI_GUARD_MAX_PACE/2;
   }
   while(%yrand > -$AI_GUARD_MIN_PACE && %yrand < $AI_GUARD_MIN_PACE)
   {
      %yrand = getRandom(1,$AI_GUARD_MAX_PACE)-$AI_GUARD_MAX_PACE/2;
   }

   //%newloc is first set to the bots current position
   %newLoc = %obj.getTransform();

   //Set the bots returning position to its marker if it's guarding
   if (%obj.doesGuard $= "guard")
      %obj.returningPos = %obj.marker.getposition();

   //If the is away from its returning position, go back to it so it doesn't wander too far away
   %basedist = vectorDist(%obj.getposition(), %obj.returningPos);
   if(%basedist > $AI_GUARD_MIN_PACE)
      %newLoc = %obj.returningTrans;

   else
   {
      //Word(0) of %newloc (which is the x value) is set to equal it's original value plus the value
      //of %xrand. The -/+ aspect of this equivalates to a left/right direction.
      %newLoc = setWord(%newLoc, 0, (getWord(%newLoc, 0) + (%xrand)));
      //Word(1) of %newloc (which is the y value) is set to equal it's original value plus the value
      //of %yrand. The -/+ aspect of this equivalates to a forward/back direction.
      %newLoc = setWord(%newLoc, 1, (getWord(%newLoc, 1) + (%yrand)));

      %basedist = vectorDist(%obj.getposition(), %newLoc);
      //If the target location is very close, don't preform a line of sight test
      if(%basedist > $AI_GUARD_LOS_BYPASS)
      {
         //Line of sight test for the position the bot wants to pace to
         %eyeTrans = %obj.getEyeTransform();
         %eyeEnd = %newLoc;
         %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
         $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
         %foundObject = getword(%searchResult,0);

         if (%foundObject > 0)
         {
            %this.pacing(%obj);
            return;
         }
      }
   }
   //Set the bot to move at a different speed than normal while pacing
   %obj.setMoveSpeed($AI_GUARD_PACE_SPEED);
   //Set the bot to look in the direction that it is moving.
   %obj.setaimlocation(%newLoc);
   //Set the bot to move towards the new position.
   %obj.setMoveDestination(%newLoc);
}
//Sidestep is used to find a random spot near the bot and attempt to have them move towards it.
function AIPlayer::SideStep(%this, %obj)
{
   //%xrand and %yrand are set to be a random number that is equal to -1/2$AI_GUARD_SIDESTEP and +1/2$AI_GUARD_SIDESTEP
   %xrand = getRandom(1,$AI_GUARD_SIDESTEP)-$AI_GUARD_SIDESTEP/2;
   %yrand = getRandom(1,$AI_GUARD_SIDESTEP)-$AI_GUARD_SIDESTEP/2;
   //%newloc is first set to the bots current position
   %newLoc = %obj.getTransform();
   //Word(0) of %newloc (which is the x value) is set to equal it's original value plus the value
   //of %xrand. The -/+ aspect of this equivalates to a left/right direction.
   %newLoc = setWord(%newLoc, 0, (getWord(%newLoc, 0) + (%xrand)));
   //Word(1) of %newloc (which is the y value) is set to equal it's original value plus the value
   //of %yrand. The -/+ aspect of this equivalates to a forward/back direction.
   %newLoc = setWord(%newLoc, 1, (getWord(%newLoc, 1) + (%yrand)));

   //If the bot is pathed, get ready to move to the correct node
   if (%obj.path !$= "")
   {
      if (%this.returningPath == 1)
      {
         %this.returningPath = 2;
      }
   }

   //If there's a target, keep aiming at it while sidestepping
   %tgt = %this.GetClosestPlayerInSightandRange(%obj);
   if(%tgt >= 0)
   {
      %obj.setAimObject(%tgt, "0 0 1");

      %basedist = vectorDist(%obj.getposition(), %newLoc);
      //If the target location is very close and we have a target player, don't preform a line of sight test
      if(%basedist > $AI_GUARD_LOS_BYPASS)
      {
         //Line of sight test for the position the bot wants to sidestep to
         %eyeTrans = %obj.getEyeTransform();
         %eyeEnd = %newLoc;
         %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
         $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
         %foundObject = getword(%searchResult,0);

         if (%foundObject > 0)
         {
            %this.sidestep(%obj);
            return;
         }
      }
   }
   //There is no target
   else
   {
      //Line of sight test for the position the bot wants to sidstep to
      %eyeTrans = %obj.getEyeTransform();
      %eyeEnd = %newLoc;
      %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
      $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
      %foundObject = getword(%searchResult,0);

      if (%foundObject > 0)
      {
         %this.sidestep(%obj);
         return;
      }
      //Set the bot to look in the direction that it is moving.
      else
         %obj.setaimlocation(%newloc);
   }
   //Set the bot to move towards the new position.
   %obj.setMoveDestination(%newLoc);
}

function AIPlayer::CheckLOStoItem(%this, %obj, %item)
{
   %basedist = vectorDist(%obj.getposition(), %item.getposition());
   //If the target item is very close, don't preform a line of sight test
   if(%basedist > $AI_GUARD_LOS_BYPASS)
   {
      %eyeTrans = %obj.getEyeTransform();
      //%eyeEnd = %item.getposition();
      %eyeEnd = %item.getWorldBoxCenter();
      %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::TerrainObjectType |
      $TypeMasks::InteriorObjectType | $TypeMasks::ItemObjectType | $TypeMasks::PlayerObjectType |
      $TypeMasks::StaticTSObjectType | $TypeMasks::StaticObjectType , %obj);
      %foundObject = getword(%searchResult,0);

      if(%foundObject == %item || %foundObject == 0)
      {
         return true;
      }
      else
      {
         return false;
      }
   }
   else
   {
      return true;
   }
}
//This is another function taken from code found on garagegames.
//It checks to see if there are any static objects blocking the view
//from the AIPlayer to the target.
function AIPlayer::CheckLOS(%this, %obj, %tgt)
{
  %eyeTrans = %obj.getEyeTransform();
  %eyeEnd = %tgt.getEyeTransform();
  %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
  $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
  %foundObject = getword(%searchResult,0);

	if (%foundObject > 0)
	{
		if(%foundObject.getType() & $TypeMasks::PlayerObjectType)
		{
			//Get the target's location and set it as the bot's return point
			%obj.returningPos = %tgt.getposition();
			%obj.returningTrans = %tgt.gettransform();
			%obj.lostest = 1;
			return true;
		}
		else
		{
			//If the bot just lost sight of the player, get the player's position a short time after that
			if(%obj.lostest == 1)
			{
			  %obj.lostest = 0;
			  %this.lostrigger = %this.schedule($AI_GUARD_LOS_TIME,"getnewguardposition", %obj, %tgt);
			}
			return false;
		}
	}
}

//Finds a cover marker that the bot can see, but the enemy cannot.
function AIPlayer::findCover(%this, %obj)
{
   if (!isObject(%obj.coverFrom) || !isObject(AICover))
      return -1;
   for (%i = 0; %i < AICover.getCount(); %i++)
   {
      %cover = AICover.getObject(%i);
      if (!%cover.used)
      {
         %eyeTrans = %obj.getEyeTransform();
         %enemyEyeTrans = %obj.coverFrom.getEyeTransform();
         %eyeEnd = %cover.getTransform();
         //Can the bot see the cover marker
         //Is the cover marker too close to the enemy
         if (VectorDist(%eyeTrans, %eyeEnd) <= %obj.detDis && VectorDist(%enemyEyeTrans, %eyeEnd) >= 15)
         {
            %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
            $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
            %foundObject = getword(%searchResult, 0);
            if (%foundObject == 0 || (%foundObject > 0 && %foundObject.getID() == %cover.getID()))
            {
               %searchResult = containerRayCast(%enemyEyeTrans, %eyeEnd, $TypeMasks::PlayerObjectType | $TypeMasks::StaticTSObjectType |
               $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj.coverFrom);
               %foundObject = getword(%searchResult,0);
               if (%foundObject > 0 && %foundObject.getID() != %cover.getID() && %foundObject.getID() != %obj.getID())
               {
                  return %cover.getID();
               }
            }
         }
      }
   }
   return -1;
}
//Tests to see if the bot's cover marker is still out of sight of the enemy.
function AIPlayer::testCover(%this, %obj)
{
   %eyeTrans = %obj.coverFrom.getEyeTransform();
   %eyeEnd = %obj.getWorldBoxCenter();
   if (VectorDist(%eyeTrans, %eyeEnd) <= %obj.detDis)
   {
      %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::StaticTSObjectType |
      $TypeMasks::TerrainObjectType | $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType);
      %foundObject = getword(%searchResult, 0);
      if (%foundObject == 0 || %foundObject.getGroup() $= "AICover")
         return false;
      else
         return true;
   }
   else
      return true;
}
//Get the player's position a short time after sight is lost
function AIPlayer::GetNewGuardPosition(%this, %obj, %tgt)
{
  %obj.returningPos = %tgt.getposition();
  %obj.returningTrans = %tgt.gettransform();
}

function AIPlayer::GetClosestHumanInSightandRange(%this, %obj)
{
	%dist=0;
	%index = -1;    //sets the initial index value to -1  The index is the id number of the nearest
							 //human target found
	%botpos = %this.getposition();		//The bots current position
	%count = ClientGroup.getCount();		//The number of clients to check

	//The for-next loop cycles through all of the valid clients
	for(%i=0; %i < %count; %i++)
	{
		%client = ClientGroup.getobject(%i);		//Get the client info for the client at index %i

		//If the client is invalid then the function bails out returning a -1 value, for no target found.
		if (%client.player !$= "" && %client.player > 0)
		{

		//The following line just changes the %client to %tgt to make it easier to follow the code below
		%tgt = %client;

		%playpos = %client.player.getposition();		//Assigns the player position to a variable

		%tempdist = vectorDist(%playpos, %botpos);	    //Determine the distance from the bot to the target


		//The first test we perform is to see if the target is within the bots range
		//Is target in range? If not bail out of checking to see if its in view.
		if (%tempdist <= %obj.detdis)
		{

			//Lower attentionlevel to increase response time...
			%this.attentionlevel--;

			//Prevent the attention level from dropping below 1
			if(%this.attentionlevel < 1) %this.attentionlevel = 1;

			//The second check is to see if the target is within the FOV of the bot.
			//Is the target within the fov field of vision of the bot?
			if(%this.Istargetinview(%obj, %tgt, %obj.fov))
			{

					//Lower attentionlevel to increase response time...
					%this.attentionlevel--;

					//Prevent the attention level from dropping below 1
					if(%this.attentionlevel < 1) %this.attentionlevel = 1;

					//The third check we run is to see if there is anything blocking the
					//target from the bot.
					if(%this.CheckLOS(%obj, %tgt))
					{

					//We lower the bots attention level again, to further increase it's
					//response time, this effectively means that the bot will respnd faster to
					//objects that are both in range and in plain sight.
					%this.attentionlevel--;

					//Prevent the attention level from dropping below 1
					if(%this.attentionlevel < 1) %this.attentionlevel = 1;

					//If there is a current target, then check the distance to the new target as
					//compared to the current set target. If the new target is closest, then set
					//the index and tempdistance to the new target.
					if(%tempdist < %dist || %dist == 0)
					{
						%dist = %tempdist;
						%index = %i;
					}
				}
			}
		}
		}
		else
		{
			//If there are no targets in view, then the bots attention will slowly lapse and increase
			//This will slow down how fast the bot thinks and how often it checks for threats.
			%this.attentionlevel = %this.attentionlevel + 0.5;
			if(%this.attentionlevel > $AI_GUARD_MAX_ATTENTION) %this.attentionlevel = $AI_GUARD_MAX_ATTENTION;
		}
	}

	return %index;
}
function AIPlayer::GetClosestPlayerInSightandRange(%this, %obj)
{
	%dist=0;
	%index = -1;    //sets the initial index value to -1  The index is the id number of the nearest
							 //human target found
	%botpos = %this.getposition();		//The bots current position
	if (%this.team != 0)
	{
      %count = ClientGroup.getCount();		//The number of clients to check

      //The for-next loop cycles through all of the valid clients
      for(%i = 0; %i < %count; %i++)
      {
         %client = ClientGroup.getobject(%i);		//Get the client info for the client at index %i
         if (%client.player !$= "" && %client.player > 0)
         {
            %className = %client.player.getClassName();
            //If the client is invalid then the function bails out returning a -1 value, for no target found.
            if (%className $= "Player")
            {
               //The following line just changes the %client to %tgt to make it easier to follow the code below
               %tgt = %client;
               %playpos = %client.player.getposition();		//Assigns the player position to a variable
               %tempdist = vectorDist(%playpos, %botpos);	    //Determine the distance from the bot to the target
               //The first test we perform is to see if the target is within the bots range
               //Is target in range? If not bail out of checking to see if its in view.
               if (%tempdist <= %obj.detdis)
               {
                  //Lower attentionlevel to increase response time...
                  %this.attentionlevel--;
                  //Prevent the attention level from dropping below 1
                  if(%this.attentionlevel < 1) %this.attentionlevel = 1;
                  //The second check is to see if the target is within the FOV of the bot.
                  //Is the target within the fov field of vision of the bot?
                  if(%this.Istargetinview(%obj, %tgt.player, %obj.fov))
                  {
                     //Lower attentionlevel to increase response time...
                     %this.attentionlevel--;
                     //Prevent the attention level from dropping below 1
                     if(%this.attentionlevel < 1)
                        %this.attentionlevel = 1;
                     //The third check we run is to see if there is anything blocking the
                     //target from the bot.
                     if(%this.CheckLOS(%obj, %tgt.player))
                     {
                        //We lower the bots attention level again, to further increase it's
                        //response time, this effectively means that the bot will respnd faster to
                        //objects that are both in range and in plain sight.
                        %this.attentionlevel--;

                        //Prevent the attention level from dropping below 1
                        if(%this.attentionlevel < 1) %this.attentionlevel = 1;

                        //If there is a current target, then check the distance to the new target as
                        //compared to the current set target. If the new target is closest, then set
                        //the index and tempdistance to the new target.
                        if(%tempdist < %dist || %dist == 0)
                        {
                           %dist = %tempdist;
                           %index = %i;
                           %humanTarget = %tgt.player;
                        }
                     }
                  }
               }
            }
         }
      }
	}
	else
	{
      %index = -1;
	}

	%dist2 = 0;
	%index2 = -1;    //sets the initial index value to -1  The index is the id number of the nearest
							 //bot target found
	%count2 = $botCounter;

   for(%i2 = 0; %i2 < %count2; %i2++)
	{
		//The following line just changes the %client to %tgt to make it easier to follow the code below
		%tgt2 = $botIDs[%i2];
      if ((%this.team != %tgt2.team || %this.team == -1) && isObject( %tgt2 ))
		{
         %playpos2 = %tgt2.getposition();		//Assigns the player position to a variable
         %tempdist2 = vectorDist(%playpos2, %botpos);	    //Determine the distance from the bot to the target
         //The first test we perform is to see if the target is within the bots range
         //Is target in range? If not bail out of checking to see if its in view.
         if (%tempdist2 <= %obj.detdis)
         {
            //Lower attentionlevel to increase response time...
            %this.attentionlevel--;
            //Prevent the attention level from dropping below 1
            if(%this.attentionlevel < 1) %this.attentionlevel = 1;
            //The second check is to see if the target is within the FOV of the bot.
            //Is the target within the fov field of vision of the bot?
            if(%this.Istargetinview(%obj, %tgt2, %obj.fov))
            {
					//Lower attentionlevel to increase response time...
					%this.attentionlevel--;
					//Prevent the attention level from dropping below 1
					if(%this.attentionlevel < 1)
					   %this.attentionlevel = 1;
					//The third check we run is to see if there is anything blocking the
					//target from the bot.
					if(%this.CheckLOS(%obj, %tgt2))
					{
                  //We lower the bots attention level again, to further increase it's
                  //response time, this effectively means that the bot will respnd faster to
                  //objects that are both in range and in plain sight.
                  %this.attentionlevel--;
                  //Prevent the attention level from dropping below 1
                  if(%this.attentionlevel < 1)
                     %this.attentionlevel = 1;
                  //If there is a current target, then check the distance to the new target as
                  //compared to the current set target. If the new target is closest, then set
                  //the index and tempdistance to the new target.
                  if(%tempdist2 < %dist2 || %dist2 == 0)
                  {
                     %dist2 = %tempdist2;
                     %index2 = %i2;
                     %botTarget = %tgt2;
                  }
               }
            }
         }
		}
	}
	if(%index >= 0 && %index2 >= 0)
	{
	   if (%dist < %dist2)
	   {
		  //echo("bot target is human: " @ %humanTarget);
	      return %humanTarget;
	   }
	   else
	   {
		  //echo("bot target is bot: " @ %botTarget);
	      return %botTarget;
	   }
	}
	else if(%index >= 0)
	{
	   //echo("bot target is human: " @ %humanTarget);
	   return %humanTarget;
	}
	else if(%index2 >= 0)
	{
	   //echo("bot target is bot: " @ %botTarget);
	   return %botTarget;
	}
	else
	{
	   //If there are no targets in view, then the bots attention will slowly lapse and increase
	   //This will slow down how fast the bot thinks and how often it checks for threats.
	   %this.attentionlevel = %this.attentionlevel + 1;
	   if(%this.attentionlevel > $AI_GUARD_MAX_ATTENTION)
	   {
	      %this.attentionlevel = $AI_GUARD_MAX_ATTENTION;
	   }
	   return %index;
	}
}
function AIPlayer::GetClosestItemInSightandRange(%this, %obj, %itemname)
{
   %dist = 0;
   %index = -1;
   %botpos = %obj.getposition();
   InitContainerRadiusSearch(%botpos, $AI_GUARD_DETECT_DISTANCE, $TypeMasks::ItemObjectType);
   while ((%item = containerSearchNext()) != 0)
  	{
      if (%item.getDataBlock().getName() $= %itemname)
      {
			%itempos = %item.getposition();
			%tempdist = vectorDist(%itempos, %botpos);

			if(%this.IsIteminview(%obj, %item, %obj.fov))
         {
				if(%this.CheckLOStoItem(%obj, %item))
				{
					if(%tempdist < %dist || %dist == 0)
					{
						%dist = %tempdist;
						%index = %item;
					}
  				}
			}
		}
	}
	return %index;
}

//This function checks to see if the target supplied is within the bots FOV
function AIPlayer::IsItemInView(%this, %obj, %item, %fov)
{
	%ang = %this.check2dangletoitem(%obj, %item);
	%visleft = 360 - (%fov/2);
	%visright = %fov/2;
	if (%ang > %visleft || %ang < %visright)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//This function checks to see if the target supplied is within the bots FOV
function AIPlayer::IsTargetInView(%this, %obj, %tgt, %fov)
{
   if (%tgt.stealth $= "")
      %tgt.stealth = $AI_GUARD_DEFAULT_PLAYER_STEALTH;
   if (%tgt.moveSilently $= "")
      %tgt.moveSilently = $AI_GUARD_DEFAULT_PLAYER_MOVE_SILENTLY;
	%ang = %this.check2dangletotarget(%obj, %tgt);
	%visleft = 360 - (%fov/2);
	%visright = %fov/2;
	%rand = getRandom(0, %obj.hearingVariance);
	echo(%obj.hearing SPC %rand SPC %tgt.stealth SPC VectorLen(%tgt.getVelocity()) SPC %tgt.moveSilently);
	if (%ang > %visleft || %ang < %visright || (%obj.hearing + %rand) > (%tgt.stealth - (VectorLen(%tgt.getVelocity()) * %tgt.moveSilently)))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Check if the location the bot is moving to is in sight.
//And if it's not, move somwhere that is in sight (if there's a better place).
function AIPlayer::dontMoveAlongTheWall(%this, %obj)
{
   //Save the original destination to another variable for later use
   %this.moveDestinationB = %this.moveDestinationA;

   if (%this.checkMovementLos(%obj))
      return;

   else
   {
      //Word(0) of %this.moveDestinationB (which is the x value) is set to equal the value of %this.moveDestinationA's Word(0).
      %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.moveDestinationA, 0)));
      //Word(1) of %this.moveDestinationB (which is the y value) is set to equal the value of %this.getposition()'s Word(1).
      %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.getposition(), 1)));

      if (%this.checkMovementLos(%obj))
      {
         //Add AI_GUARD_CORNERING's value to the destination's value
         %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.moveDestinationA, 0) + $AI_GUARD_CORNERING));
         %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.getposition(), 1) + $AI_GUARD_CORNERING));

         if (%this.checkMovementLos(%obj))
            return;

         else
         {
            //Or else subtracts AI_GUARD_CORNERING's value from the destination's value
            %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.moveDestinationA, 0) - $AI_GUARD_CORNERING));
            %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.getposition(), 1) - $AI_GUARD_CORNERING));
            %obj.setmovedestination(%this.moveDestinationB);
         }
      }
      else
      {
         //Word(0) of %this.moveDestinationB (which is the x value) is set to equal the value of %this.getposition()'s Word(0).
         %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.getposition(), 0)));
         //Word(1) of %this.moveDestinationB (which is the y value) is set to equal the value of %this.moveDestinationA's Word(1).
         %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.moveDestinationA, 1)));

         if (%this.checkMovementLos(%obj))
         {
            //Add AI_GUARD_CORNERING's value to the destination's value
            %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.getposition(), 0) + $AI_GUARD_CORNERING));
            %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.moveDestinationA, 1) + $AI_GUARD_CORNERING));

            if (%this.checkMovementLos(%obj))
              return;

            else
            {
              //Or else subtracts AI_GUARD_CORNERING's value from the destination's value
              %this.moveDestinationB = setWord(%this.moveDestinationB, 0, (getWord(%this.getposition(), 0) - $AI_GUARD_CORNERING));
              %this.moveDestinationB = setWord(%this.moveDestinationB, 1, (getWord(%this.moveDestinationA, 1) - $AI_GUARD_CORNERING));
              %obj.setmovedestination(%this.moveDestinationB);
            }
         }
         else
            %obj.setmovedestination(%this.moveDestinationA);
      }
   }
}
//Line of sight test for the position the bot wants to move to
function AIPlayer::checkMovementLos(%this, %obj)
{
    %eyeTrans = %obj.getEyeTransform();
    %eyeEnd = %this.moveDestinationB;
    %searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::StaticTSObjectType | $TypeMasks::TerrainObjectType |
    $TypeMasks::ItemObjectType | $TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
    %foundObject = getword(%searchResult,0);

    if (%foundObject == 0)
    {
        //Check to make sure the bot isn't already extremly close to its dstination
        %basedist = vectorDist(%obj.getposition(), %this.moveDestinationB);

	    if (%basedist > 0.5)
        {
		    %obj.setmovedestination(%this.moveDestinationB);
            return true;
        }
        else
        {
            return false;
        }
    }

    else
    {
        return false;
    }
}

// Gets the closest player to object. Used by the trigger to determine if a player triggered it.
function AIPlayer::GetClosestPlayer(%this, %obj)
{
	%dist=0;
	%index = -1; //sets the initial index value to -1  The index is the id number of the nearest
							 //human target found
	%botpos = %this.getposition();		//The bots current position
	%count = ClientGroup.getCount();		//The number of clients to check

	//The for-next loop cycles through all of the valid clients
	for(%i=0; %i < %count; %i++)
	{
		%client = ClientGroup.getobject(%i);		//Get the client info for the client at index %i

			//If the client is invalid then the function bails out returning a -1 value, for no
		//target found.
		if (%client.player !$= "" && %client.player > 0)
		 {
		    %index = %client.player;
		 }
	}
}

//This function is not called within this code but is left in just in case it's needed elsewhere.
function AIPlayer::animate(%this,%seq)
{
   //%this.stopThread(0);
   //%this.playThread(0,%seq);
   %this.setActionThread(%seq);
}
//-----------------------------------------------------------------------------
// Pathed AI Functions
//-----------------------------------------------------------------------------

function AIPlayer::followPath(%this, %path, %node, %obj)
{
   //Start the bot following a path
   %this.stopThread(0);
   //Check if the bot is pathed
   if (!isObject(%path))
   {
      %this.path = "";
      return;
   }

   %dist = 0;
   %tempdist = 0;
   %index = -1;
   %botpos = %this.getposition();
   %count = %path.getCount();
   //Cycle through all nodes on this path and set the closest node as the bot's current location
   while ((%node = %count) != 0)
   {
     	%nodepos = %this.path.getObject(%count - 1).getposition();
		%tempdist = vectorDist(%nodepos, %botpos);

		if(%tempdist < %dist || %dist == 0)
		{
			%dist = %tempdist;
			%index = %node;
		}
     	%count--;
   }
   %index = %index - 1;
   %this.moveToNode(%index);

   if (%index > %path.getCount() - 1)
   {
      %this.targetNode = %path.getCount() - 1;
   }
   else
   {
      %this.targetNode = %index;
   }
}

function AIPlayer::moveToNextNode(%this, %obj)
{
   //See if the bot just sidesteped
   if (%this.returningPath == 2)
   {
      //Set returningPath back to 1 for other functions
      %this.returningPath = 1;
      %this.moveToNode(%this.currentNode);
      return;
   }

   //See where the bot is and where it should be going
   if (%this.targetNode < 0 || %this.currentNode < %this.targetNode)
   {
      if (%this.currentNode < %this.path.getCount() - 1)
      {
         %this.moveToNode(%this.currentNode + 1);
      }
      else
      {
         %this.moveToNode(0);
      }
   }
   else
   {
      if (%this.currentNode == 0)
      {
         %this.moveToNode(%this.path.getCount() - 1);
      }
      else
      {
         %this.moveToNode(%this.currentNode - 1);
      }
   }
}

function AIPlayer::moveToNode(%this, %index, %obj)
{
   //Move to the given path node index
   %this.currentNode = %index;
   %node = %this.path.getObject(%index);
   %this.setMoveDestination(%node.getTransform(), %index == %this.targetNode);
   %this.targetNode = %this.currentNode + 1;

   //Make the bot face the node it's moving to
   %this.setAimLocation(%this.path.getObject(%this.currentNode).getposition());
}
