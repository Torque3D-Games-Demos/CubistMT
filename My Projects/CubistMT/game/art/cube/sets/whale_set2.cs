
singleton TSShapeConstructor(Whale_set2Dts)
{
   baseShape = "./whale_set2.dts";
};

function Whale_set2Dts::onLoad(%this)
{
   %this.addNode("mount0", "Filip_S1_Animate_Filip_S1_Jaw_Tip", "0 0 0 0 0 1 0", "0");
   %this.addNode("mount1", "Filip_S1_Animate1_ACT_Whale_S1_SKL_Whale_Jaw_Tip", "0 0 0 0 0 1 0", "0");
   %this.addNode("mount2", "Filip_S1_Animate1_ACT_Whale_S1_SKL_Whale_Fluke_Base", "0 0 0 0 0 1 0", "0");
}
