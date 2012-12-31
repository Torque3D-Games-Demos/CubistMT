
singleton TSShapeConstructor(Sea_grass_placementDae)
{
   baseShape = "./sea_grass_placement.dae";
   unit = "0.01";
   loadLights = "0";
};

function Sea_grass_placementDae::onLoad(%this)
{
   %this.removeSequence("ambient");
   %this.removeNode("Light");
   %this.removeNode("Camera");
}
