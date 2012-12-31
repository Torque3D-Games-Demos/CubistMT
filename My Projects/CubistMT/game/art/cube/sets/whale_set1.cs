
singleton TSShapeConstructor(Whale_set1Dts)
{
   baseShape = "./whale_set1.dts";
};

function Whale_set1Dts::onLoad(%this)
{
   %this.addNode("mount0", "Root", "8 25 0 1 0 0 0", "1");
   %this.addNode("mount1", "Filip_S1_Animate1_ACT_Whale_S1_SKL_Whale_Jaw_Tip", "0 0 0 0 0 1 0", "0");
}
