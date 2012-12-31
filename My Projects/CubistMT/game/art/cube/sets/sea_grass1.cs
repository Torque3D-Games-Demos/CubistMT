
singleton TSShapeConstructor(Sea_grass1Dae)
{
   baseShape = "./sea_grass1.dae";
   unit = "-1";
   loadLights = "0";
};

function Sea_grass1Dae::onLoad(%this)
{
   %this.removeNode("EnvironmentAmbientLight");
}
