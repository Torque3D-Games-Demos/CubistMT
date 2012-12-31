


$WEBVIEW_MOUSEHACK = "0 1.1 0";
$DELAY_MINIMIZE = 100000;


$webViewMod = 1.06;
function Movable3DWidgetData::swizzleObjectDown(%data, %this, %touchId, %hitPos)
{
//   echo("Movable3DWidgetData::swizzleObjectDown:" SPC %touchId SPC %hitPos);

   if( %this.currentHitPos.getIndexFromKey(%touchId)>=0 ){
      return;
   }

   if(%this.currentHitPos.count() == 0)
   {
      %this.lastTouchCount = 0;
      %this.mLastTime = getSimTime();
      commandToServer('Maximize', %this.getGhostID());
   }
   %this.currentHitPos.push_back(%touchId, %hitPos);
   %this.lastHitPos.insert(%touchId, %hitPos, %this.currentHitPos.getIndexFromKey(%touchId));
   
//   %data.scheduleMinimize(%this);
}
function Movable3DWidgetData::swizzleObjectDragged(%data, %this, %touchId, %hitPos)
{
//   echo("Movable3DWidgetData::swizzleObjectDragged:" SPC %touchId SPC %hitPos);
   %fingersCount = %this.currentHitPos.count();
   if(%fingersCount <= 0)
      return;

   // update current touch location for this touch
   %touchIndex = %this.currentHitPos.getIndexFromKey(%touchId);
   if(%touchIndex == -1)       
      return;      
   %this.currentHitPos.setValue(%hitPos, %touchIndex);

   //
   if(%touchIndex != (%fingersCount-1)) 
      return;
      
   %time = getSimTime();
   %dt = (%time - %this.mLastTime);
   
   // not enough time has passed 
//   if(%dt < 1)
//      return;


   /////////////////////////////////////////////////////////////////////////////////
   // calculate motion

   // Compute bounding squares
   %boundsCurr = BoxCreateFromPoint(%this.currentHitPos.getValue(0));
   for(%i = 1; %i < %fingersCount; %i++) 
      %boundsCurr = BoxExtend(%boundsCurr, %this.currentHitPos.getValue(%i));
   %centerCurr = getBoxCenter(%boundsCurr);
   
   if(%this.lastTouchCount != %fingersCount)
   {
      %this.mLocation = "0 0 0";
      %this.mCenterPrev = %centerCurr;
      %centerPrev = %centerCurr;
      %this.lastTouchCount = %fingersCount;
   }
   else
   {
      %centerPrev = %this.mCenterPrev;
   }
   
   

   %dmult = 1.0;
   %rwsum = 0.0;
   %rsum = 0.0;
   %maxD = 1.0;
   %motion = "0 0 0";
   
   for(%i = 0; %i < %fingersCount; %i++) 
   {
      %curTip = %this.currentHitPos.getValue(%i);
      %preTip = %this.lastHitPos.getValue(%this.lastHitPos.getIndexFromKey(%this.currentHitPos.getKey(%i)));
      
      %diffCurr = VectorSub(%curTip, %centerCurr);
      %diffPrev = VectorSub(%preTip, %centerPrev);
      
      %angleCurr = mAtan(getWord(%diffCurr,0), getWord(%diffCurr,2));
      %anglePrev = mAtan(getWord(%diffPrev,0), getWord(%diffPrev,2));
      
      %r = (%anglePrev - %angleCurr);

      if(%r > mPI()) %r -= mPI() * 2;
      else if(%r < -mPI()) %r += mPI() * 2;
      
      %currLen = VectorLen(%diffCurr);
      %prevLen = VectorLen(%diffPrev);
      
      if(%currLen > 0 && %prevLen > 0) %dmult *= (%currLen / %prevLen);
      
      if(%currLen < %prevLen)
         %rwb = %currLen / %maxD;
      else
         %rwb = %prevLen / %maxD;
      
      %rsum += %r * %rwb;
      %rwsum += %rwb;
   }
   
   %scale = mPow(%dmult, 1.0 / %fingersCount);
   
   if(%rwsum > 0.000001) %rotate = %rsum / %rwsum;
   else %rotate = 0.0;
   
   %translate = VectorSub(%centerCurr, %centerPrev);
   // hack, prevent depth adjustments from manifesting as forward velocity 
//   %translate = setWord(%translate,1,0);

   
   /////////////////////////////////////////////////////////////////////////////////
   // apply motion

   if (%this.allowTranslation) {
      %translate = VectorSub(%translate, %this.mLocation);
   } else {
      %translate = "0 0 0";
   }

   %vDamp = 0.0;
   %sDamp = 0.4;
   //%rDamp = 0.4;
  // %sDamp = 0.0;
   %rDamp = 0.0;
   
   // Update velocity
//   %this.mVelocityAccumulator =
//      (VectorAdd(VectorScale(%this.mVelocityAccumulator,%vDamp), VectorScale(VectorDivF(%translate, %dt), -(1.0 - %vDamp))));
//   %this.mVelocity = %this.mVelocityAccumulator;
//   %this.mVelocity = VectorDivF(%translate, %dt);

   // Update rotation
   if(%this.allowRotation) {
      %maxRot = mPI() * 0.06;
      %rotate = mClamp(%rotate, -%maxRot, %maxRot);

      %this.mRotationAccumulator = (%this.mRotationAccumulator * %rDamp) + %rotate;
      %rotate = (1.0 - %rDamp) * %this.mRotationAccumulator;
      %this.mRotation += %rotate;
      
      %this.mRotation = mFMod(%this.mRotation, mPI()*2);
   }
   else
   {
      %this.mRotationAccumulator = 0;
      %this.mRotation = 0;
   }

   // Update angular rate
   %this.mOmega = (%rotate / %dt);
   %maxOmega = mPI() / 60;	
   %this.mOmega = mClamp(%this.mOmega, -%maxOmega, %maxOmega);

   // Update scaling
   if (%this.allowScale) {
      //%scale = mClamp(%scale, 0.7, 1.3);
      %scale = mClamp(%scale, 0.3, 1.8);
   //   %scale = mPow(%scale, 0.7);
   
      %this.mScaleAccumulator = (%this.mScaleAccumulator * %sDamp) + (1.0 - %sDamp) * %scale;
      %scale = %this.mScaleAccumulator;
 
      %this.mScale = %scale;
   }
   else
   {
      %this.mScaleAccumulator = 1.0;
      %scale = 1.0;
   }   
   // repair mLocation after generating velocity
   %this.mLocation = VectorAdd(%this.mLocation, %translate);
   
   
   // get the velocity vector to move to the new location
 //  %velocity = VectorSub(%newPosition, %this.position);
   %velocity = %this.mLocation;
   
   // time adjust mass factor for moving even the heaviest objects
   %massFactor = (%this.getDataBlock().mass*(%dt/100));
   //%massFactor = (%this.getDataBlock().mass);
      
   // apply mass and time factor to the velocity so the object move the
   // right amount for its mass.
   %velocity = VectorScale(%velocity, %massFactor);
   
   // clamp the velocity so we don't have physics explosions
   if(VectorLen(%velocity) > %massFactor*500.0)
      %velocity = VectorScale(VectorNormalize(%velocity), %massFactor*500.0);
      
   %this.mVelocity = %velocity;

   
   // move the amount the center of the touch events has moved
//   %newPosition = VectorAdd(%this.position, %this.mLocation);
   %newPosition = VectorAdd(%this.position, %this.mLocation);
   //%offset = VectorSub(%this.position, %centerCurr);
   
   if(%this.allowRotation) {
  // if(false) {

      // translate the position into local pivot space
      %newPosition = VectorSub(%newPosition, %centerCurr);
      
      // rotate and scale the position around the pivot point
      %vectorLen = VectorLen(%newPosition) * %scale;
      %newPosition = VectorNormalize(%newPosition);
      %vecX = getWord(%newPosition,0);
      %vecY = getWord(%newPosition,2);
      %sinRot = mSin(%rotate);
      %cosRot = mCos(%rotate);
      %newX = %cosRot * %vecX - %sinRot * %vecY;
      %newY = %sinRot * %vecX + %cosRot * %vecY;
      %newPosition = %newX SPC getWord(%newPosition,1) SPC %newY;
    //  %newPosition = VectorNormalize(%newPosition);
      
      %newPosition = VectorScale(%newPosition, %vectorLen);

      
      // translate the position back into world space
      %newPosition = VectorAdd(%newPosition, %centerCurr);
      
   }
   else %rotate = 0.0;
   // translate to object space
   %newPosition = VectorSub(%newPosition, %this.position);
   
   // lock the z axis
   //%newPosition = setWord(%newPosition,1,0);  

   %this.mCenterPrev = %centerCurr;   
   
   // update last touch positions
   for(%i = 0; %i < %fingersCount; %i++) 
      %this.lastHitPos.setValue(%this.currentHitPos.getValue(%i), %this.lastHitPos.getIndexFromKey(%this.currentHitPos.getKey(%i)));
   
   %this.mLastTime = %time;
   
   
 //  %needScale = mAbs(%scale-1.0) > 0.001;
 //  %needRotation = mAbs(%rotate) > 0.001;
   %needScale = %scale != 1.0;
   %needRotation = %rotate != 0.0;
 //  %needTranslate = VectorLen(%newPosition) > 0.01;
   if(!%needScale && !%needRotation)
   {
      commandToServer('SetShapePosition', %this.getGhostID(), %newPosition);
      //SceneObjectTransformEvent::translate(ServerConnection, %this, %newPosition SPC "0 0 1 0", "1 1 1", true);
      //SceneObjectTransformEvent::translate(ServerConnection, %this, VectorAdd(%this.position, %newPosition) SPC "0 1 0" SPC %this.mRotation, %this.scale, false);
//      ShapeBaseTransformEvent::translate(ServerConnection, %this, %newPosition, "1 1 1", %this.mVelocity, true);
//      ShapeBaseTransformEvent::translate(ServerConnection, %this, VectorAdd(%this.position, %newPosition) SPC "0 1 0" SPC %this.mRotation, %this.scale, %this.mVelocity, false);
//      echo(%this.position SPC "+" SPC %newPosition SPC "=" SPC VectorAdd(%this.position, %newPosition));
   }
//   else if((%needScale || %needRotation))
   else
   {
      commandToServer('SetShapeTransform', %this.getGhostID(), %newPosition, %this.mRotation, %this.mScale);   
      //SceneObjectTransformEvent::translate(ServerConnection, %this, %newPosition SPC "0 1 0" SPC %rotate, %scale SPC %scale SPC %scale, false);
      //SceneObjectTransformEvent::translate(ServerConnection, %this, VectorAdd(%this.position, %newPosition) SPC "0 1 0" SPC %this.mRotation, VectorScale(%this.scale , %scale), false);
//      ShapeBaseTransformEvent::translate(ServerConnection, %this, %newPosition SPC "0 1 0" SPC %rotate, %scale SPC %scale SPC %scale, %this.mVelocity, true);
//      ShapeBaseTransformEvent::translate(ServerConnection, %this, VectorAdd(%this.position, %newPosition) SPC "0 1 0" SPC %this.mRotation, VectorScale(%this.scale, %scale), %this.mVelocity, false);
   }

//   %data.scheduleMinimize(%this);
   

}
function Movable3DWidgetData::swizzleObjectUp(%data, %this, %touchId)
{
//   echo("Movable3DWidgetData::swizzleObjectUp:" SPC %touchId SPC %hitPos);
   %index = %this.currentHitPos.getIndexFromKey(%touchId);
   if(%index == -1)
      return;
      
   %this.currentHitPos.erase(%index);
   %this.lastHitPos.erase(%this.lastHitPos.getIndexFromKey(%touchId));   
   
   
   // when last touch lifted apply physics
   if(%this.currentHitPos.count() == 0)
   {
    //  %time = getSimTime();
    //  %dt = (%time - %this.mLastTime);
   //   commandToServer('PositionImpulse', %this.getGhostID(), VectorSub(%this.mCenterPrev, %this.position));
   }
   
}

function Movable3DWidgetData::localToGlobal(%data, %this, %p)
{
   %m = %this.getTransform();
   %r = MatrixMulVector(%m, %p);
   %r = VectorMul(%r, %this.scale);
   %r = VectorAdd(%r, %this.position);
   return %r;
}
function Movable3DWidgetData::globalToLocal(%data, %this, %p)
{

   %m = %this.getWorldTransform();
   %r = %p;
 //  %r = VectorSub(%p, %this.position);
  // %r = VectorDiv(%r, VectorScale(%this.scale,0.96));
  // %r = MatrixMulVector(%m, %r);
  
   %r = MatrixCreate(%r, "0 0 1 0");
   %r = MatrixMultiply(%r, %m);
   %r = getWords(%r, 0, 2);

   %r = VectorDiv(%r, %this.scale);
   %r = VectorScale(%r, $webViewMod);
 //  %r = VectorAdd(%r, "0.01 0.01 0.01");
   
   

//   %pm = MatrixCreate(%p);
//   %localm = MatrixMultiply(%pm, %this.getWorldTransform());
//   %localm = MatrixDisplace(

   return %r;
}
function Movable3DWidgetData::mapToParent(%data, %this, %p)
{
//   %m2 = %this.getTransform();
//   %m = MatrixMultiply(%m2, MatrixCreate("0 0 0", "0 1 0" SPC %this.mRotation));
   %m = MatrixCreate("0 0 0", "0 1 0" SPC %this.mRotation);
//   %m = MatrixCreate(%this.mLocation, "0 1 0" SPC %this.mRotation);
   //m.rotate(m_rotation);
   //m.scale(m_scale, m_scale);
   //m.translate(m_location.getX(), m_location.getY());
   
   //%r = "0 0 0";
//   %r = VectorScale(MatrixMulPoint(%m, %p), %this.mScale);
//   %r = MatrixMulVector(%m, %p);
 //  %r = MatrixMulPoint(%m, %p);
 
 
//   %r = VectorScale(MatrixMulVector(%m, %p), %this.mScale);
//   %r = VectorAdd(%r, %this.mLocation);	
   
   %r = MatrixMulVector(%m, %p);
//   %r = VectorMul(%r, VectorScale(%this.nativeScale, %this.mScale));
//   %r = VectorMul(%r, VectorScale(%this.scale, %this.mScale));
   %r = VectorScale(%r, %this.mScale);
   %r = VectorAdd(%r, %this.mLocation);
//   %r = VectorAdd(%r, %this.position);
   return %r;
}


function Movable3DWidgetData::scheduleMinimize(%data, %this)
{
   if(%this.timedMinId)
   {
      cancel(%this.timedMinId);         
      %this.timedMinId = 0;
   }
//   %this.timedMinId = %this.schedule($DELAY_MINIMIZE, "minimize");
   %this.timedMinId = schedule($DELAY_MINIMIZE, 0, "commandToServer",'Minimize', %this.getGhostID());
}



function Movable3DWidgetData::onMaterialsLoaded(%data, %this)
{
   echo("Movable3DWidgetData::onMaterialsLoaded:" SPC %this.getTextureResolution(1) );
//   echo("Movable3DWidget::OnMaterialsLoaded" );

   if(!isObject(%this.webViewData))
      commandToServer('scaleObjectNative', %this.getGhostID(), %this.getTextureResolution(1));

}
/*
function Movable3DWidgetWebData::onGetScrollData(%data, %this, %contentWidth, %contentHeight, %preferredWidth, %scrollX, %scrollY)
{
   echo("Movable3DWidgetWebData::onGetScrollData:" SPC %contentWidth SPC %contentHeight );
   
   %nativeRes = %contentWidth SPC %contentHeight;
      
   if(%this.getResolution() !$= %nativeRes)
   {
      commandToServer('scaleWebObjectNative', %this.getGhostID(), %nativeRes);
   }
}
function Movable3DWidgetWebData::onFinishLoading(%this)
{  
   %this.createObject("T3D");
   %this.setObjectCallback("T3D", "timeOut", %this.getId() @ ".timeOut", "");
   %this.setObjectCallback("T3D", "closeWindow", %this.getId() @ ".closeWindow", "");
   %this.setObjectCallback("T3D", "openBubble", %this.getId() @ ".openBubble", "");
   %this.setObjectCallback("T3D", "showFishPage", %this.getId() @ ".showFishPage", "");
   %this.setObjectCallback("T3D", "log", %this.getId() @ ".log", "");

//   %this.setObjectCallback("T3D", "mouseEventReturn", %this.getId() @ ".mouseEventReturn");
}
//$clientCloseCount = 0;
function Movable3DWidgetWebData::closeWindow(%this, %string)
{
//   echo("javascript closeWindow reason " @ %string);
   $clientCloseCount = $clientCloseCount + 1;
   echo("webpage close request (client)" @ $clientCloseCount);
   commandToServer('Movable3DWidgetDataClose', %this.getGhostID()); 
}
function Movable3DWidgetWebData::timeOut(%this, %string)
{
   $clientCloseCount = $clientCloseCount + 1;
   echo("webpage close request (client)" @ $clientCloseCount);
   commandToServer('Movable3DWidgetDataTimeOut', %this.getGhostID()); 
}
function Movable3DWidgetWebData::log(%this, %string)
{
   echo("jlog: " @ %string);
}
function Movable3DWidgetWebData::openBubble(%this, %string)
{
   echo("Movable3DWidgetWebData::openBubble" SPC %string);
   commandToServer('LaunchBubble', %this.getGhostID(), %string ); 
}
function Movable3DWidgetWebData::showFishPage(%this, %string)
{
   echo("Movable3DWidgetWebData::showFishPage" SPC %string);
   commandToServer('showFishPage', %this.getGhostID(), %string ); 
}

function Movable3DWidgetWebData::mouseEventReturn(%this, %processed)
{
   if(!%processed)
   {
      if(!%this.movePage)
         %this.movePage = 1;
   }
}
*/
function Movable3DWidgetData::onAddClient(%data, %this)
{
   echo("onAddClient " @ %this @ " " @ %this.getClassName());
   
   %this.mLocation = "0 0 0";
   %this.mRotation = 0;
   %this.mScale = 1;

   %this.mVelocityAccumulator = "0 0 0";
	%this.mRotationAccumulator = 0;
	%this.mScaleAccumulator = 1;
	%this.momentum = 0.9;

// todo: remove this hack by allowing dynamic variables to snyc to client
   %this.allowRotation = "1";
   %this.allowScale = "1";
   %this.allowTranslation = "1";
   //%this.mStartPos = "0 0 0";
   %this.currentHitPos = new ArrayObject();
   %this.lastHitPos = new ArrayObject();
   %this.touchPoints = new ArrayObject();
   
   %this.touchCount = 0;
   %this.lastTouchCount = 0;
   %this.touchMode = 0;
   %this.lastTouchMode = 0;
   
   %this.mCenterPrev = "0 0 0";
   
//   commandToServer('InstantMinimize', %this.getGhostID());
   
}
//$mod = 0.1;
function Movable3DWidgetData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);

//   if( isObject(%this.webViewData) ){
//      commandToServer('WebPageActive', %this.getGhostID()); 
//   }
//   commandToServer('PositionObjectDown', %this, %hitPos); 
   if(isObject(%this.webViewData) && !%this.webViewData.movePage)
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
      //   %webPoint = ((%u*2)-1.0) SPC (1.0-(%v*2));
         %webPoint = (%u-0.5) SPC (0.5-%v);
         //%webPoint = (0.5-%u) SPC (%v-0.5);
      }
      else
      {
         // simple hit point on objects plane
         %upVector = "0 1 0"; // %this.getUpVector();
         %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }
      %this.webViewData.injectMouseMove(%webPoint);
      %this.webViewData.injectMouseDown(0);
//      commandToServer('Maximize', %this.getGhostID());
   //   %data.scheduleMinimize(%this);
   }
   else
   {
      // simple hit point on objects plane
  //    %upVector = "0 1 0"; // %this.getUpVector();
  //    %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
 //     %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
      
      //commandToServer('SwizzleObjectDown', %this.getGhostID(), 1, %hitPos);
      %data.swizzleObjectDown(%this, 10000, %hitPos);   
   //   echo("Movable3DWidgetData::OnMouseDown" SPC %hitPos);
   }
}
function Movable3DWidgetData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   if( isObject(%this.webViewData) ){
//      commandToServer('WebPageActive', %this.getGhostID()); 
//   }
 //  commandToServer('PositionObjectDragged', %this, %hitPos);  
   if(isObject(%this.webViewData) && !%this.webViewData.movePage)
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
         //%webPoint = (%u-0.5) SPC ((-%v)+0.5);
         %webPoint = (%u-0.5) SPC (0.5-%v);
      }
      else
      {
         // simple hit point on objects plane
      //   %upVector = "0 1 0"; // %this.getUpVector();
      //   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
      //   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }
      %this.webViewData.injectMouseMove(%webPoint);
   //   %data.scheduleMinimize(%this);
   }
   else
   {
      // simple hit point on objects plane
//      %upVector =  "0 1 0"; // %this.getUpVector();
//      %t = -(VectorDot(VectorSub(getCameraP(), %this.position), %upVector) / VectorDot(%vec, %upVector));
//      %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());


   //   commandToServer('SwizzleObjectDragged', %this.getGhostID(), 1, %hitPos); 
      if(isObject(%this.webViewData) && %this.webViewData.movePage == 1)
      {
         %data.swizzleObjectDown(%this, 10000, %hitPos); 
         %this.webViewData.movePage = 2;             
      }
      %data.swizzleObjectDragged(%this, 10000, %hitPos);              
   //   echo("Movable3DWidgetData::OnMouseDragged" SPC %hitPos);
   }
}
function Movable3DWidgetData::OnMouseMove(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   if( isObject(%this.webViewData) ){
//      commandToServer('WebPageActive', %this.getGhostID()); 
//   }
   if(%this.webViewData)
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
         //%webPoint = (0.5-%u) SPC (%v-0.5);
         %webPoint = (%u-0.5) SPC (0.5-%v);
      //   %webPoint = %u SPC %v;
      }
      else
      {
         // simple hit point on objects plane
      //   %upVector = "0 1 0"; // %this.getUpVector();
      //   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
      //   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }

   //   %localPos = VectorScale(%data.globalToLocal(%this, %hitPos), $WEBVIEW_MOUSEHACK);
      %this.webViewData.injectMouseMove(%webPoint);
   //   %data.scheduleMinimize(%this);
   }
}

function Movable3DWidgetData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   
   if(isObject(%this.webViewData) && !%this.webViewData.movePage)  
      %this.webViewData.injectMouseUp(0);
   else
   {
   //   commandToServer('SwizzleObjectUp', %this.getGhostID(), 1);  
      %data.swizzleObjectUp(%this, 10000); 
      if(isObject(%this.webViewData))
      {
         %this.webViewData.movePage = 0;
         %this.webViewData.injectMouseUp(0);
      }
   //   echo("Movable3DWidgetData::OnMouseUp" SPC %hitPos);
   }
}

function Movable3DWidgetData::OnMiddleMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);

   // simple hit point on objects plane
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

//   commandToServer('PositionObjectDown', %this, %hitPos); 
//   commandToServer('SwizzleObjectDown', %this.getGhostID(), 1, %hitPos); 
   %data.swizzleObjectDown(%this, 1, %hitPos);                
}
function Movable3DWidgetData::OnMiddleMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   // simple hit point on objects plane
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

 //  commandToServer('PositionObjectDragged', %this, %hitPos);  
//   commandToServer('SwizzleObjectDragged', %this.getGhostID(), 1, %hitPos);            
   %data.swizzleObjectDragged(%this, 1, %hitPos);              

}
function Movable3DWidgetData::OnMiddleMouseUp(%this, %obj, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   //commandToServer('SwizzleObjectUp', %this.getGhostID(), 1);  
   %data.swizzleObjectUp(%this, 1);              
}
/*
function Movable3DWidget::OnRightMouseDown(%this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   // simple hit point on objects plane
   %t = -(VectorDot(VectorSub(%pos, %this.position), %this.getEyeVector()) / VectorDot(%vec, %this.getEyeVector()));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   commandToServer('ScaleObjectDown', %this, %hitPos);   
   
}
function Movable3DWidget::OnRightMouseDragged(%this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
// killed by bad skin code in torque
//   %this.sizeWidth += -getWord(%offset,1);
//   %this.sizeHeight += -getWord(%offset,0);
//   %this.setThreadPosition(1, %this.sizeWidth);
//   %this.setThreadPosition(2, %this.sizeHeight);

     // simple hit point on objects plane
   %t = -(VectorDot(VectorSub(%pos, %this.position), %this.getEyeVector()) / VectorDot(%vec, %this.getEyeVector()));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   
    commandToServer('ScaleObjectDragged', %this, %hitPos);   
}
*/
function Movable3DWidgetData::OnRightMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   // simple hit point on objects plane
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   
   %this.fakeTouchPos = %this.position;
   %data.swizzleObjectDown(%this, 10002, %this.fakeTouchPos);   
   %data.swizzleObjectDown(%this, 10003, %hitPos);
/*
   %data.swizzleObjectDown(%this, 10000, %hitPos);        
*/
}
function Movable3DWidgetData::OnRightMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %upVector = "0 1 0"; // %this.getUpVector();
   %cam = ServerConnection.getCameraObject();
   %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
      
   %data.swizzleObjectDragged(%this, 10002, %this.fakeTouchPos);   
   %data.swizzleObjectDragged(%this, 10003, %hitPos);  
/*
   %data.swizzleObjectDragged(%this, 10000, %hitPos);   
*/            
}
function Movable3DWidgetData::OnRightMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %data.swizzleObjectUp(%this, 10002);   
   %data.swizzleObjectUp(%this, 10003); 
/*
   %data.swizzleObjectUp(%this, 10000);   
*/
   TouchGui.mouseUnlockObject(%this);
}
function Movable3DWidgetData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo("+Touch");
//   if( isObject(%this.webViewData) ){
//      commandToServer('WebPageActive', %this.getGhostID()); 
//   }
   // lock the touchId so we get the moves and up regardless
   TouchGui.touchIDLockObject(%touchId, %this);

   // create a touch array if we don't have one
   if(!isObject(%this.touchPoints))
      %this.touchPoints = new ArrayObject(){};
   
   // adjust UV for web page hit point
   %u = getWord(%touchUVCoord,0);
   %v = getWord(%touchUVCoord,1);
   %webPoint = (%u-0.5) SPC (0.5-%v);
   
   // create an entry for this touchId
   %this.touchPoints.add(%touchId, %webPoint);

   // is this a web page
   if(isObject(%this.webViewData))
   {
      
      // web page wants us to move
      if(%this.webViewData.movePage)
      {
         // simple hit point on objects plane
      //   %upVector = "0 1 0"; // %this.getUpVector();
      //   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
      //   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
         
         // page has told us to move so add all existing touches to the swizzle
         if(%this.webViewData.movePage == 1)
         {
            // not a new move anymore
            %this.webViewData.movePage = 2;
                        
            // cleanup page mouse state
            //%this.webViewData.injectMouseUp(0);

            // add a down event for each missed touch
            for(%i = 0; %i < %this.touchPoints.count(); %i++)
               %data.swizzleObjectDown(%this, %this.touchPoints.getKey(%i), %hitPos); 
         }
         else
         {
            // new touch since we were told to move
            %data.swizzleObjectDown(%this, %touchId, %hitPos);  
         }
      }
      else 
      {
         if(!%this.webViewData.enableMultiTouch)
         {
            // page has told us nothing so use as mouse
            if(%this.touchPoints.count() == 1)  
            {
            //   %this.webViewData.injectMouseMove(%webPoint);
               
               // if we want to drag webviews we need to know from
               // the page as soon as possible.  This forces a responce
               // from the page early.
            //   %this.webViewData.injectMouseDown(0);
            //   %this.webViewData.injectMouseUp(0);
            
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseDown(0);

            }
         }
         else
         {
            %this.webViewData.injectTouchDown(%this.touchPoints.count()-1, %webPoint);
         }
      }
   }
   else
   {
      // not web widget
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

      %data.swizzleObjectDown(%this, %touchId, %hitPos);  
   }

   return true;
}
function Movable3DWidgetData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo(">Touch");
//   if( isObject(%this.webViewData) ){
//      commandToServer('WebPageActive', %this.getGhostID()); 
//   }
   // is it a web page
   if(isObject(%this.webViewData))
   {
      // web page wants us to move
      if(%this.webViewData.movePage)
      {
         // simple hit point on objects plane
      //   %upVector = "0 1 0"; // %this.getUpVector();
      //   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
      //   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

         // on move start
         if(%this.webViewData.movePage == 1)
         {
            // not a new move anymore
            %this.webViewData.movePage = 2; 
                        
            // cleanup page mouse state
            //%this.webViewData.injectMouseUp(0);
               
            // add a down event for each missed touch
            for(%i = 0; %i < %this.touchPoints.count(); %i++)
               %data.swizzleObjectDown(%this, %this.touchPoints.getKey(%i), %hitPos); 
         }
         
         // send current touch to swizzle
         %data.swizzleObjectDragged(%this, %touchId, %hitPos);              
      }
      else 
      {
         if(!%this.webViewData.enableMultiTouch)
         {

            // use the first touch as a mouse in the page
            if(%this.touchPoints.getKey(0) == %touchId)
            {
               // adjust UV for web page hit point
               %u = getWord(%touchUVCoord,0);
               %v = getWord(%touchUVCoord,1);
               %webPoint = (%u-0.5) SPC (0.5-%v);
            
//               echo("webpoint = " @ %webPoint);
               %this.webViewData.injectMouseMove(%webPoint);
            }
         }
         else     
            %this.webViewData.injectTouchMove(%this.touchPoints.getIndexFromKey(%touchId), %webPoint);
      }
   }
   else
   {
      // not web widget
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

      %data.swizzleObjectDragged(%this, %touchId, %hitPos);  
   }
      
   return true;
}
function Movable3DWidgetData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo("-Touch");
   // is it a web page
   if(isObject(%this.webViewData))
   {
      // web page wants us to move
      if(%this.webViewData.movePage)
      {
         // simple hit point on objects plane
      //   %upVector = "0 1 0"; // %this.getUpVector();
      //   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
      //   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

         // on move start
         if(%this.webViewData.movePage == 1)
         {
            // not a new move anymore
            %this.webViewData.movePage = 2;  
            
            // cleanup page mouse state
            //%this.webViewData.injectMouseUp(0);           
               
            // add a down event for each missed touch
            for(%i = 0; %i < %this.touchPoints.count(); %i++)
               %data.swizzleObjectDown(%this, %this.touchPoints.getKey(%i), %hitPos); 
         }
         
         // send up for the current event
         %data.swizzleObjectUp(%this, %touchId, %hitPos);
         
         // reset for new touches
         if(%this.touchPoints.count() == 1)
            %this.webViewData.movePage = 0;  
      }  
      
      // adjust UV for web page hit point
      %u = getWord(%touchUVCoord,0);
      %v = getWord(%touchUVCoord,1);
      %webPoint = (%u-0.5) SPC (0.5-%v);
            
      if(!%this.webViewData.enableMultiTouch)
      {
         if(%touchId == %this.touchPoints.getKey(0))
         {
            %this.webViewData.injectMouseMove(%webPoint);
            %this.webViewData.injectMouseUp(0);
         }
         else
         {
            // detect clicks and send them to the page
            %index = %this.touchPoints.getIndexFromKey(%touchId);
            %lastPoint = %this.touchPoints.getValue(%index);
            %lastX = getWord(%lastPoint,0);
            %lastY = getWord(%lastPoint,1);
            %currX = getWord(%webPoint,0);
            %currY = getWord(%webPoint,1);
            if(mAbs(%lastX-%currX) <= 0.1 && mAbs(%lastY-%currY) <= 0.1)
            {
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseDown(0);
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseUp(0);
            }
         }
      }
      else 
         %this.webViewData.injectTouchUp(%this.touchPoints.getIndexFromKey(%touchId), %webPoint);

   }
   else
   {
      // not web widget
      %upVector = "0 1 0"; // %this.getUpVector();
      %cam = ServerConnection.getCameraObject();
      %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
      %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

      %data.swizzleObjectUp(%this, %touchId, %hitPos);  
   }
   
   // clean touchId out of the touch array
   %this.touchPoints.erase(%index);
   
   // unlock this touchId so others can use it
   TouchGui.touchIDUnlockObject(%touchId, %this);
    
   return true;
}
function Movable3DWidgetData::onRemoveClient(%data, %this)
{
//   echo("onRemove " @ %this @ " " @ %this.getClassName());
   // cleanup touch locks before we are deleted
   for(%i = 0; %i < %this.touchPoints.count(); %i++)
      TouchGui.touchIDUnlockObject(%this.touchPoints.getKey(%i), %this);

   // delete the array of touch points
   %this.touchPoints.delete();
   %this.touchPoints = 0;
}
function Movable3DWidgetData::onEndSequence(%data, %this, %slot)
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////

function Static3DWidgetData::onAddClient(%data, %this)
{
//   echo("onAddClient " @ %this @ " " @ %this.getClassName());
   
   %this.touchPoints = new ArrayObject();
}
function Static3DWidgetData::onRemoveClient(%data, %this)
{
//   echo("onRemove " @ %this @ " " @ %this.getClassName());

   // cleanup touch locks before we are deleted
   for(%i = 0; %i < %this.touchPoints.count(); %i++)
      TouchGui.touchIDUnlockObject(%this.touchPoints.getKey(%i), %this);

   // delete the array of touch points
   %this.touchPoints.delete();
   %this.touchPoints = 0;
}

function Static3DWidgetData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);

   if(isObject(%this.webViewData))
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
         %webPoint = (%u-0.5) SPC (0.5-%v);
      }
      else
      {
         // simple hit point on objects plane
         %upVector = "0 1 0"; // %this.getUpVector();
         %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }
      %this.webViewData.injectMouseMove(%webPoint);
      %this.webViewData.injectMouseDown(0);
   }
}
function Static3DWidgetData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   if(isObject(%this.webViewData))
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
         %webPoint = (%u-0.5) SPC (0.5-%v);
      }
      else
      {
         // simple hit point on objects plane
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }
      %this.webViewData.injectMouseMove(%webPoint);
   }
}
function Static3DWidgetData::OnMouseMove(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   if(%this.webViewData)
   {
      if(%mouseUVCoord !$= "-1 -1")
      {
         %u = getWord(%mouseUVCoord,0);
         %v = getWord(%mouseUVCoord,1);
         %webPoint = (%u-0.5) SPC (0.5-%v);
      }
      else
      {
         %upVector = "0 1 0"; // %this.getUpVector();
         %cam = ServerConnection.getCameraObject();
         %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
         %localPos = %data.globalToLocal(%this, %hitPos);
         %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
      }
      %this.webViewData.injectMouseMove(%webPoint);
   }
}

function Static3DWidgetData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   
   if(isObject(%this.webViewData))  
      %this.webViewData.injectMouseUp(0);
}

function Static3DWidgetData::OnMiddleMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);

   // simple hit point on objects plane
//   %upVector = "0 1 0"; // %this.getUpVector();
//   %cam = ServerConnection.getCameraObject();
//   %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
//   %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

//   %data.swizzleObjectDown(%this, 1, %hitPos);                
}
function Static3DWidgetData::OnMiddleMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   // simple hit point on objects plane
//   %upVector = "0 1 0"; // %this.getUpVector();
//   %cam = ServerConnection.getCameraObject();
//   %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
//   %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());

//   %data.swizzleObjectDragged(%this, 1, %hitPos);              

}
function Static3DWidgetData::OnMiddleMouseUp(%this, %obj, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   
//   %data.swizzleObjectUp(%this, 1);              
}
function Static3DWidgetData::OnRightMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   // simple hit point on objects plane
//   %upVector = "0 1 0"; // %this.getUpVector();
//   %cam = ServerConnection.getCameraObject();
//   %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
//   %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
   
//   %data.swizzleObjectDown(%this, 10000, %hitPos);        
}
function Static3DWidgetData::OnRightMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{           
//   %upVector = "0 1 0"; // %this.getUpVector();
//   %cam = ServerConnection.getCameraObject();
//   %t = -(VectorDot(VectorSub(%cam.getPosition(), %this.position), %upVector) / VectorDot(%vec, %upVector));
//   %hitPos = VectorAdd(VectorScale(%vec, %t), %cam.getPosition());
//   %data.swizzleObjectDragged(%this, 10000, %hitPos);   
}
function Static3DWidgetData::OnRightMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   %data.swizzleObjectUp(%this, 10000);   
   TouchGui.mouseUnlockObject(%this);
}
function Static3DWidgetData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo("+Touch");
   // lock the touchId so we get the moves and up regardless
   TouchGui.touchIDLockObject(%touchId, %this);

   // create a touch array if we don't have one
   if(!isObject(%this.touchPoints))
      %this.touchPoints = new ArrayObject(){};
   
   // adjust UV for web page hit point
   %u = getWord(%touchUVCoord,0);
   %v = getWord(%touchUVCoord,1);
   %webPoint = (%u-0.5) SPC (0.5-%v);
   
   // create an entry for this touchId
   %this.touchPoints.add(%touchId, %webPoint);

   // is this a web page
   if(isObject(%this.webViewData))
   {
      if(!%this.webViewData.enableMultiTouch)
      {
         // page has told us nothing so use as mouse
         if(%this.touchPoints.count() == 1)  
         {
            %this.webViewData.injectMouseMove(%webPoint);
            %this.webViewData.injectMouseDown(0);
         }
      }
      else
      {
         %this.webViewData.injectTouchDown(%this.touchPoints.count()-1, %webPoint);
      }
   }

   return true;
}
function Static3DWidgetData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo(">Touch");
   // is it a web page
   if(isObject(%this.webViewData))
   {
      if(!%this.webViewData.enableMultiTouch)
      {
         // use the first touch as a mouse in the page
         if(%this.touchPoints.getKey(0) == %touchId)
         {
            // adjust UV for web page hit point
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
         
//               echo("webpoint = " @ %webPoint);
            %this.webViewData.injectMouseMove(%webPoint);
         }
      }
      else     
         %this.webViewData.injectTouchMove(%this.touchPoints.getIndexFromKey(%touchId), %webPoint);

   }
      
   return true;
}
function Static3DWidgetData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo("-Touch");
   // is it a web page
   if(isObject(%this.webViewData))
   {
      // adjust UV for web page hit point
      %u = getWord(%touchUVCoord,0);
      %v = getWord(%touchUVCoord,1);
      %webPoint = (%u-0.5) SPC (0.5-%v);
            
      if(!%this.webViewData.enableMultiTouch)
      {
         if(%touchId == %this.touchPoints.getKey(0))
         {
            %this.webViewData.injectMouseMove(%webPoint);
            %this.webViewData.injectMouseUp(0);
         }
         else
         {
            // detect clicks and send them to the page
            %index = %this.touchPoints.getIndexFromKey(%touchId);
            %lastPoint = %this.touchPoints.getValue(%index);
            %lastX = getWord(%lastPoint,0);
            %lastY = getWord(%lastPoint,1);
            %currX = getWord(%webPoint,0);
            %currY = getWord(%webPoint,1);
            if(mAbs(%lastX-%currX) <= 0.1 && mAbs(%lastY-%currY) <= 0.1)
            {
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseDown(0);
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseUp(0);
            }
         }
      }
      else 
         %this.webViewData.injectTouchUp(%this.touchPoints.getIndexFromKey(%touchId), %webPoint);
   }
   // clean touchId out of the touch array
   %this.touchPoints.erase(%index);

   // unlock this touchId so others can use it
   TouchGui.touchIDUnlockObject(%touchId, %this);
    
   return true;
}

////////////////////////////////////////////////////////////////////////////////////////////


/*
function Movable3DWidgetData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
      TouchGui.touchIDLockObject(%touchId, %this);
      
      if(%this.touchPoints.count() == 0 && %this.webViewData)
      {
         %this.lastTouchMode = %this.touchMode;
         %this.touchMode = 1;
      //   echo("Movable3DWidgetData::OnTouchDown touchMode =" SPC %this.touchMode);
         //   %this.firstTouchId = %touchId;
         //   %this.firstTouchPos = %hitPos;
         //   %localPos = VectorScale(%data.globalToLocal(%this, %hitPos), $WEBVIEW_MOUSEHACK);
         if(%touchUVCoord !$= "-1 -1")
         {
            %this.touchPoints.push_back(%touchId, %touchPoint);
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
            //%webPoint = %u SPC %v;
         
         }
         else
         {
            // simple hit point on objects plane
            %upVector = "0 1 0"; // %this.getUpVector();
            %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
            %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
            %this.touchPoints.push_back(%touchId, %hitPos);
            
            %localPos = %data.globalToLocal(%this, %hitPos);
            %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
         }

         %this.webViewData.lastMouseMove = %webPoint;
         %this.webViewData.injectMouseMove(%webPoint);
         commandToServer('Maximize', %this.getGhostID());
         %data.scheduleMinimize(%this);
      }
      else if(%this.touchPoints.count() == 1 && %this.webViewData)
      {
         %this.lastTouchMode = %this.touchMode;
         %this.touchMode = 2;
      //   echo("Movable3DWidgetData::OnTouchDown touchMode =" SPC %this.touchMode);
         %this.secondTouchId = %touchId;
      //   %this.secondTouchPos = %hitPos;
         
      //   %localPos = VectorScale(%data.globalToLocal(%this, %hitPos), $WEBVIEW_MOUSEHACK);
         if(%touchUVCoord !$= "-1 -1")
         {
            %this.touchPoints.push_back(%touchId, %touchPoint);
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
            //%webPoint = %u SPC %v;
         }
         else
         {
            // simple hit point on objects plane
            %upVector = "0 1 0"; // %this.getUpVector();
            %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
            %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
            %this.touchPoints.push_back(%touchId, %hitPos);

            %localPos = %data.globalToLocal(%this, %hitPos);
            %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
         }

         %this.webViewData.lastMouseMove = %webPoint;
         %this.webViewData.injectMouseMove(%webPoint);
         if(%this.lastTouchMode == 1)
         {
       //     %this.webViewData.injectMouseUp(0);
       //     if(%this.webViewData.injectMouseDownId)
       //        cancel(%this.webViewData.injectMouseDownId);
         }
       //  %this.webViewData.injectMouseDown(2);
         %box = BoxCreateFromPoint(%data.globalToLocal(%this, %this.touchPoints[0]));
         %box = BoxExtend(%box, %localPos);
         %this.lastScrollHit = getBoxCenter(%box);
         
         commandToServer('Maximize', %this.getGhostID());
         %data.scheduleMinimize(%this);
      }
      else
      {
         %upVector = "0 1 0"; // %this.getUpVector();
         %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %this.touchPoints.push_back(%touchId, %hitPos);

         %this.lastTouchMode = %this.touchMode;
         %this.touchMode = 0;
      //   echo("Movable3DWidgetData::OnTouchDown touchMode =" SPC %this.touchMode);
         
         if(%this.lastTouchMode == 2 && %this.webViewData)
         {
         //   %this.webViewData.injectMouseUp(2);
         
            // add the two fingers that were used for scrolling the web page
            for(%i = 0; %i < %this.touchPoints.count(); %i++)
               %data.swizzleObjectDown(%this, %this.touchPoints.getKey(%i), %this.touchPoints.getValue(%i));

         //   %data.swizzleObjectDown(%this, %this.firstTouchId, %this.firstTouchPos);
         //   %data.swizzleObjectDown(%this, %this.secondTouchId, %this.secondTouchPos);
         }
         else
         
         //commandToServer('SwizzleObjectDown', %this.getGhostID(), %touchId, %hitPos); 
         //echo("Movable3DWidgetData::OnTouchDown: 2" SPC %touchId SPC %hitPos);
            %data.swizzleObjectDown(%this, %touchId, %hitPos);
      }
//   }
   return true;
}
function Movable3DWidgetData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   if(%this.touchPoints.count() > 0)
   {     
//   %upVector = "0 1 0"; // %this.getUpVector();
//   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
//   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   
      %index = %this.touchPoints.getIndexFromKey(%touchId);
     // %this.touchPoints[%index] = %hitPos;
      

  //    echo("Movable3DWidget::OnTouchMove" SPC %touchId SPC %hitPos);
//echo("Movable3DWidgetData::OnTouchMove: 1" SPC %touchId SPC %hitPos);
//%data.swizzleObjectDragged(%this, %touchId, %hitPos);
//return true;

      // maintain an array of last touch positions
//      %index = %this.lastTouchMove.getIndexFromKey(%touchId);
//      if(%index == -1)
//      {
//         echo("OnTouchMove event had no touch down");
//         return true;
//      }
     //    echo("Movable3DWidgetData::OnTouchMove touchMode =" SPC %this.touchMode);
      
      if(%this.touchMode == 1 && %this.webViewData)
      {
      //   %localPos = VectorScale(%data.globalToLocal(%this, %hitPos), $WEBVIEW_MOUSEHACK);
         if(%touchUVCoord !$= "-1 -1")
         {
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
            %this.touchPoints[%index] = %touchPoint;
         }
         else
         {

            // simple hit point on objects plane
            %upVector = "0 1 0"; // %this.getUpVector();
            %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
            %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
            %localPos = %data.globalToLocal(%this, %hitPos);
            %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
            %this.touchPoints[%index] = %hitPos;
         }
         
         // smooth mouse moves
         //echo("%localPos =" SPC %localPos);
//         echo("VectorSub("SPC %this.webViewData.lastMouseMove SPC "," SPC %localPos SPC ") =" SPC VectorSub(%this.webViewData.lastMouseMove, %localPos));
     //    %offset = VectorSub(%this.webViewData.lastMouseMove, %localPos);         
     //    %offset = VectorScale(%offset, 0.1);         
     //    %localPos = VectorAdd(%this.webViewData.lastMouseMove,%offset);
     //    %this.webViewData.lastMouseMove = %localPos; 
//         echo("%localPosSmooth =" SPC %localPos);
         
         // send move to web page        
      //   %this.webViewData.lastMouseMove = %localPos;
         %this.webViewData.injectMouseMove(%webPoint);
         
      //   if(%this.firstTouchId == %touchId)
      //      %this.firstTouchPos = %hitPos;
      }
      else if(%this.touchMode == 2 && %this.webViewData)
      {
      //   %this.webViewData.injectMouseUp(0);
      //   %localPos = VectorScale(%data.globalToLocal(%this, %hitPos), $WEBVIEW_MOUSEHACK);
         //%localPos = %data.globalToLocal(%this, %hitPos);
         //%this.webViewData.injectMouseMove(getWord(%localPos,0) SPC getWord(%localPos,2));
         
      //   if(%this.firstTouchId == %touchId)
      //      %this.firstTouchPos = %hitPos;
      //   if(%this.secondTouchId == %touchId)
      //      %this.secondTouchPos = %hitPos;
            
         %box = BoxCreateFromPoint(%this.touchPoints[%this.firstTouchId]);
         %box = BoxExtend(%box, %this.touchPoints[%this.secondTouchId]);
         %scrollHit = %data.globalToLocal(%this, getBoxCenter(%box));
      //   %offset = VectorDist(%scrollHit, %this.lastScrollHit);
         %offset = VectorSub(%scrollHit, %this.lastScrollHit);

         if(%touchUVCoord !$= "-1 -1")
         {
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
            %this.touchPoints[%index] = %touchPoint;
         }
         else
         {
            // simple hit point on objects plane
            %upVector = "0 1 0"; // %this.getUpVector();
            %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
            %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
            %localPos = %data.globalToLocal(%this, %hitPos);
            %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
            %this.touchPoints[%index] = %hitPos;
         }


         %this.webViewData.injectMouseMove(%webPoint);
         %this.webViewData.injectMouseWheel(-getWord(%offset,1) SPC -getWord(%offset,0));
         %this.lastScrollHit = %scrollHit;
         
         //%this.webViewData.injectMouseMove(getWord(%localPos,0) SPC getWord(%localPos,2));

         %data.scheduleMinimize(%this);
      }
      else
      {
         %upVector = "0 1 0"; // %this.getUpVector();
         %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
         %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
         %this.touchPoints[%index] = %hitPos;

      //   %dist = VectorDist(%this.lastTouchMove.getValue(%index), %hitPos);
      //   if(%dist > 0.001)
      //   {
            //commandToServer('SwizzleObjectDragged', %this.getGhostID(), %touchId, %hitPos);
      //      echo("Movable3DWidgetData::OnTouchMove: 2" SPC %touchId SPC %hitPos);
            %data.swizzleObjectDragged(%this, %touchId, %hitPos);
      //      %this.lastTouchMove.setValue(%hitPos, %index);
      //   }
      }
      %data.scheduleMinimize(%this);
   }
   return true;
}
function Movable3DWidgetData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
//   echo("Movable3DWidgetData::OnTouchUp:" SPC %touchId);
   TouchGui.touchIDUnlockObject(%touchId, %this);
    
   %changeCheck = %this.touchPoints.count();
   %index = %this.touchPoints.getIndexFromKey(%touchId);
   %this.touchPoints.erase(%index);
   if(%changeCheck == %this.touchPoints.count())
      return;
      

//   echo("Movable3DWidgetData::OnTouchUp touchMode =" SPC %this.touchMode);
     if(%this.touchMode == 1 && %this.webViewData)
      {
         %this.lastTouchMode = %this.touchMode;
         %this.touchMode = 0;
         
//         if(%this.webViewData.injectMouseDownId)
//         {
         //   cancel(%this.webViewData.injectMouseDownId);
            
            if(%touchUVCoord !$= "-1 -1")
            {
               %u = getWord(%touchUVCoord,0);
               %v = getWord(%touchUVCoord,1);
               %webPoint = (%u-0.5) SPC (0.5-%v);
            }
            else
            {
               // simple hit point on objects plane
               %upVector = "0 1 0"; // %this.getUpVector();
               %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
               %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
               %localPos = %data.globalToLocal(%this, %hitPos);
               %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
            }
            
            %shift = VectorLen(VectorSub(%webPoint, %this.webViewData.lastMouseMove));
            if(%shift < 0.1)
            {
               echo("send touch click to web view");
               %this.webViewData.injectMouseMove(%webPoint);
               %this.webViewData.injectMouseDown(0);
               %this.webViewData.injectMouseMove(%webPoint);
   //         }
               %this.webViewData.injectMouseUp(0);
            }
         //commandToServer('SwizzleObjectUp', %this.getGhostID(), %touchId);   
      //   %this.swizzleObjectUp(%this, %touchId);
      }
      if(%this.touchMode == 2 && %this.webViewData)
      {
         %this.lastTouchMode = %this.touchMode;
         %this.touchMode = 1;

         if(%touchUVCoord !$= "-1 -1")
         {
            %u = getWord(%touchUVCoord,0);
            %v = getWord(%touchUVCoord,1);
            %webPoint = (%u-0.5) SPC (0.5-%v);
         }
         else
         {
            // simple hit point on objects plane
            %upVector = "0 1 0"; // %this.getUpVector();
            %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
            %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
            %localPos = %data.globalToLocal(%this, %hitPos);
            %webPoint = getWord(%localPos,0) SPC getWord(%localPos,2);
         }
        
         %shift = VectorLen(VectorSub(%webPoint, %this.webViewData.lastMouseMove));
         if(%shift < 0.1)
         {
            %this.webViewData.injectMouseMove(%webPoint);
            %this.webViewData.injectMouseDown(1);
            %this.webViewData.injectMouseMove(%webPoint);
            %this.webViewData.injectMouseUp(1);
         }

      }
      else
      {
         //commandToServer('SwizzleObjectUp', %this.getGhostID(), %touchId);  
         %data.swizzleObjectUp(%this, %touchId);
         
         if(%this.touchPoints.count() == 2 && %this.webViewData)
         {
            %this.lastTouchMode = %this.touchMode;
            %this.touchMode = 2;
         //   %this.webViewData.injectMouseDown(2);
         
            for(%i = 0; %i < %this.touchPoints.count(); %i++)
               %data.swizzleObjectUp(%this, %this.touchPoints.getKey(%i));
         }
      }

   return true;
}
*/






function AirHockeyPaddleData::onAddClient(%data, %this)
{
//   if(%this.lastTouchMove)
//      %this.lastTouchMove.delete();
//   %this.lastTouchMove = new ArrayObject();
   
   
   
   
//   %this.schedule(0, "updateCollisionBox");
   //commandToServer('SetThreadPosition', %this, 1);  
   //%this.schedule(0, "setNativeScale"); 
   
   %this.mLocation = "0 0 0";

   %this.mVelocityAccumulator = "0 0 0";
	%this.momentum = 0.9;

   %this.currentHitPos = new ArrayObject();
   %this.lastHitPos = new ArrayObject();
   %this.touchPoints = new ArrayObject();
   
   %this.touchCount = 0;
   %this.lastTouchCount = 0;
   %this.touchMode = 0;
   %this.lastTouchMode = 0;
   
   %this.mCenterPrev = "0 0 0";
   
//   commandToServer('InstantMinimize', %this.getGhostID());
   
}

function AirHockeyPaddleData::swizzleObjectDown(%data, %this, %touchId, %hitPos)
{
   if(%this.currentHitPos.count() == 0)
   {
      %this.lastTouchCount = 0;
      %this.mLastTime = getSimTime();
   //   %this.mLocation = "0 0 0";
   //   commandToServer('Maximize', %this.getGhostID());
   }
   %this.currentHitPos.push_back(%touchId, %hitPos);
   %this.lastHitPos.insert(%touchId, %hitPos, %this.currentHitPos.getIndexFromKey(%touchId));
   
//   %data.scheduleMinimize(%this);
}
function AirHockeyPaddleData::swizzleObjectDragged(%data, %this, %touchId, %hitPos)
{
   %fingersCount = %this.currentHitPos.count();
   if(%fingersCount <= 0)
      return;

   // update current touch location for this touch
   %touchIndex = %this.currentHitPos.getIndexFromKey(%touchId);
   if(%touchIndex == -1) 
   { 
      // grab touch events that brush the paddle
      %data.swizzleObjectDown(%this, %touchId, %hitPos);     
      return;   
   }
   
   %this.currentHitPos.setValue(%hitPos, %touchIndex);

   //
   if(%touchIndex != (%fingersCount-1)) 
      return;
      
   %time = getSimTime();
   %dt = (%time - %this.mLastTime);
   
   // not enough time has passed 
   if(%dt < 1)
      return;

//   cancel(%this.movekillId);

   /////////////////////////////////////////////////////////////////////////////////
   // calculate motion

   // Compute bounding squares
   %boundsCurr = BoxCreateFromPoint(%this.currentHitPos.getValue(0));
   for(%i = 1; %i < %fingersCount; %i++) 
      %boundsCurr = BoxExtend(%boundsCurr, %this.currentHitPos.getValue(%i));
   %centerCurr = getBoxCenter(%boundsCurr);
  
   
   // update last touch positions
   for(%i = 0; %i < %fingersCount; %i++) 
      %this.lastHitPos.setValue(%this.currentHitPos.getValue(%i), %this.lastHitPos.getIndexFromKey(%this.currentHitPos.getKey(%i)));
   
   %this.mLastTime = %time;
   
   commandToServer('SetPaddlePosition', %this.getGhostID(), %centerCurr, %dt);
   
//   %this.movekillId = %this.schedule( 2, "commandToServer", 'SetPaddleStop', %this.getGhostID());
}
function AirHockeyPaddleData::swizzleObjectUp(%data, %this, %touchId)
{
//   commandToServer('SetPaddlePosition', %this.getGhostID(), %this.mLocation);

   %index = %this.currentHitPos.getIndexFromKey(%touchId);
   if(%index == -1)
      return;
      
   %this.currentHitPos.erase(%index);
   %this.lastHitPos.erase(%this.lastHitPos.getIndexFromKey(%touchId));   
}


function AirHockeyPaddleData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   
   // simple hit point on objects plane
   %upVector = "0 0 1"; // %this.getUpVector();
   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   
   %data.swizzleObjectDown(%this, 10000, %hitPos);   
//   %this.isMouseDown = true;
   TouchGui.noCursor = true;
}
function AirHockeyPaddleData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   // simple hit point on objects plane
   %upVector = "0 0 1"; // %this.getUpVector();
   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   
   %data.swizzleObjectDragged(%this, 10000, %hitPos);              
}
function AirHockeyPaddleData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   TouchGui.noCursor = false;
//   %this.isMouseDown = false;
   TouchGui.mouseUnlockObject(%this);
   %data.swizzleObjectUp(%this, 10000);              
}

function AirHockeyPaddleData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);

   // simple hit point on objects plane
   %upVector = "0 0 1"; // %this.getUpVector();
   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   %data.swizzleObjectDown(%this, %touchId, %hitPos);   
   return true;                      
}
function AirHockeyPaddleData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   // simple hit point on objects plane
   %upVector = "0 0 1"; // %this.getUpVector();
   %t = -(VectorDot(VectorSub(%pos, %this.position), %upVector) / VectorDot(%vec, %upVector));
   %hitPos = VectorAdd(VectorScale(%vec, %t), %pos);
   %data.swizzleObjectDragged(%this, %touchId, %hitPos);              
   return true;                      
}
function AirHockeyPaddleData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   %data.swizzleObjectUp(%this, %touchId);              
   TouchGui.touchIDUnlockObject(%touchId, %this);
   return true;                      
}
//**************************************************************
// touch of fish to get info 
//**************************************************************
function FishPlayerClass::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   TouchGui.mouseLockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchDown(%this, %pos, %vec, %mousePoint, %touchId);
   return true;                      
}
function FishPlayerClass::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = 10000; //hack
   newInfoTouchMove(%this, %pos, %vec, %mousePoint, %touchId);                       
   return true;                      
}
function FishPlayerClass::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
//   TouchGui.mouseUnlockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchUp(%this, %pos, %vec, %mousePoint, %touchId);                       
   return true;                      
}

function FishPlayerClass::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);
   newInfoTouchDown(%this, %pos, %vec, %touchPoint, %touchId);
//   return true;                      
   return false;                      
}
function FishPlayerClass::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   newInfoTouchMove(%this, %pos, %vec, %touchPoint, %touchId);                       
//   return true;                      
   return false;                      
}
function FishPlayerClass::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDUnlockObject(%touchId, %this);
   newInfoTouchUp(%this, %pos, %vec, %touchPoint, %touchId);                       
//   return true;                      
   return false;                      
}
////////////////////////////////////////////////////////////////////////////////
// A class for gameBaseData to play an animation onAdd and allow for interaction

function StaticShapeAutoPlay::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchDown(%this, %pos, %vec, %mousePoint, %touchId);
}
function StaticShapeAutoPlay::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = 10000; //hack
   newInfoTouchMove(%this, %pos, %vec, %mousePoint, %touchId);                       
}
function StaticShapeAutoPlay::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchUp(%this, %pos, %vec, %mousePoint, %touchId);                       
}

function StaticShapeAutoPlay::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);
   newInfoTouchDown(%this, %pos, %vec, %touchPoint, %touchId);
//   return true;                      
   return false;                      
}
function StaticShapeAutoPlay::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   newInfoTouchMove(%this, %pos, %vec, %touchPoint, %touchId);                       
//   return true;                      
   return false;                      
}
function StaticShapeAutoPlay::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDUnlockObject(%touchId, %this);
   newInfoTouchUp(%this, %pos, %vec, %touchPoint, %touchId);                       
//   return true;                      
   return false;                      
}
/*
function DugongSet1_SSData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchDown(%this, %pos, %vec, %mousePoint, %touchId);
}
function DugongSet1_SSData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = 10000; //hack
   newInfoTouchMove(%this, %pos, %vec, %mousePoint, %touchId);                       
}
function DugongSet1_SSData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   %touchId = 10000; //hack
   newInfoTouchUp(%this, %pos, %vec, %mousePoint, %touchId);                       
}

function DugongSet1_SSData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);
   newInfoTouchDown(%this, %pos, %vec, %touchPoint, %touchId);
   return true;                      
}
function DugongSet1_SSData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   newInfoTouchMove(%this, %pos, %vec, %touchPoint, %touchId);                       
   return true;                      
}
function DugongSet1_SSData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDUnlockObject(%touchId, %this);
   newInfoTouchUp(%this, %pos, %vec, %touchPoint, %touchId);                       
   return true;                      
}
*/

////////////////////////////////////////////////////////////////////////////////


new ParticleData(SandParticle)
{
   textureName          = "art/shapes/particles/smoke";
   dragCoefficient      = 0.3;
   gravityCoefficient   = -0.01;   // rises slowly
   inheritedVelFactor   = 1.00;
   lifetimeMS           = 3000;
 //  lifetimeVarianceMS   = 250;
   lifetimeVarianceMS   = 2000;
   useInvAlpha          = true;
   spinRandomMin        = -30.0;
   spinRandomMax        = 30.0;

   sizes[0]      = 0.75;
   sizes[1]      = 1.385;
   sizes[2]      = 3.25;
//   sizes[0]      = 0.75/2;
//   sizes[1]      = 1.385/2;
//   sizes[2]      = 3.25/2;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;

   colors[0]     = "1 1 1 1";
   colors[1]     = "1 1 1 0.7";
   colors[2]     = "1 1 1 0.0";
   

//   softnessDistance = "0.005";
 //   highResOnly = false;

      renderReflection = 0;
 //  blendStyle = "SUBTRACTIVE";
};
new ParticleEmitterData(SandPuffEmitter)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 5;

   ejectionVelocity = 0.015;
   velocityVariance = 0.01;

   thetaMin         = 0.0;
   thetaMax         = 90.0;

   particles        = "SandParticle";
//   overrideBounds = "-2 -2 -1 2 2 1";
//   overrideBounds = "-1 -1 -1 1 1 1";
//   overrideBounds = "-2 0 -2 2 1 2";
//   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
   highResOnly = true;
   sortParticles = true;
   softnessDistance = 0.3;
};

SandParticle.reloadOnLocalClient();
SandPuffEmitter.reloadOnLocalClient();

new ParticleData( WaterRippleParticle )
{
   textureName          = "art/shapes/particles/wake";
   dragCoefficient     = "0.0";
   gravityCoefficient   = "0.0";
   inheritedVelFactor   = "0.0";
   lifetimeMS           = "2500";
   lifetimeVarianceMS   = "200";
   windCoefficient = "0.0";
   useInvAlpha = "1";
   spinRandomMin = "30.0";
   spinRandomMax = "30.0";

   animateTexture = true;
   framesPerSec = 1;
   animTexTiling = "2 1";
   animTexFrames = "0 1";

   colors[0]     = "1 1 1 0.1";
   colors[1]     = "1 1 1 0.7";
   colors[2]     = "1 1 1 0.3";
   colors[3]     = "0.5 0.5 0.5 0";

   sizes[0]      = "1.0";
   sizes[1]      = "2.0";
   sizes[2]      = "3.0";
   sizes[3]      = "3.5";

   times[0]      = "0.0";
   times[1]      = "0.25";
   times[2]      = "0.5";
   times[3]      = "1.0";
//   softnessDistance = "0.005";


};
new ParticleEmitterData( WaterRippleEmitter )
{
   ejectionPeriodMS = "200";
   periodVarianceMS = "10";

   ejectionVelocity = "0";
   velocityVariance = "0";

   ejectionOffset   = "0";

   thetaMin         = "89";
   thetaMax         = "90";

   phiReferenceVel  = "0";
   phiVariance      = "1";

   alignParticles = "1";
   alignDirection = "0 0 1";

   particles = "WaterRippleParticle";
   overrideBounds = "-1 -1 -1 1 1 1";
      highResOnly = true;
         sortParticles = true;
   softnessDistance = 0.02;

};
WaterRippleParticle.reloadOnLocalClient();
WaterRippleEmitter.reloadOnLocalClient();

/*
datablock ParticleData(LiftoffDust)
{
   dragCoefficient      = 1.0;
   gravityCoefficient   = -0.01;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = true;
   spinRandomMin        = -90.0;
   spinRandomMax        = 500.0;
   colors[0]     = "1.0 1.0 1.0 1.0";
   sizes[0]      = 1.0;
   times[0]      = 1.0;
   textureName = "art/shapes/particles/dustParticle";
};

datablock ParticleEmitterData(LiftoffDustEmitter)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 2.0;
   velocityVariance = 0.0;
   ejectionOffset   = 0.0;
   thetaMin         = 90;
   thetaMax         = 90;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   useEmitterColors = true;
   particles = "LiftoffDust";
};
*/

new ParticleData(ClientBubbleParticle)
{
   dragCoefficient      = "0";
   gravityCoefficient   = "-0.05";
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = "2000";
   lifetimeVarianceMS   = "1000";
   useInvAlpha          = false;
   textureName          = "art/cube/interface/touch_cursor1_diffuse.png";
   colors[0]     = "0.661417 0.795276 1 0.393701";
//   colors[1]     = "0.661417 0.795276 1 0.393701";
   colors[1]     = "0.661417 0.795276 1 0.393701";
   colors[2]     = "0.669291 0.795276 1 0";
   sizes[0]      = "0.02";
   sizes[1] = "0.02";
   sizes[2] = "0";
   times[0]      = 0.0;
   times[1] = "0.5";
   times[2] = "1";
//   animTexName = "art/cube/interface/touch_cursor1_diffuse.png";
//   softParticles = "1";
   spinSpeed = "0";
   spinRandomMin = "-292";
   spinRandomMax = "-41.1";
   renderReflection = 0;
//   softnessDistance = "0.005";
};
new ParticleEmitterData(ClientBubbleEmitter)
{
   ejectionPeriodMS = "400";
   periodVarianceMS = "10";
   ejectionVelocity = "0.2";
   ejectionOffset   = "0.1";
   velocityVariance = "0.01";
   thetaMin         = 0;
   thetaMax         = "26.25";
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "ClientBubbleParticle";
   blendStyle = "ADDITIVE";
   
//   renderReflection = false;
   overrideBounds = "-1 -1 -1 1 1 1";
   highResOnly = true;
      sortParticles = true;
   softnessDistance = 0.02;

};
ClientBubbleParticle.reloadOnLocalClient();
ClientBubbleEmitter.reloadOnLocalClient();
new ParticleData( ClientSplashParticle )
{
   dragCoefficient      = 1;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = -0.0;
   lifetimeMS           = 600;
   lifetimeVarianceMS   = 0;
   colors[0]     = "0.7 0.8 1.0 1.0";
   colors[1]     = "0.7 0.8 1.0 0.5";
   colors[2]     = "0.7 0.8 1.0 0.0";
   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.5;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};
new ParticleEmitterData( ClientSplashEmitter )
{
   ejectionPeriodMS = 1;
   periodVarianceMS = 0;
   ejectionVelocity = 3;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 60;
   thetaMax         = 80;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   orientParticles  = true;
   lifetimeMS       = 100;
   particles = "ClientSplashParticle";
      highResOnly = true;
         sortParticles = true;

   softnessDistance = 0.02;

};
ClientSplashParticle.reloadOnLocalClient();
ClientSplashEmitter.reloadOnLocalClient();

new ParticleData( ClientWakeParticle )
{
   textureName          = "art/shapes/particles/wake";
   dragCoefficient     = "0.0";
   gravityCoefficient   = "0.0";
   inheritedVelFactor   = "0.0";
   lifetimeMS           = "2500";
   lifetimeVarianceMS   = "200";
   windCoefficient = "0.0";
   useInvAlpha = "1";
   spinRandomMin = "30.0";
   spinRandomMax = "30.0";

   animateTexture = true;
   framesPerSec = 1;
   animTexTiling = "2 1";
   animTexFrames = "0 1";

   colors[0]     = "1 1 1 0.1";
   colors[1]     = "1 1 1 0.7";
   colors[2]     = "1 1 1 0.3";
   colors[3]     = "0.5 0.5 0.5 0";

   sizes[0]      = "1.0";
   sizes[1]      = "2.0";
   sizes[2]      = "3.0";
   sizes[3]      = "3.5";

   times[0]      = "0.0";
   times[1]      = "0.25";
   times[2]      = "0.5";
   times[3]      = "1.0";
};
new ParticleEmitterData( ClientWakeEmitter )
{
   ejectionPeriodMS = "200";
   periodVarianceMS = "10";

   ejectionVelocity = "0";
   velocityVariance = "0";

   ejectionOffset   = "0";

   thetaMin         = "89";
   thetaMax         = "90";

   phiReferenceVel  = "0";
   phiVariance      = "1";

   alignParticles = "1";
   alignDirection = "0 0 1";

   particles = "ClientWakeParticle";
      highResOnly = true;
         sortParticles = true;
   softnessDistance = 0.02;


};
ClientWakeParticle.reloadOnLocalClient();
ClientWakeEmitter.reloadOnLocalClient();


$SplashEmitter = 1;
$BubbleEmitter = 2;
$SandPuffEmitter = 3;
$WaterRippleEmitter = 4;
$WakeEmitter = 5;

function particleAtMount(%this, %slot, %presetId)
{
   %mat = %this.getSlotTransform(%slot);
   %position = getWords(%mat,0,2);
   %upVector = VectorNormalize(MatrixMulVector(%mat,"0 0 1"));
   switch(%presetId)
   {
   case $SplashEmitter:
         %emitter = new ParticleEmitter()
         {
            datablock = "ClientSplashEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %emitter.onAdd();
         %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
         %emitter.schedule(500, "deleteWhenEmpty");

   case $BubbleEmitter:
         %emitter = new ParticleEmitter()
         {
            datablock = "ClientBubbleEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %emitter.onAdd();
         %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
         %emitter.schedule(500, "deleteWhenEmpty");
         
   case $SandPuffEmitter:
         %emitter = new ParticleEmitter()
         {
            datablock = "SandPuffEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %emitter.onAdd();
         %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
         %emitter.schedule(500, "deleteWhenEmpty");
         
   case $WaterRippleEmitter:
         %emitter = new ParticleEmitter()
         {
            datablock = "WaterRippleEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %emitter.onAdd();
         %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
         %emitter.schedule(500, "deleteWhenEmpty");
   }
}
function mountParticle(%this, %slot, %presetId)
{
   %mat = %this.getSlotTransform(%slot);
   %position = getWords(%mat,0,2);
//   %upVector = VectorNormalize(MatrixMulVector(%mat,"0 0 1"));
   switch(%presetId)
   {
   case $SplashEmitter:
      if(!isObject(%this.splashEmitter))
      {
         %emitter = new ParticleEmitter()
         {
            datablock = "ClientSplashEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %this.splashEmitter = %emitter;
      }
   case $BubbleEmitter:
      if(!isObject(%this.bubbleEmitter))
      {
         %emitter = new ParticleEmitter()
         {
            datablock = "ClientBubbleEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %this.bubbleEmitter = %emitter;
      }
   case $WakeEmitter:
      if(!isObject(%this.wakeEmitter))
      {
         %emitter = new ParticleEmitter()
         {
            datablock = "ClientWakeEmitter";
            velocity = 1;
            parentGroup = "";
         };
         %this.wakeEmitter = %emitter;
      }
   }
   if(!isObject(%emitter)){
      echo("no emitter");
   }
   %emitter.onAdd();
   %emitter.lastPosition = %position;
   %emitter.object = %this;
   %emitter.slot = %slot;
   
   updateParticle(%emitter);
}
function unmountParticle(%this, %slot, %presetId)
{
   %mat = %this.getSlotTransform(%slot);
   %position = getWords(%mat,0,2);
   %upVector = VectorNormalize(MatrixMulVector(%mat,"0 0 1"));
   switch(%presetId)
   {
   case $SplashEmitter:
      if(isObject(%this.splashEmitter))
      {
         cancel(%this.splashEmitter.updateParticleId);
         %this.splashEmitter.deleteWhenEmpty();
         %this.splashEmitter = "";
      }
   case $BubbleEmitter:
      if(isObject(%this.bubbleEmitter))
      {
      //   %this.unmountObject(%this.bubbleEmitter);
         cancel(%this.bubbleEmitter.updateParticleId);
         %this.bubbleEmitter.deleteWhenEmpty();
         %this.bubbleEmitter = "";
       //  %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
       //  %emitter.schedule(500, "deleteWhenEmpty");
      }
   case $WakeEmitter:
      if(isObject(%this.wakeEmitter))
      {
      //   %this.unmountObject(%this.bubbleEmitter);
         cancel(%this.wakeEmitter.updateParticleId);
         %this.wakeEmitter.deleteWhenEmpty();
         %this.wakeEmitter = "";
       //  %emitter.emitParticles(%position, true, %upVector, "0 -1 0", 1);
       //  %emitter.schedule(500, "deleteWhenEmpty");
      }
      
   }
}
function updateParticle(%emitter)
{
   if(isObject(%emitter))
   {
      %mat = %emitter.object.getSlotTransform(%emitter.slot);
      %position = getWords(%mat,0,2);
     // %upVector = VectorNormalize(MatrixMulVector(%mat,"0 0 1"));

      %emitter.emitParticles(%position, true, "0 0 1", VectorSub(%emitter.lastPosition,%position), 32); 
      %emitter.lastPosition = %position;
      
      
      %emitter.updateParticleId = schedule(32, 0, "updateParticle", %emitter);  
   }
}

function DugongSet1_SSData::onTrigger(%data, %this, %index, %state)
{
//   echo("DugongSet1_SSData::onTrigger = ",%index,", ",%state);
   
   switch(%index)
   {
   case 0:
  //   if(%state)
  //       particleAtMount(%this, 0, $SandPuffEmitter);      
  //    else
         particleAtMount(%this, 0, $SandPuffEmitter);      
   case 1:   
    //     particleAtMount(%this, 1, $WaterRippleEmitter);      
    //     particleAtMount(%this, 2, $WaterRippleEmitter);      
         particleAtMount(%this, 1, $BubbleEmitter);      
         particleAtMount(%this, 2, $BubbleEmitter);      
   
   case 2:   
     //    particleAtMount(%this, 3, $WaterRippleEmitter);      
     //    particleAtMount(%this, 4, $WaterRippleEmitter);      
      
   }
     

}

function GreenSeaTurtleSet1_SSData::onTrigger(%data, %this, %index, %state)
{
//   echo("DugongSet1_SSData::onTrigger = ",%index,", ",%state);
   
   switch(%index)
   {
   case 0:
      echo("GreenSeaTurtleSet1_SSData::onTrigger: 0");
  // ClientPlay(
       //  sfxPlayOnce():
        // particleAtMount(%this, 0, $SandPuffEmitter);      
   case 1:   
      echo("GreenSeaTurtleSet1_SSData::onTrigger: 1");
    //     particleAtMount(%this, 1, $WaterRippleEmitter);      
    //     particleAtMount(%this, 2, $WaterRippleEmitter);      
       //  particleAtMount(%this, 1, $BubbleEmitter);      
        // particleAtMount(%this, 2, $BubbleEmitter);      
   
   case 2:   
      echo("GreenSeaTurtleSet1_SSData::onTrigger: 2");
     //    particleAtMount(%this, 3, $WaterRippleEmitter);      
     //    particleAtMount(%this, 4, $WaterRippleEmitter);      
      
   }
     

}



function DiverSet1_SSData::onTrigger(%data, %this, %index, %state)
{
//   echo("DiverSet1_SSData::onTrigger = ",%index,", ",%state);
   
   switch(%index)
   {
   case 0:
      if(%state)
      {
      //   particleAtMount(%this, 0, $SplashEmitter);
      //   particleAtMount(%this, 1, $SplashEmitter);
         particleAtMount(%this, 0, $BubbleEmitter);
         particleAtMount(%this, 1, $BubbleEmitter);
         mountParticle(%this, 4, $BubbleEmitter);
      }
      else
      {
         unmountParticle(%this, 4, $BubbleEmitter);
      //   particleAtMount(%this, 0, $SplashEmitter);
      //   particleAtMount(%this, 1, $SplashEmitter);
         particleAtMount(%this, 0, $BubbleEmitter);
         particleAtMount(%this, 1, $BubbleEmitter);
      }
   case 1:   
      if(%state)
      {
         mountParticle(%this, 6, $BubbleEmitter);
      //   mountParticle(%this, 7, $wakeEmitter);
      //   mountParticle(%this, 7, $BubbleEmitter);
      }
      else
      {
         unmountParticle(%this, 6, $BubbleEmitter);
       //  unmountParticle(%this, 7, $wakeEmitter);
       //  unmountParticle(%this, 7, $BubbleEmitter);
      }
   }
}
////////////////////////////////////////////////////////////////////////////////

function MenuButtonCloseData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseLockObject(%this);
   commandToServer('Menu_OnCloseDown',%this.getGhostID());                       
}
function MenuButtonCloseData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}
function MenuButtonCloseData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   TouchGui.mouseUnlockObject(%this);
   commandToServer('Menu_OnCloseUp',%this.getGhostID());                       
}

function MenuButtonCloseData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);
   commandToServer('Menu_OnCloseDown',%this.getGhostID()); 
   return true;                      
}
function MenuButtonCloseData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
function MenuButtonCloseData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDUnlockObject(%touchId, %this);
   commandToServer('Menu_OnCloseUp',%this.getGhostID());                       
   return true;                      
}



////////////////////////////////////////////////////////////////////////////////

function MenuImageGridData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   commandToServer('MIG_OnButtonDown',%this.getGhostID());                       
}
function MenuImageGridData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}
function MenuImageGridData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}

function MenuImageGridData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   commandToServer('MIG_OnButtonDown',%this.getGhostID());                       
   return true;                      
}
function MenuImageGridData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
function MenuImageGridData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}


////////////////////////////////////////////////////////////////////////////////
//



function ViewButtonData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   commandToServer('ViewButton_OnClick',%this.getGhostID());                       
}
function ViewButtonData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}
function ViewButtonData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}

function ViewButtonData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   commandToServer('ViewButton_OnClick',%this.getGhostID());                       
   return true;                      
}
function ViewButtonData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
function ViewButtonData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}


function ClientCmdChangeView(%viewId)
{
   switch(%viewId)
   {
   case 0:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::mosaicOffset[$pref::Cube::clientIDCurr]);
   case 1:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo1Offset[$pref::Cube::clientIDCurr]);
   case 2:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo2Offset[$pref::Cube::clientIDCurr]);
   case 3:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo3Offset[$pref::Cube::clientIDCurr]);
   case 4:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo4Offset[$pref::Cube::clientIDCurr]);
   case 5:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo5Offset[$pref::Cube::clientIDCurr]);
   case 6:   handleLoadMultiView(0,$pref::cube::mosaicResolution,$pref::cube::mosaicFOV,$pref::Cube::demo6Offset[$pref::Cube::clientIDCurr]);
   }
   
//   $pref::SFX::FMOD::speakerPositionFrontLeft = $pref::cube::speakerPositionFrontLeft[%viewId];
//   $pref::SFX::FMOD::speakerPositionFrontRight = $pref::cube::speakerPositionFrontRight[%viewId];
//   fmodUpdate3DSpeakerPositions();


}
function ClientCmdChangeEarOffset(%speakerSetId)
{
   %cam = ServerConnection.getCameraObject();
   %cam.earOffset = $pref::cube::earOffset[%speakerSetId];
}

/*
function ClientCmdSetRenderEnabled(%ghost, %value)
{
   %this = ServerConnection.resolveGhostID(%ghost);
  // %this.hidden = %value;
//   %this.isRenderEnabled = %value;
 //  %this.castShadowsOnly = true;
}
*/
/*
function createReefChunk(%path, %isRenderEnabled)
{
 //  if(isObject(%replace)) %replace.delete();  
   %obj = new TSStatic() {
      shapeName = %path;
      enableInputEvents = "0";
      position = "0 0 0";
      rotation = "0 0 1 0";
      scale = "1 1 1";
      hidden = "0";
      canSave = "1";
      canSaveDynamicFields = "1";
      isRenderEnabled = %isRenderEnabled;
      collisionType = "None";      
      parentGroup = ClientMissionCleanup;
   };
  // %obj.onAdd();
   return %obj;
}
function ClientCmdUpdateReef(%viewId)
{
   
   %clientID = $pref::Cube::clientIDCurr;
         
   if(isObject($reefTop1)) $reefTop1.delete();  
   if(isObject($reefTop2)) $reefTop2.delete();  
   if(isObject($reefFront1)) $reefFront1.delete();  
   if(isObject($reefFront2)) $reefFront2.delete();  
   if(isObject($reefFront3)) $reefFront3.delete();  
   if(isObject($reefFront4)) $reefFront4.delete();  
   if(isObject($reefFront5)) $reefFront5.delete();  
   if(isObject($reefFront6)) $reefFront6.delete();  
   if(isObject($reefFront7)) $reefFront7.delete();  
   if(isObject($reefFront8)) $reefFront8.delete();  
   if(isObject($reefFront9)) $reefFront9.delete();  
   if(isObject($reefFront10)) $reefFront10.delete();  
   
   switch(%viewId)
   {
   case 0: 
      $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
      $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
      if(%clientID == 6 || %clientID == 7 || %clientID == 3)
      {
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else
      {
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", true);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", true);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", true);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", true);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", true);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", true);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", true);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", true);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", true);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", true);
      }
      
   case 1:
      if(%clientID == 0)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1_high.dts", true);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2_high.dts", true);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 1)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1_high.dts", true);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 2)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2_high.dts", true);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 3)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 4)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1_high.dts", true);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 5)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2_high.dts", true);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 6)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 7)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 8)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1_high.dts", true);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2_high.dts", true);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
   case 2:
   
      if(%clientID == 0)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3_high.dts", true);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4_high.dts", true);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 1)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3_high.dts", true);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 2)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4_high.dts", true);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 3)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 4)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3_high.dts", true);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 5)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4_high.dts", true);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 6)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 7)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 8)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3_high.dts", true);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4_high.dts", true);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      
   case 3:
      if(%clientID == 0)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5_high.dts", true);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6_high.dts", true);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 1)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5_high.dts", true);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 2)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6_high.dts", true);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 3)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front0.dts", false);
      }
      else if(%clientID == 4)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5_high.dts", true);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 5)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6_high.dts", true);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 6)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", true);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 7)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 8)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5_high.dts", true);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6_high.dts", true);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
   case 4:
      if(%clientID == 0)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7_high.dts", true);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8_high.dts", true);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 1)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7_high.dts", true);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 2)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8_high.dts", true);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 3)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 4)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7_high.dts", true);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 5)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8_high.dts", true);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 6)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 7)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 8)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7_high.dts", true);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8_high.dts", true);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }

   case 5:
      if(%clientID == 0)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9_high.dts", true);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10_high.dts", true);
      }
      else if(%clientID == 1)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9_high.dts", true);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 2)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10_high.dts", true);
      }
      else if(%clientID == 3)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 4)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9_high.dts", true);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 5)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10_high.dts", true);
      }
      else if(%clientID == 6)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 7)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", true);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9.dts", false);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10.dts", false);
      }
      else if(%clientID == 8)
      {
         $reefTop1 = createReefChunk("art/cube/sets/reef_top1.dts", false);
         $reefTop2 = createReefChunk("art/cube/sets/reef_top2.dts", false);
         $reefFront1 = createReefChunk("art/cube/sets/reef_front1.dts", false);
         $reefFront2 = createReefChunk("art/cube/sets/reef_front2.dts", false);
         $reefFront3 = createReefChunk("art/cube/sets/reef_front3.dts", false);
         $reefFront4 = createReefChunk("art/cube/sets/reef_front4.dts", false);
         $reefFront5 = createReefChunk("art/cube/sets/reef_front5.dts", false);
         $reefFront6 = createReefChunk("art/cube/sets/reef_front6.dts", false);
         $reefFront7 = createReefChunk("art/cube/sets/reef_front7.dts", false);
         $reefFront8 = createReefChunk("art/cube/sets/reef_front8.dts", false);
         $reefFront9 = createReefChunk("art/cube/sets/reef_front9_high.dts", true);
         $reefFront10 = createReefChunk("art/cube/sets/reef_front10_high.dts", true);
      }
   }
}

*/
function LightrayButtonData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   commandToServer('LightrayButton_OnClick');                       
}
function LightrayButtonData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}
function LightrayButtonData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}

function LightrayButtonData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   commandToServer('LightrayButton_OnClick');                       
   return true;                      
}
function LightrayButtonData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
function LightrayButtonData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}

function ClientCmdLightrayToggle()
{
   LightRayPostUnderwaterFX.isEnabled = !LightRayPostUnderwaterFX.isEnabled;
}

function ClientCmdSetUrl(%webdata, %url)
{
   echo("client recieved set url command for " @ %url);
   %wd = ServerConnection.resolveGhostID(%webdata);
   
//   %wd = resolveObjectFromGhostIndex(%webdata);
   %wd.setURL(%url);   
}


//**************************************************************************
// debug button
//**************************************************************************

function DebugButtonData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   commandToServer('DebugButton_OnClick');                       
}
function DebugButtonData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}
function DebugButtonData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
}

function DebugButtonData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   commandToServer('DebugButton_OnClick');                       
   return true;                      
}
function DebugButtonData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
function DebugButtonData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   return true;                      
}
//*********************************************************************************************
//
// fish bubble buttons
//
//*********************************************************************************************
new ArrayObject(bubbleTouchLocks){};

$MOUSE_TOUCH_ID = 10000;

function BubbleButtonData::OnMouseDown(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = $MOUSE_TOUCH_ID;
   TouchGui.mouseLockObject(%this);
   bubbleTouchLocks.push_back(%touchId, %this);
    commandToServer('BubbleButtonDown',%this.getGhostID(), %mousePoint, %pos, %vec, %mouseUVCoord);                          
}
function BubbleButtonData::OnMouseDragged(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = $MOUSE_TOUCH_ID;
   %index =  bubbleTouchLocks.getIndexFromKey(%touchId);
   if( %index>=0 ){
      %object = bubbleTouchLocks.getValue(%index);
      if( %object== %this  ){ 
        commandToServer('BubbleButtonDragged',%this.getGhostID(), %mousePoint, %pos, %vec, %mouseUVCoord);                          
      }
   }
}

function BubbleButtonData::OnMouseUp(%data, %this, %modifier, %mousePoint, %mouseClickCount, %pos, %vec, %mouseUVCoord)
{
   %touchId = $MOUSE_TOUCH_ID;
   TouchGui.mouseUnlockObject(%this);
   %index =  bubbleTouchLocks.getIndexFromKey(%touchId);
   if( %index>= 0){
      bubbleTouchLocks.erase(%index);      
   }
   if( isObject(%this) ){
      commandToServer('BubbleButtonUp',%this.getGhostID(), %mousePoint, %pos, %vec, %mouseUVCoord);                          
   }
}
function BubbleButtonData::OnTouchDown(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   TouchGui.touchIDLockObject(%touchId, %this);
   bubbleTouchLocks.push_back(%touchId, %this);
   if( isObject(%this)  && %this.getGhostID()>=0  ){
      commandToServer('BubbleButtonDown',%this.getGhostID(), %touchPoint, %pos, %vec, %touchUVCoord);                          
   }
   return true;                      
}
function BubbleButtonData::OnTouchMove(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   %index =  bubbleTouchLocks.getIndexFromKey(%touchId);
   if( %index>=0 ){
      %object = bubbleTouchLocks.getValue(%index);
      if( %object== %this ){ 
         if( isObject(%this) && %this.getGhostID()>=0 /*&& TouchGui.isTouchIDLockedObject(%this)*/ ){
           commandToServer('BubbleButtonDragged',%this.getGhostID(), %mousePoint, %pos, %vec, %mouseUVCoord);                          
         }
      }
   }
   return true;                      
}
function BubbleButtonData::OnTouchUp(%data, %this, %touchId, %touchPoint, %pos, %vec, %touchUVCoord)
{
   %index =  bubbleTouchLocks.getIndexFromKey(%touchId);
   if( %index>= 0){
      bubbleTouchLocks.erase(%index);      
   }
   TouchGui.touchIDUnlockObject(%touchId, %this);
   if( isObject(%this)  && %this.getGhostID()>=0  ){
      commandToServer('BubbleButtonUp',%this.getGhostID(), %touchPoint, %pos, %vec, %touchUVCoord);                         
   }
   return true;                      
}
//*********************************************************************************************
//
// new web page stuff
//
//*********************************************************************************************
function newInfoTouchDown(%this, %pos, %vec, %mousePoint, %touchId)
{
   commandToServer('InfoTouchDown',%this.getGhostID(), %pos, %vec, %mousePoint, %touchId);
}
function newInfoTouchMove(%this, %pos, %vec, %mousePoint, %touchId)
{
   commandToServer('InfoTouchMove',%this.getGhostID(), %pos, %vec, %mousePoint, %touchId);                       
}
function newInfoTouchUp(%this, %pos, %vec, %mousePoint, %touchId)
{
   commandToServer('InfoTouchUp',%this.getGhostID(), %pos, %vec, %mousePoint, %touchId);                       
}



//*********************************************************************************************
//
// misc
//
//*********************************************************************************************



function clientCmdShowPlayGui()
{
   if (!isToolBuild() || !Editor::checkActiveLoadDone())
   {
      if (Canvas.getContent() != PlayGui.getId())
      {
         Canvas.setContent(PlayGui);
      //   ControlsHelpDlg.toggle();
      }
   }
}
      
function clientCmdResetWind(%ghost, %randomSeed)
{
   %this = ServerConnection.resolveGhostID(%ghost);

   if(isObject(%this))
      %this.resetWind(%randomSeed);
}

      // todo: set on server
function clientCmdPreloadWebData(%ghost)
{
   %this = ServerConnection.resolveGhostID(%ghost);

   if(isObject(%this))
   {
      %this.clientOnly = true; // todo: why isn't this set already
      %this.setUrl(getFishWebRoot() @ "/index_master.html?f=basslet");
   }
}

function clientCmdSetUrl(%ghost, %url)
{
   %this = ServerConnection.resolveGhostID(%ghost);
   
   %this.setUrl(%url);
}
