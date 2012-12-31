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

// This is the default save location for any Decal datablocks created in the
// Decal Editor (this script is executed from onServerCreated())

datablock DecalData(ScorchBigDecal)
{
   Material = "DECAL_scorch";
   size = "5.0";
   lifeSpan = "50000";
};

datablock DecalData(ScorchRXDecal)
{
   Material = "DECAL_RocketEXP";
   size = "5.0";
   lifeSpan = "20000";
   randomize = "1";
   texRows = "2";
   texCols = "2";
   clippingAngle = "80";
   screenStartRadius = "200";
   screenEndRadius = "100";
};

datablock DecalData(LeavesDebris)
{
   Material = "DECAL_leavesdebris";
   textureCoordCount = "0";
   fadeStartPixelSize = "800";
};

datablock DecalData(MossPatch)
{
   Material = "DECAL_moss";
   fadeStartPixelSize = "800";
   textureCoordCount = "0";
};

/*
datablock DecalData(Needles)
{
   Material = "DECAL_needles";
   fadeStartPixelSize = "800";
   textureCoordCount = "0";
};
*/

datablock DecalData(DECAL_sandroadend)
{
   Material = "DECAL_sandroadend_material";
   textureCoordCount = "0";
   renderPriority = "1";
   fadeStartPixelSize = "800";
};

/*
datablock DecalData(WornEarth)
{
   textureCoordCount = "0";
   Material = "DECAL_wornearth";
   fadeStartPixelSize = "800";
};

datablock DecalData(ExposedRock)
{
   textureCoordCount = "0";
   Material = "DECAL_exposedrock";
   fadeStartPixelSize = "800";
};

datablock DecalData(BeachDebris_01)
{
   textureCoordCount = "0";
   Material = "DECAL_beachdebris01";
   fadeStartPixelSize = "800";
};


datablock DecalData(shorerock)
{
   textureCoordCount = "0";
   Material = "DECAL_shorerock01";
   fadeStartPixelSize = "800";
};


datablock DecalData(seaweed)
{
   textureCoordCount = "0";
   Material = "DECAL_seaweed01";
   fadeStartPixelSize = "400";
   fadeEndPixelSize = "50";
   renderPriority = "9";
};

datablock DecalData(cliffcover)
{
   textureCoordCount = "0";
   Material = "DECAL_cliffcover01";
   fadeStartPixelSize = "1000";
   clippingAngle = "90";
};


datablock DecalData(volcanowall)
{
   textureCoordCount = "0";
   Material = "DECAL_volcanowall01";
   fadeStartPixelSize = "1000";
   clippingAngle = "90";
};
*/


datablock DecalData(DirtTrailEnd)
{
   Material = "DecalDirtTrail";
   textureCoordCount = "0";
};

datablock DecalData(bulletHoleDecal)
{
   Material = "DECAL_bulletHole";
   size = "0.25";
   lifeSpan = "5000";
   randomize = "1";
   texRows = "2";
   texCols = "2";
   screenStartRadius = "20";
   screenEndRadius = "5";
   clippingAngle = "180";
};
