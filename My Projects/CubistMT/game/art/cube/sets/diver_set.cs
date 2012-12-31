
singleton TSShapeConstructor(Diver_setDts)
{
   baseShape = "./diver_set.dts";
};

function Diver_setDts::onLoad(%this)
{
   %this.addTrigger("ambient", "1308", "1");
   %this.addTrigger("ambient", "7802", "-1");
   %this.addTrigger("ambient", "0", "2");
   %this.addTrigger("ambient", "741", "-2");
   %this.addTrigger("ambient", "8154", "2");
   %this.addTrigger("ambient", "8579", "-2");
}
