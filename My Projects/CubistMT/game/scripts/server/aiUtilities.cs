//The AIUTILITIES.CS file creates the ai resources and contians usefull functions for controlling AI
//The kangaroo uses a simple state machine to control it's actions.

$AI_RESOURCE_MARKER_HIDE = true;				//Turns resource marker hiding on or off - useful when editing maps.

$AI_RESOURCE_GRASS_MASK = 1;              //mask for a grass resource
$AI_RESOURSE_SUNNY_MASK = 2;              //mask for a sunny spot resource
$AI_RESOURSE_HIDING_MASK = 4;              //mask for a hiding spot resource
$AI_RESOURSE_TREE_MASK = 8;               //mask for climbable tree
$AI_RESOURSE_AIR_MASK = 16;
$AI_READY_TO_MOVE = 1; //the kangaroo is ready to move
$AI_WAITING_FOR_ANIM = 0; //the kangaroo is waiting for animation to finish before moving

datablock StaticShapeData(AIResourceMarker)
{
   category = "AIMarker";
   shadowEnable = false;
   shapeFile = "core/art/shapes/octahedron.dts";
};

//The LoadEntities function replaces the markers placed in the map with the AI bots during the
//mission loading.
function AIUtilities::LoadEntities()
{
   deleteVariables("$ai_resources*");

   $ai_num_resources=0;
   //$ai_resources = new ArrayObject();
   
   %default_id = $AI_RESOURCE_GRASS_MASK;

   %position = "0 0 0";
   %radius = 100000.0;

   InitContainerRadiusSearch(%position, %radius, $TypeMasks::StaticObjectType);
   while ((%targetObject = containerSearchNext()) != 0)
   {
      if(%targetobject.getclassname() $= "StaticShape")
      {
         if (%targetobject.getDataBlock().getName() $= "AIResourceMarker")
         {
            $ai_resources[$ai_num_resources] = %targetobject;
            if (%targetObject.id $= "")
            {
               %targetObject.id = %default_id;
            }
            $ai_num_resources++;
         }
      }
   }
   echo("Loading AI Resources...");
	//This determines whether to hide or not hide the markers during mission loading.
	//It's helpful to have the markers visible when editing the map and fine tuning the bot
	//placement.
	//This search is identical to the one above, only it hides the markers if found.
	if ($AI_RESOURCE_MARKER_HIDE == true)
	{
		echo("Hiding AI Resource markers...");		
		%position = "0 0 0";
		%radius = 100000.0;
		InitContainerRadiusSearch(%position, %radius, $TypeMasks::StaticObjectType);
		while ((%targetObject = containerSearchNext()) != 0)
  		{
 			if(%targetobject.getclassname() $= "StaticShape")
 			{	
 				if (%targetobject.getDataBlock().getName() $= "AIResourceMarker")
    	  			%targetobject.sethidden(true);	
  			}
		}
	}
}

//AITargeting

//   Return the angle of a vector in relation to world origin
function AIUtilities::GetAngleofVector(%vec)
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
                                                        

//Modified to be a static function
//angle returned is within the 0-360 range.
function AIUtilities::Check2DAngletoItem(%pos,%eyeVec, %itemPos)
{
	%eyeangle = AIUtilities::getAngleofVector(%eyeVec);
	%posVec = VectorSub(%itemPos, %pos);
	%posangle = AIUtilities::getAngleofVector(%posVec);
	%angle = %posangle - %eyeAngle;
	%angle = %angle ? %angle : %angle * -1;
   if (%angle < 0)  %angle = %angle + 360;
	return %angle;
}

//Modified to be a static function
//angle returned is within the 0-360 range.
function AIUtilities::Check2DAngletoTarget(%pos,%eyeVec, %tgt)
{
	%eyeangle = AIUtilities::getAngleofVector(%eyeVec);
	%posVec = VectorSub(%tgt.player.getPosition(), %pos);
	%posangle = AIUtilities::getAngleofVector(%posVec);
	%angle = %posangle - %eyeAngle;
	%angle = %angle ? %angle : %angle * -1;
   if (%angle < 0)  %angle = %angle + 360;
	return %angle;
}

function AIUtilities::get2DAngle(%posVec, %eyeVec)
{
	%eyeangle = AIUtilities::getAngleofVector(%eyeVec);
	%posangle = AIUtilities::getAngleofVector(%posVec);
	%angle = %posangle - %eyeAngle;
	%angle = %angle ? %angle : %angle * -1;
   if (%angle < 0)  %angle = %angle + 360;
	return %angle;
}

//returns a new Location within %maxDistance from %pos
function AIUtilities::GetNewLocCloseTo(%pos,%maxDistance)
{
   //%xrand and %yrand are set to be a random number that is less than %maxDistance and greater than -%maxDistance
   %xrand = mSin(getRandom(10000)*3.14/5000);
   %yrand = mSqrt(1-%xrand*%xrand);
   if (getRandom(0,1) > 0.5)
   {
      %yrand = -%yrand;
   }
   //%newloc is first set to the bots current position
   %newLoc = %pos;
   //Word(0) of %newloc (which is the x value) is set to equal it's original value plus the value
   //of %xrand*%maxDistance. The -/+ aspect of this equivalates to a left/right direction.
   %newLoc = setWord(%newLoc, 0, (getWord(%newLoc, 0) + (%xrand*%maxDistance)));
   //Word(1) of %newloc (which is the y value) is set to equal it's original value plus the value
   //of %yrand*%maxDistance. The -/+ aspect of this equivalates to a forward/back direction.
   %newLoc = setWord(%newLoc, 1, (getWord(%newLoc, 1) + (%yrand*%maxDistance)));
   return %newLoc;
}

//This function checks to see if the target supplied is within the bots FOV
function AIUtilities::IsItemInView(%obj, %itemPos, %fov)
{
	%ang = AIUtilities::check2dangletoitem(%obj.getPosition(),%obj.getEyeVector(), %itemPos);
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
function AIUtilities::IsTargetInView(%obj, %tgt, %fov)
{
	%ang = AIUtilities::Check2DAngletoItem(%obj.getPosition(),%obj.getEyeVector(), %tgt.getPosition());
	
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

//This function returns the position of closest AIResource to %pos that is not %not and is
// of type %mask
// returns %not if no other resources found
function AIUtilities::GetAnotherAIResource(%pos,%mask,%not)
{
   %minDist = -1; //the current minimum distance to new resource found, < 0 if none found
	for(%i = 0; %i<$ai_num_resources; %i++)
   {
      %targetobject = $ai_resources[%i];
      %objectpos = %targetobject.getPosition();
      %tempdist = vectorDist(%pos, %objectpos);
      if ((%tempdist < %minDist || %minDist < 0) && %targetObject != %not)
      {
         if ((%targetobject.id & %mask) > 0)
         {
            %newresource = %targetobject;
            %minDist = %tempdist;
         }
      }
  	}
   if (%minDist > 0)
   {
      //we have found a new position
      return %newresource;
   }
   else
   {
      //no new resource found, use current
      return %not;
   }
}

//This function returns the position of closest AIResource to %pos that is not %not and is
// of type %mask and within %maxAngle from %dirAngle
// returns %not if no other resources found
function AIUtilities::GetAnotherAIResourceWithinAngle(%pos,%mask,%not,%dirAngle,%maxAngle)
{
   %minDist = -1; //the current minimum distance to new resource found, < 0 if none found
	for(%i = 0; %i<$ai_num_resources; %i++)
   {
      %targetobject = $ai_resources[%i];
      %objectpos = %targetobject.getPosition();
      %tempdist = vectorDist(%pos, %objectpos);
      if ((%tempdist < %minDist || %minDist < 0) && %targetObject != %not)
      {
         if ((%targetobject.id & %mask) > 0)
         {
            %angleToResource = AIUtilities::GetAngleofVector(VectorSub(%objectPos,%pos));
            %angleDiff = %angleToResource-%dirAngle;
            if (%angleDiff<-180)
            {
               %angleDiff += 360;
            }
            else if (%angleDiff>180)
            {
               %angleDiff -= 360;
            }
            if (%angleDiff >= -%maxAngle && %angleDiff <= %maxAngle)
            {
               //echo("found new ai resource ("@%targetobject@") at angle "@%angleDiff);
               %newresource = %targetobject;
               %minDist = %tempdist;
            }
         }
      }
  	}
   if (%minDist > 0)
   {
      //we have found a new position
      return %newresource;
   }
   else
   {
      //no new resource found, use current
      return %not;
   }
}

function AIUtilities::get2DDistance(%vec1, %vec2)
{
   %diff = (GetWord(%vec2, 0)-GetWord(%vec1, 0)) @" "@ (GetWord(%vec2, 1)-GetWord(%vec1, 1));
   return VectorLen(%diff);
}

function AIUtilities::getRandomAIResource(%mask,%not)
{
   AIUtilities::getRandomAIResource(%mask,%not,"0 0 0", 10000);
}

function AIUtilities::getRandomAIResource(%mask,%not,%pos, %range)
{
   %array = new ArrayObject();
   %numFound = 0;
	for(%i = 0; %i<$ai_num_resources; %i++)
   {
      %targetobject = $ai_resources[%i];
      %objectpos = %targetobject.getPosition();
      %tempdist = vectorDist(%pos, %objectpos);
      if (%tempdist < %range && %targetObject != %not)
      {
         if ((%targetobject.id & %mask) > 0)
         {
            //echo("found new ai resource ("@%targetobject@") at angle "@%angleDiff);
            %array.push_back(%numFound,%targetobject);
            %numFound++;
         }
      }
  	}
   if (%numFound > 0)
   {
      %newResource = %array.getValue(getRandom(10000) % %numFound);
   }
   else
   {
      %newresource = %not;
   }
      
   %array.delete();

   return %newresource;
}
function AIUtilities::VectorRotate2D(%vector, %angle_in_deg)
{
   %angle_in_rad = mDegToRad(%angle_in_deg);
	%vecx = getWord(%vector,0);
	%vecy = getWord(%vector,1);
   %newvecx = mcos(%angle_in_rad)*%vecx - msin(%angle_in_rad)*%vecy;
   %newvecy = msin(%angle_in_rad)*%vecx + mcos(%angle_in_rad)*%vecy;
   //echo("rotate("@%angle_in_deg@") =("@%vecx@","@%vecy@")->("@%newvecx@","@%newvecy@")");
   %vector = setWord(%vector,0,%newvecx);
   %vector = setWord(%vector,1,%newvecy);
   //echo("%vector ="@%vector);
   return %vector;
}

function AIUtilities::VectorTangentLeft(%vector)
{
	%vecx = getWord(%vector,0);
	%vecy = getWord(%vector,1);
   %newvecx = -%vecy;
   %newvecy = %vecx;
   %vector = setWord(%vector,0,%newvecx);
   %vector = setWord(%vector,1,%newvecy);
   return %vector;
}

function AIUtilities::VectorTangentRight(%vector)
{
	%vecx = getWord(%vector,0);
	%vecy = getWord(%vector,1);
   %newvecx = %vecy;
   %newvecy = -%vecx;
   %vector = setWord(%vector,0,%newvecx);
   %vector = setWord(%vector,1,%newvecy);
   return %vector;
}

function AIUtilities::GetClosestHumanInSightandRange(%obj, %range)
{
	%dist=0;
	%index = -1; //sets the initial index value to -1  The index is the id number of the nearest
							 //human target found
	%botpos = %obj.getposition();		//The bots current position
	%count = ClientGroup.getCount();		//The number of clients to check
	
	//The for-next loop cycles through all of the valid clients
	for(%i=0; %i < %count; %i++)
	{
		%client = ClientGroup.getobject(%i);		//Get the client info for the client at index %i
		
			//If the client is invalid then the function bails out returning a -1 value, for no
		//target found.
		if (%client.player !$= "" && %client.player > 0)	
		 {
		
				 
		 //The following line just changes the %client to %tgt to make it easier to follow the code below
		%tgt = %client;
		
		%playpos = %client.player.getposition();		//Assigns the player position to a variable
		
		%tempdist = vectorDist(%playpos, %botpos);	//Determine the distance from the bot to the target
		
		//The first test we perform is to see if the target is within the bots range
		//Is target in range? If not bail out of checking to see if its in view.
		if (%tempdist <= %range)
		{
					
					//Lower attentionlevel to increase response time...
					%obj.attentionlevel--;
						
					//Prevent the attention level from dropping below 1
					if(%obj.attentionlevel < 1) %obj.attentionlevel=1;
			
			//The second check is to see if the target is within the FOV of the bot.
			//Is the target within the fov field of vision of the bot?
			if(AIUtilities::IsTargetInView(%obj, %tgt.player, %obj.fov))
				{
					
					//Lower attentionlevel to increase response time...
					%obj.attentionlevel--;
					
						//Prevent the attention level from dropping below 1
					if(%obj.attentionlevel < 1) %obj.attentionlevel=1;
					//The third check we run is to see if there is anything blocking the
					//target from the bot.
					if(AIUtilities::CheckLOStoPos(%obj, %playpos))
					{
					//We lower the bots attention level again, to further increase it's
					//response time, this effectively means that the bot will respnd faster to
					//objects that are both in range and in plain sight.
					%obj.attentionlevel--;
					
						//Prevent the attention level from dropping below 1
					if(%obj.attentionlevel < 1) %obj.attentionlevel=1;
					
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
				%obj.attentionlevel = %obj.attentionlevel + 0.5;
				if(%obj.attentionlevel > $KANGAROO_MAX_ATTENTION) %obj.attentionlevel=$KANGAROO_MAX_ATTENTION;
					
			}
	}
	return (%index < 0)?0:ClientGroup.getobject(%index).player;
}

function AIUtilities::getClosestScaryObjectInSightAndRange(%obj,%range)
{
	%dist=0;
	%index = 0; 
	%botpos = %obj.getposition();
	InitContainerRadiusSearch(%botpos, %range, $TypeMasks::ProjectileObjectType | $TypeMasks::PlayerObjectType);
   while ((%scaryobj = containerSearchNext()) != 0)
   {
      //echo("found object");
 		if (%scaryobj.getDataBlock().getName() $= "SpearProjectile" ||
          %scaryobj.getDataBlock().getName() $= "BoomerangProjectile" ||
          %scaryobj.getDataBlock().getName() $= "NPCPlayer" ||
          %scaryobj.getDataBlock().getName() $= "BoyNPCPlayer" ||
          %scaryobj.getDataBlock().getName() $= "PlayerBody")
     	{
         %scaryobjpos = %scaryobj.getposition();
         %tempdist = vectorDist(%scaryobjpos, %botpos);
         //echo("%obj.fov = "@%obj.fov);
         if(AIUtilities::IsItemInView(%obj, %scaryobjpos, %obj.fov))
         {
            if(AIUtilities::CheckLOStoPos(%obj, %scaryobjpos))
            {
               %obj.attentionlevel--;
					//Prevent the attention level from dropping below 1
					if(%obj.attentionlevel < 1) %obj.attentionlevel=1;
               if(%tempdist < %dist || %dist == 0)
               {
                  %dist = %tempdist;
                  %index = %scaryobj;
               }
            }
         }
      }
	}
	return %index;
}

function AIUtilities::getClosestFrighteningShapeInRange(%obj,%range)
{
   %human = AIUtilities::GetClosestHumanInSightandRange(%obj,%range);
   //echo("human = "@%human);
   %scaryobj = AIUtilities::getClosestScaryObjectInSightAndRange(%obj,%range);
   //echo("scaryobj = "@%scaryobj);
   if (%human > 0)
   {
      %humanDist = VectorDist(%obj,%human.getPosition());
      if (%scaryobj > 0)
      {
         %scaryObjDist = VectorDist(%obj,%scaryobj.getPosition());
         return (%humanDist < %scaryObjDist)?%human:%scaryobj;
      }
      return %human;
   }
   return %scaryobj;
}

function AIUtilities::CheckLOStoPos(%obj, %position)
{
	%eyeTrans = VectorAdd(%obj.getEyeTransform(),"0 0 1");
   %eyeEnd = %position;
	%searchResult = containerRayCast(%eyeTrans, %eyeEnd, $TypeMasks::TerrainObjectType |
		$TypeMasks::InteriorObjectType | $TypeMasks::StaticObjectType, %obj);
   
	%foundObject = getword(%searchResult,0);
		
	if(%foundObject == 0)
	{
      //echo("in LOS");
		return true;
	}
	else
	{
      //echo(%foundObject@" blocking LOS");
		return false;
	}
}

function AIUtilities::playSound(%obj,%filename)
{
}

function AIUtilities::getClosestPlayer(%obj, %datablock, %range, %fov, %inLOS)
{
   //echo("getClosestPlayer("@%obj@","@%datablock@","@%range@","@%fov@","@%inLOS@")");
   %minDist = -1; //the current minimum distance to new resource found, < 0 if none found

	%pos = %obj.getposition();
   if ($fov !$= "") %dirangle = AIUtilities::GetAngleOfVector(%obj.getEyeVector());
	InitContainerRadiusSearch(%pos, %range, $TypeMasks::PlayerObjectType | $TypeMasks::ShapeBaseObjectType);
   while ((%targetobject = containerSearchNext()) != 0)
   {
      //echo("found player "@%targetobject);
      //echo("player.getDataBlock().getName() = "@%targetobject.getDataBlock().getName());
      if (%targetobject.getDataBlock().getName() $= %datablock)
      {
         %objectpos = %targetobject.getPosition();
         %tempdist = vectorDist(%pos, %objectpos);
         if ((%tempdist < %minDist || %minDist < 0))
         {
            //echo("closer than previous");
            if ($fov !$= "")
            {
               %angleToResource = AIUtilities::GetAngleofVector(VectorSub(%objectPos,%pos));
               %angleDiff = %angleToResource-%dirAngle;
               if (%angleDiff<-180)
               {
                  %angleDiff += 360;
               }
               else if (%angleDiff>180)
               {
                  %angleDiff -= 360;
               }
            }
            if (%fov $= "" || (%angleDiff >= -%maxAngle && %angleDiff <= %maxAngle))
            {
               //echo("infov");
               if (!%inLOS || AIUtilities::CheckLOStoPos(%obj, %objectpos))
               {
                  //echo("inlos");
                  %newresource = %targetobject;
                  %minDist = %tempdist;
               }
            }
         }
      }
   }
   //we have found a new position
   return %newresource;
}

function AIUtilities::getPlayerByName(%name, %position, %radius, %type)
{   
   if (%position $= "") %position = "0 0 0";
   if (%radius $= "") %radius = 100000.0;
   if (%type $= "") %type = $TypeMasks::PlayerObjectType;

   InitContainerRadiusSearch(%position, %radius, %type);
   while ((%targetObject = containerSearchNext()) != 0)
   {
      if (%targetObject.name $= %name || %targetObject.marker.name $= %name) return %targetObject;
   }
   return "";
}

function AIUtilities::GetRandomStaticShape(%position, %datablock, %range)
{
   %radius = (%range $= "")?10000:%range;
   %array = new ArrayObject();
   %num = 0;
   InitContainerRadiusSearch(%position, %radius, $TypeMasks::StaticObjectType);
   while ((%targetObject = containerSearchNext()) != 0)
   {
      if(%targetobject.getclassname() $= "StaticShape")
      {
         if (%targetobject.getDataBlock().getName() $= %datablock)
         {
            %array.push_back(%num,%targetobject);
            %num++;
         }
      }
   }
   if (%num == 0) return "";
   else
   {
      %rand = GetRandom(%num-1);
      %result = %array.getValue(%rand);
      return %result;
   }
}

function AIUtilities::getClosestHumanInRange(%obj,%range)
{
	%dist=0;
	%index = -1; //sets the initial index value to -1  The index is the id number of the nearest
							 //human target found
	%botpos = %obj.getposition();		//The bots current position
	%count = ClientGroup.getCount();		//The number of clients to check
	
	//The for-next loop cycles through all of the valid clients
	for(%i=0; %i < %count; %i++)
	{
		%client = ClientGroup.getobject(%i);		//Get the client info for the client at index %i
		
		//If the client is invalid then the function bails out returning a -1 value, for no
      //target found.
		if (%client.player !$= "" && %client.player > 0)	
      {
         //The following line just changes the %client to %tgt to make it easier to follow the code below
         %tgt = %client;
         
         %playpos = %client.player.getposition();		//Assigns the player position to a variable
         
         %tempdist = vectorDist(%playpos, %botpos);	//Determine the distance from the bot to the target
         
         //The first test we perform is to see if the target is within the bots range
         //Is target in range? If not bail out of checking to see if its in view.
         if (%tempdist <= %range)
         {
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
	return (%index < 0)?0:ClientGroup.getobject(%index).player;
}