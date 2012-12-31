//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------

$ENABLE_SWISH = 1;

$WEBVIEW_MOUSEHACK = "0 1.1 0";
$SWISH_LIFE = 5*1000;   //kill turbulence trigger boxes after this time of non movement
                         //this is a partial hack to fix problem of touch up events not getting through


$SWISH_EFFECT_DISTANCE  = 2.0;   //when fish are this far away from swish, it has no effect
$SWISH_BOX_SIZE    =  $SWISH_EFFECT_DISTANCE @ " 12 " @ $SWISH_EFFECT_DISTANCE;
$SCARE_BOX_SIZE    =  "2 2 5";


////////////////////////////////////////////////////////////////////////////////

function pointToPos(%posOne, %posTwo)
{
    // sub the two positions so we get a vector pointing from the origin in the
    // direction we want our object to face
    %vec = VectorSub(%posTwo, %posOne);
	
    // pull the values out of the vector
    %x = firstWord(%vec);
    %y = getWord(%vec, 1);
    %z = getWord(%vec, 2);
	
    // this finds the distance from origin to our point
    %len = vectorLen(%vec);
	
    // -------- X
    // given the rise and length of our vector this will give us the angle in radians
    %rotAngleX = mATan(%z, %len);
	
    // -------- Z
    // get the angle for the z axis
    %rotAngleZ = mATan(%x, %y);
	
    // create 2 matrices, one for the z rotation, the other for the x rotation
    %matrix = MatrixCreateFromEuler("0 0" SPC %rotAngleZ * -1);
    %matrix2 = MatrixCreateFromEuler(%rotAngleX SPC "0 0");
	
    // now multiply them together so we end up with the rotation we want
    %finalMat = MatrixMultiply(%matrix, %matrix2);
	
    // we're done, send the proper numbers back
    return getWords(%finalMat, 3, 6);
}


////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
/*


function MovableImageWidget::OnMouseDown(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}
function MovableImageWidget::OnMouseDragged(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}
function MovableImageWidget::OnMouseUp(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}

function MovableImageWidget::OnRightMouseDown(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.getParent().pushToBack(%this);
   %this.maximize();
   
   %this.mouseLock();
   %this.lastMousePoint = %mousePoint;
   
   // touched again so dont go minimizing on us
   %this.scheduleMinimize();

}
function MovableImageWidget::OnRightMouseDragged(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %offsetx = getWord(%this.lastMousePoint,0) - getWord(%mousePoint,0);
   %offsety = getWord(%this.lastMousePoint,1) - getWord(%mousePoint,1);
   %this.lastMousePoint = %mousePoint;
   
   %this.setPosition(getWord(%this.position,0)-%offsetx, getWord(%this.position,1)-%offsety);
   %this.scheduleMinimize();
}
function MovableImageWidget::OnRightMouseUp(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.mouseUnlock();
   %this.scheduleMinimize();
}
function MovableImageWidget::OnTouchDown(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == -1)
   {
      %this.currentTouchId = %touchId;
      
      %this.getParent().pushToBack(%this);
      %this.maximize();
      
      %this.lastTouchPoint = %touchPoint;
      
      // touched again so dont go minimizing on us
      %this.scheduleMinimize();
   }
   else if(%this.secondTouchId == -1)
   {
      %this.secondTouchId = %id;
      %this.lastSecondTouchPoint = %touchPoint;
   }
}
function MovableImageWidget::OnTouchMove(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == %touchId)
   {
      if(%this.secondTouchId == -1)
      {
         // move wdiget
         %offsetx = getWord(%this.lastTouchPoint,0) - getWord(%touchPoint,0);
         %offsety = getWord(%this.lastTouchPoint,1) - getWord(%touchPoint,1);
         %this.lastTouchPoint = %touchPoint;
      
         %this.setPosition(getWord(%this.position,0)-%offsetx, getWord(%this.position,1)-%offsety);
      }
      else
      {
         // move scale
         %offsetx = getWord(%this.lastTouchPoint,0) - getWord(%touchPoint,0);
         %offsety = getWord(%this.lastTouchPoint,1) - getWord(%touchPoint,1);
         
         %this.lastTouchPoint = %touchPoint;
      
         %this.setPosition(getWord(%this.position,0)-%offsetx, getWord(%this.position,1)-%offsety);
      
         %lenNewx = getWord(%touchPoint,0) - getWord(lastSecondTouchPoint,0);
         %lenNewy = getWord(%touchPoint,1) - getWord(lastSecondTouchPoint,1);
         %lenOldx = getWord(%this.lastTouchPoint,0) - getWord(lastSecondTouchPoint,0);
         %lenOldy = getWord(%this.lastTouchPoint,1) - getWord(lastSecondTouchPoint,1);
         %extentx = getWord(%this.getExtent(),0);
         %extenty = getWord(%this.getExtent(),1);
         
         %difx = %lenNewx - %lenOldx;
         %dify = %lenNewy - %lenOldy;
         %this.setExtent(%extentx - %difx, %extenty - %dify);
      }
      %this.scheduleMinimize();
   }
}
function MovableImageWidget::OnTouchUp(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == %touchId)
   {
      if(%this.secondTouchId == -1)
      {
         %this.currentTouchId = -1;
         %this.scheduleMinimize();
      }
      else
      {
         %this.currentTouchId = %this.secondTouchId;
         %this.lastTouchPoint = %this.lastSecondTouchPoint;
         %this.secondTouchId = -1;
      }
   }
   else if(%this.secondTouchId == %touchId)
      %this.secondTouchId = -1;
}

function MovableImageWidget::scheduleMinimize(%this)
{
   if(%this.timedMinId)
   {
      cancel(%this.timedMinId);         
      %this.timedMinId = 0;
   }
   %this.timedMinId = %this.schedule($DELAY_MINIMIZE, "minimize");
}
function MovableImageWidget::childResized(%this, %child)
{
   %this.setExtent(%child.getExtent());
}

function MovableImageWidget::OnWake(%this)
{
   %this.currentTouchId = -1;
   %this.secondTouchId = -1;
   %this.lastPoint = "0 0";
}


function MovableImageWidget::minimize(%this)
{
   if(%this.minimizePosition)
      %this.position = %this.minimizePosition;
   %this.getObject(0).setExtent(%this.minimizeExtent);
   %this.minimized = true;
}
function MovableImageWidget::maximize(%this)
{
   %this.minimizePosition = %this.position;
   %this.getObject(0).setExtent(%this.getObject(0).getNativeExtent());
   %this.minimized = false;
}


////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////
function MovableWidget::OnWake(%this)
{
   %this.currentTouchId = -1;
   %this.lastPoint = "0 0";
}
function MovableWidget::OnMouseDown(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}
function MovableWidget::OnMouseDragged(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}
function MovableWidget::OnMouseUp(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.scheduleMinimize();
}
function MovableWidget::OnRightMouseDown(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.getParent().pushToBack(%this);
   %this.maximize();
   
   %this.mouseLock();
   %this.lastMousePoint = %mousePoint;
   
   // touched again so dont go minimizing on us
   %this.scheduleMinimize();

}
function MovableWidget::OnRightMouseDragged(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %offsetx = getWord(%this.lastMousePoint,0) - getWord(%mousePoint,0);
   %offsety = getWord(%this.lastMousePoint,1) - getWord(%mousePoint,1);
   %this.lastMousePoint = %mousePoint;
   
   %this.setPosition(getWord(%this.position,0)-%offsetx, getWord(%this.position,1)-%offsety);
   %this.scheduleMinimize();
}
function MovableWidget::OnRightMouseUp(%this, %modifier, %mousePoint, %mouseClickCount)
{
   %this.mouseUnlock();
   %this.scheduleMinimize();
}
function MovableWidget::OnTouchDown(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == -1)
   {
      %this.currentTouchId = %touchId;
      
      %this.getParent().pushToBack(%this);
      %this.maximize();
      
      %this.lastTouchPoint = %touchPoint;
      
      // touched again so dont go minimizing on us
      %this.scheduleMinimize();
      return true;
      
   }
   return false;
}
function MovableWidget::OnTouchMove(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == %touchId)
   {
      %offsetx = getWord(%this.lastTouchPoint,0) - getWord(%touchPoint,0);
      %offsety = getWord(%this.lastTouchPoint,1) - getWord(%touchPoint,1);
      %this.lastTouchPoint = %touchPoint;
   
      %this.setPosition(getWord(%this.position,0)-%offsetx, getWord(%this.position,1)-%offsety);
      %this.scheduleMinimize();
      return true;
      
   }
   return false;
}
function MovableWidget::OnTouchUp(%this, %touchId, %touchPoint)
{
   if(%this.currentTouchId == %touchId)
   {
      %this.currentTouchId = -1;
      %this.scheduleMinimize();
      return true;
      
   }
   return false;
}

function MovableWidget::scheduleMinimize(%this)
{
   if(%this.timedMinId)
   {
      cancel(%this.timedMinId);         
      %this.timedMinId = 0;
   }
   %this.timedMinId = %this.schedule($DELAY_MINIMIZE, "minimize");
}

function MovableWidget::minimize(%this)
{
   if(%this.minimizePosition)
      %this.position = %this.minimizePosition;
   %this.setExtent(%this.minimizeExtent);
   %this.minimized = true;
}
function MovableWidget::maximize(%this)
{
   %this.minimizePosition = %this.position;
   %this.setExtent(%this.getNativeExtent());
   %this.minimized = false;
}

function serverCmdSetVelocity(%client, %shape, %velocity)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %serverObject.setVelocity(%velocity);
}
function serverCmdPositionObjectDown(%client, %shape, %hitPos)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %serverObject.maximize();
   
   // simple hit point on objects plane
   %serverObject.lastMousePoint = %hitPos;
   %serverObject.scheduleMinimize();
}
function serverCmdPositionObjectDragged(%client, %shape, %hitPos)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %offset = VectorSub(%hitPos, %serverObject.lastMousePoint);
   
   %position = VectorAdd(%serverObject.position, setWord(%offset,1,0));
   %serverObject.position = %position;
      
   %serverObject.lastMousePoint = %hitPos;
   %serverObject.scheduleMinimize();
}
*/
function serverCmdScaleObjectNative(%client, %shape, %nativeRes)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   if(!%this.hasWebViewData()) 
   %this.scaleObjectNative(%nativeRes);
}
function serverCmdscaleWebObjectNative(%client, %shape, %nativeRes)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);  
   if(isObject(%this.object))
   %this.object.scaleObjectNative(%nativeRes);
}

function Movable3DWidget::scaleObjectNative(%this, %nativeRes)
{
   if(%this.isNativeScale)
      return;

   %width = getWord(%nativeRes,0);
   %height = getWord(%nativeRes,1);
   
   if(%width > 0 && %height > 0)
   {
      if(%width > %height)
      {
         %value = (%height/%width);
         if(%value)
            %this.nativeScale = "1 1" SPC %value; 
      }
      else
      {
         %value = (%width/%height);
         if(%value)
            %this.nativeScale = %value SPC "1 1"; 
      }
      %this.setScale(VectorMul(%this.scale, %this.nativeScale)); 
      %this.minimizeScale = %this.scale;
   }
   
   %this.isNativeScale = true;
}
/*
function serverCmdScaleObjectDown(%client, %shape, %hitPos)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %serverObject.lastMousePoint = %hitPos;
   %serverObject.scheduleMinimize();
}
function serverCmdScaleObjectDragged(%client, %shape, %hitPos)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   if(%this.lockAspect)
   {
      
   }  
   %box = %serverObject.getWorldBox();
   %min = getWords(%box, 0, 2);
   %max = getWords(%box, 3, 5);
   %size = VectorSub(%max, %min);
   // get the offset and apply it to the widget scale
   %offset = VectorSub(%hitPos, %serverObject.lastMousePoint);
   %scale = VectorAdd(%serverObject.scale, ((getWord(%size,0) * getWord(%offset,0) * 2) SPC (getWord(%size,1) * getWord(%offset,1) * 2) SPC (getWord(%size,2) * getWord(%offset,2) * 2)));
   %serverObject.setScale(%scale);

   %serverObject.lastMousePoint = %hitPos;
   %serverObject.scheduleMinimize();
}
function serverCmdSwizzleObjectCenterDown(%client, %shape, %touchId)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.swizzleObjectDown(%touchId, %shape.position);
}
function serverCmdSwizzleObjectDown(%client, %shape, %touchId, %hitPos)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.swizzleObjectDown(%touchId, %hitPos);
}

function serverCmdSwizzleObjectDragged(%client, %shape, %touchId, %hitPos)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.swizzleObjectDragged(%touchId, %hitPos); 
}
function serverCmdSwizzleObjectUp(%client, %shape, %touchId)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.swizzleObjectUp(%touchId);
}
function serverCmdScheduleMinimize(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.scheduleMinimize();
}
*/

function serverCmdMaximize(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.maximize(%client.camera.getPosition());
}

function serverCmdInstantMinimize(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.instantMinimize();
}
function serverCmdMinimize(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);   
   %this.minimize();
}


function serverCmdPlayThread(%client, %shape, %thread)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %serverObject.playThread(0, %thread);
}
function serverCmdSetThreadPosition(%client, %shape, %pos)
{
   %serverObject = %client.resolveObjectFromGhostIndex(%shape);   
   %serverObject.setThreadPosition(0, %pos);
}

// get intersection between ray from camera to position on plane a fixed distance from the camer
function clampToCameraPlane(%cam, %pos, %planeDistance)
{
   if(!isObject(%cam))
   {
      echo("clampToCameraPlane: no camera");
      return;
   }

   %transform = %cam.getTransform();
   %eyePosition = %cam.getPosition();
   %eyeDirection = %cam.getForwardVector();
   
   %rel = VectorSub(%pos, %eyePosition);
   %ff = VectorDot(%rel, %eyeDirection);
   if( %ff!=0 )
   {
      %t = %planeDistance / %ff;
      return VectorAdd(%eyePosition, VectorScale(%rel, %t));
   }
   else
   {
      //o-oh position is in plane going through camera position
      return %pos;  
   }
}

function serverCmdSetShapeTransform(%client, %shape, %newPosition, %rotation, %scale)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 

 // used for rigidshape
   if(%this.isFrozen())
   {
      %this.freezeSim(false);
      %this.reset();
   }

   if(%this.allowRotation)
   {
      %cam = %client.camera.getTransform();
      %dir = getWord(%client.camera.getForwardVector(),1);
      if(%dir > 0.0)
         %rot = MatrixMultiply(%cam, "0 0 0 0 1 0" SPC %rotation);
      else
         %rot = MatrixMultiply(%cam, "0 0 0 0 1 0" SPC -%rotation);
         
      %newRot = getWords(%rot, 3, -1);
      
      %this.rotate = %rotation;
   }

   if(!%this.allowTranslation)
   {
      %p = %this.position;
   }
   else
   {
      %p = VectorAdd(%this.position, %newPosition);
      %this.distanceToCamera = 40;
      %p = clampToCameraPlane(%client.camera, %p, %this.distanceToCamera);
   }
   
   if(%this.allowScale)
   %this.setScale(VectorScale(%this.scale, %scale));

 //  if(%this.allowRotation)
      %this.setTransform(%p SPC %newRot);
//   else
//      %this.setTransform(%p);
}

//this only works for web pages
function serverCmdSetShapePosition(%client, %shape, %newPosition)
{

   %this = %client.resolveObjectFromGhostIndex(%shape); 
   if(!%this.allowTranslation)
      return;
      
   // if rigidshape
   if(%this.isFrozen())
   {
      %this.freezeSim(false);
      %this.reset();
   }
      
   // point at camera
//   %cam = %client.camera.getTransform();
//   %newRot = getWords(%cam, 3, -1);

   // align to camera
   %rotation = %this.rotate;
      
   %cam = %client.camera.getTransform();
   %dir = getWord(%client.camera.getForwardVector(),1);
   if(%dir > 0.0)
      %rot = MatrixMultiply(%cam, "0 0 0 0 1 0" SPC %rotation);
   else
      %rot = MatrixMultiply(%cam, "0 0 0 0 1 0" SPC -%rotation);
      
   %newRot = getWords(%rot, 3, -1);
   
   %p = VectorAdd(%this.position, %newPosition);


//   %this.pushedBack = 0;
//   %this.distanceToCamera = $WEBPAGE_TO_CAMERA_DISTANCE;
// hack for image view
   %this.distanceToCamera = 40;
   %p = clampToCameraPlane(%client.camera, %p, %this.distanceToCamera);
   %this.setTransform(%p SPC %newRot);
//   %this.position = %p;
}
function serverCmdPositionImpulse(%client, %shape, %newPosition)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 
   %dt=1;

   %velocity = %newPosition;
   
   // time adjust mass factor for moving even the heaviest objects
   %massFactor = (%this.getDataBlock().mass*(%dt));
      
   // apply mass and time factor to the velocity so the object move the
   // right amount for its mass.
   %velocity = VectorScale(%velocity, %massFactor);
   
   // clamp the velocity so we don't have physics explosions
   if(VectorLen(%velocity) > %massFactor*500)
      %velocity = VectorScale(VectorNormalize(%velocity), %massFactor*500);
      
   // lock the y axis and apply downward pressure
   %velocity = setWord(%velocity,1,0);  
   
   // send the new impulse to the physics system
   %this.applyImpulse(VectorAdd(%this.getDatablock().massCenter,%this.position), %velocity);
}

function serverCmdTransformImpulse(%client, %newPosition, %rotation, %scale, %dt)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 
   
   // get the velocity vector to move to the new location
   %velocity = VectorSub(%newPosition, %this.position);
   
   // time adjust mass factor for moving even the heaviest objects
   %massFactor = ((%this.dataBlock.mass*100)/%dt);
      
   // apply mass and time factor to the velocity so the object move the
   // right amount for its mass.
   %velocity = VectorScale(%velocity, %massFactor);
   
   // clamp the velocity so we don't have physics explosions
   if(VectorLen(%velocity) > %massFactor*500.0)
      %velocity = VectorScale(VectorNormalize(%velocity), %massFactor*500.0);
      
   // lock the y axis and apply downward pressure
   %velocity = setWord(%velocity,1,0);  
   
   // remove existing momentum from the new velocity
   %velocity = VectorSub(%velocity, %this.getLinVelocity());
   
   // send the new impulse to the physics system
   %this.setScale(VectorScale(%this.scale, %scale));
   %this.rotation = "0 1 0" SPC mRadToDeg(%rotation);   
   %this.applyImpulse(%this.getCMassPosition(), %velocity);
}

function Movable3DWidget::minimize(%this)
{
   if(%this.closeOnMinimize)
   {
       %this.delete();
       return;
   }
   
   if(%this.minimized)
      return;
      
   %this.position = %this.minimizePosition;
   %this.playThread(0, "minimize");
   %this.setObjectBox("-0.125 -0.05 -0.125 0.125 0.05 0.125");
   
   if(isObject(%this.minimizeMount))
      %this.minimizeMount.mountObject(%this, %this.minimizeMountIndex, %this.minimizeMountTransform);
   
   %this.minimized = true;
   
}
function Movable3DWidget::maximize(%this, %camPos)
{
   if(!%this.minimized)
      return;
   %this.minimized = false;
   %this.minimizePosition = %this.position;
   
   if(isObject(%this.minimizeMount))
   {
      %parent = %this.minimizeMount;
      %trans = %this.getMountedTransform();
      %parent.unmountObject(%this);
      
      %this.setTransform(%trans);
   }

   %position = VectorAdd(%this.position, VectorScale(VectorNormalize(VectorSub(%this.position,%camPos)),(1000.0/getRandom(900,1000))*(-(%this.maximizeForwardOffset))));
   %this.position = %position;
   %this.maximizeDepth = getWord(%position,1);
   %this.playThread(0, "maximize");
   %this.setObjectBox("-0.5 -0.05 -0.5 0.5 0.05 0.5");
}

function Movable3DWidget::instantMinimize(%this)
{
   %this.minimize();
   %this.setThreadPosition(0, 1);
}


function Movable3DWidget::OnAdd(%this)
{
}
function Movable3DWidgetData::OnCollision(%data, %this, %collObj, %vec, %len)
{
   if(%this.class $= Movable3DWidget && %collObj.class $= Movable3DWidget)
   {
      %camera = ClientGroup.getObject(0).camera;
      if(!isObject(%camera))
         return;


      if(%this.distanceToCamera >= %collObj.distanceToCamera)
      {
         %master = %this;
         %slave = %collObj;
      }
      else
      {
         %master = %collObj;
         %slave = %this;
      }
      %slave.distanceToCamera = %master.distanceToCamera + $WEBPAGE_THICKNESS;
      
      %slave.position = clampToCameraPlane(%camera, %slave.getPosition(), %slave.distanceToCamera);
      
      %slave.freezeSim(true);
      %master.freezeSim(true);
      %slave.reset();
      %master.reset();
   }
}



$worldscale = 4096.0;
function SimpleImageDirectoryLoader::onAdd(%this)
{
   if(%this.imagePath $= "")
      return;
   
   loadDir(%this.imagePath);
   
   %canvasWidth = 4320;
   %canvasHeight = 1920;
   
   %screenToScene = 0.01;    
        
   %margin = 256;
   %thumbWidth = 256;
   %thumbHeight = 256;
   %gapSize = 16;
   
   %canvasWidth -= (%margin*2);
   %canvasHeight -= (%margin*2);
   
   %blockWidth = %thumbWidth + %gapSize;
   %blockHeight = %thumbHeight + %gapSize;
   %rowLimit = mFloor((%canvasWidth + %gapSize) / %blockWidth);
   %centerWidth = -(((%canvasWidth - ((%blockWidth * %rowLimit)-%gapSize)) * 0.5)+%margin);
   %centerHeight = ((%canvasHeight - ((%blockHeight * %rowLimit)-%gapSize)) * 0.5)+%margin;
   
   %posx = %gapSize;
   %posy = %gapSize;
   %count = 0;

   for( %fileName = findFirstFile( %this.imagePath @ "/*.*" ); %fileName !$= ""; %fileName = findNextFile() )
   {
      if( isFile(%fileName) )
      {
         // create a widget to display the file
         if(%count != 0)
         {
            %col = mFMod(%count,%rowLimit);
            %row = mFloor(%count/%rowLimit);
            
            %posx = %col * %blockWidth;
            %posy = %row * -%blockHeight;   
         }
         else
         {
            %posx = 0;
            %posy = 0;   
         }
         %count++;
         
         
         %posx += %centerWidth;
         %posy += %centerHeight;
         
         %posx -= (%canvasWidth * 0.5) - (%margin*2); 
         %posy += (%canvasHeight) + %margin; 
         
         %posx *= %screenToScene; 
         %posy *= %screenToScene; 
         
         %sThumbWidth = (2800 * %screenToScene) / (%ThumbWidth * %screenToScene);
         %sThumbHeight = (2800 * %screenToScene) / (%ThumbHeight * %screenToScene);
      //   %sThumbWidth = (%ThumbWidth * %screenToScene);
      //   %sThumbHeight = (%ThumbHeight * %screenToScene);

         switch$(fileExt(%fileName))
         {
         case ".jpg": %this.createImageWidget3D(%fileName, %posx, %posy, %sThumbWidth, %sThumbHeight, true);
         case ".png": %this.createImageWidget3D(%fileName, %posx, %posy, %sThumbWidth, %sThumbHeight, true);
         case ".url": %this.createWebWidget3DFromShortcut(%fileName, %posx, %posy, %sThumbWidth, %sThumbHeight, true);
         default: %this.createWebWidget3DFromFile(%fileName, %posx, %posy, %sThumbWidth, %sThumbHeight, true);
         }
      }
   }
}

function SimpleImageDirectoryLoader::createImageWidget(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
    // todo: need to do this on all the client via client command
   %baseCtrl = new GuiInputCtrl() {
      class = MovableImageWidget;
      canSaveDynamicFields = "0";
      Enabled = "1";
      isContainer = "1";
      Profile = "GuiInputCtrlProfile";
      HorizSizing = "top";
      VertSizing = "bottom";
      Extent = "0 0";
      MinExtent = "0 0";
      canSave = "0";
      isDecoy = "0";
      Visible = "1";
      tooltipprofile = "GuiToolTipProfile";
      hovertime = "1000";
   };
 
   %bitmapCtrl = new GuiBitmapCtrl() {
      canSaveDynamicFields = "0";
      Enabled = "1";
      isContainer = "0";
      Profile = "GuiBitmapCtrlProfile";
      HorizSizing = "top";
      VertSizing = "bottom";
      Extent = "0 0";
      MinExtent = "0 0";
      canSave = "0";
      isDecoy = "0";
      Visible = "1";
      tooltipprofile = "GuiToolTipProfile";
      hovertime = "1000";
      bitmap = "";
      lockAspect = true;
   };
   
   %baseCtrl.setPosition(%posx, %posy);   
   %baseCtrl.setExtent(%thumbWidth SPC %thumbHeight);   
   %baseCtrl.minimizeExtent = %baseCtrl.getExtent();
   %baseCtrl.minimized = "1";

   %bitmapCtrl.setBitmap(%fileName, true); 
   %bitmapCtrl.setExtent(%baseCtrl.getExtent()); 
   %baseCtrl.addGuiControl(%bitmapCtrl);
   
   WidgetScrollCtrl.addGuiControl(%baseCtrl);
}
function SimpleImageDirectoryLoader::createWebWidgetFromShortcut(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %file = new FileObject();
   %file.openForRead(%fileName);
   %line1 = %file.readLine();
   %text = %file.readLine();
   if(strIsMatchExpr("URL=*",%text,false))
   {
      %url = getSubStr(%text,4);
      %this.createWebWidget(%url, %posx, %posy, %thumbWidth, %thumbHeight);
   }
}
function SimpleImageDirectoryLoader::createWebWidgetFromFile(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %this.createWebWidget(getWorkingDirectory() @ "/" @ %fileName, %posx, %posy, %thumbWidth, %thumbHeight);
}
function SimpleImageDirectoryLoader::createWebWidget(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
 // todo: need to do this on all the clients via client command
   %webView = new GuiWebViewCtrl() {
      class = MovableWidget;
      canSaveDynamicFields = "0";
      Enabled = "1";
      isContainer = "0";
      Profile = "GuiInputCtrlProfile";
      HorizSizing = "top";
      VertSizing = "bottom";
      Extent = "1024 768";
      MinExtent = "0 0";
      canSave = "0";
      isDecoy = "0";
      Visible = "1";
      tooltipprofile = "GuiToolTipProfile";
      hovertime = "1000";
      
      url = "";
      lockAspect = true;
   };
   
   %webView.setPosition(%posx, %posy);   
   %webView.setURL(%fileName);  
   %webView.setExtent(%thumbWidth SPC %thumbHeight);   
   %webView.minimizeExtent = %webView.getExtent();
   %webView.minimizePosition = %webView.position;
   %webView.minimized = "1";
       
   WidgetScrollCtrl.addGuiControl(%webView);
}
function SimpleImageDirectoryLoader::createWebWidget3DFromShortcut(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %file = new FileObject();
   %file.openForRead(%fileName);
   %line1 = %file.readLine();
   %text = %file.readLine();
   if(strIsMatchExpr("URL=*",%text,false))
   {
      %url = getSubStr(%text,4);
      %this.createWebWidget3D(%url, %posx, %posy, %thumbWidth, %thumbHeight);
   }
}
function SimpleImageDirectoryLoader::createWebWidget3DFromFile(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %this.createWebWidget3D(getWorkingDirectory() @ "/" @ %fileName, %posx, %posy, %thumbWidth, %thumbHeight);
}
function SimpleImageDirectoryLoader::createImageWidget3D(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   
    
   %client = ClientGroup.getObject(0);
  // %webView = new PhysicsShape() {
  // %webView = new RigidShape() {
   %webView = new StaticShape() {
      class = "Movable3DWidget";
      dataBlock = Movable3DWidgetData;
  //    position = %posx SPC "0" SPC %posy;
  //    rotation = %client.camera.rotation;
      scale = %thumbWidth SPC 1 SPC %thumbHeight;
      canSave = "0";
      canSaveDynamicFields = "0";
      enableInputEvents = true;
 //     skin = "web=" @ %fileName;
 //     parentGroup = %webView;
         lockAspect = true;
         resizeWebViewData = false;
         allowRotation = true;
         allowScale = true;
         allowTranslation = true;
   };
   %this.add(%webView);
   
   %webView.setSkinName("web=" @ %fileName);
   %webView.OnAdd();
   %webView.instantMinimize();
   %webView.position = %posx SPC "0" SPC %posy;
   %webView.rotation = %client.camera.rotation;
}
function SimpleImageDirectoryLoader::createWebWidget3D(%this, %fileName, %posx, %posy, %thumbWidth, %thumbHeight, %maintainAspect)
{
   %data = new WebViewData() {
      category = "WebPage";
      canSave = "0";
      canSaveDynamicFields = "0";
      url = %fileName;  
      
      resolution = "1024 768"; 
   };
   %this.add(%data);

  // %webView = new PhysicsShape() {
   %client = ClientGroup.getObject(0);
   %webView = new RigidShape() {
      class = "Movable3DWidget";
      webViewData = %data;
      dataBlock = "Movable3DWidgetData";
  //    position = %posx SPC "0" SPC %posy;
  //    rotation = %client.camera.rotation;
      scale = "1 1 1";
      scale = %thumbWidth SPC 1 SPC %thumbHeight;
      canSave = "0";
      canSaveDynamicFields = "1";
      enableInputEvents = true;
         lockAspect = true;
         resizeWebViewData = false;
         allowRotation = false;
         allowScale = true;
         allowTranslation = true;
   };
   %this.add(%webView);
   %webView.OnAdd();
   %webView.instantMinimize();
   %webView.position = %posx SPC "0" SPC %posy;
   %webView.rotation = %client.camera.rotation;
}

/////////////////////////////////////////////////////////////////////
//

datablock ParticleEmitterNodeData( TestEmitterNodeData )
{
   timeMultiple = 0.5;
};

datablock ParticleEmitterNodeData(ReefBubbleEmitterNodeData)
{
   timeMultiple = 1;
};

datablock ParticleData(ReefBubbleParticleData)
{
   dragCoefficient      = "1";
   gravityCoefficient   = "-0.05";
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = "8000";
   lifetimeVarianceMS   = "0";
   useInvAlpha          = false;
   textureName          = "art/cube/interface/touch_cursor1_diffuse.png";
   colors[0]     = "0.661417 0.795276 1 0.393701";
   colors[1]     = "0.661417 0.795276 1 0.393701";
   colors[2]     = "0.669291 0.795276 1 0";
   sizes[0]      = "0.02";
   sizes[1] = "0.02";
   sizes[2] = "0";
   times[0]      = 0.0;
   times[1] = "0.5";
   times[2] = "1";
   animTexName = "art/cube/interface/touch_cursor1_diffuse.png";
   ejectionPeriodMS = "1";
   ejectionVelocity = "1";
   velocityVariance = "0.5";
   thetaMax = "80";
   overrideAdvance = "0";
   particles = "ReefBubbleParticleData";
   blendStyle = "ADDITIVE";
   softParticles = "1";
   softnessDistance = "0.05";
   spinSpeed = "0";
   spinRandomMin = "-292";
   spinRandomMax = "-41.1";
   renderReflection = 0;
};
datablock ParticleEmitterData(ReefBubbleEmitterData)
{
   ejectionPeriodMS = "10";
   periodVarianceMS = 0;
   ejectionVelocity = "0.2";
   ejectionOffset   = "0";
   velocityVariance = "0.01";
   thetaMin         = 0;
   thetaMax         = "26.25";
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "ReefBubbleParticleData";
   blendStyle = "ADDITIVE";

   highResOnly = true;
   sortParticles = true;
   softnessDistance = 0.02;

   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
};

datablock ParticleEmitterNodeData(ReefAmbientBubbleEmitterNodeData)
{
   timeMultiple = 1;
};
datablock ParticleData(ReefAmbientBubbleParticleData)
{
   dragCoefficient      = "0.997067";
   gravityCoefficient   = "-0.01221";
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = "100000";
   lifetimeVarianceMS   = "0";
   useInvAlpha          = false;
   textureName          = "art/cube/interface/touch_cursor1_diffuse.png";
   colors[0]     = "0.653543 0.795276 1 0.393701";
   colors[1]     = "0.653543 0.795276 1 0.393701";
   colors[2]     = "0.669291 0.795276 1 0";
   sizes[0]      = "0.01";
   sizes[1] = "0.01";
   sizes[2] = "0";
   times[0]      = 0.0;
   times[1] = "0.5";
   times[2] = "1";
   animTexName = "art/cube/interface/touch_cursor1_diffuse.png";
   ejectionPeriodMS = "1";
   ejectionVelocity = "1";
   velocityVariance = "0.5";
   thetaMax = "80";
   overrideAdvance = "0";
   particles = "ReefAmbientBubbleParticleData";
   blendStyle = "ADDITIVE";
   softParticles = "1";
   softnessDistance = "0.05";
   spinSpeed = "0";
   spinRandomMin = "-292";
   spinRandomMax = "-42";
   

   renderReflection = 0;

};
datablock ParticleEmitterData(ReefAmbientBubbleEmitterData)
{
   ejectionPeriodMS = "20";
   periodVarianceMS = "0";
   ejectionVelocity = "0.2";
   ejectionOffset   = "20";
   velocityVariance = "0";
   thetaMin         = 0;
   thetaMax         = "26";
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "ReefAmbientBubbleParticleData";
   blendStyle = "ADDITIVE";
   
   highResOnly = true;
   sortParticles = true;
   softnessDistance = 0.02;

   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
};


//***shark bubbles******************************************
datablock ParticleData(SharkParticleData)
{
   dragCoefficient      = "0";
   gravityCoefficient   = "-0.05";
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = "12000";
   lifetimeVarianceMS   = "0";
   useInvAlpha          = false;
   textureName          = "art/cube/interface/touch_cursor1_diffuse.png";

   colors[0]     = "0.661417 0.795276 1 0.4";
   colors[1]     = "0.661417 0.795276 1 0.2";
   colors[2]     = "0.669291 0.795276 1 0";

   sizes[0]      = "0.2";
   sizes[1]      = "0.4";
   sizes[2]      = "0.6";

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;

   animTexName = "art/cube/interface/touch_cursor1_diffuse.png";
   
   velocityVariance = "0.5";
   thetaMax = "80";
   overrideAdvance = "0";
   particles = "SharkParticleData";
   blendStyle = "ADDITIVE";
   softParticles = "1";
   softnessDistance = "0.05";
   spinSpeed = "0";
   spinRandomMin = "-292";
   spinRandomMax = "-41.1";
   renderReflection = 0;

};
datablock ParticleEmitterNodeData( SharkEmitterNodeData )
{
   timeMultiple = 2;
};
datablock ParticleEmitterData(SharkEmitterData)
{
   ejectionPeriodMS = "20";
   periodVarianceMS = 0;
   ejectionVelocity = "0.2";
   ejectionOffset   = "0";
   velocityVariance = "0.1";
   thetaMin         = 0;
   thetaMax         = "90";
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "SharkParticleData";
   blendStyle = "ADDITIVE";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   highResOnly = true;
      sortParticles = true;
   softnessDistance = 0.02;

};



new ArrayObject(TouchEffectors){};
new ArrayObject(InformationBubbles){};

function serverCmdReefTouchDown(%client, %touchId, %position)
{   
   %touchPosition = %position;
   %cam = %client.camera;
   %planeOffset = VectorScale(%cam.getForwardVector(), $pref::Cube::mosaicInteractionPlane+5);
   
   %upVector = "0 1 0"; // %this.getUpVector();
   %vec = VectorNormalize(VectorSub(%position, %cam.getPosition()));
   %t = -(VectorDot(VectorSub("0 0 0",%planeOffset), %upVector) / VectorDot(%vec, %upVector));
   %position = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

   // face camera
   %rot = pointToPos(%position, %cam.position);
   
   if( $ENABLE_SWISH ){
      %obj = new Trigger() {
         class = TurbulenceTriggerClass;
         dataBlock = TurbulenceTriggerData;
         scale = $SWISH_BOX_SIZE;
         polyhedron = "-0.5 0.5 -0.5 1.0 0.0 0.0 0.0 -1.0 0.0 0.0 0.0 1.0";
         velocity = "0 0 0";
         velocityTime = getSimTime();
         time = getSimTime();
         capturedFishCount = 0;
         direction = %vec;
      };
      %obj.soundInit();
      %obj.setTransform(%position SPC %rot);

      %obj.lastPosition = %position;
      TouchEffectors.add(%touchId, %obj);
      %obj.appointment = %obj.schedule($SWISH_LIFE,"suicide");
   }
}
$SWISH_SOUND_TRIGGER_SPEED = 1.0;
$SWISH_SOUND_RETRIGGER_DELAY = 0;
$SWISH_SOUND_QUICK_DELAY = 300;

function TurbulenceTriggerClass::soundInit(%this)
{
   %this.soundHasPlayed = 0;
   %this.soundLastTime = getSimTime();
   %this.soundBelowTriggerSpeed = 1;
}
function TurbulenceTriggerClass::playSound(%this, %client, %sound)
{
   %client.play3D(%sound,%this.getTransform());
}
//called when trigger moves
function TurbulenceTriggerClass::soundUpdate(%this, %client)
{
   if( VectorLen(%this.velocity) > $SWISH_SOUND_TRIGGER_SPEED )
   {
      if(%this.soundBelowTriggerSpeed){
         %timeSinceLast = getSimTime()-%this.soundLastTime;
         if( %this.soundHasPlayed==0 || (%timeSinceLast >= $SWISH_SOUND_RETRIGGER_DELAY) )
         {
            %sound = "";
            %index = getRandom(0,$swishListCount-1);
            %sound = $swishList[%index];
            %this.soundHasPlayed=1;
            %this.soundLastTime = getSimTime();
            %this.schedule(getRandom(0,100),"playSound",%client,%sound);
         }
      }
      %this.soundBelowTriggerSpeed = 0;
   }else{
      %this.soundBelowTriggerSpeed = 1;
   }
}

function TurbulenceTriggerClass::suicide(%this)
{
   %index = TouchEffectors.getIndexFromValue(%this);
   if(%index<0){
      echo("Failed to find trigger");
   }else{
      TouchEffectors.erase(%index);
   }
   %this.delete();
}
function serverCmdReefTouchMove(%client, %touchId, %position)
{
   // move on a plane
   %touchPosition = %position;
   %cam = %client.camera;
   %planeOffset = VectorScale(%cam.getForwardVector(), $pref::Cube::mosaicInteractionPlane+5);
   
   %upVector = "0 1 0"; // %this.getUpVector();
   %vec = VectorNormalize(VectorSub(%position, %cam.getPosition()));
   %t = -(VectorDot(VectorSub("0 0 0",%planeOffset), %upVector) / VectorDot(%vec, %upVector));
   %position = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   
   %index = TouchEffectors.getIndexFromKey(%touchId);

   if(%index>=0){
      %obj = TouchEffectors.getValue(%index);
      cancel(%obj.appointment);
      %obj.appointment = %obj.schedule($SWISH_LIFE,"suicide");
      %oldPosition = %obj.position;
         
      // face camera
      %rot = pointToPos(%position, %cam.position);
      %obj.setTransform(%position SPC %rot);
      
      %cam = %client.camera;
      %obj.direction = VectorSub(%position, %cam.getPosition());
      %obj.direction = VectorNormalize( %obj.direction );
      
      
      
      %currentTime = getSimTime();
      %dt = (%currentTime - %obj.time) / 1000.0;
      if(%dt>0){
         %delta = VectorSub(%position, %oldPosition);
         %obj.velocityTime = %currentTime / 1000.0;
         %obj.velocity = VectorScale(%delta, 1.0/%dt);
         %obj.soundUpdate(%client);         
      }
      %obj.time = %currentTime;
   }
}
function serverCmdReefTouchUp(%client, %touchId, %position, %camPos)
{
   if($NO_TOUCH_EFFECT){
      return;
   }
   // prevent touch up happening before touch down
   schedule(0, 0, "ReefTouchUp", %client, %touchId, %position, %camPos);
}
function ReefTouchUp(%client, %touchId, %position, %camPos)
{
   if($NO_TOUCH_EFFECT){
      return;
   }
   
   %index = TouchEffectors.getIndexFromKey(%touchId);
   if(%index>=0){

      %obj = TouchEffectors.getValue(%index);
      
      TouchEffectors.erase(%index);
      cancel(%obj.appointment);
      %obj.delete();   
   }
   else
      echo("ReefTouchUp: touch up before touch down");

   // move on a plane
   %cam = %client.camera;
   %planeOffset = VectorScale(%cam.getForwardVector(), $pref::Cube::mosaicInteractionPlane);
   
   %upVector = "0 1 0"; // %this.getUpVector();
   %vec = VectorNormalize(VectorSub(%position, %cam.getPosition()));
   %t = -(VectorDot(VectorSub("0 0 0",%planeOffset), %upVector) / VectorDot(%vec, %upVector));
   %position = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   
   %emitter = new ParticleEmitterNode()
   {
   //   active = false;
      emitter = ReefBubbleEmitterData;
      velocity = 1;
      position = %position;
      datablock = "TestEmitterNodeData";
      parentGroup = MissionCleanup;
   };
   %emitter.setObjectBox("-10 -10 -10 10 10 10");
   %emitter.setScopeAlways();
   %emitter.schedule(100, setActive, false);
   %emitter.schedule(101, delete);
}



////////////////////////////////////////////////////////////////////////////////
//

function StaticShapeAutoPlay::onAdd(%data, %this)
{
   %data._onAdd(%this);
}
function StaticShapeAutoPlay::_onAdd(%data, %this)
{
   %this.scareEnabled = false;
   if( %data.scare )
   {
      //add scare away trigger box
      %this.scareBox = new Trigger() {
         dataBlock = ScareTriggerData;
         position = "1000000 1000000 1000000";
         rotation = "0 0 1 0";
         scale = %data.scareBoxSize;
         polyhedron = "-0.5 0.5 -0.5 1.0 0.0 0.0 0.0 -1.0 0.0 0.0 0.0 1.0";
         direction = "0 0 1";
         hidden = "1";
         canSave = "1";
         canSaveDynamicFields = "1";
      };
      
      %smRenderTriggers = true;
      if( !%this.mountObject(%this.scareBox, 0 ) )
      {
         echo("failed to mount trigger");
      }
   }
   
   echo(%this.getDataBlock().shapefile SPC "   |delay =" SPC %this.delayStart/1000 SPC "   |loop=" SPC %this.loopInterval/1000);

   if(%this.delayStart != 0)
   {
      %this.schedule(0, setCloaked, true);
      %data.schedule(%this.delayStart, playAnimation, %this);
   }
   else
      %data.playAnimation(%this);      
}

   
function StaticShapeAutoPlay::playAnimation(%data, %this)
{
   %data._playAnimation(%this);
}
function StaticShapeAutoPlay::_playAnimation(%data, %this)
{
   if( !isObject(%this) ){
      echo("Invalid object sent to playAnimation()");      
   }
   if(%this.delayStart != 0)
      %this.setCloaked(false); 

   echo("START ANIMATION : " @ %this.getDataBlock().shapefile SPC "   |simtime=" SPC getSimTime()/1000);
  
   %this.stopThread(0);
   if(%this.animation !$= "")
      %this.playThread(0, %this.animation);
   else if(%data.animation !$= "")
      %this.playThread(0, %data.animation);
      
   if(%this.animationSpeed != 0.0)
      %this.setThreadTimeScale(0, %this.animationSpeed);

   if(%this.loopInterval != 0)
      %data.schedule(%this.loopInterval, playAnimation, %this);
      
   if( %data.scare )
   {
      %this.scareEnabled = true;
   }
   if( %data.bubbles )
   {
      %this.bubbleNode = new ParticleEmitterNode()
      {
         emitter = SharkEmitterData;
         velocity = 1;
         datablock = "SharkEmitterNodeData";
         parentGroup = MissionCleanup;
      };
      %this.bubbleNode.setObjectBox("-10 -10 -10 10 10 10");
      %this.bubbleNode.setScopeAlways();
      
      
      if( !%this.mountObject(%this.bubbleNode, 0 ) )
      {
         echo("failed to mount particle emitter to StaticShapeAutoPlay");
      }
   }
}
function StaticShapeAutoPlay::onEndSequence(%data, %this, %threadIndex)
{
   if(%threadIndex == 1)
   {
      %this.destroyThread(1);
   }
   
   if(%threadIndex > 0)
      return;
      
   %this.setCloaked(true);
   
   if( %data.scare )
   {
      %this.scareEnabled = false;
   }
   if( isObject(%this.bubbleNode) )
   {
      %this.bubbleNode.schedule(100, setActive, false);
      %this.bubbleNode.schedule(101, delete);
      %this.bubbleNode = "";
   }
}
function StaticShapeAutoPlay::onSelect()
{
   
}
function StaticShapeAutoPlay::onDeselect()
{
   
}

function StaticShapeAutoPlay::onRemove(%data, %this)
{
   if( isObject(%this.scareBox) )
   {
      %this.scareBox.delete();
   }
}


function ReefCurtain::onAdd(%this)
{
   
   %this.cloth = new PxCloth() {
      canSave = "0";
      canSaveDynamicFields = "0";
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      Material = %this.Material;
      samples = %this.samples;
      size = %this.size;
      bending = %this.bending;
      damping = %this.damping;
      triangleCollision = %this.triangleCollision;
      selfCollision = %this.selfCollision;
      density = %this.density;
      thickness = %this.thickness; // size.x / samples.x
      friction = %this.friction;
      bendingStiffness = %this.bendingStiffness;
      dampingCoefficient = %this.dampingCoefficient;
      collisionResponseCoefficient = %this.collisionResponseCoefficient;
      attachmentResponseCoefficient = %this.attachmentResponseCoefficient;
      attachmentPointScale = %this.attachmentPointScale;
      attachments = %this.attachments;
      parentGroup = %this;
   };
   
   
   %this.animationPos = 0;
   %this.originalPosition = %this.cloth.position;
   %this.originalScale = %this.cloth.attachmentPointScale;
   
   if(%this.animationSpeed == 0)
      %this.animationSpeed = 0.001;
   %this.schedule(%this.delayStart, "update", %this);          
}
function ReefCurtain::update(%this)
{
   %this.animationPos += %this.animationSpeed;
   %this.cloth.setTransform(VectorLerp(%this.originalPosition,%this.offsetPos,%this.animationPos));
   
   if(%this.animationPos < 1.0)
      %this.updateId = %this.schedule(32, "update", %this);
}

////////////////////////////////////////////////////////////////////////////////

datablock TriggerData(TurbulenceTriggerData : DefaultTrigger)
{
};

datablock TriggerData(ScareTriggerData : DefaultTrigger)
{
};

/////////////////////////////////////////////////////////////////////
function TurbulenceTriggerData::onEnterTrigger(%data, %this, %obj)
{
   if( %obj.getClassName() $= "AIFishPlayer" ){
      %obj.onEnterTrigger(%this, $Trigger::Swish);
   }
}
function TurbulenceTriggerData::onLeaveTrigger(%data, %this, %obj)
{
   if( %obj.getClassName() $= "AIFishPlayer" ){
      %obj.onLeaveTrigger(%this, $Trigger::Swish);
   }
}
/////////////////////////////////////////////////////////////////////
function ScareTriggerData::onEnterTrigger(%data, %this, %obj)
{
   if( %obj.getClassName() $= "AIFishPlayer" ){
      %obj.onEnterTrigger(%this, $Trigger::Scare);
   }
}
function ScareTriggerData::onLeaveTrigger(%data, %this, %obj)
{
   if( %obj.getClassName() $= "AIFishPlayer" ){
      %obj.onLeaveTrigger(%this, $Trigger::Scare);
   }
}
/////////////////////////////////////////////////////////////////////
//
datablock StaticShapeData(SchoolTarget_SSData)
{
   category = "AIMarker";
   shapeFile = "art/cube/ai/school_target.dts";
};

function AISpline::onAdd(%data, %this)
{
   if(%this.delayStart)
      %data.schedule( %this.delayStart, "_onAdd", %this );
   else
      %data._onAdd(%this);
}
function AISpline::_onAdd(%data, %this)
{
   %this.playThread(0, %data.animation);
   if(%this.animationSpeed != 0.0)
      %this.setThreadTimeScale(0, %this.animationSpeed);
      
  // %this.path1 = new PhysicalZone(){};
  
   %this.path1 = new StaticShape() {
      enableInputEvents = "0";
      dataBlock = "SchoolTarget_SSData";
      position = "0 0 0";
      rotation = "0 0 1 0";
      scale = "1 1 1";
      hidden = "1";
      canSave = "1";
      canSaveDynamicFields = "1";
      isRenderEnabled = false;
   };
   %this.mountObject(%this.path1, 0 );
}



/////////////////////////////////////////////////////////////////////
//

function serverCmdMIG_OnButtonDown(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape); 
   if(%this.parentGroup.minimized)
   {
      %this.parentGroup.maximize();
   }
   else
   {
      %this.parentGroup.minimize();
   }
}

function MenuImageGridClass::onAdd(%this)
{
   if(%this.imagePath $= "")
      return;
   
   loadDir(%this.imagePath);
   
   %count = 0;
   %currentRow = 0;
   for( %fileName = findFirstFile( %this.imagePath @ "/*.*" ); %fileName !$= ""; %fileName = findNextFile() )
   {
      if( isFile(%fileName) )
      {

         %popup = 0;
         switch$(fileExt(%fileName))
         {
         case ".jpg": %popup = createImageWidget3D(%fileName, "0 0 0", "1 0 0 0", 0.25, 0.25, true);
         case ".png": %popup = createImageWidget3D(%fileName, "0 0 0", "1 0 0 0", 0.25, 0.25, true);
         case ".url": %popup = createWebWidget3DFromShortcut(%fileName, "0 0 0", "1 0 0 0", 0.25, 0.25, true);
         default: %popup = createWebWidget3D(%fileName, "0 0 0", "1 0 0 0", 0.25, 0.25, true);
         }
         
         if(isObject(%popup))
         {
            if(!isObject(%this.row[%currentRow]))
            {
               %this.row[%currentRow] = new StaticShape() {
                  dataBlock = "MenuImageGridRowData";
                  position = VectorAdd(%this.position, 0 SPC (%currentRow*0.1) SPC 0 );
                  rotation = %this.rotation;
                  scale = %this.scale;
                  canSave = "0";
                  canSaveDynamicFields = "0";
                     parentGroup = %this;
               };
            }
            
            // calc local mount index
            %mountIndex = %count-(%currentRow*10);
            
            %popup.onAdd();
            %popup.instantMinimize();
            
            // mount new image to row object
            %mountOffset = "0 0 0 0 0 1" SPC %halfPi;
            %this.row[%currentRow].mountObject(%popup, %mountIndex, %mountOffset);
            %popup.minimizeMount = %this.row[%currentRow];
            %popup.minimizeMountIndex = %mountIndex;
            %popup.minimizeMountTransform = %mountOffset;
            


            // update stepping values
            if(%mountIndex == 9)
               %currentRow++;
            %count++;
         }
      }
   }
   
   // create button to activate row animations
   %this.button = new StaticShape() {
      enableInputEvents = "1";
      dataBlock = "MenuImageGridData";
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      parentGroup = %this;
      isRenderEnabled = false;
   };
   
   
   %this.minimized = true;
}


function MenuImageGridClass::maximize(%this)
{
   if(!%this.minimized)
      return;
      
   for(%i = 0; isObject(%this.row[%i]); %i++)
   {
      %row = %this.row[%i];
      %row.playThread(0, "maximize");
      %offset = "0 0" SPC ((%i+1)*%this.rowOffset);
      for(%j = 0; %j <= 1.0; %j+=0.01)
         %row.schedule(2000*%j, setTransform, VectorAdd(%this.position, VectorScale(%offset, %j)));
   }
   %this.minimized = false;
}


function MenuImageGridClass::minimize(%this)
{
   if(%this.minimized)
      return;
      
   for(%i = 0; isObject(%this.row[%i]); %i++)
   {
      %row = %this.row[%i];
      %row.playThread(0, "maximize");
      %row.setThreadDir(0, false);
      %offset = "0 0" SPC ((%i+1)*%this.rowOffset);
      for(%j = 1.0; %j >= 0.0; %j-=0.01)
         %row.schedule(2000*(1.0 - %j), setTransform, VectorAdd(%this.position, VectorScale(%offset, %j)));
   }
   %this.minimized = true;
}



////////////////////////////////////////////////////////////////////////////////

function serverCmdViewButton_OnClick(%client, %shape)
{
   %this = %client.resolveObjectFromGhostIndex(%shape);
          
   
   if(isObject(%this) && isObject(%this.parentGroup))
      %this.parentGroup.onChangeView(%this, %client);

}
function ViewButtonClass::onAdd(%this)
{
   // set default view
   %this.viewId = 0;
}
function ViewButtonClass::onUnselect(%this)
{
}
function ViewButtonClass::onSelect(%this)
{
}

function ViewButtonComboClass::onAdd(%this)
{
}

function createReefChunk(%path, %isRenderEnabled, %position, %rotation)
{
   %obj = new TSStatic() {
      shapeName = %path;
      enableInputEvents = "0";
      position = %position;
      rotation = %rotation;
      scale = "1 1 1";
      hidden = "0";
      canSave = "1";
      canSaveDynamicFields = "1";
      castShadowsOnly = !%isRenderEnabled;
      collisionType = "None";  
      noAutoScope = true;  
      playAmbient = false;  
      meshCulling = "0";
   };
   %obj.setNoAutoScope();
   return %obj;
}


$currentViewId = 0;

function preloadWebData(%cl, %webData)
{
 //  %webData.url = getFishWebRoot() @ "/index_master.html?f=basslet";
   %webData.url = getFishWebRoot() @ "/wiki/Anthiinae";
}
function changeViewAllClients(%viewId)
{
   $currentViewId = %viewId;

   if(isObject($reefTop1_low))
   {
      $reefTop1_shadow.delete();
      $reefTop2_shadow.delete();
      
      $reefTop1_low.delete();
      $reefTop2_low.delete();
      
      $reefFront1_shadow.delete();
      $reefFront2_shadow.delete();
      $reefFront3_shadow.delete();
      $reefFront4_shadow.delete();
      $reefFront5_shadow.delete();
      $reefFront6_shadow.delete();
      $reefFront7_shadow.delete();
      $reefFront8_shadow.delete();
      $reefFront9_shadow.delete();
      $reefFront10_shadow.delete();
      
      $reefFront1_low.delete();
      $reefFront2_low.delete();
      $reefFront3_low.delete();
      $reefFront4_low.delete();
      $reefFront5_low.delete();
      $reefFront6_low.delete();
      $reefFront7_low.delete();
      $reefFront8_low.delete();
      $reefFront9_low.delete();
      $reefFront10_low.delete();
      
      $reefFront1_high.delete();
      $reefFront2_high.delete();
      $reefFront3_high.delete();
      $reefFront4_high.delete();
      $reefFront5_high.delete();
      $reefFront6_high.delete();
      $reefFront7_high.delete();
      $reefFront8_high.delete();
      $reefFront9_high.delete();
      $reefFront10_high.delete();
   }
   if(!isObject($reefTop1_low))
   {
      $reefTop1_shadow =  createReefChunk("art/cube/sets/reef_t1.dae", false, "0 0 0", "0 0 1 180");
      $reefTop2_shadow =  createReefChunk("art/cube/sets/reef_t2.dae", false, "0 0 0", "0 0 1 180");
      
      $reefTop1_low =  createReefChunk("art/cube/sets/reef_t1.dae", true, "0 0 0", "0 0 1 180");
      $reefTop2_low =  createReefChunk("art/cube/sets/reef_t2.dae", true, "0 0 0", "0 0 1 180");
      
      $reefFront1_shadow =  createReefChunk("art/cube/sets/reef_b1.dae", false, "0 0 0", "0 0 1 180");
      $reefFront2_shadow =  createReefChunk("art/cube/sets/reef_b2.dae", false, "0 0 0", "0 0 1 180");
      $reefFront3_shadow =  createReefChunk("art/cube/sets/reef_b3.dae", false, "0 0 0", "0 0 1 180");
      $reefFront4_shadow =  createReefChunk("art/cube/sets/reef_b4.dae", false, "0 0 0", "0 0 1 180");
      $reefFront5_shadow =  createReefChunk("art/cube/sets/reef_b5.dae", false, "0 0 0", "0 0 1 180");
      $reefFront6_shadow =  createReefChunk("art/cube/sets/reef_b6.dae", false, "0 0 0", "0 0 1 180");
      $reefFront7_shadow =  createReefChunk("art/cube/sets/reef_b7.dae", false, "0 0 0", "0 0 1 180");
      $reefFront8_shadow =  createReefChunk("art/cube/sets/reef_b8.dae", false, "0 0 0", "0 0 1 180");
      $reefFront9_shadow =  createReefChunk("art/cube/sets/reef_b9.dae", false, "0 0 0", "0 0 1 180");
      $reefFront10_shadow =  createReefChunk("art/cube/sets/reef_b10.dae", false, "0 0 0", "0 0 1 180");
      
      $reefFront1_low =  createReefChunk("art/cube/sets/reef_b1.dae", true, "0 0 0", "0 0 1 180");
      $reefFront2_low =  createReefChunk("art/cube/sets/reef_b2.dae", true, "0 0 0", "0 0 1 180");
      $reefFront3_low =  createReefChunk("art/cube/sets/reef_b3.dae", true, "0 0 0", "0 0 1 180");
      $reefFront4_low =  createReefChunk("art/cube/sets/reef_b4.dae", true, "0 0 0", "0 0 1 180");
      $reefFront5_low =  createReefChunk("art/cube/sets/reef_b5.dae", true, "0 0 0", "0 0 1 180");
      $reefFront6_low =  createReefChunk("art/cube/sets/reef_b6.dae", true, "0 0 0", "0 0 1 180");
      $reefFront7_low =  createReefChunk("art/cube/sets/reef_b7.dae", true, "0 0 0", "0 0 1 180");
      $reefFront8_low =  createReefChunk("art/cube/sets/reef_b8.dae", true, "0 0 0", "0 0 1 180");
      $reefFront9_low =  createReefChunk("art/cube/sets/reef_b9.dae", true, "0 0 0", "0 0 1 180");
      $reefFront10_low =  createReefChunk("art/cube/sets/reef_b10.dae", true, "0 0 0", "0 0 1 180");
      
      $reefFront1_high =  createReefChunk("art/cube/sets/reef_b1_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront2_high =  createReefChunk("art/cube/sets/reef_b2_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront3_high =  createReefChunk("art/cube/sets/reef_b3_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront4_high =  createReefChunk("art/cube/sets/reef_b4_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront5_high =  createReefChunk("art/cube/sets/reef_b5_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront6_high =  createReefChunk("art/cube/sets/reef_b6_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront7_high =  createReefChunk("art/cube/sets/reef_b7_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront8_high =  createReefChunk("art/cube/sets/reef_b8_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront9_high =  createReefChunk("art/cube/sets/reef_b9_high.dae", true, "0 0 0", "0 0 1 180");
      $reefFront10_high =  createReefChunk("art/cube/sets/reef_b10_high.dae", true, "0 0 0", "0 0 1 180");
   }
   //new code
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
      commandToClient( %cl, 'ChangeView', %viewId );
	}

   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
      if( !%cl.isAIControlled() )
      {
         AIFishInfoWebView0.clearScopeToClient(%cl);
         AIFishInfoWebView1.clearScopeToClient(%cl);
         AIFishInfoWebView2.clearScopeToClient(%cl);
         AIFishInfoWebView3.clearScopeToClient(%cl);
         AIFishInfoWebView4.clearScopeToClient(%cl);
         AIFishInfoWebView5.clearScopeToClient(%cl);
         AIFishInfoWebView6.clearScopeToClient(%cl);
         AIFishInfoWebView7.clearScopeToClient(%cl);
         AIFishInfoWebView8.clearScopeToClient(%cl);
         AIFishInfoWebView9.clearScopeToClient(%cl);
         AIFishInfoWebView10.clearScopeToClient(%cl);
         AIFishInfoWebView11.clearScopeToClient(%cl);
         AIFishInfoWebView12.clearScopeToClient(%cl);
         AIFishInfoWebView13.clearScopeToClient(%cl);
         AIFishInfoWebView14.clearScopeToClient(%cl);
         AIFishInfoWebView15.clearScopeToClient(%cl);
         AIFishInfoWebView16.clearScopeToClient(%cl);
         AIFishInfoWebView17.clearScopeToClient(%cl);
         AIFishInfoWebView18.clearScopeToClient(%cl);
         AIFishInfoWebView19.clearScopeToClient(%cl);         
                   
         %clientID = %cl.clientId;
         switch(%viewId)
         {
         case 6:  //queensland museum view
            $reefTop1_shadow.scopeToClient(%cl);
            $reefFront1_shadow.scopeToClient(%cl);
            $reefFront2_shadow.scopeToClient(%cl);
            $reefFront3_shadow.scopeToClient(%cl);
            $reefFront4_shadow.scopeToClient(%cl);
            $reefFront5_shadow.scopeToClient(%cl);
            $reefFront6_shadow.scopeToClient(%cl);
            $reefFront8_shadow.scopeToClient(%cl);
            $reefFront9_shadow.scopeToClient(%cl);
            $reefFront10_shadow.scopeToClient(%cl);
            
            $reefFront7_high.scopeToClient(%cl);
            $reefTop2_low.scopeToClient(%cl);
            
            AIFishInfoWebView0.scopeToClient(%cl);
            AIFishInfoWebView1.scopeToClient(%cl);
            AIFishInfoWebView2.scopeToClient(%cl);
            AIFishInfoWebView3.scopeToClient(%cl);
            preloadWebData(%cl, AIFishInfoWebView0);
            preloadWebData(%cl, AIFishInfoWebView1);
            preloadWebData(%cl, AIFishInfoWebView2);
            preloadWebData(%cl, AIFishInfoWebView3);
            preloadWebData(%cl, AIFishInfoWebView4);
         
            %cl.getCameraObject().setEarOffset($pref::cube::earOffset[10]);
         
         case 0: 
            $reefTop1_low.scopeToClient(%cl);
            $reefTop2_low.scopeToClient(%cl);

            if(%clientID == 3)
            {
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else
            {
               $reefFront1_low.scopeToClient(%cl);
               $reefFront2_low.scopeToClient(%cl);
               $reefFront3_low.scopeToClient(%cl);
               $reefFront4_low.scopeToClient(%cl);
               $reefFront5_low.scopeToClient(%cl);
               $reefFront6_low.scopeToClient(%cl);
               $reefFront7_low.scopeToClient(%cl);
               $reefFront8_low.scopeToClient(%cl);
               $reefFront9_low.scopeToClient(%cl);
               $reefFront10_low.scopeToClient(%cl);
               
            }
            
            preloadWebData(%cl, AIFishInfoWebView0);
            preloadWebData(%cl, AIFishInfoWebView1);
            preloadWebData(%cl, AIFishInfoWebView2);
            preloadWebData(%cl, AIFishInfoWebView3);
            preloadWebData(%cl, AIFishInfoWebView4);
            preloadWebData(%cl, AIFishInfoWebView5);
            preloadWebData(%cl, AIFishInfoWebView6);
            preloadWebData(%cl, AIFishInfoWebView7);
            preloadWebData(%cl, AIFishInfoWebView8);
            preloadWebData(%cl, AIFishInfoWebView9);
            preloadWebData(%cl, AIFishInfoWebView10);
            preloadWebData(%cl, AIFishInfoWebView11);
            preloadWebData(%cl, AIFishInfoWebView12);
            preloadWebData(%cl, AIFishInfoWebView13);
            preloadWebData(%cl, AIFishInfoWebView14);
            preloadWebData(%cl, AIFishInfoWebView15);
            preloadWebData(%cl, AIFishInfoWebView16);
            preloadWebData(%cl, AIFishInfoWebView17);
            preloadWebData(%cl, AIFishInfoWebView18);
            preloadWebData(%cl, AIFishInfoWebView19);

            AIFishInfoWebView0.scopeToClient(%cl);
            AIFishInfoWebView1.scopeToClient(%cl);
            AIFishInfoWebView2.scopeToClient(%cl);
            AIFishInfoWebView3.scopeToClient(%cl);
            AIFishInfoWebView4.scopeToClient(%cl);
            AIFishInfoWebView5.scopeToClient(%cl);
            AIFishInfoWebView6.scopeToClient(%cl);
            AIFishInfoWebView7.scopeToClient(%cl);
            AIFishInfoWebView8.scopeToClient(%cl);
            AIFishInfoWebView9.scopeToClient(%cl);
            AIFishInfoWebView10.scopeToClient(%cl);
            AIFishInfoWebView11.scopeToClient(%cl);
            AIFishInfoWebView12.scopeToClient(%cl);
            AIFishInfoWebView13.scopeToClient(%cl);
            AIFishInfoWebView14.scopeToClient(%cl);
            AIFishInfoWebView15.scopeToClient(%cl);
            AIFishInfoWebView16.scopeToClient(%cl);
            AIFishInfoWebView17.scopeToClient(%cl);
            AIFishInfoWebView18.scopeToClient(%cl);
            AIFishInfoWebView19.scopeToClient(%cl);
            
            %cl.getCameraObject().setEarOffset($pref::cube::earOffset[10]);
            
            
         case 1:
            if(%clientID == 0 || %clientID == 13)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_high.scopeToClient(%cl);
               $reefFront2_high.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               preloadWebData(%cl, AIFishInfoWebView0);
               preloadWebData(%cl, AIFishInfoWebView1);
               preloadWebData(%cl, AIFishInfoWebView2);
               preloadWebData(%cl, AIFishInfoWebView3);
               preloadWebData(%cl, AIFishInfoWebView4);
               AIFishInfoWebView0.scopeToClient(%cl);
               AIFishInfoWebView1.scopeToClient(%cl);
               AIFishInfoWebView2.scopeToClient(%cl);
               AIFishInfoWebView3.scopeToClient(%cl);

               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[0]);
            }
            else if(%clientID == 1)
            {
               $reefFront1_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView0);
               preloadWebData(%cl, AIFishInfoWebView1);
               AIFishInfoWebView0.scopeToClient(%cl);
               AIFishInfoWebView1.scopeToClient(%cl);
         
               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               //commandToClient(%cl, 'ChangeEarOffset', 0);
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[0]);
            }
            else if(%clientID == 2)
            {
               $reefFront2_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView2);
               preloadWebData(%cl, AIFishInfoWebView3);
               AIFishInfoWebView2.scopeToClient(%cl);
               AIFishInfoWebView3.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[1]);
            }
            else if(%clientID == 3)
            {
               $reefFront3_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView4);
               preloadWebData(%cl, AIFishInfoWebView5);
               AIFishInfoWebView4.scopeToClient(%cl);
               AIFishInfoWebView5.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[2]);
            }
            else if(%clientID == 4)
            {
               $reefFront4_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView6);
               preloadWebData(%cl, AIFishInfoWebView7);
               AIFishInfoWebView6.scopeToClient(%cl);
               AIFishInfoWebView7.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[3]);
            }
            else if(%clientID == 5)
            {
               $reefFront5_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView8);
               preloadWebData(%cl, AIFishInfoWebView9);
               AIFishInfoWebView8.scopeToClient(%cl);
               AIFishInfoWebView9.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[4]);
            }
            else if(%clientID == 6)
            {
               $reefFront6_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView10);
               preloadWebData(%cl, AIFishInfoWebView11);
               AIFishInfoWebView10.scopeToClient(%cl);
               AIFishInfoWebView11.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[5]);
            }
            else if(%clientID == 7)
            {
               $reefFront7_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView12);
               preloadWebData(%cl, AIFishInfoWebView13);
               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 8)
            {
               $reefFront8_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView14);
               preloadWebData(%cl, AIFishInfoWebView15);
               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[7]);
            }
            else if(%clientID == 9)
            {
               $reefFront9_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView16);
               preloadWebData(%cl, AIFishInfoWebView17);
               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 10)
            {
               $reefFront10_high.scopeToClient(%cl);
               preloadWebData(%cl, AIFishInfoWebView18);
               preloadWebData(%cl, AIFishInfoWebView19);
               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);

               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[9]);
            }
            else if(%clientID == 11)
            {
               $reefTop1_low.scopeToClient(%cl);

               //---shadows----
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[10]);
            }
            else if(%clientID == 12)
            {
               $reefTop2_low.scopeToClient(%cl);
               
               //---shadows----
               $reefTop1_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[10]);
            }
         case 2:
         
            if(%clientID == 0 || %clientID == 13)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_high.scopeToClient(%cl);
               $reefFront4_high.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView4.scopeToClient(%cl);
               AIFishInfoWebView5.scopeToClient(%cl);
               AIFishInfoWebView6.scopeToClient(%cl);
               AIFishInfoWebView7.scopeToClient(%cl);

               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[2]);
            }
            else if(%clientID == 1)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_high.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 2)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_high.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[7]);
            }
            else if(%clientID == 3)
            {
               $reefTop1_low.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 4)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_high.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView4.scopeToClient(%cl);
               AIFishInfoWebView5.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[2]);
            }
            else if(%clientID == 5)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_high.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView6.scopeToClient(%cl);
               AIFishInfoWebView7.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[3]);
            }
            else if(%clientID == 6)
            {
               $reefTop1_low.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 7)
            {
               $reefTop1_low.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 8)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_high.scopeToClient(%cl);
               $reefFront4_high.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView4.scopeToClient(%cl);
               AIFishInfoWebView5.scopeToClient(%cl);
               AIFishInfoWebView6.scopeToClient(%cl);
               AIFishInfoWebView7.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[2]);
            }
            else if(%clientID == 9)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_high.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView8.scopeToClient(%cl);
               AIFishInfoWebView9.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[4]);
            }
            else if(%clientID == 10)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_high.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView10.scopeToClient(%cl);
               AIFishInfoWebView11.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[5]);
            }            
         case 3:
            if(%clientID == 0 || %clientID == 13)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_high.scopeToClient(%cl);
               $reefFront6_high.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView8.scopeToClient(%cl);
               AIFishInfoWebView9.scopeToClient(%cl);
               AIFishInfoWebView10.scopeToClient(%cl);
               AIFishInfoWebView11.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[4]);
            }
            else if(%clientID == 1)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_high.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 2)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_high.scopeToClient(%cl);
               
               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[9]);
            }
            else if(%clientID == 3)
            {
               $reefTop1_low.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 4)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_high.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView8.scopeToClient(%cl);
               AIFishInfoWebView9.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[4]);
            }
            else if(%clientID == 5)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_high.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView10.scopeToClient(%cl);
               AIFishInfoWebView11.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[5]);
            }
            else if(%clientID == 6)
            {
               $reefTop1_low.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 7)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 8)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_high.scopeToClient(%cl);
               $reefFront6_high.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView8.scopeToClient(%cl);
               AIFishInfoWebView9.scopeToClient(%cl);
               AIFishInfoWebView10.scopeToClient(%cl);
               AIFishInfoWebView11.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[4]);
            }
            else if(%clientID == 9)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_high.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 10)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_high.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
         case 4:
            if(%clientID == 0 || %clientID == 13)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_high.scopeToClient(%cl);
               $reefFront8_high.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);
               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 1)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_high.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView0.scopeToClient(%cl);
               AIFishInfoWebView1.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[0]);
            }
            else if(%clientID == 2)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_high.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView2.scopeToClient(%cl);
               AIFishInfoWebView3.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[1]);
            }
            else if(%clientID == 3)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 4)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_high.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 5)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_high.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[7]);
            }
            else if(%clientID == 6)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 7)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 8)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_high.scopeToClient(%cl);
               $reefFront8_high.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView12.scopeToClient(%cl);
               AIFishInfoWebView13.scopeToClient(%cl);
               AIFishInfoWebView14.scopeToClient(%cl);
               AIFishInfoWebView15.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[6]);
            }
            else if(%clientID == 9)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_high.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 10)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_high.scopeToClient(%cl);
               
               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[9]);
            }
            
         case 5:
            if(%clientID == 0 || %clientID == 13)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_high.scopeToClient(%cl);
               $reefFront10_high.scopeToClient(%cl);
               
               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);
               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 1)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_high.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView4.scopeToClient(%cl);
               AIFishInfoWebView5.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[2]);
            }
            else if(%clientID == 2)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_high.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView6.scopeToClient(%cl);
               AIFishInfoWebView7.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[3]);
            }
            else if(%clientID == 3)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 4)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_high.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);

               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 5)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_high.scopeToClient(%cl);

               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[9]);
            }
            else if(%clientID == 6)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 7)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_low.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
            }
            else if(%clientID == 8)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_high.scopeToClient(%cl);
               $reefFront10_high.scopeToClient(%cl);
               
               AIFishInfoWebView16.scopeToClient(%cl);
               AIFishInfoWebView17.scopeToClient(%cl);
               AIFishInfoWebView18.scopeToClient(%cl);
               AIFishInfoWebView19.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[8]);
            }
            else if(%clientID == 9)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_high.scopeToClient(%cl);
               $reefFront2_shadow.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView0.scopeToClient(%cl);
               AIFishInfoWebView1.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[0]);
            }
            else if(%clientID == 10)
            {
               $reefTop1_shadow.scopeToClient(%cl);
               $reefTop2_shadow.scopeToClient(%cl);
               $reefFront1_shadow.scopeToClient(%cl);
               $reefFront2_high.scopeToClient(%cl);
               $reefFront3_shadow.scopeToClient(%cl);
               $reefFront4_shadow.scopeToClient(%cl);
               $reefFront5_shadow.scopeToClient(%cl);
               $reefFront6_shadow.scopeToClient(%cl);
               $reefFront7_shadow.scopeToClient(%cl);
               $reefFront8_shadow.scopeToClient(%cl);
               $reefFront9_shadow.scopeToClient(%cl);
               $reefFront10_shadow.scopeToClient(%cl);
               
               AIFishInfoWebView2.scopeToClient(%cl);
               AIFishInfoWebView3.scopeToClient(%cl);
               
               %cl.getCameraObject().setEarOffset($pref::cube::earOffset[1]);
            }
         }      
      }
   }
   //reset button positions
	if( isObject(ViewButtonCombo) )
	{
   	%client = ClientGroup.getObject(0);
      %this = ViewButtonCombo;  
      %count = %this.getCount();
      for (%i = 0; %i < %count; %i++)
      {
         %obj = %this.getObject(%i);
         %obj.position = $pref::Cube::DemoViewButtonPosition[%viewId];
         %obj.scale = $pref::Cube::DemoViewButtonScale[%viewId];
         %obj.rotation = %client.camera.rotation;
      }
	}
}
function ViewButtonComboClass::onChangeView(%this, %selected, %client)
{
   if( isObject(%this.selected) ){
      %this.selected.onUnselect();
   }
   %this.selected = %selected;
   %this.selected.onSelect();
   %viewId = %this.selected.viewId;
   
   changeViewAllClients(%viewId);
}

function serverCmdLightrayButton_OnClick(%client)
{
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
      if( !%cl.isAIControlled() )
         commandToClient( %cl, 'LightrayToggle' );
   }
}
function serverCmdDebugButton_OnClick(%client)
{
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
      if( !%cl.isAIControlled() )
         commandToClient( %cl, 'DebugToggle' );
   }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



function CloneStaticShapeToNodeClass::createObjects(%this,%shapeName,%nodeName,%shape,%enableInputEvents,%shapeIndex)
{
   %i = 0;
   %index = -1;
   
   if(%shapeName $= "")
      return;
      
   // check for single node
   if((%index = %shape.getNodeIndex(%nodeName)) != -1)
   {
      %this.createObject(%index,%shapeName,%shape,%enableInputEvents,%shapeIndex,0);
   }
      
   // create objects for each node
   
// dts export
//   while((%index = %shape.getNodeIndex(%nodeName @ " (" @ (%i++) @ ")")) != -1)
// max collada export
//   while((%index = %shape.getNodeIndex(%nodeName @ "__" @ (%i++) @ "_")) != -1)
// lightwave collada export
   while((%index = %shape.getNodeIndex(%nodeName @ "_(" @ (%i++) @ ")")) != -1)
      %this.createObject(%index,%shapeName,%shape,%enableInputEvents,%shapeIndex,%i-1);
}

function CloneStaticShapeToNodeClass::createObject(%this,%index,%shapeDatablock,%shape,%enableInputEvents,%shapeIndex,%loopIndex)
{  
   if(!isObject(%shapeDatablock))
      return;
   
//   %scale = %shape.getNodeScale(%index);
   if(%this.offsetTransform !$= "")
      %transform = MatrixMultiply(%this.offsetTransform, %shape.getNodeTransform(%index));
   else
      %transform = %shape.getNodeTransform(%index);
      
   // collada axis hack max
   //%transform = MatrixMultiply(%transform,"0 0 0 1 0 0 -1.552");
   // collada axis hack lightwave
   %transform = MatrixMultiply(%transform,"0 0 0 1 0 0 1.552");


   %scale = "1 1 1";
   
  // eval("$scaleOveride = " @ %this.getId() @ ".scale" @ %shapeIndex @ "[" @ %loopIndex @ "];");
   %scaleOveride = eval("%scaleOveride = " @ %this.getId() @ ".scale" @ %shapeIndex @ "[" @ %loopIndex @ "];");
   if(%scaleOveride)
      %scale = %scaleOveride;
   
   if(%shapeDatablock.getClassName() $= "TSForestItemData")
   {
      if(!%this.forest.isFilled)
         %this.forest.addItemWithTransform(%shapeDatablock,%transform,%scale);
   }
   else
   {
      //echo("Creating " @ %shapeDatablock @ " at " @ %transform);
      %object = new StaticShape() {
         canSaveDynamicFields = "0";
         Enabled = "1";
         dataBlock = %shapeDatablock;
      //   Enable = "1";
         parentGroup = %this;
         enableInputEvents = %enableInputEvents;
         inputEventsMethod = %shapeDatablock.inputEventsMethod;
         
      };
      %object.setTransform(%transform);
      %object.setScale(%scale);
   
   }   
}
function CloneStaticShapeToNodeClass::onAdd(%this)
{
   if(%this.nodeName[0] $= "" || %this.nodeShapeName $= "" || %this.hasProcessed) return;
         
   %shapeData = new ShapeBaseData() {
      shapeFile = %this.nodeShapeName;
   };
   %shape = new ShapeBase() {
      canSaveDynamicFields = "1";
      Enabled = "0";
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      offsetTransform = %this.offsetTransform;
      dataBlock = %shapeData;
   };
   
   for(%i=0; %this.shapeDatablock[%i] !$= ""; %i++)
   {
      %this.createObjects(%this.shapeDatablock[%i], %this.nodeName[%i], %shape, %this.enableInputEvents[%i], %i);
   }
    
   %shape.schedule(%this.delayStart+1, "delete");
   %shapeData.schedule(%this.delayStart+1, "delete");
   
   if(!%this.hasProcessed)    
      %this.hasProcessed = true;
}







function CloneRandomToNodeClass::createObjects(%this,%nodeName,%shape)
{
   %i = 0;
   %index = -1;
         
   // check for single node
   if((%index = %shape.getNodeIndex(%nodeName)) != -1)
   {
      %this.createObject(%index,%shape);
   }
      
   // create objects for each node
// dts export
//   while((%index = %shape.getNodeIndex(%nodeName @ " (" @ (%i++) @ ")")) != -1)
// max collada export
//   while((%index = %shape.getNodeIndex(%nodeName @ "__" @ (%i++) @ "_")) != -1)
// lightwave collada export
   while((%index = %shape.getNodeIndex(%nodeName @ "_(" @ (%i++) @ ")")) != -1)
      %this.createObject(%index,%shape);
}
function CloneRandomToNodeClass::createObject(%this,%index,%shape)
{  
   if(%this.offsetTransform !$= "")
      %transform = MatrixMultiply(%this.offsetTransform, %shape.getNodeTransform(%index));
   else
      %transform = %shape.getNodeTransform(%index);
      
   %flip = 0;
   if(%this.flipFrequency != 0)
   {
      if(getRandomFloat(0,1)<%this.flipFrequency)
         %flip = 1;
   }
   %ran = getRandomFloat(0,1);
   %freq = 0;
   %index = 0;
   for(%i = 0; %i < %this.shapeCount; %i++)
   {
      %freq += %this.frequency[%i];       
      if(%ran < %freq)
      {
         %index = %i;
         break;
      }
   }
   
   %shapeDatablock = %this.shapeDatablock[%index];
   %enableInputEvents = %this.enableInputEvents[%index];
   
   %scaleRange = %this.scaleRange[%index];
   if(%scaleRange !$= "")
      %scale = getRandomFloat(%scaleRange.x,%scaleRange.y);
   else %scale = 1;
   
   // collada axis hack max
   //%transform = MatrixMultiply(%transform,"0 0 0 1 0 0 -1.552");
   // collada axis hack lightwave
   %transform = MatrixMultiply(%transform,"0 0 0 1 0 0 1.552");
   if(%flip)
   {
      %transform = MatrixMultiply(%transform,"0 0 0 0 0 1 3.14");
   }
   
   %transform = MatrixMultiply(%shape.getTransform(),%transform);
   
   if(!isObject(%shapeDatablock))
      return;
   
   if(%shapeDatablock.getClassName() $= "TSForestItemData")
   {
      if(!%this.forest.isFilled)
      {
         %this.forest.addItemWithTransform(%shapeDatablock,%transform,%scale SPC %scale SPC %scale);
      }
   }
   else
   {
      //echo("Creating " @ %shapeDatablock @ " at " @ %transform);
      %object = new StaticShape() {
         canSaveDynamicFields = "0";
         Enabled = "1";
         dataBlock = %shapeDatablock;
      //   Enable = "1";
         parentGroup = %this;
         enableInputEvents = %enableInputEvents;
         scale = %scale SPC %scale SPC %scale;
      };
      %object.setTransform(%transform);
   }
}
function CloneRandomToNodeClass::onAdd(%this)
{
   if(%this.nodeName[0] $= "" || %this.nodeShapeName $= "" || %this.hasProcessed) return;
                 
   %shapeData = new ShapeBaseData() {
      shapeFile = %this.nodeShapeName;
   };
   %shape = new ShapeBase() {
      canSaveDynamicFields = "1";
      Enabled = "0";
      position = %this.position;
      rotation = %this.rotation;
      scale = %this.scale;
      offsetTransform = %this.offsetTransform;
      dataBlock = %shapeData;
   };
   
 //  for(%i=0; %this.shapeDatablock[%i] !$= ""; %i++)
 //  {
      %this.schedule(%this.delayStart, "createObjects", %this.nodeName[0], %shape);
 //  }
    
   %shape.schedule(%this.delayStart+1, "delete");
   %shapeData.schedule(%this.delayStart+1, "delete");
   
   if(!%this.hasProcessed)    
      %this.hasProcessed = true;
}

///////////////////////////////////////////////////////////////////////


function scopeReefChunks(%viewId)
{
   //---
   $reefFront1_low.rangeCenter = "0.05 0.91122485";
   $reefFront2_low.rangeCenter = "0.15 0.91122485";
   $reefFront3_low.rangeCenter = "0.25 0.91122485";
   $reefFront4_low.rangeCenter = "0.35 0.91122485";
   $reefFront5_low.rangeCenter = "0.45 0.91122485";
   $reefFront6_low.rangeCenter = "0.55 0.91122485";
   $reefFront7_low.rangeCenter = "0.65 0.91122485";
   $reefFront8_low.rangeCenter = "0.75 0.91122485";
   $reefFront9_low.rangeCenter = "0.85 0.91122485";
   $reefFront10_low.rangeCenter = "0.95 0.91122485";
   //---
   $reefFront1_high.rangeCenter = "0.05 0.91122485";
   $reefFront2_high.rangeCenter = "0.15 0.91122485";
   $reefFront3_high.rangeCenter = "0.25 0.91122485";
   $reefFront4_high.rangeCenter = "0.35 0.91122485";
   $reefFront5_high.rangeCenter = "0.45 0.91122485";
   $reefFront6_high.rangeCenter = "0.55 0.91122485";
   $reefFront7_high.rangeCenter = "0.65 0.91122485";
   $reefFront8_high.rangeCenter = "0.75 0.91122485";
   $reefFront9_high.rangeCenter = "0.85 0.91122485";
   $reefFront10_high.rangeCenter = "0.95 0.91122485";
   //---
   $reefFront1_shadow.rangeCenter = "0.05 0.91122485";
   $reefFront2_shadow.rangeCenter = "0.15 0.91122485";
   $reefFront3_shadow.rangeCenter = "0.25 0.91122485";
   $reefFront4_shadow.rangeCenter = "0.35 0.91122485";
   $reefFront5_shadow.rangeCenter = "0.45 0.91122485";
   $reefFront6_shadow.rangeCenter = "0.55 0.91122485";
   $reefFront7_shadow.rangeCenter = "0.65 0.91122485";
   $reefFront8_shadow.rangeCenter = "0.75 0.91122485";
   $reefFront9_shadow.rangeCenter = "0.85 0.91122485";
   $reefFront10_shadow.rangeCenter = "0.95 0.91122485";
   //---
   $reefTop1_low.rangeCenter = "0.25 0.64797035";
   $reefTop2_low.rangeCenter = "0.75 0.64797035";
   //---
   $reefTop1_shadow.rangeCenter = "0.25 0.64797035";
   $reefTop2_shadow.rangeCenter = "0.75 0.64797035";
   //---
   %count = ClientGroup.getCount();
   echo("Viewid = " @ %viewId);
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
		
		echo("***********************************************");
		echo("Clientid = " @ %cl.clientId);
      %offset = getOffset(%viewId, %cl.clientId);
      %x0 = getWord(%offset,0);   
      %x1 = getWord(%offset,2);   
      %y0 = getWord(%offset,1);   
      %y1 = getWord(%offset,3);  
		echo("Range: x=" SPC %x0 SPC %x1 SPC "y=" SPC %y0 SPC %y1);
		
      scopeChunk(%cl, $reefTop1_low, %viewId);
      scopeChunk(%cl, $reefTop2_low, %viewId);
      scopeChunkInverse(%cl, $reefTop1_shadow, %viewId);
      scopeChunkInverse(%cl, $reefTop2_shadow, %viewId);
      
      scopeChunkInverse(%cl, $reefFront1_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront2_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront3_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront4_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront5_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront6_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront7_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront8_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront9_shadow, %viewId);
      scopeChunkInverse(%cl, $reefFront10_shadow, %viewId);
      
      %lowRes = (%viewId==0);
      
      if(%lowRes)
      {		
   		scopeChunk(%cl, $reefFront1_low, %viewId);
   		scopeChunk(%cl, $reefFront2_low, %viewId);
   		scopeChunk(%cl, $reefFront3_low, %viewId);
   		scopeChunk(%cl, $reefFront4_low, %viewId);
   		scopeChunk(%cl, $reefFront5_low, %viewId);
   		scopeChunk(%cl, $reefFront6_low, %viewId);
   		scopeChunk(%cl, $reefFront7_low, %viewId);
   		scopeChunk(%cl, $reefFront8_low, %viewId);
   		scopeChunk(%cl, $reefFront9_low, %viewId);
   		scopeChunk(%cl, $reefFront10_low, %viewId);
      }
      else
      {
   		scopeChunk(%cl, $reefFront1_high, %viewId);
   		scopeChunk(%cl, $reefFront2_high, %viewId);
   		scopeChunk(%cl, $reefFront3_high, %viewId);
   		scopeChunk(%cl, $reefFront4_high, %viewId);
   		scopeChunk(%cl, $reefFront5_high, %viewId);
   		scopeChunk(%cl, $reefFront6_high, %viewId);
   		scopeChunk(%cl, $reefFront7_high, %viewId);
   		scopeChunk(%cl, $reefFront8_high, %viewId);
   		scopeChunk(%cl, $reefFront9_high, %viewId);
   		scopeChunk(%cl, $reefFront10_high, %viewId);
      }
	}
}
function scopeWebData(%viewId)
{
   %count = ClientGroup.getCount();
   for (%i = 0; %i < %count; %i++)
	{
		%cl = ClientGroup.getObject(%i);
		for(%slot=0; %slot<20; %slot++)
		{
         %webdata = "AIFishInfoWebView" @ %slot;
         %xFraction = (1/20)*%slot + 0.025;
         %webdata.rangeCenter = %xFraction SPC "0.91122485";
        
         scopeChunk(%cl, %webdata, %viewId);
		}
	}
}



//**************************************************
// cinimatics sound mounts
// 
//**************************************************


function DiverSet1_SSData::onAdd(%data, %this)
{
   %data._onAdd(%this);
   
   %diver = new SFXEmitter() {
      track = DiverSet1DiverSnd;
      playOnAdd = false;
   };
   %boat = new SFXEmitter() {
      track = DiverSet1BoatSnd;
      playOnAdd = false;
   };
   
   
   %this.mountObject(%diver, 4);
   %this.mountObject(%boat, 6);
   %this.boatSnd = %boat;
   %this.diverSnd = %diver;
   
   // ha
   %boat.stop();
   %diver.stop();
}
function DiverSet1_SSData::playAnimation(%data, %this)
{
   %data._playAnimation(%this);
   
   // sound stuff
   %this.diverSnd.play();
   %this.boatSnd.play();
}



function WhaleSet1_SSData::onAdd(%data, %this)
{
   %data._onAdd(%this);
   
   %sound1 = new SFXEmitter() {
      track = WhaleDiveBreachesSnd;
      playOnAdd = false;
   };
   %sound2 = new SFXEmitter() {
      track = WhaleDiveCallSnd;
      playOnAdd = false;
   };

   
   %this.mountObject(%sound1, 0);
   %this.mountObject(%sound2, 1);
   %this.sound1 = %sound1;
   %this.sound2 = %sound2;
}
function WhaleSet1_SSData::playAnimation(%data, %this)
{
   // standard stuff
   %data._playAnimation(%this);
   
   // sound stuff
   %this.sound1.play();
   %this.sound2.play();
}

function WhaleSet2_SSData::onAdd(%data, %this)
{
   %data._onAdd(%this);
   
   %sound1 = new SFXEmitter() {
      track = WhaleCallsSnd;
      playOnAdd = false;
   };
   %sound3 = new SFXEmitter() {
      track = WhaleSwimSnd;
      playOnAdd = false;
   };
   
   
   %this.mountObject(%sound1, 0);
 //  %this.mountObject(%sound2, 1);
   %this.mountObject(%sound3, 2);
   %this.sound1 = %sound1;
 //  %this.sound2 = %sound2;
   %this.sound3 = %sound3;
}
function WhaleSet2_SSData::playAnimation(%data, %this)
{
   // standard stuff
   %data._playAnimation(%this);

   
   // sound stuff
   %this.sound1.play();
//   %this.sound2.play();
   %this.sound3.play();
}



function DugongSet1_SSData::onAdd(%data, %this)
{
   %data._onAdd(%this);
   
 //  %sound1 = new SFXEmitter() {
 //     track = DugongSet1SwimSnd;
 //     playOnAdd = false;
 //  };
   %sound2 = new SFXEmitter() {
      track = DugongSet1WaterSnd;
      playOnAdd = false;
   };
   
   
//   %this.mountObject(%sound1, 0);
   %this.mountObject(%sound2, 1);
//   %this.sound1 = %sound1;
   %this.sound2 = %sound2;
}
function DugongSet1_SSData::playAnimation(%data, %this)
{
   // standard stuff
   %data._playAnimation(%this);

   
   // sound stuff
 //  %this.sound1.play();
   %this.sound2.play();
}



