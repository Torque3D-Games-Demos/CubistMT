
singleton TSShapeConstructor(Green_sea_turtle_set1Dts)
{
   baseShape = "./green_sea_turtle_set1.dts";
};

function Green_sea_turtle_set1Dts::onLoad(%this)
{
   %this.addTrigger("ambient", "0", "1");
}
