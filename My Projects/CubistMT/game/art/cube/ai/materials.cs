singleton Material(goldenperchMaterial)
{
   mapTo = "goldenperch";
   diffuseMap[0] = "blank.png";
   translucent = "0";
   pixelSpecular = true;
};
singleton Material(goldenperchAlphaMaterial)
{
   mapTo = "alpha_goldenperch";
   diffuseMap[0] = "blank.png";
   translucent = "1";
   translucentZWrite = "0";
   translucentBlendOp = "Sub";
   alphaRef = "128";
   alphaTest = "0";
   pixelSpecular = true;
};
singleton Material(harl_diffuseMaterial)
{
   mapTo = "harl_diffuse";
   diffuseMap[0] = "blank.png";
   translucent = "0";
   translucentZWrite = "0";
   translucentBlendOp = "Sub";
   alphaRef = "64";
   alphaTest = "0";
   pixelSpecular = true;
};
singleton Material(moorish_diffuseMaterial)
{
   mapTo = "moorish_diffuse";
   diffuseMap[0] = "blank.png";
   translucent = "0";
   translucentZWrite = "0";
   translucentBlendOp = "Sub";
   alphaRef = "64";
   alphaTest = "0";
   pixelSpecular = true;
};


singleton Material(moorishidol_alpha_diffuseMaterial)
{
   mapTo = "moorishidol_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "1";
   translucentZWrite = "1";
   translucentBlendOp = "Sub";
   alphaRef = "33";
   alphaTest = "1";
   
   pixelSpecular = true;
   subSurface = "0";
   // normalMap[0] = "blank.png";
   specularPower[0] = "1";
};

singleton Material(seahorse_diffuseMaterial)
{
   mapTo = "seahorse_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "0";
   pixelSpecular = true;
   subSurface = "0";
   // normalMap[0] = "blank.png";
   specularPower[0] = "30";
};

singleton Material(seahorse_alpha_diffuseMaterial)
{
   mapTo = "seahorse_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "1";
   translucentZWrite = "0";
   translucentBlendOp = "Sub";
   alphaRef = "128";
   alphaTest = "0";
   
   pixelSpecular = true;
   subSurface = "0";
   // normalMap[0] = "blank.png";
   specularPower[0] = "30";
};


singleton Material(cuttlefish_diffuseMaterial)
{
   mapTo = "TEX_cuttlefish_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "0";
   pixelSpecular = "1";
   subSurface = "0";
   // normalMap[0] = "blank.png";
   specularPower[0] = "16";
   minnaertConstant[0] = "-1";
   subSurfaceColor[0] = "0.2 0.2 1 1";
};

singleton Material(base_bluetang_alpha_diffuseMaterial)
{
   mapTo = "base_bluetang_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   bumpTex[0] = "blank.png";
   specularMap[0] = "blank.png";
   translucent = "1";
   translucentZWrite = "0";
   translucentBlendOp = "Sub";
   alphaRef = "128";
   alphaTest = "0";
   pixelSpecular = true;
};

singleton Material(base_bluetang_diffuseMaterial)
{
   mapTo = "base_bluetang_diffuse";
   diffuseMap[0] = "blank.png";
   bumpTex[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "0";
   pixelSpecular = true;
};
singleton Material(SardinellaMaterial)
{
   mapTo = "Sardinella_Diffuse";
   diffuseMap[0] = "blank.png";
   translucent = "0";
   pixelSpecular = true;
};
singleton Material(SardinellaAlphaMaterial)
{
   mapTo = "Sardinella_alpha_Diffuse";
   diffuseMap[0] = "blank.png";
   translucent = "0";
   translucentZWrite = "1";
   translucentBlendOp = "Sub";
   alphaRef = "248";
   alphaTest = "1";
   pixelSpecular = true;
   specularPower[0] = "49";
   subSurface[0] = "0";
   subSurfaceColor[0] = "0.666667 0.690196 0.411765 1";
   specular[0] = "0.992157 0.992157 0.992157 1";
   materialTag0 = "RoadAndPath";
};
singleton Material(bluefin_trevally_alpha_mat)
{
   mapTo = "bluefin_trevally_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   translucentBlendOp = "Sub";
   // normalMap[0] = "blank.png";
   specularPower[0] = "21";
   pixelSpecular[0] = "1";
   subSurface[0] = "0";
   subSurfaceColor[0] = "0.247059 0.403922 0.517647 1";
   translucent = "1";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "6";
   materialTag0 = "RoadAndPath";
   minnaertConstant[0] = "-1";
   doubleSided = "0";
   specularMap[0] = "blank.png";
};
singleton Material(bluefin_trevally_mat)
{
   mapTo = "bluefin_trevally_diffuse";
   diffuseMap[0] = "blank.png";
   translucentBlendOp = "Sub";
   // normalMap[0] = "blank.png";
   specularPower[0] = "21";
   pixelSpecular[0] = "1";
   subSurface[0] = "0";
   subSurfaceColor[0] = "0.247059 0.403922 0.517647 1";
   translucent = "0";
   translucentZWrite = "0";
   materialTag0 = "RoadAndPath";
   alphaRef = "0";
   minnaertConstant[0] = "-1";
   specular[0] = "0.992157 0.992157 0.992157 1";
   specularMap[0] = "blank.png";
   alphaTest = "0";
};
singleton Material(orange_basselet_alpha_diffuse_mat)
{
   mapTo = "orange_basselet_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "26";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   translucent = "1";
   alphaRef = "60";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaTest = "1";
   materialTag0 = "RoadAndPath";
};
singleton Material(orange_basselet_diffuse_mat)
{
   mapTo = "orange_basselet_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "26";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   materialTag0 = "RoadAndPath";
};
singleton Material(MoorishIdol_diffuse_mat)
{
   mapTo = "MoorishIdol_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "21";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
};
singleton Material(snapper_alpha_mat)
{
   mapTo = "snapper_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   specularPower[0] = "40";
   pixelSpecular[0] = "1";
   doubleSided = "0";
   translucent = "1";
   translucentBlendOp = "Sub";
   alphaRef = "53";
   showFootprints = "1";
   minnaertConstant[0] = "-1";
   translucentZWrite = "1";
   alphaTest = "1";
   diffuseColor[0] = "1 1 1 1";
   glow[0] = "0";
   // normalMap[0] = "blank.png";
   materialTag0 = "RoadAndPath";
};
singleton Material(snapper_mat)
{
   mapTo = "snapper_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "40";
   pixelSpecular[0] = "1";
   translucent = "0";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "1";
   materialTag0 = "RoadAndPath";
   translucentBlendOp = "LerpAlpha";
};
singleton Material(lionfish_alpha_mat)
{
   mapTo = "lionfish_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaTest = "0";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
};
singleton Material(lionfish_alpha_mat)
{
   mapTo = "lionfish_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
   translucentBlendOp = "Sub";
};
singleton Material(lionfish_mat)
{
   mapTo = "lionfish_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   materialTag0 = "RoadAndPath";
};
singleton Material(bluestreak_wrasse_female_mat)
{
   mapTo = "bluestreak_wrasse_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
};
singleton Material(bluestreak_wrasse_alpha_mat)
{
   mapTo = "bluestreak_wrasse_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
   translucentBlendOp = "Sub";
};
singleton Material(parrotfish_female_mat)
{
   mapTo = "parrotfish_female_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   minnaertConstant[0] = "-1";
   translucentBlendOp = "None";
   translucentZWrite = "0";
   materialTag0 = "RoadAndPath";
};
singleton Material(parrotfish_female_alpha_mat)
{
   mapTo = "parrotfish_female_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaRef = "120";
   materialTag0 = "RoadAndPath";
   minnaertConstant[0] = "-1";
   alphaTest = "1";
};
singleton Material(parrotfish_juvenile_mat)
{
   mapTo = "parrotfish_juvenile_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
};
singleton Material(parrotfish_juvenile_alpha_mat)
{
   mapTo = "parrotfish_juvenile_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
   translucentBlendOp = "Sub";
};
singleton Material(parrotfish_male_alpha_mat)
{
   mapTo = "parrotfish_male_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
};
singleton Material(parrotfish_male_alpha_mat)
{
   mapTo = "parrotfish_male_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   translucent = "1";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "53";
   materialTag0 = "RoadAndPath";
   translucentBlendOp = "Sub";
};
singleton Material(remora_mat)
{
   mapTo = "remora_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "49";
   pixelSpecular[0] = "1";
   minnaertConstant[0] = "-1";
   materialTag0 = "RoadAndPath";
};
singleton Material(remora2_mat : remora_mat)
{
   mapTo = "remora";
};
singleton Material(remora_alpha_diffuse_mat)
{
   mapTo = "remora_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "49";
   pixelSpecular[0] = "1";
   translucentBlendOp = "Sub";
   alphaTest = "1";
   alphaRef = "13";
   translucentZWrite = "1";
   translucent = "1";
   materialTag0 = "RoadAndPath";
};
singleton Material(harlequintuskfish_mat)
{
   mapTo = "harlequintuskfish_diffuse";
   diffuseMap[0] = "blank.png";
   minnaertConstant[0] = "-1";
   translucentBlendOp = "None";
   alphaRef = "20";
   materialTag0 = "RoadAndPath";
   // normalMap[0] = "blank.png";
   specularPower[0] = "20";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
};


singleton Material(harlequintuskfish_alpha_diffuseMaterial)
{
   mapTo = "harlequintuskfish_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "1";
   pixelSpecular = true;
   subSurface = "0";
   // normalMap[0] = "blank.png";
   subSurfaceColor[0] = "1 0.2 0.2 1";
   materialTag0 = "RoadAndPath";
   specularPower[0] = "20";
   minnaertConstant[0] = "-1";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "67";
   showFootprints = "0";
};

singleton Material(moonjellyfish_mat)
{
   mapTo = "moonjellyfish1_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "84";
   specular = "0.5 0.5 0.5 1";
   pixelSpecular[0] = "1";
   minnaertConstant[0] = "-1";
   doubleSided = "1";
   translucent = "1";
   translucentBlendOp = "Add";
   alphaRef = "46";
   showFootprints = "0";
   glow[0] = "0";
   castShadows = "0";
};
singleton Material(clownfish_diffuseMaterial)
{
   mapTo = "clownfish_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   translucentBlendOp = "Sub";
};
singleton Material(parrotfish_male_mat)
{
   mapTo = "parrotfish_male_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "41";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   materialTag0 = "RoadAndPath";
};
singleton Material(moonjellyfish2_alpha_mat)
{
   mapTo = "moonjellyfish2_alpha";
   diffuseMap[0] = "blank.png";
   diffuseColor[0] = "0.5 0.5 0.5 1";
   castShadows = "0";
   translucent = "1";
   translucentBlendOp = "add";
   specularMap[0] = "blank.png";
   glow[0] = "0";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
};
singleton Material(DefaultMaterial13)
{
   mapTo = "pufferfish_shared_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   translucent = "1";
   translucentBlendOp = "Sub";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "93";
};
singleton Material(DefaultMaterial)
{
   mapTo = "pufferfish_inflated_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
};
singleton Material(pufferfish_eye_shared_diffuse_mat)
{
   mapTo = "pufferfish_eye_shared_diffuse";
   diffuseMap[0] = "blank.png";
   // normalMap[0] = "blank.png";
   specularPower[0] = "1";
   pixelSpecular[0] = "1";
   specularMap[0] = "blank.png";
   translucent = "0";
   translucentBlendOp = "Sub";
   translucentZWrite = "0";
   alphaTest = "0";
   alphaRef = "100";
};


singleton Material(clownfish_alpha_diffuseMaterial)
{
   mapTo = "clownfish_alpha_diffuse";
   diffuseMap[0] = "blank.png";
   specularMap[0] = "blank.png";

   translucent = "1";
   translucentZWrite = "1";
   translucentBlendOp = "Sub";
   alphaRef = "40";
   alphaTest = "1";
   
   pixelSpecular = true;
   subSurface = "0";
   // normalMap[0] = "blank.png";
};
