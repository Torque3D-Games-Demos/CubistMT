
singleton TSShapeConstructor(Dendro2Dae)
{
   baseShape = "./dendro2.dae";
   unit = "0.01";
   loadLights = "0";
//   upAxis = "Y_AXIS";
   singleDetailSize = "3";
};

function Dendro2Dae::onLoad(%this)
{
   %this.removeNode("light");
   %this.removeNode("Camera");
   %this.removeNode("EnvironmentAmbientLight");
   %this.setDetailLevelSize("2", "128");
   %this.setDetailLevelSize("27", "64");
   %this.setDetailLevelSize("211", "2");
   %this.removeNode("DefCamera");
}
