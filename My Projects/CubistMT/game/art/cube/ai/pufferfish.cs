
singleton TSShapeConstructor(PufferfishDts)
{
   baseShape = "./pufferfish.dts";
};

function PufferfishDts::onLoad(%this)
{
   /*
   %this.addVisibility("swim_root", 0, 0, 1);
   %this.addVisibility("swim_root", 0, 1, 0);
   %this.addVisibility("swim_root", 0, 2, 0);
   %this.addVisibility("swim_root", 0, 3, 0);
   %this.addVisibility("swim_root", 0, 4, 0);
   %this.addVisibility("swim_root", 0, 5, 0);
   %this.addVisibility("swim_root", 0, 6, 0);
   %this.addVisibility("swim_root", 0, 7, 0);
   %this.addVisibility("swim_root", 0, 8, 0);
   %this.addVisibility("swim_root", 0, 9, 0);
   
   %this.addVisibility("swim_forward", 0, 0, 1);
   %this.addVisibility("swim_forward", 0, 1, 0);
   %this.addVisibility("swim_forward", 0, 2, 0);
   %this.addVisibility("swim_forward", 0, 3, 0);
   %this.addVisibility("swim_forward", 0, 4, 0);
   %this.addVisibility("swim_forward", 0, 5, 0);
   %this.addVisibility("swim_forward", 0, 6, 0);
   %this.addVisibility("swim_forward", 0, 7, 0);
   %this.addVisibility("swim_forward", 0, 8, 0);
   %this.addVisibility("swim_forward", 0, 9, 0);
   
   */
   
   // clear all frames to most common values
   %this.addVisibility("interact", 0, 552, 0, 1);
   %this.addVisibility("interact", 0, 552, 1, 0);
   %this.addVisibility("interact", 0, 552, 2, 0);
   %this.addVisibility("interact", 0, 552, 3, 0);
   %this.addVisibility("interact", 0, 552, 4, 0);
   %this.addVisibility("interact", 0, 552, 5, 0);
   %this.addVisibility("interact", 0, 552, 6, 0);
   %this.addVisibility("interact", 0, 552, 7, 0);
   %this.addVisibility("interact", 0, 552, 8, 0);
   %this.addVisibility("interact", 0, 552, 9, 0);
   
   // set the invisible frame ranges for the main object
   %this.addVisibility("interact", 9, 511,   0, 0);
   
   // set the visible frame ranges
   %this.addVisibility("interact", 9, 16,    1, 1);
   %this.addVisibility("interact", 506, 511,   1, 1);
   
   %this.addVisibility("interact", 16, 39,    2, 1);
   %this.addVisibility("interact", 501, 506,   2, 1);
   
   %this.addVisibility("interact", 39, 46,    3, 1);
   %this.addVisibility("interact", 497, 501,   3, 1);
   
   %this.addVisibility("interact", 46, 69,    4, 1);
   %this.addVisibility("interact", 493, 497,   4, 1);
   
   %this.addVisibility("interact", 69, 76,    5, 1);
   %this.addVisibility("interact", 489, 493,   5, 1);
   
   %this.addVisibility("interact", 76, 99,    6, 1);
   %this.addVisibility("interact", 485, 489,   6, 1);
   
   %this.addVisibility("interact", 99, 106,   7, 1);
   %this.addVisibility("interact", 480, 485,   7, 1);
   
   %this.addVisibility("interact", 106, 128,   8, 1);
   %this.addVisibility("interact", 474, 480,   8, 1);
   
   %this.addVisibility("interact", 128, 474,   9, 1);
   

}
