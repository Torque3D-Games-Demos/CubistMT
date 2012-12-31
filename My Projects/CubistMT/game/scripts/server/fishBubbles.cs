//***********************************************************
// bubble fish
//***********************************************************
// in bubbleOffset, z->moves up and down y->centers horizontally


function makeRotationMatrix(%angX, %angY, %angZ)
{
   %matrix_x = MatrixCreate("0 0 0", "1 0 0" SPC %angX);
   %matrix_y = MatrixCreate("0 0 0", "0 1 0" SPC %angY);
   %matrix_z = MatrixCreate("0 0 0", "0 0 1" SPC %angZ);

   %sum = MatrixMultiply(%matrix_y, %matrix_x);
   %sum = MatrixMultiply(%matrix_z, %sum);   
   return %sum;
}


datablock StaticShapeData(BubbleFishData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/bubble_unknown.dae";
   bubbleScale = "0.5";
   bubbleOffset = "0 0 0";
   bubbleRotation = makeRotationMatrix(0,0,0);
   animName = "swim_root"; //anim to play in bubble
   titleMaterial = "title_bad";
};

datablock StaticShapeData(ClamBubble : BubbleFishData)
{
   shapeFile = "art/cube/sets/clam.dts";
   bubbleScale = "0.2";
   bubbleOffset = "0 0 -0.03";
   animName = "ambient";
   titleMaterial = "title_clam";
};
/*
datablock StaticShapeData(PorcelainCrabBubble : BubbleFishData)
{
   shapeFile = "art/cube/sets/porcelain_crab.dts";
   bubbleScale = "0.9";
   bubbleOffset = "0 0 -0.03";
   animName = "ambient";
   titleMaterial = "title_porcelaincrab";
};
*/
datablock StaticShapeData(HarlequinTuskFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/harlequintuskfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   bubbleScale = "0.25";
   bubbleOffset = "0 0.02 -0.03";
   titleMaterial = "title_tuskfish";
};
datablock StaticShapeData(MoorishIdolFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/moorishidol.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   bubbleScale = "0.4";
   bubbleOffset = "0 0.015 -0.04";
   titleMaterial = "title_moorish";
};
datablock StaticShapeData(BlueTangFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/bluetang.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_bluetang";
   bubbleScale = "0.26";
   bubbleOffset = "0 0.015 -0.03";
};
datablock StaticShapeData(ClownFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/clownfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_clownfish";
   bubbleScale = "0.6";
   bubbleOffset = "0 0.01 -0.04";
};
datablock StaticShapeData(SeahorseBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/seahorse.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_seahorse";
   bubbleScale = "0.5";
   bubbleOffset = "0 0 -0.065";
};
datablock StaticShapeData(CuttleFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/cuttlefish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_cuttlefish";
   bubbleScale = "0.18";
   bubbleOffset = "0 -0.025 -0.01";
};
datablock StaticShapeData(SardinellaFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/sardinella.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_sardinella";
   bubbleScale = "0.4";
   bubbleOffset = "0 -0.007 0";
   animName = "swim_forward";
};
datablock StaticShapeData(BluefinTrevallyBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/bluefin_trevally.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_trevally";
   bubbleScale = "0.072";
   bubbleOffset = "0 0.025 -0.02";
   animName = "swim_forward";
};
datablock StaticShapeData(OrangeBasseletBubble : BubbleFishData)
{
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_basslet";
   bubbleScale = "0.65";
   bubbleOffset = "0 0.02 -0.02";
   titleMaterial = "title_basslet";
};
datablock StaticShapeData(MoonJellyfishBubble : BubbleFishData)
{
   shapeFile = "art/cube/ai/moon_jellyfish.dts";
   titleMaterial = "title_jellyfish";
   bubbleScale = "0.17";
   bubbleOffset = "0 0 -0.03";
};
datablock StaticShapeData(SnapperBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/snapper.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_snapper";
   bubbleScale = "0.23";
   bubbleOffset = "0 0.014 0";
};
datablock StaticShapeData(LionFishBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/lionfish.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_lionfish";
   bubbleScale = "0.2";
   bubbleOffset = "0 0 -0.01";
};
datablock StaticShapeData(BluestreakedWrasseBubble : BubbleFishData)
{
   shapeFile = "art/cube/ai/bluestreaked_wrasse.dts";
   titleMaterial = "title_cleaner";
   bubbleScale = "0.55";
   bubbleOffset = "0 0.02 -0.02";
   animName = "swim_forward"; //anim to play in bubble
};
datablock StaticShapeData(ParrotfishFemaleBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/parrotfish_female.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_parrotfish";
   bubbleScale = "0.15";
   bubbleOffset = "0 0.02 -0.03";
};
datablock StaticShapeData(ParrotfishJuvenileBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/parrotfish_juvenile.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_parrotfish";
   bubbleScale = "1.0";
   bubbleOffset = "0 0.02 -0.03";
};
datablock StaticShapeData(ParrotfishMaleBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/parrotfish_male.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_parrotfish";
   bubbleScale = "0.10";
   bubbleOffset = "0 0.02 -0.03";
};
datablock StaticShapeData(RemoraBubble : BubbleFishData)
{
//   shapeFile = "art/cube/ai/remora.dts";
   shapeFile = "art/cube/ai/orange_basselet.dts";
   titleMaterial = "title_remora";
   bubbleScale = "0.12";
   bubbleOffset = "0 0.03 -0.01";
};
datablock StaticShapeData(PufferFishBubble : BubbleFishData)
{
   shapeFile = "art/cube/ai/pufferfish.dts";
   titleMaterial = "title_pufferfish";
   bubbleScale = "0.9";
   bubbleOffset = "0 0.01 -0.0";
   animName = "swim_root";
};
/*
datablock StaticShapeData(UrchinBubble : BubbleFishData)
{
   shapeFile = "art/cube/sets/urchin.dts";
   titleMaterial = "title_urchin";
   bubbleScale = "0.25";
   bubbleOffset = "0 0 -0.02";
   animName = "ambient";
};
*/
datablock StaticShapeData(StarfishBubble : BubbleFishData)
{
   shapeFile = "art/cube/sets/starfish.dts";
   titleMaterial = "title_starfish";
   bubbleScale = "0.20";
   bubbleOffset = "0 0 0";
   animName = "ambient";
};
/*
datablock StaticShapeData(DendroBubble1 : BubbleFishData)
{
	shapeFile = "art/cube/sets/dendro1.dae";
   titleMaterial = "title_dendro";
   bubbleScale = "0.3";
   bubbleOffset = "0 -0.05 0";
};
datablock StaticShapeData(DendroBubble2 : BubbleFishData)
{
	shapeFile = "art/cube/sets/dendro2.dae";
   titleMaterial = "title_dendro";
   bubbleScale = "0.3";
   bubbleOffset = "0 -0.03 0";
};
datablock StaticShapeData(DendroBubble3 : BubbleFishData)
{
	shapeFile = "art/cube/sets/dendro3.dae";
   titleMaterial = "title_dendro";
   bubbleScale = "0.3";
   bubbleOffset = "0 -0.04 0";
};

datablock StaticShapeData(HermitCrabBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/hermitcrab_bubble.dts";
   titleMaterial = "title_hermit";
   bubbleScale = "0.3";
   bubbleOffset = "0 0 -0.02";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(GrouperBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/grouper_bubble.dts";
   titleMaterial = "title_potatocod";
   bubbleScale = "0.00045";
   bubbleOffset = "0 -0.02 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
*/

datablock StaticShapeData(DugongBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/dugong_bubble.dts";
   titleMaterial = "title_dugong";
   bubbleScale = "0.025";
   bubbleOffset = "0 -0.01 -0.01";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

/*
datablock StaticShapeData(DiverBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/diver_bubble.dts";
//hack, doesnt exist yet   titleMaterial = "title_basslet";
   bubbleScale = "0.04";
   bubbleOffset = "0 -0.01 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

datablock StaticShapeData(BlueSpottedStingRayBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/bluespotted_stingray_bubble.dts";
   titleMaterial = "title_ray";
   bubbleScale = "0.17";
   bubbleOffset = "0 -0.015 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(GreenSeaTurtleBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/green_sea_turtle_bubble.dts";
   titleMaterial = "title_turtle";
   bubbleScale = "0.060";
   bubbleOffset = "0 0 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(MantaRayBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/manta_ray_bubble.dts";
   titleMaterial = "title_manta";
   bubbleScale = "0.022";
   bubbleOffset = "0 -0.01 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

datablock StaticShapeData(GreyReefSharkBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/grey_reef_shark_bubble.dts";
   titleMaterial = "title_shark";
   bubbleScale = "0.03";
   bubbleOffset = "0 0 0";
   bubbleRotation = makeRotationMatrix(90,180,180);

   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(WhitemouthMorayEelBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/whitemouth_morayeel_bubble.dts";
   titleMaterial = "title_moray";
   bubbleScale = "0.003";
   bubbleOffset = "0 0 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

datablock StaticShapeData(FanwormBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/fanworm.dts";
   titleMaterial = "title_fanworm";
   bubbleScale = "0.5";
   bubbleOffset = "0 0 -0.029";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(ShrimpBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/shrimp.dts";
   titleMaterial = "title_harlequinshrimp";
   bubbleScale = "0.8";
   bubbleOffset = "0 0 -0.01";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

datablock StaticShapeData(SeacucumberBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/seacucumber.dts";
   titleMaterial = "title_cucumber";
   bubbleScale = "0.8";
   bubbleOffset = "0 0 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};

datablock StaticShapeData(NudibranchBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/nudibranch.dts";
   titleMaterial = "title_nudibranch";
   bubbleScale = "0.8";
   bubbleOffset = "0 0 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(WhaleSharkBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/whaleshark_bubble.dts";
   titleMaterial = "title_whaleshark";
   bubbleScale = "0.008";
   bubbleOffset = "0 -0.02 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(WhaleBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/ai/whale_philip_stage2.dts";
   titleMaterial = "title_whale";
   bubbleScale = "0.010";
   bubbleOffset = "0 0.015 0";
   animName = "swim_forward";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(SunfishBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/sunfish_bubble.dts";
   titleMaterial = "title_sunfish";
   bubbleScale = "0.03";
   bubbleOffset = "0 -0.02 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(HammerHeadBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/hammerhead_bubble.dts";
   titleMaterial = "title_hammerhead";
   bubbleScale = "0.00027";
   bubbleOffset = "0 -0.01 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
datablock StaticShapeData(TigerSharkBubbleData : BubbleFishData)
{
	shapeFile = "art/cube/sets/tigershark_bubble.dts";
   titleMaterial = "title_tigershark";
   bubbleScale = "0.015";
   bubbleOffset = "0 -0.02 0";
   animName = "ambient";
   overrideBounds = "-0.2 -0.2 -0.2 0.2 0.2 0.2";
};
*/