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

// This is the default save location for any TSForestItemData(s) created in the
// Forest Editor Editor (this script is executed from onServerCreated())

datablock TSForestItemData(ExampleForestMesh)
{
   shapeFile = "art/shapes/trees/defaulttree/defaulttree.dae";
   internalName = "ExampleForestMesh";
   windScale = "1";
   trunkBendScale = "0.02";
   branchAmp = "0.05";
   detailAmp = "0.1";
   detailFreq = "0.2";
   mass = "5";
   rigidity = "20";
   dampingCoefficient = "0.2";
   tightnessCoefficient = "4";
};

datablock TSForestItemData(Dendro1ForestMesh)
{
   shapeFile = "art/cube/sets/dendro1.dae";
   internalName = "Dendro1ForestMesh";
   windScale = "1";
   trunkBendScale = "0.03";
   radius = "0.1";
   branchAmp = "0.005";
   detailAmp = "0.001";
   detailFreq = "0.001";
   mass = "0.5";
   rigidity = "2";
   dampingCoefficient = "0.5";
   tightnessCoefficient = "0.7";
   collidable = false;
};

datablock TSForestItemData(Dendro2ForestMesh : Dendro1ForestMesh)
{
   shapeFile = "art/cube/sets/dendro2.dae";
   internalName = "Dendro2ForestMesh";
};
datablock TSForestItemData(Dendro3ForestMesh : Dendro1ForestMesh)
{
   shapeFile = "art/cube/sets/dendro3.dae";
   internalName = "Dendro3ForestMesh";
};

datablock TSForestItemData(SeaGrass1ForestMesh)
{
   shapeFile = "art/cube/sets/sea_grass1.dae";
   internalName = "SeaGrass1ForestMesh";
   windScale = "0.4";
   trunkBendScale = "0.05";
   radius = "0.1";
   branchAmp = "0";
   detailAmp = "1";
   detailFreq = "0.000001";
   mass = "0.01";
   rigidity = "1";
   dampingCoefficient = "0.0001";
   tightnessCoefficient = "0.7";
   collidable = false;
};
datablock TSForestItemData(SeaGrass2ForestMesh : SeaGrass1ForestMesh)
{
   shapeFile = "art/cube/sets/sea_grass2.dae";
   internalName = "SeaGrass2ForestMesh";
};
datablock TSForestItemData(SeaGrass3ForestMesh : SeaGrass1ForestMesh)
{
   shapeFile = "art/cube/sets/sea_grass3.dae";
   internalName = "SeaGrass3ForestMesh";
};
datablock TSForestItemData(SeaGrass4ForestMesh : SeaGrass1ForestMesh)
{
   shapeFile = "art/cube/sets/sea_grass4.dae";
   internalName = "SeaGrass4ForestMesh";
};

datablock TSForestItemData(AnemoneLeftForestMesh)
{
   shapeFile = "art/cube/sets/anemone_set1.dae";
   internalName = "AnemoneLeftForestMesh";
   windScale = "1";
   trunkBendScale = "0.01";
   radius = "0.1";
   branchAmp = "0.005";
   detailAmp = "0.02";
   detailFreq = "0.04";
   mass = "1";
   rigidity = "2";
   dampingCoefficient = "0.9";
   tightnessCoefficient = "0.7";
   collidable = false;
};
datablock TSForestItemData(AnemoneRightForestMesh : AnemoneLeftForestMesh)
{
   shapeFile = "art/cube/sets/anemone_set2.dae";
   internalName = "AnemoneRightForestMesh";
};



