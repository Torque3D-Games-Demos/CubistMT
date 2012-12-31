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
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------

$NO_TOUCH_EFFECT = 0;

function PlayGui::onWake(%this)
{
   // Turn off any shell sounds...
   // sfxStop( ... );

   $enableDirectInput = "1";
   activateDirectInput();

   // Message hud dialog
 //  if ( isObject( MainChatHud ) )
 //  {
 //     Canvas.pushDialog( MainChatHud );
 //     chatHud.attach(HudMessageVector);
 //  }      
   
   // just update the action map here
   moveMap.push();

   // hack city - these controls are floating around and need to be clamped
   if ( isFunction( "refreshCenterTextCtrl" ) )
      schedule(0, 0, "refreshCenterTextCtrl");
   if ( isFunction( "refreshBottomTextCtrl" ) )
      schedule(0, 0, "refreshBottomTextCtrl");
      
// demo for sgi      
    //     metrics(fps);

}

function PlayGui::onSleep(%this)
{
//   if ( isObject( MainChatHud ) )
//      Canvas.popDialog( MainChatHud );
   
   // pop the keymaps
   moveMap.pop();
}

function PlayGui::clearHud( %this )
{
   Canvas.popDialog( MainChatHud );

   while ( %this.getCount() > 0 )
      %this.getObject( 0 ).delete();
}

//-----------------------------------------------------------------------------

function refreshBottomTextCtrl()
{
   BottomPrintText.position = "0 0";
}

function refreshCenterTextCtrl()
{
   CenterPrintText.position = "0 0";
}

//-----------------------------------------------------------------------------


$LockMouseEvents = false;

function TouchGrabber::onTouchDown( %this, %id, %point )
{
   echo("TouchGrabber::onTouchDown(", %id, ", ", %point, ");" );
   if(%this.currentTouchId == -1)
   {
      $LockMouseEvents = true;
      %this.currentTouchId = %id;
      %this.lastPoint = %point;
      $mvYaw = 0;
      $mvPitch = 0;
   }
   else if(TouchGrabber.currentTouchId != %id)
   {
      %pointx = getWord(%point,0);     
      %pointy = getWord(%point,1);     
      %lastPointx = getWord(%this.lastPoint,0);     
      %lastPointy = getWord(%this.lastPoint,1);     
      if(mAbs(%pointx - %lastPointx) + mAbs(%pointy-%lastPointy) > 256)
         $mvTriggerCount0++;
   }
}
function TouchGrabber::onTouchMove( %this, %id, %point )
{
   echo("TouchGrabber::onTouchMove(", %id, ", ", %point, ");" );
   if(%this.currentTouchId == %id)
   {
      %pointx = getWord(%point,0);     
      %pointy = getWord(%point,1);     
      %lastPointx = getWord(%this.lastPoint,0);     
      %lastPointy = getWord(%this.lastPoint,1);     
      
      $mvYaw = -getMouseAdjustAmount((%lastPointx-%pointx)) * 2;//*(-0.1*));
      $mvPitch = -getMouseAdjustAmount((%lastPointy-%pointy)) * 2;//*(-0.1));
      %this.lastPoint = %point;
   }
   else if(%this.currentTouchId == -1)
   {
      $LockMouseEvents = true;
      %this.currentTouchId = %id;
      %this.lastPoint = %point;
   }
   
}
function TouchGrabber::onTouchUp( %this, %id, %point )
{
   echo("TouchGrabber::onTouchUp(", %id, ", ", %point, ");" );
   if(%this.currentTouchId == %id)
   {
      %this.currentTouchId = -1;
      $mvYaw = 0;
      $mvPitch = 0;
      $LockMouseEvents = false;
   }
}

function MoveTouchGrabber::onTouchDown( %this, %id, %point )
{
   if(MoveTouchGrabber.currentTouchId == -1)
   {
      $LockMouseEvents = true;
      MoveTouchGrabber.currentTouchId = %id;
      MoveTouchGrabber::onTouchMove(%id, %point);
   }
}
function MoveTouchGrabber::onTouchMove( %this, %id, %point )
{
   if(MoveTouchGrabber.currentTouchId == %id)
   {
      %height = getWord(MoveTouchGrabber.getExtent(),1);
      %touchH = getWord(%point,1);
      %delta = (((%touchH/%height) - 0.5)*2.0);
      if(%delta > 0.0)
         movebackward(%delta);
      else moveforward(-%delta);
   }
   else if(MoveTouchGrabber.currentTouchId == -1)
   {
      $LockMouseEvents = true;
      MoveTouchGrabber.currentTouchId = %id;
   }
}
function MoveTouchGrabber::onTouchUp( %this, %id, %point )
{
   if(MoveTouchGrabber.currentTouchId == %id)
   {
      MoveTouchGrabber.currentTouchId = -1;
      movebackward(0);
      moveforward(0);
      $LockMouseEvents = false;
   }
}

function TouchGui::createTouchCursor( %this, %id, %point )
{
   if(!isObject(%this.touchCursor[%id]))  
   {
      %cursor = new GuiBitmapCtrl() {
         bitmap = "art/cube/interface/touch_cursor1_diffuse.png";
         translucent = true;
         translucentBlendOp = "Add";
         canHit = false;
      };
      %this.add(%cursor);
      %this.touchCursor[%id] = %cursor;
      %cursor.position = VectorSub(%point, VectorScale(%cursor.getExtent(),0.5));
   } 
}
function TouchGui::moveTouchCursor( %this, %id, %point )
{
   %cursor = %this.touchCursor[%id];
   if(isObject(%cursor))
      %cursor.position = VectorSub(%point, VectorScale(%cursor.getExtent(),0.5));
}
function TouchGui::deleteTouchCursor( %this, %id )
{
   %cursor = %this.touchCursor[%id];
   if(isObject(%cursor))
   {
      %this.remove(%cursor);
      %this.touchCursor[%id].delete();
      %this.touchCursor[%id] = "";
   }
}


new ParticleData(ReefCursorParticleData)
{
   dragCoefficient      = "0.01";
   gravityCoefficient   = "-0.001";
   inheritedVelFactor   = 0.01;
   constantAcceleration = 0.0;
   lifetimeMS           = "300";
   lifetimeVarianceMS   = "100";
   useInvAlpha          = false;
   textureName          = "art/cube/interface/particle_1.png";
   colors[0]     = "0.653543 0.795276 1 0.393701";
   colors[1]     = "0.653543 0.795276 1 0.393701";
   colors[2]     = "0.669291 0.795276 1 0";
   sizes[0]      = "0.0183117";
   sizes[1]      = "0.299091";
   sizes[2]      = "0.299091";
   times[0]      = 0.0;
   times[1]      = "0.5";
   times[2]      = "1.0";
   animTexName = "art/cube/interface/particle_1.png";
   ejectionPeriodMS = "1";
   ejectionVelocity = "0";
   velocityVariance = "0";
   thetaMax = "80";
   overrideAdvance = "0";
   particles = "ReefCursorParticleData";
   blendStyle = "NORMAL";
   sizes[1] = "0.0183117";
   sizes[2] = "0";
   sizes[3] = "0";
   times[1] = "0.5";
   times[2] = "1";
   spinSpeed = "0";
   spinRandomMin = "-292";
   spinRandomMax = "-42";
};
ReefCursorParticleData.reloadOnLocalClient();

new ParticleData(ReefCursorParticle2Data : ReefCursorParticleData)
{
   textureName          = "art/cube/interface/particle_2.png";
   particles = "ReefCursorParticle2Data";
};
ReefCursorParticle2Data.reloadOnLocalClient();

new ParticleEmitterData(ReefCursorEmitterData)
{
   ejectionPeriodMS = "20";
   periodVarianceMS = "5";
   ejectionVelocity = "0";
   ejectionOffset   = "0";
   velocityVariance = "0";
   thetaMin         = 0;
   thetaMax         = "26";
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = true;
   particles = "ReefCursorParticleData";
   blendStyle = "NORMAL";
   
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
   orientParticles = "0";
   highResOnly = true;
      sortParticles = true;
   softnessDistance = 0.02;

};

ReefCursorEmitterData.reloadOnLocalClient();

new ParticleEmitterData(ReefCursorEmitter2Data : ReefCursorEmitterData)
{
   particles = "ReefCursorParticle2Data";
   ejectionPeriodMS = "200";

};
ReefCursorEmitter2Data.reloadOnLocalClient();

new ArrayObject(TouchCursorEffect){};
function TouchGui::createTouchCursorEffect( %this, %id, %worldPos )
{
   if($NO_TOUCH_EFFECT){
      return;
   }
   
   %cam = ServerConnection.getCameraObject();
   %planeOffset = VectorScale(%cam.getForwardVector(), $pref::Cube::mosaicInteractionPlane);
   
   %upVector = "0 1 0"; // %this.getUpVector();
   %vec = VectorNormalize(VectorSub(%worldPos, %cam.getPosition()));
   %t = -(VectorDot(VectorSub("0 0 0",%planeOffset), %upVector) / VectorDot(%vec, %upVector));
   %position = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   
   %emitter = new ParticleEmitter()
   {
      datablock = "ReefCursorEmitterData";
      velocity = 1;
      position = %position;
      parentGroup = "";
   };
   %emitter.onAdd();
   %emitter.lastPosition = %position;
   %emitter2 = new ParticleEmitter()
   {
      datablock = "ReefCursorEmitter2Data";
      velocity = 1;
      position = %position;
      parentGroup = "";
   };
   %emitter2.onAdd();
   %emitter.emitter2 = %emitter2;

   TouchCursorEffect.add(%id, %emitter);
}
function TouchGui::moveTouchCursorEffect( %this, %id, %worldPos )
{
   %time = getSimTime();
   %obj = TouchCursorEffect.getValue(TouchCursorEffect.getIndexFromKey(%id));
   if(!isObject(%obj))
      return;
      
   %dt = %time-%obj.lastTime;
   if(%dt < 1)
      return;
   %obj.lastTime = %time;
   
   %cam = ServerConnection.getCameraObject();
   %planeOffset = VectorScale(%cam.getForwardVector(), $pref::Cube::mosaicInteractionPlane);
   
   %upVector = "0 1 0"; // %this.getUpVector();
   %vec = VectorNormalize(VectorSub(%worldPos, %cam.getPosition()));
   %t = -(VectorDot(VectorSub("0 0 0",%planeOffset), %upVector) / VectorDot(%vec, %upVector));
   %position = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   

   %vel = VectorSub(%obj.lastPosition, %position);   
   
   %obj.emitParticles(%position, true, "0 0 1", %vel, 1);
   %obj.emitter2.emitParticles(%position, true, "0 0 1", %vel, 1);
   %obj.lastPosition = %position;

}
function TouchGui::deleteTouchCursorEffect( %this, %id )
{
   %index = TouchCursorEffect.getIndexFromKey(%id);
   %obj = TouchCursorEffect.getValue(%index);
   if(isObject(%obj))
   {
      TouchCursorEffect.erase(%index);
      if(isObject(%obj.emitter2))
         %obj.emitter2.deleteWhenEmpty();
      %obj.deleteWhenEmpty();
   }
}

function TouchGui::onMouseDown( %this, %modifier, %mousePoint, %mouseClickCount )
{
   %worldPos = %this.unProject(%mousePoint SPC 1.0);
   %this.createTouchCursorEffect(10000, %worldPos);
   
   commandToServer('ReefTouchDown', 10000, %worldPos);
}
function TouchGui::onMouseMove( %this, %modifier, %mousePoint, %mouseClickCount )
{
}
function TouchGui::OnMouseDragged( %this, %modifier, %mousePoint, %mouseClickCount )
{   
   %worldPos = %this.unProject(%mousePoint SPC 1.0);
   %this.moveTouchCursorEffect(10000, %worldPos);

   commandToServer('ReefTouchMove', 10000, %worldPos);
}

$version = 1;  //0=old version, 1=new version
$zEffect = 10; //how far away from camera to draw bubble effects

function TouchGui::onMouseUp( %this,  %modifier, %mousePoint, %mouseClickCount )
{      
   %worldPos = %this.unProject(%mousePoint SPC 1.0);
   %this.deleteTouchCursorEffect(10000);

   commandToServer('ReefTouchUp', 10000, %worldPos);
}
function TouchGui::onTouchDown( %this, %id, %touchPoint )
{
   %worldPos = %this.unProject(%touchPoint SPC 1.0);
   %this.createTouchCursorEffect(%id, %worldPos);
   
   commandToServer('ReefTouchDown', %id, %worldPos);
   return true;
}

function TouchGui::onTouchMove( %this, %id, %touchPoint )
{ 
   %worldPos = %this.unProject(%touchPoint SPC 1.0);
   %this.moveTouchCursorEffect(%id, %worldPos);
   
   commandToServer('ReefTouchMove', %id, %worldPos);
   return true;
}

function TouchGui::onTouchUp( %this, %id, %touchPoint )
{
   %worldPos = %this.unProject(%touchPoint SPC 1.0);
   %this.deleteTouchCursorEffect(%id);
   
   commandToServer('ReefTouchUp', %id, %worldPos);
   return true;
}
