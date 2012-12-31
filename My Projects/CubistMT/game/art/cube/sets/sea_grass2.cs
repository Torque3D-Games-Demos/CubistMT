
singleton TSShapeConstructor(Sea_grass2Dae)
{
   baseShape = "./sea_grass2.dae";
   unit = "-1";
   loadLights = "0";
};

function Sea_grass2Dae::onLoad(%this)
{
   %this.removeNode("EnvironmentAmbientLight");
}
