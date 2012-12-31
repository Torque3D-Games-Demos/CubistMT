
singleton TSShapeConstructor(Reef_placement_grassDae)
{
   baseShape = "./reef_placement_grass.dae";
   loadLights = "0";
};

function Reef_placement_grassDae::onLoad(%this)
{
   %this.removeNode("Light");
   %this.removeNode("SKT_Sun");
   %this.removeNode("4Cam");
   %this.removeSequence("ambient");
}
