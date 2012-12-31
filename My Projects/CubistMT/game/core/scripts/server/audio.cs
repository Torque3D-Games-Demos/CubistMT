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

function ServerPlay2D(%profile)
{
   // Play the given sound profile on every client.
   // The sounds will be transmitted as an event, not attached to any object.
   for(%idx = 0; %idx < ClientGroup.getCount(); %idx++)
      ClientGroup.getObject(%idx).play2D(%profile);
}
function ServerPlay3D(%profile,%transform)
{
   // Play the given sound profile at the given position on every client
   // The sound will be transmitted as an event, not attached to any object.
   for(%idx = 0; %idx < ClientGroup.getCount(); %idx++)
      ClientGroup.getObject(%idx).play3D(%profile,%transform);
}
// like above except that the sound is only played if it is in the 
// horizontal angular range of the client
function ServerPlay3DAngle(%profile,%transform)
{
   // Play the given sound profile at the given position on every client
   // The sound will be transmitted as an event, not attached to any object.
   
   
   
   if($currentViewId<0 ||  $currentViewId>5  )
   {
    //  ServerPlay2D(%profile);
      ServerPlay3D(%profile,%transform);
   }
   else
   {
      %position = getWords(%transform, 0, 2);
      %fraction = getFractionAcross(%position);   
      for(%idx = 0; %idx < ClientGroup.getCount(); %idx++)
      {
         %cl = ClientGroup.getObject(%idx);
         %clientID = %cl.clientId;
         %offset = getOffset($currentViewId, %clientID);
         %fraction0 = getWord(%offset,0);
         %fraction1 = getWord(%offset,2);
         if( %fraction>=%fraction0 && %fraction<=%fraction1 )
         {
          //  %cl.play2D(%profile);
            %cl.play3D(%profile, %transform);
            
            %idx += 1;
            if(%idx < ClientGroup.getCount())
               ClientGroup.getObject(%idx).play3D(%profile, %transform);
            %idx -= 2;
            if(%idx >= 0)
               ClientGroup.getObject(%idx).play3D(%profile, %transform);
            
            break;
         }
      }
   }
}
// given a 3d position, work out the fraction it is across the full viewable angle of the reef
function getFractionAcross(%position)
{
   //get camera position & orientation
	%client = ClientGroup.getObject(0);
   %camera = %client.camera;
   %forward = %camera.getForwardVector();   
   %right = %camera.getRightVector();   
   %up = %camera.getUpVector();   
   %pos = %camera.getPosition();   
   
   
   %tilt = mDegToRad($CAMERA_TILT);
   %sin = mSin(%tilt);
   %cos = mCos(%tilt);
   %pos = "0 0 0";
   %right = "1 0 0";
   
   %forward = "0" SPC %cos SPC (%sin*-1.0);
   %up = "0" SPC %sin SPC %cos;
    
   
   //get x & z position in camera space & get horizontal intercept on front clipping plane 1 unit from camera
   %rel = VectorSub(%position, %pos);
   %localX = VectorDot(%rel, %right);
   %localZ = VectorDot(%rel, %forward);
   %localTan = %localX / %localZ;
   
   //range of view on front clipping plane
   %field = $CAMERA_FIELD;
   %xRange = mTan( mDegToRad( %field * 0.5 ) );
   
   %fraction = (%localTan + %xRange) / (2*%xRange) ;
   return %fraction;
}
function getOffset(%viewId, %clientId)
{
   switch(%viewId){
   case 0:
      return $pref::Cube::mosaicOffset[%clientId];
   case 1:  
      return $pref::Cube::demo1Offset[%clientId];
   case 2:  
      return $pref::Cube::demo2Offset[%clientId];
   case 3:  
      return $pref::Cube::demo3Offset[%clientId];
   case 4:  
      return $pref::Cube::demo4Offset[%clientId];
   case 5:  
      return $pref::Cube::demo5Offset[%clientId];
   default:
      return "";
   }
}


