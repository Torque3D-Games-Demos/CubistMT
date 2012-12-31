
singleton TSShapeConstructor(Dendro1Dae)
{
   baseShape = "./dendro1.dae";
   unit = "0.01";
   loadLights = "0";
   singleDetailSize = "3";
//   upAxis = "Y_AXIS";
};

function Dendro1Dae::onLoad(%this)
{
   %this.removeNode("light");
   %this.removeNode("Camera");
   %this.removeNode("EnvironmentAmbientLight");
   %this.removeNode("DefCamera");
   %this.setDetailLevelSize("1", "128");
   %this.setDetailLevelSize("2", "64");
   %this.setDetailLevelSize("210", "2");
}
