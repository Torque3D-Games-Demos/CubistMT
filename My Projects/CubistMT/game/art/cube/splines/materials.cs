
singleton Material(ANM_testSpline_whaleTex)
{
   mapTo = "whaleTex";
   diffuseMap[0] = "Calf";
   translucentBlendOp = "None";
};

singleton Material(ANM_testSpline_Pink_CTRL)
{
   mapTo = "Pink_CTRL";
   diffuseColor[0] = "0.8 0 0.8 0.75";
   translucent = "1";
};

singleton Material(ANM_testSpline_Yel_CTRL)
{
   mapTo = "Yel_CTRL";
   diffuseColor[0] = "0.8 0.8 0 0.75";
   translucent = "1";
};

singleton Material(ANM_testSpline_Top)
{
   mapTo = "Top";
   diffuseColor[0] = "0 0.8 0 0.75";
   translucent = "1";
};

singleton Material(ANM_testSpline_Right_CTRL)
{
   mapTo = "Right_CTRL";
   diffuseColor[0] = "0.8 0 0 0.75";
   translucent = "1";
};

singleton Material(ANM_testSpline_blinn4)
{
   mapTo = "blinn4";
   diffuseColor[0] = "0.8 0.8 0.8 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "38.4";
   translucentBlendOp = "None";
};

singleton Material(ANM_testSpline_pupil)
{
   mapTo = "pupil";
   diffuseColor[0] = "0 0 0 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "38.4";
   translucentBlendOp = "None";
};

singleton Material(ANM_testSpline_Left_CTRL)
{
   mapTo = "Left_CTRL";
   diffuseColor[0] = "0 0 0.8 0.75";
   translucent = "1";
};

singleton Material(ANM_testSpline_teeth)
{
   mapTo = "teeth";
   diffuseColor[0] = "0.996078 0.996078 0.996078 1";
   translucentBlendOp = "Sub";
   castShadows = "0";
   translucentZWrite = "1";
   alphaTest = "1";
   alphaRef = "214";
   diffuseMap[0] = "art/cube/sets/blank.png";
   // normalMap[0] = "art/cube/sets/blank.png";
   specularPower[0] = "46";
   pixelSpecular[0] = "1";
};
