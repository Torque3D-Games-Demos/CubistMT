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
// Reef
//-----------------------------------------------------------------------------

/*
$pref::cube::earOffset0 = "-6.3189 6 1";
$pref::cube::earOffset1 = "-4.9147 6 1";
$pref::cube::earOffset10 = "0 6 1";
$pref::cube::earOffset2 = "-3.5105 6 1";
$pref::cube::earOffset3 = "-2.1063 6 1";
$pref::cube::earOffset4 = "-0.7021 6 1";
$pref::cube::earOffset5 = "0.7021 6 1";
$pref::cube::earOffset6 = "2.1063 6 1";
$pref::cube::earOffset7 = "3.5105 6 1";
$pref::cube::earOffset8 = "4.9147 6 1";
$pref::cube::earOffset9 = "6.3189 6 1";
*/

datablock SFXDescription( AudioReefBackgroundLoop : AudioMusic )
{
   is3D        = false;
   isLooping   = true;
   isStreaming = true;
   volume = 1;
   fadeLoops = false;
   priority = 1000;
   
   
};
datablock SFXDescription( AudioReefGui : AudioEffect )
{
   is3D              = true;
//   ReferenceDistance = 0.01;
//   MaxDistance       = 10.0;
   ReferenceDistance = 0.5;
   MaxDistance       = 10.0;
   volume = 0.9;
 //   volume = 1;
};
//--------
datablock SFXDescription( AudioReefSwish : AudioGui )
{
   is3D              = true;
//   ReferenceDistance = 0.01;
//   MaxDistance       = 10.0;
   ReferenceDistance = 0.5;
   MaxDistance       = 7.0;
   volume = 0.2;
//   volume = 1.0;
};
datablock SFXDescription( AudioReefSwishB : AudioReefSwish )
{
   pitch = 0.5;
};
datablock SFXDescription( AudioReefSwishC : AudioReefSwish )
{
   pitch = 0.75;
};


datablock SFXDescription( AudioReefCinimaticMono : AudioEffect )
{
   is3D        = true;
   isLooping   = false;
   isStreaming = true;
   
//   isStreaming = true;
   ReferenceDistance = 1.0;
   MaxDistance       = 50.0;

  // ReferenceDistance = 10;
  // MaxDistance       = 60.0;
   volume = 0.8;
};



// Always declare audio Descriptions (the type of sound) before Profiles (the
// sound itself) when creating new ones.

// ----------------------------------------------------------------------------
// Now for the profiles - these are the usable sounds
// ----------------------------------------------------------------------------

datablock SFXProfile(ThrowSnd)
{
   filename = "art/sound/throw";
   description = AudioClose3d;
   preload = false;
};

datablock SFXProfile(OOBWarningSnd)
{
   filename = "art/sound/orc_pain";
   description = "AudioLoop2D";
   preload = false;
};
//**********************************************************
// reef gui sounds
//**********************************************************
datablock SFXProfile(SonarBlipSnd)
{
   filename = "art/cube/sound/reef interface/Sonar Blip";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(Button1Snd)
{
   filename = "art/cube/sound/reef interface/Button 1";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(CloseSnd)
{
   filename = "art/cube/sound/reef interface/close";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(Bubble1Snd)
{
   filename = "art/cube/sound/reef interface/Bubble 1";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(Bubble2Snd)
{
   filename = "art/cube/sound/reef interface/Bubble 2";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(Bubble3Snd)
{
   filename = "art/cube/sound/reef interface/Bubble 3";
   description = AudioReefGui;
   preload = true;
};
datablock SFXProfile(Bubble4Snd)
{
   filename = "art/cube/sound/reef interface/Bubble 4";
   description = AudioReefGui;
   preload = true;
};
//**********************************************************
// water swishing
//**********************************************************
datablock SFXProfile(Swish1Snd)
{
   filename = "art/cube/sound/reef interface/Swish 1";
   description = AudioReefSwish;
   preload = true;
};
datablock SFXProfile(Swish2Snd)
{
   filename = "art/cube/sound/reef interface/Swish 2";
   description = AudioReefSwish;
   preload = true;
};
datablock SFXProfile(Swish3Snd)
{
   filename = "art/cube/sound/reef interface/Swish 3";
   description = AudioReefSwish;
   preload = true;
};
//----
datablock SFXProfile(Swish4Snd)
{
   filename = "art/cube/sound/reef interface/Swish 1";
   description = AudioReefSwishB;
   preload = true;
};
datablock SFXProfile(Swish5Snd)
{
   filename = "art/cube/sound/reef interface/Swish 2";
   description = AudioReefSwishB;
   preload = true;
};
datablock SFXProfile(Swish6Snd)
{
   filename = "art/cube/sound/reef interface/Swish 3";
   description = AudioReefSwishB;
   preload = true;
};
//----
datablock SFXProfile(Swish7Snd)
{
   filename = "art/cube/sound/reef interface/Swish 1";
   description = AudioReefSwishC;
   preload = true;
};
datablock SFXProfile(Swish8Snd)
{
   filename = "art/cube/sound/reef interface/Swish 2";
   description = AudioReefSwishC;
   preload = true;
};
datablock SFXProfile(Swish9Snd)
{
   filename = "art/cube/sound/reef interface/Swish 3";
   description = AudioReefSwishC;
   preload = true;
};
//----

$swishList[0] = Swish2Snd;
$swishList[1] = Swish3Snd;
$swishList[2] = Swish5Snd;
$swishList[3] = Swish6Snd;
$swishList[4] = Swish8Snd;
$swishList[5] = Swish9Snd;

$swishListCount = 6;

//**********************************************************
// reef background
//**********************************************************
datablock SFXProfile(StereoUnderwaterHumSnd)
{
   filename = "art/cube/sound/Stereo Underwater Hum.ogg";
   description = AudioReefBackgroundLoop;
   preload = true;
};
/*
datablock SFXProfile(StereoAmbienceSnd)
{
   filename = "art/cube/sound/Reef Sounds/Stereo Ambience";
   description = AudioReefBackgroundLoop;
   preload = true;
};
datablock SFXProfile(StereoBubblesSnd)
{
   filename = "art/cube/sound/Reef Sounds/Stereo Bubbles";
   description = AudioReefBackgroundLoop;
   preload = true;
};
*/


datablock SFXProfile(WhaleCallsSnd)
{
   filename = "art/cube/sound/WhaleNurse  - Whale calls.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
/*
datablock SFXProfile(WhalePassSnd)
{
   filename = "art/cube/sound/Whale nurse/Whale Pass";
   description = AudioReefCinimaticMono;
   preload = true;
};
*/
datablock SFXProfile(WhaleSwimSnd)
{
   filename = "art/cube/sound/WhaleNurse  - Whale move.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};

datablock SFXProfile(WhaleDiveBreachesSnd)
{
   filename = "art/cube/sound/Whale dive/WhaleDive - water sounds.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
datablock SFXProfile(WhaleDiveCallSnd)
{
   filename = "art/cube/sound/WhaleDive - Whale call.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};

datablock SFXProfile(DiverSet1DiverSnd)
{
   filename = "art/cube/sound/Diver Final Slow.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
datablock SFXProfile(DiverSet1BoatSnd)
{
   filename = "art/cube/sound/Boat Final Slow.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
/*
datablock SFXProfile(DugongSet1WaterSnd)
{
   filename = "art/cube/sound/Dugong Animation/Breaching Sounds.L.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
*/
datablock SFXProfile(DugongSet1SwimSnd)
{
   filename = "art/cube/sound/Dugong.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};

datablock SFXProfile(TurtleSet1LeftSnd)
{
   filename = "art/cube/sound/Turtle Animation/Turtle L.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};
datablock SFXProfile(TurtleSet1RightSnd)
{
   filename = "art/cube/sound/Turtle Animation/Turtle R.ogg";
   description = AudioReefCinimaticMono;
   preload = true;
};






/*

datablock SFXDescription()
{
   mFilename
};


datablock SFXProfile()
{
   filename = ""
};

*/

