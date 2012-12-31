
singleton TSShapeConstructor(Reef_placement_miscDae)
{
   baseShape = "./reef_placement_misc.dae";
};

function Reef_placement_miscDae::onLoad(%this)
{
   %this.removeSequence("ambient");
   %this.removeNode("Light");
   %this.removeNode("SKT_Sun");
   %this.removeNode("4Cam");
}
