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

function TimeOfDay::onAdd( %this )
{
}

function TimeOfDay::onTimeEvent( %this, %identifier, %currentTime, %currentElevation )
{
}

function SimSet::setAllLightsEnabled( %this, %enabled )
{
   for ( %i = 0; %i < %this.getCount(); %i++ )
   {
      %light = %this.getObject( %i );  
      %light.setLightEnabled( %enabled );
   }
}

function TimeOfDay::onAnimateStart( %this )
{   
}

function TimeOfDay::onAnimateDone( %this )
{
}

function TimeJumpTrigger::onAdd( %this, %trigger )
{
   // Setup default parameters.
   if ( %trigger.todObject $= "" )   
      %trigger.todObject = "theTod";
   if ( %trigger.targetTime $= "" )
      %trigger.targetTime = 90; // sun elevation 0 to 360  
   if ( %trigger.speed $= "" )
      %trigger.speed = 10; // degrees per second
}

function TimeJumpTrigger::onEnterTrigger(%this,%trigger,%obj)
{
   // Only player objects can trigger time of day changes.
   if ( !%obj.isMemberOfClass( "Player" ) )
      return;
      
   // Make sure we have a time of day object.                     
   %tod = %trigger.todObject;   
   if ( !isObject( %tod ) )   
      return;
                     
   // Animate it.
   %tod.animate( %trigger.targetTime, %trigger.speed );
   
   switch(%trigger.musicID)
   {
      case 0:
      $BGMusic.play();
      $CaveMusic.stop();
      
      case 1:
      $CaveMusic.play();
      $BGMusic.stop();
      
      case 2:
      $CaveMusic.stop();      
      $BGMusic.stop();
   }
}
