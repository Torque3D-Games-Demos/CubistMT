
singleton TSShapeConstructor(Dendro3Dae)
{
   baseShape = "./dendro3.dae";
   unit = "0.01";
   loadLights = "0";
 //  upAxis = "Y_AXIS";
   singleDetailSize = "3";
};

function Dendro3Dae::onLoad(%this)
{
   %this.removeNode("Camera");
   %this.removeNode("light");
   %this.removeNode("EnvironmentAmbientLight");
   %this.setDetailLevelSize("212", "2");
   %this.setDetailLevelSize("3", "128");
   %this.setDetailLevelSize("28", "64");
   %this.removeNode("DefCamera");
}
