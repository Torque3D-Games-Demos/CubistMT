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

//-----------------------------------------------------------------------------
// Demo Pathed AIPlayer.
//-----------------------------------------------------------------------------

datablock PlayerData(DemoPlayer : DefaultPlayerData)
{
   shootingDelay = 2000;
};
//////////////////////////////////////
//
// TRIGGER CONTROLLER
// This code handles the placing and behavior of aiSoldierTriggers
/////////////////////////////////////////

datablock TriggerData(guardTrigger)
{
   tickPeriodMS = 100;
};

function guardTrigger::onEnterTrigger(%this, %trigger, %obj)
{
  echo(%trigger @ " has been triggered!");
  // we've been triggered.  Now check to see if the player triggered the trigger
  // we don't want other enemies to keep spawing more enemies!
  %tgtid = AIPlayer::GetClosestPlayer(%trigger);
         //echo("nearest human is " @ %tgtid);
  // check to see if the player triggered this.
  if (%tgtid == %obj)
  {
      // if triggerMany is set, then we shouldn't do anything.  (or do something different.)
      // if you want a trigger to always spawn an enemy, set the trigger's triggerMany value to "true"
      // default behavior is to trigger once.
     if (!%trigger.triggerMany && !%trigger.doneOnce)
     {

         // set the spawnGroup variable to pass on to the spawn function
         %spawnGroup = %trigger.spawnGroup;

         // let the game know we've already been triggered once.
         %trigger.doneOnce = true;

         // spawn the group
         AIPlayer::spawnByGroup(%spawnGroup);

     }
     else
     {
        // we've been triggered before.  Don't do anything
        // If you wanted to do something different, this is where you would put it.
     }
  }
}

////////////////////////////////////////
//This is the default datablock for the Guard.
//I changed the stock datablock name from those used in AIPLAYER.CS
//I did this to allow me to create different classes of bots with their own
//thinking and reaction routines for each class.
///////////////////////////////////
//
//You can specifiy as many datablocks as you have characters.
//The first variable after PlayerData must be a unique name. The second variable (after the semicolon)
//must be a valid body type.
/*
datablock PlayerData(Bot1 : DefaultPlayerData)
{
   maxDamage = 100;
   maxForwardSpeed = 14;
   maxBackwardSpeed = 13;
   maxSideSpeed = 13;

   shootingDelay = 50;
};

datablock PlayerData(Bot2 : Bot1)
{
   shapeFile = "art/shapes/actors/Gideon/gideonRed.dts";
};


//These functions must be duplicated for every ai datablock
function Bot1::onReachDestination(%this, %obj) { DefaultPlayer::onReachDestination(%this, %obj); }
function Bot1::OnDamage(%this, %obj, %delta) { DefaultPlayer::OnDamage(%this, %obj, %delta); }
function Bot2::onReachDestination(%this, %obj) { DefaultPlayer::onReachDestination(%this, %obj); }
function Bot2::OnDamage(%this, %obj, %delta) { DefaultPlayer::OnDamage(%this, %obj, %delta); }

*/