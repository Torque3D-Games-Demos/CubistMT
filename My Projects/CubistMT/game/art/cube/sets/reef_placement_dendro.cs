
singleton TSShapeConstructor(Reef_placement_dendroDae)
{
   baseShape = "./reef_placement_dendro.dae";
};

function Reef_placement_dendroDae::onLoad(%this)
{
   %this.removeSequence("ambient");
   %this.removeNode("Light");
   %this.removeNode("SKT_Sun");
   %this.removeNode("4Cam");
}
