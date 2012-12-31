


//when using physics object for web page, uncomment web page below and comment one above
//datablock RigidShapeData( Movable3DWidgetData )
datablock StaticShapeData( Movable3DWidgetData )
{	
	collisionType = "None";
	overrideBounds = "-0.5 -0.049 -0.5 0.5 0.049 0.5";
   
   category = "Web";
	shapeFile = "art/cube/shell/web_animated.dts";

	class = "Movable3DWidgetData";
	allowTranslation = true;
	allowRotation = true;
	allowScale = true;
	
   // Rigid Body
   mass = 10;
                              // if zero it defaults to object bounding box
   drag = 1;                // Drag coefficient
   bodyFriction = 0.0;
   bodyRestitution = 0.000001;
   integration = 1;           // Physics integration: TickSec/Rate
   collisionTol = 0.02;        // Collision distance tolerance
   contactTol = 0.02;          // Contact velocity tolerance
   
   minRollSpeed = 0;
   triggerDustHeight = 1;
   dustHeight = 10;
   dragForce = 30.0;
   vertFactor = 1.0;
   skins = "base";
};
datablock StaticShapeData( Static3DWidgetData : Movable3DWidgetData )
{
	class = "Static3DWidgetData";
	superClass = "Movable3DWidgetData";
   shapeFile = "art/cube/shell/web_basic.dts";
};



datablock StaticShapeData( BackdropRiver_StaticShapeData )
{	
   category = "Backdrops";
	shapeFile = "art/cube/backdrops/backdrop_river.dts";
	renderInReflections = false;
};


datablock PhysicsShapeData( AirHockeyBaseData )
{	
   category = "AirHockey";
	shapeName = "art/cube/airhockey/airhockey_base.dts";
	class = "AirHockeyTableData";
	
   lifetime = 60.0;
   lifetimeVariance = 0.0;

   velocity = 0.9;
   velocityVariance = 0;
      
   mass = 0;
   dynamicFriction = 0;
   staticFriction = 0.0;
   restitution = 0.0;
   linearDamping = 0.01;
   angularDamping = 0.1;
   linearSleepThreshold = 1.0;
   angularSleepThreshold = 1.0;
   waterDampingScale = 1.0;
   buoyancyDensity = 2;
   castShadows = "1";
   friction = "0.001";
};
datablock PhysicsShapeData( AirHockeyTableData )
{	
   category = "AirHockey";
	shapeName = "art/cube/airhockey/airhockey_table.dts";
	
   lifetime = 60.0;
   lifetimeVariance = 0.0;

   velocity = 0.9;
   velocityVariance = 0;
      
   mass = 0;
   dynamicFriction = 0;
   staticFriction = 0.0;
   restitution = 0.0;
   linearDamping = 0.01;
   angularDamping = 0.1;
   linearSleepThreshold = 0.1;
   angularSleepThreshold = 0.1;
   waterDampingScale = 1.0;
   buoyancyDensity = 2;
   castShadows = "1";
   friction = "0";
};

datablock PhysicsShapeData( AirHockeyPaddleData )
{	
   category = "AirHockey";
	shapeName = "art/cube/airhockey/airhockey_paddle.dts";
	class = "AirHockeyPuckData";

   lifetime = 60.0;
   lifetimeVariance = 0.0;

   velocity = 0.9;
   velocityVariance = 0;
      
   mass = 2;
   dynamicFriction = 0;
   staticFriction = 0.0;
   restitution = 0.00000001;
   linearDamping = 0.0025;
   angularDamping = 1;
   linearSleepThreshold = 0.1;
   angularSleepThreshold = 0.1;
   waterDampingScale = 1.0;
   buoyancyDensity = 2;
   castShadows = "1";
   friction = "0.025";
};
datablock PhysicsShapeData( AirHockeyPuckData )
{	
   category = "AirHockey";
	shapeName = "art/cube/airhockey/airhockey_puck.dts";

   lifetime = 60.0;
   lifetimeVariance = 0.0;

   velocity = 0.9;
   velocityVariance = 0;
      
   mass = 1;
   dynamicFriction = 0;
   staticFriction = 0.0;
   restitution = 0.00000001;
   linearDamping = 0.0025;
   angularDamping = 1;
   linearSleepThreshold = 0.1;
   angularSleepThreshold = 0.1;
   waterDampingScale = 1.0;
   buoyancyDensity = 2;
   castShadows = "1";
   friction = "0.0";
};




////////////////////////////////////////////////////////////////////////////////

datablock StaticShapeData( TestSpline_SSData )
{	
   class = "AISpline";
   category = "AI Splines";
	shapeFile = "art/cube/splines/testSpline.dts";
   overrideBounds = "-300 -300 -300 300 300 300";
   animation = "root";
};



datablock StaticShapeData( SardinellaPath1_SSData )
{	
   class = "AISpline";
   category = "AI Splines";
	shapeFile = "art/cube/splines/sardinella_path1.dts";
   overrideBounds = "-300 -300 -300 300 300 300";
   animation = "ambient";
};

datablock StaticShapeData( BluefinTrevallySpline1_SSData )
{	
   class = "AISpline";
   category = "AI Splines";
	shapeFile = "art/cube/splines/bluefin_trevally_spline1.dts";
   overrideBounds = "-300 -300 -300 300 300 300";
   animation = "ambient";
};



////////////////////////////////////////////////////////////////////////////////
//
datablock StaticShapeData( ViewButtonTop1Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_top1.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButtonTop2Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_top2.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButton1Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_touch1.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButton2Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_touch2.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButton3Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_touch3.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButton4Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_touch4.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( ViewButton5Data )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/screens_icon_touch5.dts";
	class = "ViewButtonData";
};
datablock StaticShapeData( LightrayButtonData )
{	
   category = "CubeDemo";
	shapeFile = "art/cube/interface/lightray_icon.dts";
//pg:stop linkage error	
   class = "LightrayButtonData";
};


// (((frameCount / fps) * secToTick) * animationSpeed) + scheduleGap
$DayLength           = 1000 * 60 * 60 * 12;
//$GapLength           = 60000;
$GapLength           = 0;
$MantaRaySet1Length  = (((3024 / 30) * 1000) / 0.85) + $GapLength;
$WhaleSet1Length     = (((2440 / 30) * 1000) / (0.75*0.9)) + $GapLength;
$WhaleSet2Length     = (((2864 / 30) * 1000) / (0.75*0.9)) + $GapLength;
$DugongSet1Length    = (((3800 / 30) * 1000) / 0.9) + $GapLength;
$DiverSet1Length     = (((8580 / 30) * 1000) / 0.95) + $GapLength;
$WhaleSharkSet1Length= (((8000 / 30) * 1000) / 0.95) + $GapLength;
$SunFishSet1Length   = (((8000 / 30) * 1000) / 1) + $GapLength;
$HammerHeadSet1Length= (((5901 / 30) * 1000) / 1) + $GapLength;
$TigerSharkSet1Length= (((7650 / 30) * 1000) / 0.95) + $GapLength;
$cinimaticsLength    = $WhaleSet1Length + $MantaRaySet1Length + $DugongSet1Length + $WhaleSet2Length + $DiverSet1Length + $SunFishSet1Length + $HammerHeadSet1Length + $TigerSharkSet1Length;



function getFishWebRoot()
{
//   return "file://" @ getWorkingDirectory() @ "/html";
   return "http://en.wikipedia.org";
   
}

/*
datablock StaticShapeData(MantaRaySet1_SSData)
{
   bubbleDataBlock = MantaRayBubbleData;
   shapeFile = "art/cube/sets/manta_ray_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=manta";  
};


datablock StaticShapeData(WhaleSet1_SSData)
{
   bubbleDataBlock = WhaleBubbleData;
   shapeFile = "art/cube/sets/whale_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=whale";  
};
datablock StaticShapeData(WhaleSet2_SSData)
{
   bubbleDataBlock = WhaleBubbleData;
   shapeFile = "art/cube/sets/whale_set2.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=whale";  
};


datablock StaticShapeData(GreyReefSharkSet1_SSData)
{
   infoUrl = getFishWebRoot() @ "/index_master.html?f=shark";
   bubbleDataBlock = GreyReefSharkBubbleData;
   shapeFile = "art/cube/sets/grey_reef_shark_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   scareBoxSize = "2 2 5";
   scare = true;
};
datablock StaticShapeData(GreyReefSharkSet2_SSData)
{
   infoUrl = getFishWebRoot() @ "/index_master.html?f=shark";
   bubbleDataBlock = GreyReefSharkBubbleData;
   shapeFile = "art/cube/sets/grey_reef_shark_set2.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   scareBoxSize = "2 2 5";
   scare = true;
};
datablock StaticShapeData(BluespottedStingRaySet1_SSData)
{
   infoUrl = getFishWebRoot() @ "/index_master.html?f=ray";  
    
   bubbleDataBlock = BlueSpottedStingRayBubbleData;
   shapeFile = "art/cube/sets/bluespotted_stingray_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
};
datablock StaticShapeData(GreenSeaTurtleSet1_SSData)
{
   bubbleDataBlock = GreenSeaTurtleBubbleData;
   class = "GreenSeaTurtleSet1_SSData";
   shapeFile = "art/cube/sets/green_sea_turtle_set1.dts";
   superClass = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=turtle";  
};
datablock StaticShapeData(WhitemouthMorayEelLeftSet1_SSData)
{
   bubbleDataBlock = WhitemouthMorayEelBubbleData;
   shapeFile = "art/cube/sets/whitemouth_morayeel_left_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=moray";  
};
datablock StaticShapeData(WhitemouthMorayEelRightSet1_SSData : WhitemouthMorayEelLeftSet1_SSData)
{
   bubbleDataBlock = WhitemouthMorayEelBubbleData;
   shapeFile = "art/cube/sets/whitemouth_morayeel_right_set1.dts";
};


datablock StaticShapeData(HermitCrab1Set1_SSData)
{
   bubbleDataBlock = HermitCrabBubbleData;
   shapeFile = "art/cube/sets/hermitcrab1_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=hermit";  
};
datablock StaticShapeData(HermitCrab2Set1_SSData : HermitCrab1Set1_SSData)
{
   bubbleDataBlock = HermitCrabBubbleData;
   shapeFile = "art/cube/sets/hermitcrab2_set1.dts";
};
datablock StaticShapeData(HermitCrab3Set1_SSData : HermitCrab1Set1_SSData)
{
   bubbleDataBlock = HermitCrabBubbleData;
   shapeFile = "art/cube/sets/hermitcrab3_set1.dts";
};
datablock StaticShapeData(HermitCrab4Set1_SSData : HermitCrab1Set1_SSData)
{
   bubbleDataBlock = HermitCrabBubbleData;
   shapeFile = "art/cube/sets/hermitcrab4_set1.dts";
};
datablock StaticShapeData(GrouperSet1_SSData)
{
   bubbleDataBlock = GrouperBubbleData;
   shapeFile = "art/cube/sets/grouper_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=potatocod";  
};
*/
datablock StaticShapeData(DugongSet1_SSData)
{
   class = "DugongSet1_SSData";
   bubbleDataBlock = DugongBubbleData;
   shapeFile = "art/cube/sets/dugong_set1.dts";
   superClass = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=dugong";  
};
/*
datablock StaticShapeData(DiverSet1_SSData)
{
   bubbleDataBlock = DiverBubbleData;
   shapeFile = "art/cube/sets/diver_set.dts";
   class = "DiverSet1_SSData";
   superClass = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_intro.html?f=futureofthereef";  
};
datablock StaticShapeData(WhaleSharkSet1_SSData)
{
   bubbleDataBlock = WhaleSharkBubbleData;
   shapeFile = "art/cube/sets/whaleshark_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=whaleshark";  
};
datablock StaticShapeData(SunFishSet1_SSData)
{
   bubbleDataBlock = SunFishBubbleData;
   shapeFile = "art/cube/sets/sunfish_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=sunfish";  
};
datablock StaticShapeData(HammerHeadSet1_SSData)
{
   bubbleDataBlock = HammerHeadBubbleData;
   shapeFile = "art/cube/sets/hammerhead_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=hammerhead";  
};
datablock StaticShapeData(TigerSharkSet1_SSData)
{
   bubbleDataBlock = TigerSharkBubbleData;
   shapeFile = "art/cube/sets/tigershark_set1.dts";
   class = "StaticShapeAutoPlay";
   animation = "ambient";
   overrideBounds = "-100000 -100000 -100000 100000 100000 100000";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=tigershark";  
};


*/
datablock StaticShapeData( Starfish_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = StarfishBubble;
   category = "Static AI";
	shapeFile = "art/cube/sets/starfish.dts";
   animation = "ambient";
   interactAnim = "interact";
   inputEventsMethod = "Mesh";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=starfish";  
};
/*
datablock StaticShapeData( Urchin_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = UrchinBubble;
   category = "Static AI";
	shapeFile = "art/cube/sets/urchin.dts";
   animation = "ambient";
   interactAnim = "interact";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=urchin";  
   inputEventsMethod = "BoundingBox";
};


*/
datablock StaticShapeData( Clam_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = ClamBubble;
   category = "Static AI";
	shapeFile = "art/cube/sets/clam.dts";
   animation = "ambient";
   interactAnim = "close";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=clam";  
   inputEventsMethod = "Mesh";

};
/*
datablock StaticShapeData( PorcelainCrab_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = PorcelainCrabBubble;
   category = "Static AI";
	shapeFile = "art/cube/sets/porcelain_crab.dts";
   animation = "ambient";
   interactAnim = "interact";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=porcelaincrab"; 
   inputEventsMethod = "BoundingBox"; 
};

datablock StaticShapeData( Nudibranch_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = NudibranchBubbleData;
   category = "Static AI";
	shapeFile = "art/cube/sets/nudibranch.dts";
   animation = "ambient";
   interactAnim = "interact";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=nudibranch"; 
   overrideBounds = "-0.025 -0.1 0 0.025 0.1 0.04"; 
   inputEventsMethod = "BoundingBox";
};
datablock StaticShapeData( Seacucumber_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = SeacucumberBubbleData;
   category = "Static AI";
	shapeFile = "art/cube/sets/seacucumber.dts";
   animation = "ambient";
   interactAnim = "interact";
   inputEventsMethod = "Mesh";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=cucumber";  
};
datablock StaticShapeData( Shrimp_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = ShrimpBubbleData;
   category = "Static AI";
	shapeFile = "art/cube/sets/shrimp.dts";
   animation = "ambient";
   interactAnim = "interact";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=harlequinshrimp";  
   inputEventsMethod = "BoundingBox";
};
datablock StaticShapeData( Fanworm_SSData )
{	
   class = "StaticShapeAutoPlay";
   bubbleDataBlock = FanwormBubbleData;
   category = "Static AI";
	shapeFile = "art/cube/sets/fanworm.dts";
   animation = "ambient";
   interactAnim = "interact";
   infoUrl = getFishWebRoot() @ "/index_master.html?f=fanworm";  
   overrideBounds = "-0.07 -0.07 0 0.07 0.07 0.16"; 
   inputEventsMethod = "BoundingBox";
};

*/

//**********************************************************************
// bubble gui components
//**********************************************************************
datablock StaticShapeData(BubbleBodyData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/bubbleui_front_active.dts";
	overrideBounds = "-1 -0.4 -1 1 0.4 1";
};
//---------------
datablock StaticShapeData( BubbleButtonInformationUpData : BubbleBodyData )
{	
   shapeFile = "art/cube/interface/bubbleui_info_active.dts";
   class = "BubbleButtonData";
	overrideBounds = "-0.4 -0.2 -0.4 0.4 0.2 0.4";
};
datablock StaticShapeData( BubbleButtonInformationDownData : BubbleButtonInformationUpData )
{	
   shapeFile = "art/cube/interface/bubbleui_info_inactive.dts";
   class = "BubbleButtonData";
};
//---------------
datablock StaticShapeData( BubbleButtonCancelUpData : BubbleButtonInformationUpData )
{	
   shapeFile = "art/cube/interface/bubbleui_close_active.dts";
   class = "BubbleButtonData";
};
datablock StaticShapeData( BubbleButtonCancelDownData : BubbleButtonInformationUpData )
{	
   shapeFile = "art/cube/interface/bubbleui_close_inactive.dts";
   class = "BubbleButtonData";
};
//---------------

datablock StaticShapeData( BubbleButtonDragData : BubbleButtonInformationUpData )
{	
   shapeFile = "art/cube/interface/bubble_dragbar.dae";
   class = "BubbleButtonData";
};
datablock StaticShapeData( BubbleButtonTouchData : BubbleButtonInformationUpData )
{	
   shapeFile = "art/cube/interface/bubble_touch.dae";
   class = "BubbleButtonData";
};

datablock StaticShapeData( BubbleButtonTimerData : BubbleBodyData )
{	
   shapeFile = "art/cube/interface/bubbleui_timer.dts";
   class = "BubbleButtonData";
};
datablock StaticShapeData( BubbleButtonTitleData : BubbleBodyData )
{	
   shapeFile = "art/cube/interface/bubbleui_title.dts";
   class = "BubbleButtonData";
};


datablock StaticShapeData( CogsSData )
{	
   category = "Reef Interface";
   shapeFile = "art/cube/interface/bubble_anim.dae";
   class = "BubbleButtonData";
 	overrideBounds = "-0.06 -06 -06 06 06 06";
  
};
datablock StaticShapeData( BadCogsSData : CogsSData )
{	
   shapeFile = "art/cube/interface/badcog.dae";
   class = "BubbleButtonData";
};

datablock StaticShapeData(BubbleBodyBackData : BubbleBodyData)
{
   category = "Reef Interface";
   shapeFile = "art/cube/interface/bubbleui_back.dts";
};



