
singleton TSShapeConstructor(TestSplineDts)
{
   baseShape = "./testSpline.dts";
};

function TestSplineDts::onLoad(%this)
{
   %this.setBounds("1e+031 1e+031 1e+031 -1e+031 -1e+031 -1e+031");
}
