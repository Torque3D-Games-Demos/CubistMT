/*
singleton Material(WebMaterial)
{
   mapTo = "web";
   diffuseMap[0] = "web.png";
   translucent = false;
};
*/

singleton Material(floodMaterial)
{
   mapTo = "flood";
   diffuseMap[0] = "flood.png";
//   bumpTex[0] = "flood_NRM.png";
//   dynamicCubemap = true;
//   webViewData[0] = WebTestData;

   emissive = true;

   translucent = false;
//   dynamicCubemap = true;
//   planarReflection = true;
   pixelSpecular = true;
   useAnisotropic = true;
};

singleton GFXStateBlockData(flood_waterStateBlockData){
  zEnable = true;
  zWriteEnable = false; 
};
singleton Material(flood_waterMaterial)
{
   mapTo = "flood_water";
   diffuseMap[0] = "flood.png";
//   bumpTex[0] = "flood_NRM.png";

//   dynamicCubemap = true;
//   webViewData[0] = WebTestData;

   emissive = true;
   

   translucent = true;
   translucentZWrite = "1";
   alphaRef = "0";
   alphaTest = "1";
//   dynamicCubemap = true;
//   planarReflection = true;
//   colorMultiply = true;

   pixelSpecular = true;

   useAnisotropic = true;
   translucentBlendOp = "Mul";
};
/*

singleton CustomMaterial( flood_waterMaterial )
{
   mapTo = "flood_water";
   texture[0] = "flood.png";
//   bumpTex[0] = "flood_NRM.png";
//   stateBlock = "flood_waterMaterial";

//   dynamicCubemap = true;
//   webViewData[0] = WebTestData;
//   emissive = true;

   translucent = false;
//   dynamicCubemap = true;
//   planarReflection = true;
//   colorMultiply = true;

   pixelSpecular = true;
};
*/

singleton Material(flood_bitsMaterial)
{
   mapTo = "flood_bits";
   diffuseMap[0] = "flood_bits.png";
//   bumpTex[0] = "flood_NRM.png";
   
   translucent = "1";
   translucentZWrite = "1";
   translucentBlendOp = "LerpAlpha";
   //translucentBlendOp = "AddAlpha";
   alphaRef = "128";
   emissive = "1";
   alphaTest = "0";
 //  dynamicCubemap = true;
 //  planarReflection = true;
   pixelSpecular = true;
   useAnisotropic = true;
};
/*
singleton CustomMaterial( floodMaterial )
{
   mapTo = "flood";
//   texture[0] = "flood.png";
//   texture[1] = "flood_NRM.png";
//   texture[2] = "$cubeMap";
//   shader = BumpReflect;
   
//   texture[1] = "flood.png";
//   texture[0] = "flood_NRM.png";
//   texture[3] = "$dynamicCubemap";
   
   sampler["diffMap"] = "flood.png";
   sampler["bumpMap"] = "flood_NRM.png";
   sampler["cubeMap"] = "$dynamicCubemap";  
    
   shader = BumpCubeDiff;
   
   version = 2.0;
   dynamicCubemap = true;
   translucent = false;
   emissive = true;
};
singleton CustomMaterial( floodMaterial )
{
   mapTo = "flood";
//   texture[0] = "flood.png";
//   texture[1] = "flood_NRM.png";
//   texture[2] = "$cubeMap";
//   shader = BumpReflect;
   
//   texture[1] = "flood.png";
//   texture[0] = "flood_NRM.png";
//   texture[3] = "$dynamicCubemap";
   
   sampler["texMap"] = "flood.png";
   sampler["bumpMap"] = "flood_NRM.png";
   sampler["refractMap"] = "$backbuff";  
    
   shader = ReflectBump;
   
   planarReflection = true;
   version = 2.0;
//   dynamicCubemap = true;
   translucent = false;
   emissive = true;
};
*/

/*

singleton CustomMaterial( flood_bitsMaterial )
{
   mapTo = "flood_bits";
   texture[0] = "flood_bits.png";
   texture[1] = "$reflectbuff";
   texture[2] = "flood_NRM.png";
   shader = BumpReflect;
   version = 2.0;
   planarReflection = true;
   translucent = true;
//   dynamicCubemap = true;
};
*/
// floodMaterial.setShaderConst( "$cubeTrans", "1 0 0" SPC "0 1 0" SPC "0 0 1" );
//function floodMaterial::setShaderConsts(%this )
//{      
//  %this.setShaderConst( "$cubeTrans", "1 0 0" SPC "0 1 0" SPC "0 0 1" );
         
//}
