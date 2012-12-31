

singleton Material(Unnamed_mat)
{
   mapTo = "Unnamed";
   glow[0] = "0";
   emissive[0] = "0";
   diffuseColor[0] = "0.513726 0.513726 0.513726 1";
   castShadows = "0";
   translucent = "0";
   alphaTest = "0";
   alphaRef = "255";
};






singleton Material(anemone_mat)
{
   mapTo = "lambert2";
   diffuseMap[0] = "art/cube/sets/blank.png";
   translucentBlendOp = "None";
   // normalMap[0] = "art/cube/sets/blank.png";
   specularPower[0] = "43";
   pixelSpecular[0] = "1";
   diffuseColor[0] = "0.592 0.592 0.592 1";
   specular[0] = "0.592 0.592 0.592 1";
};

singleton Material(anemone_tenticles_mat)
{
   mapTo = "tenticles";
   diffuseMap[0] = "art/cube/sets/Magbit_00.png";
   specular[0] = "0.992157 0.992157 0.992157 1";
   specularPower[0] = "44";
   translucentBlendOp = "Sub";
   // normalMap[0] = "art/cube/sets/blank.png";
   pixelSpecular[0] = "1";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "33";
   castShadows = "0";

   subSurface[0] = "1";
   subSurfaceColor[0] = "1 1 1 1";
   subSurfaceRolloff[0] = "0.3";
   
   doubleSided = true;
};

