
$AIRHOCKEY_PUCK_STARTPOS = "0 0 34";
$AIRHOCKEY_PADDLELEFT_STARTPOS = "11.9 -24.4 34";
$AIRHOCKEY_PADDLERIGHT_STARTPOS = "-11.9 24.4 34";
$AIRHOCKEY_PHYSICSTIME = 4.0;


//function serverCmdSetPaddlePosition(%client, %shape, %newPosition, %velocity2)
function serverCmdSetPaddlePosition(%client, %shape, %touchCenter, %dt)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 
   setPaddlePosition(%this, %touchCenter, %dt);
}
function setPaddlePosition(%this, %touchCenter, %dt)
{
   // get the velocity vector to move to the new location
   %velocity = VectorSub(%touchCenter, %this.position);
   
   // time adjust mass factor for moving even the heaviest objects
   %massFactor = ((%this.dataBlock.mass*100)/%dt)/$AIRHOCKEY_PHYSICSTIME;
      
   // apply mass and time factor to the velocity so the object move the
   // right amount for its mass.
   %velocity = VectorScale(%velocity, %massFactor);
   
//   %velocity = VectorScale(%velocity, VectorLen(%velocity));
//   %velocity = VectorNormalize(%velocity);

   // clamp the velocity so we don't have physics explosions
   if(VectorLen(%velocity) > %massFactor*50.0)
      %velocity = VectorScale(VectorNormalize(%velocity), %massFactor*5.0);
//   else if(VectorLen(%velocity) < 1.0)
//      %velocity = VectorScale(VectorNormalize(%velocity), 1.0);
      
   // lock the y axis and apply downward pressure
   %velocity = setWord(%velocity,2,-100/%dt);  
   
   // remove existing momentum from the new velocity
   %velocity = VectorSub(%velocity, %this.getLinVelocity());
   
   // send the new impulse to the physics system
   %this.applyImpulse(%this.getCMassPosition(), %velocity);// * %distFactor));
}

function serverCmdSetPaddleStop(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 
   %this.setLinVelocity("0 0 0");
}

function AirHockey::onAdd(%this)
{
//   %airHockeyTable = new StaticShape() {
   %airHockeyBase = new PhysicsShape() {
	   //shapeName = "art/cube/airhockey/airhockey_table.dts";
      dataBlock = AirHockeyBaseData;
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      enableInputEvents = true;
    //  collisionType = "Visible Mesh";
   };
   %this.add(%airHockeyTable);
   %this.shapeTable = %airHockeyTable;
   
   %airHockeyTable = new PhysicsShape() {
	   //shapeName = "art/cube/airhockey/airhockey_table.dts";
      dataBlock = AirHockeyTableData;
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      enableInputEvents = true;
    //  collisionType = "Visible Mesh";
   };
   %this.add(%airHockeyTable);
   %this.shapeTable = %airHockeyTable;
   
   %this.puckStartPos = VectorAdd(%this.position, $AIRHOCKEY_PUCK_STARTPOS);
   %this.paddleLeftStartPos = VectorAdd(%this.position, $AIRHOCKEY_PADDLELEFT_STARTPOS);
   %this.paddleRightStartPos = VectorAdd(%this.position, $AIRHOCKEY_PADDLERIGHT_STARTPOS);

   %airHockeyPuck = new PhysicsShape() {
      dataBlock = "AirHockeyPuckData";
      position = %this.puckStartPos;
      rotation = %this.rotation;
      scale = %this.scale;
      enableInputEvents = true;
   };
   %this.add(%airHockeyPuck);
   %this.shapePuck = %airHockeyPuck;
   
   %airHockeyPaddleLeft = new PhysicsShape() {
      dataBlock = AirHockeyPaddleData;
      position = %this.paddleLeftStartPos;
      rotation = %this.rotation;
      scale = %this.scale;
   //   hidden = true;
      enableInputEvents = true;
   };
   %this.add(%airHockeyPaddleLeft);
   %this.shapePaddleLeft = %airHockeyPaddleLeft;
   
   %airHockeyPaddleRight = new PhysicsShape() {
      dataBlock = AirHockeyPaddleData;
      position = %this.paddleRightStartPos;
      rotation = %this.rotation;
      scale = %this.scale;
   //   hidden = true;
      enableInputEvents = true;
   };
   %this.add(%airHockeyPaddleRight);
   %this.shapePaddleRight = %airHockeyPaddleRight;
   
   physicsSetTimeScale($AIRHOCKEY_PHYSICSTIME);
   
}