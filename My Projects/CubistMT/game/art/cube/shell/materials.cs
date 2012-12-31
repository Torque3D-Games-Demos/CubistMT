/*
singleton Material(WebMaterial)
{
   mapTo = "web";
   diffuseMap[0] = "web.png";
   translucent = false;
};
*/

/*
singleton Material(WebDynamicMaterial)
{
   mapTo = "web";
   diffuseMap[0] = "web.png";
//   webViewData[0] = WebTestData;
   translucentBlendOp = LerpAlpha;
   translucent = "1";
   translucentZWrite = "0";
   useAnisotropic = true;
//   pixelSpecular = true;
   emissive[0] = "1";
};
*/
singleton Material(DefaultMaterial4)
{
   mapTo = "art/cube/interface/sample_info_display.png";
   diffuseMap[0] = "art/cube/interface/sample_info_display";
   emissive[0] = "1";
   translucent = "1";
   translucentZWrite = "1";
   alphaRef = "20";
   materialTag0 = "RoadAndPath";
};

singleton Material(WebDynamicMaterial)
{
   mapTo = "web";
   diffuseMap[0] = "art/cube/shell/web.png";
   useAnisotropic[0] = "1";
   emissive[0] = "1";
   translucent = "1";
   translucentBlendOp = "Sub";
};
