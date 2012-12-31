singleton Material(touch_cursor1_diffuseMaterial)
{
   diffuseMap[0] = "touch_cursor1_diffuse";
   translucent = true;
   translucentZWrite = "0";
   translucentBlendOp = "Mul";
   alphaRef = "0";
   alphaTest = "0";
};
/*pgmat
singleton Material(bubble_mat)
{
   mapTo = "bubble";
//   diffuseMap[0] = "art/cube/interface/menu_bluetang";
   translucent = "1";
   dynamicCubemap = true;
   diffuseColor = "0 0 0 0.5";
};

singleton Material(bubble_mat)
{
   mapTo = "bubble";
   diffuseColor[0] = "0.996078 0.996078 0.996078 0.071";
   pixelSpecular[0] = "1";
   translucent = "1";
   cubemap = "DesertSkyCubemap";
   diffuseMap[0] = "core/art/water/depthcolor_ramp.png";
   specularPower[0] = "64";
};
*/

singleton Material(menu_mat)
{
   mapTo = "menu";
   diffuseMap[0] = "art/cube/interface/menu_bluetang.png";
   useAnisotropic[0] = "1";
   emissive[0] = "1";
   translucent = "1";
};

singleton Material(cube_screens_icon_mat)
{
   mapTo = "cube_screens_icon_diffuse";
   diffuseMap[0] = "art/cube/interface/cube_screens_icon_diffuse.png";
   translucentBlendOp = "Sub";
   translucent = true;
   translucentZWrite = false;
   useAnisotropic = true;
//   emissive = true;
};
singleton Material(lightray_icon_mat : cube_screens_icon_mat)
{
   mapTo = "lightray_icon";
   diffuseMap[0] = "art/cube/interface/lightray_icon.png";
};

singleton Material(sample_info_display_mat)
{
   mapTo = "sample_info_display";
   diffuseMap[0] = "art/cube/interface/sample_info_display.png";
   translucentBlendOp = "Sub";
   translucent = true;
   translucentZWrite = false;
   useAnisotropic = true;
   emissive = true;
};

singleton Material(popup_timerl_mat)
{
   mapTo = "popup_timerl";
   diffuseColor[0] = "0.992157 0.992157 0.992157 1";
   diffuseMap[0] = "art/cube/interface/popup_timerl";
   useAnisotropic[0] = "1";
   emissive[0] = "1";
   translucent = "1";
};


singleton Material(bubble_unknown_red_unknown)
{
   mapTo = "red_unknown";
   diffuseColor[0] = "0.64 0.0425698 0 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_body_bubble_blue)
{
   mapTo = "bubble_blue";
   diffuseColor[0] = "0 0.0415591 0.64 1";
   specular[0] = "0 0.0406863 0.63415 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_information_bubble_green)
{
   mapTo = "bubble_green";
   diffuseColor[0] = "0 0.64 0.0491855 1";
   specular[0] = "0.5 0.473333 0.473333 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_information_bubble_white)
{
   mapTo = "bubble_white";
   diffuseColor[0] = "0.64 0.64 0.64 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_cancel_bubble_red)
{
   mapTo = "bubble_red";
   diffuseColor[0] = "0.64 0 0.0271448 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_touch_bubble_seethru)
{
   mapTo = "bubble_seethru";
   diffuseColor[0] = "0.64 0.64 0.64 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   translucent = "1";
};

singleton Material(bubble_body_bubble_semitrans)
{
   mapTo = "bubble_semitrans";
   diffuseColor[0] = "0.64 0.64 0.64 0.581921";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucent = "1";
};


singleton Material(bubble_dragbar_Material_material)
{
   mapTo = "Material-material";
   diffuseColor[0] = "0.64 0.64 0.64 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_dragbar_invisible)
{
   mapTo = "invisible";
   diffuseColor[0] = "0.8 0.8 0.8 1";
   specular[0] = "0.471751 0.471751 0.471751 0";
   specularPower[0] = "50";
   doubleSided = "1";
   translucent = "1";
};

singleton Material(bubble_progress_red_material)
{
   mapTo = "red-material";
   diffuseColor[0] = "0.64 0 0.0573517 1";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   emissive[0] = "1";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_prelude_whitepaint_material)
{
   mapTo = "whitepaint-material";
   diffuseColor[0] = "1 1 1 1";
   emissive[0] = "1";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(bubble_prelude_bluepaint_material)
{
   mapTo = "bluepaint-material";
   diffuseColor[0] = "0.26 0.79 0.88 1";
   emissive[0] = "1";
   doubleSided = "1";
   translucentBlendOp = "None";
};
//*********************************************
// cogs material anim
//*********************************************
singleton Material(badcog_material)
{
   mapTo = "badcog-material";
   diffuseMap[0] = "badcog";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucent = "1";
};
singleton Material(bubble_anim_base_anim)
{
   mapTo = "base_anim";
   diffuseMap[0] = "art/cube/interface/coganim";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucent = "1";
   animFlags[0] = "0x00000001";
   scrollDir[0] = "0.103 0.241";
   scrollOffset[0] = "0.0 0.0";
};

//*********************************
singleton Material(bubble_anim_base_anim0 : bubble_anim_base_anim)
{
   mapTo = "c000_anim";
   scrollOffset[0] = "0.125 0.000";
};
singleton Material(bubble_anim_base_anim1 : bubble_anim_base_anim)
{
   mapTo = "c001_anim";
   scrollOffset[0] = "0.125 0.000";
};
singleton Material(bubble_anim_base_anim2 : bubble_anim_base_anim)
{
   mapTo = "c002_anim";
   scrollOffset[0] = "0.250 0.000";
};
singleton Material(bubble_anim_base_anim3 : bubble_anim_base_anim)
{
   mapTo = "c003_anim";
   scrollOffset[0] = "0.375 0.000";
};
singleton Material(bubble_anim_base_anim4 : bubble_anim_base_anim)
{
   mapTo = "c004_anim";
   scrollOffset[0] = "0.500 0.000";
};
singleton Material(bubble_anim_base_anim5 : bubble_anim_base_anim)
{
   mapTo = "c005_anim";
   scrollOffset[0] = "0.625 0.000";
};
singleton Material(bubble_anim_base_anim6 : bubble_anim_base_anim)
{
   mapTo = "c006_anim";
   scrollOffset[0] = "0.750 0.000";
};
singleton Material(bubble_anim_base_anim7 : bubble_anim_base_anim)
{
   mapTo = "c007_anim";
   scrollOffset[0] = "0.875 0.000";
};
//*********************************
singleton Material(bubble_anim_base_anim8 : bubble_anim_base_anim)
{
   mapTo = "c008_anim";
   scrollOffset[0] = "0.000 0.125";
};
singleton Material(bubble_anim_base_anim9 : bubble_anim_base_anim)
{
   mapTo = "c009_anim";
   scrollOffset[0] = "0.125 0.125";
};
singleton Material(bubble_anim_base_anim10 : bubble_anim_base_anim)
{
   mapTo = "c010_anim";
   scrollOffset[0] = "0.250 0.125";
};
singleton Material(bubble_anim_base_anim11 : bubble_anim_base_anim)
{
   mapTo = "c011_anim";
   scrollOffset[0] = "0.375 0.125";
};
singleton Material(bubble_anim_base_anim12 : bubble_anim_base_anim)
{
   mapTo = "c012_anim";
   scrollOffset[0] = "0.500 0.125";
};
singleton Material(bubble_anim_base_anim13 : bubble_anim_base_anim)
{
   mapTo = "c013_anim";
   scrollOffset[0] = "0.625 0.125";
};
singleton Material(bubble_anim_base_anim14 : bubble_anim_base_anim)
{
   mapTo = "c014_anim";
   scrollOffset[0] = "0.750 0.125";
};
singleton Material(bubble_anim_base_anim15 : bubble_anim_base_anim)
{
   mapTo = "c015_anim";
   scrollOffset[0] = "0.875 0.125";
};
//*********************************
singleton Material(bubble_anim_base_anim16 : bubble_anim_base_anim)
{
   mapTo = "c016_anim";
   scrollOffset[0] = "0.000 0.250";
};
singleton Material(bubble_anim_base_anim17 : bubble_anim_base_anim)
{
   mapTo = "c017_anim";
   scrollOffset[0] = "0.125 0.250";
};
singleton Material(bubble_anim_base_anim18 : bubble_anim_base_anim)
{
   mapTo = "c018_anim";
   scrollOffset[0] = "0.250 0.250";
};
singleton Material(bubble_anim_base_anim19 : bubble_anim_base_anim)
{
   mapTo = "c019_anim";
   scrollOffset[0] = "0.375 0.250";
};
singleton Material(bubble_anim_base_anim20 : bubble_anim_base_anim)
{
   mapTo = "c020_anim";
   scrollOffset[0] = "0.500 0.250";
};
singleton Material(bubble_anim_base_anim21 : bubble_anim_base_anim)
{
   mapTo = "c021_anim";
   scrollOffset[0] = "0.625 0.250";
};
singleton Material(bubble_anim_base_anim22 : bubble_anim_base_anim)
{
   mapTo = "c022_anim";
   scrollOffset[0] = "0.750 0.250";
};
singleton Material(bubble_anim_base_anim23 : bubble_anim_base_anim)
{
   mapTo = "c023_anim";
   scrollOffset[0] = "0.875 0.250";
};
//*********************************
singleton Material(bubble_anim_base_anim24 : bubble_anim_base_anim)
{
   mapTo = "c024_anim";
   scrollOffset[0] = "0.000 0.375";
};
singleton Material(bubble_anim_base_anim25 : bubble_anim_base_anim)
{
   mapTo = "c025_anim";
   scrollOffset[0] = "0.125 0.375";
};
singleton Material(bubble_anim_base_anim26 : bubble_anim_base_anim)
{
   mapTo = "c026_anim";
   scrollOffset[0] = "0.250 0.375";
};
singleton Material(bubble_anim_base_anim27 : bubble_anim_base_anim)
{
   mapTo = "c027_anim";
   scrollOffset[0] = "0.375 0.375";
};
singleton Material(bubble_anim_base_anim28 : bubble_anim_base_anim)
{
   mapTo = "c028_anim";
   scrollOffset[0] = "0.500 0.375";
};
singleton Material(bubble_anim_base_anim29 : bubble_anim_base_anim)
{
   mapTo = "c029_anim";
   scrollOffset[0] = "0.625 0.375";
};
singleton Material(bubble_anim_base_anim30 : bubble_anim_base_anim)
{
   mapTo = "c030_anim";
   scrollOffset[0] = "0.750 0.375";
};
singleton Material(bubble_anim_base_anim31 : bubble_anim_base_anim)
{
   mapTo = "c031_anim";
   scrollOffset[0] = "0.875 0.375";
};
//*********************************
singleton Material(bubble_anim_base_anim32 : bubble_anim_base_anim)
{
   mapTo = "c032_anim";
   scrollOffset[0] = "0.000 0.500";
};
singleton Material(bubble_anim_base_anim33 : bubble_anim_base_anim)
{
   mapTo = "c033_anim";
   scrollOffset[0] = "0.125 0.500";
};
singleton Material(bubble_anim_base_anim34 : bubble_anim_base_anim)
{
   mapTo = "c034_anim";
   scrollOffset[0] = "0.250 0.500";
};
singleton Material(bubble_anim_base_anim35 : bubble_anim_base_anim)
{
   mapTo = "c035_anim";
   scrollOffset[0] = "0.375 0.500";
};
singleton Material(bubble_anim_base_anim36 : bubble_anim_base_anim)
{
   mapTo = "c036_anim";
   scrollOffset[0] = "0.500 0.500";
};
singleton Material(bubble_anim_base_anim37 : bubble_anim_base_anim)
{
   mapTo = "c037_anim";
   scrollOffset[0] = "0.625 0.500";
};
singleton Material(bubble_anim_base_anim38 : bubble_anim_base_anim)
{
   mapTo = "c038_anim";
   scrollOffset[0] = "0.750 0.500";
};
singleton Material(bubble_anim_base_anim39 : bubble_anim_base_anim)
{
   mapTo = "c039_anim";
   scrollOffset[0] = "0.875 0.500";
};
//*********************************
singleton Material(bubble_anim_base_anim40 : bubble_anim_base_anim)
{
   mapTo = "c040_anim";
   scrollOffset[0] = "0.000 0.625";
};
singleton Material(bubble_anim_base_anim41 : bubble_anim_base_anim)
{
   mapTo = "c041_anim";
   scrollOffset[0] = "0.125 0.625";
};
singleton Material(bubble_anim_base_anim42 : bubble_anim_base_anim)
{
   mapTo = "c042_anim";
   scrollOffset[0] = "0.250 0.625";
};
singleton Material(bubble_anim_base_anim43 : bubble_anim_base_anim)
{
   mapTo = "c043_anim";
   scrollOffset[0] = "0.375 0.625";
};
singleton Material(bubble_anim_base_anim44 : bubble_anim_base_anim)
{
   mapTo = "c044_anim";
   scrollOffset[0] = "0.500 0.625";
};
singleton Material(bubble_anim_base_anim45 : bubble_anim_base_anim)
{
   mapTo = "c045_anim";
   scrollOffset[0] = "0.625 0.625";
};
singleton Material(bubble_anim_base_anim46 : bubble_anim_base_anim)
{
   mapTo = "c046_anim";
   scrollOffset[0] = "0.750 0.625";
};
singleton Material(bubble_anim_base_anim47 : bubble_anim_base_anim)
{
   mapTo = "c047_anim";
   scrollOffset[0] = "0.875 0.625";
};
//*********************************
singleton Material(bubble_anim_base_anim48 : bubble_anim_base_anim)
{
   mapTo = "c048_anim";
   scrollOffset[0] = "0.000 0.750";
};
singleton Material(bubble_anim_base_anim49 : bubble_anim_base_anim)
{
   mapTo = "c049_anim";
   scrollOffset[0] = "0.125 0.750";
};
singleton Material(bubble_anim_base_anim50 : bubble_anim_base_anim)
{
   mapTo = "c050_anim";
   scrollOffset[0] = "0.250 0.750";
};
singleton Material(bubble_anim_base_anim51 : bubble_anim_base_anim)
{
   mapTo = "c051_anim";
   scrollOffset[0] = "0.375 0.750";
};
singleton Material(bubble_anim_base_anim52 : bubble_anim_base_anim)
{
   mapTo = "c052_anim";
   scrollOffset[0] = "0.500 0.750";
};
singleton Material(bubble_anim_base_anim53 : bubble_anim_base_anim)
{
   mapTo = "c053_anim";
   scrollOffset[0] = "0.625 0.750";
};
singleton Material(bubble_anim_base_anim54 : bubble_anim_base_anim)
{
   mapTo = "c054_anim";
   scrollOffset[0] = "0.750 0.750";
};
singleton Material(bubble_anim_base_anim55 : bubble_anim_base_anim)
{
   mapTo = "c055_anim";
   scrollOffset[0] = "0.875 0.750";
};
//*********************************
singleton Material(bubble_anim_base_anim56 : bubble_anim_base_anim)
{
   mapTo = "c056_anim";
   scrollOffset[0] = "0.000 0.875";
};
singleton Material(bubble_anim_base_anim57 : bubble_anim_base_anim)
{
   mapTo = "c057_anim";
   scrollOffset[0] = "0.125 0.875";
};
singleton Material(bubble_anim_base_anim58 : bubble_anim_base_anim)
{
   mapTo = "c058_anim";
   scrollOffset[0] = "0.250 0.875";
};
singleton Material(bubble_anim_base_anim59 : bubble_anim_base_anim)
{
   mapTo = "c059_anim";
   scrollOffset[0] = "0.375 0.875";
};
singleton Material(bubble_anim_base_anim60 : bubble_anim_base_anim)
{
   mapTo = "c060_anim";
   scrollOffset[0] = "0.500 0.875";
};
singleton Material(bubble_anim_base_anim61 : bubble_anim_base_anim)
{
   mapTo = "c061_anim";
   scrollOffset[0] = "0.625 0.875";
};
singleton Material(bubble_anim_base_anim62 : bubble_anim_base_anim)
{
   mapTo = "c062_anim";
   scrollOffset[0] = "0.750 0.875";
};
singleton Material(bubble_anim_base_anim63 : bubble_anim_base_anim)
{
   mapTo = "c063_anim";
   scrollOffset[0] = "0.875 0.875";
};


singleton Material(taskbar_body_Material)
{
   mapTo = "Material";
   diffuseColor[0] = "0.13 0.13 0.13 1";
   specular[0] = "0 0 0 1";
   specularPower[0] = "50";
   doubleSided = "1";

};


singleton Material(icon_view1_view)
{
   mapTo = "view";
   diffuseMap[0] = "viewbutton";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucentBlendOp = "None";
};

singleton Material(icon_taskbar_base_taskicon)
{
   mapTo = "base_taskicon";
   diffuseMap[0] = "icons";
   specular[0] = "0.5 0.5 0.5 1";
   specularPower[0] = "50";
   doubleSided = "1";
   translucent = "1";
   animFlags[0] = "0x00000001";
   scrollDir[0] = "0.103 0.241";
   scrollOffset[0] = "0.0 0.0";
};
//---spieces----------------------------------------------------
singleton Material(species_up : icon_taskbar_base_taskicon)
{
   mapTo = "spup_taskicon";
   scrollOffset[0] = "0.00 0.00";
};
singleton Material(species_active : icon_taskbar_base_taskicon)
{
   mapTo = "spac_taskicon";
   scrollOffset[0] = "0.00 0.50";
};
singleton Material(species_touched : icon_taskbar_base_taskicon)
{
   mapTo = "spto_taskicon";
   scrollOffset[0] = "0.00 0.25";
};
//---conservation----------------------------------------------------
singleton Material(conservation_up : icon_taskbar_base_taskicon)
{
   mapTo = "coup_taskicon";
   scrollOffset[0] = "0.25 0.00";
};
singleton Material(conservation_active : icon_taskbar_base_taskicon)
{
   mapTo = "coac_taskicon";
   scrollOffset[0] = "0.25 0.50";
};
singleton Material(conservation_touched : icon_taskbar_base_taskicon)
{
   mapTo = "coto_taskicon";
   scrollOffset[0] = "0.25 0.25";
};
//---credits----------------------------------------------------
singleton Material(credits_up : icon_taskbar_base_taskicon)
{
   mapTo = "crup_taskicon";
   scrollOffset[0] = "0.50 0.00";
};
singleton Material(credits_active : icon_taskbar_base_taskicon)
{
   mapTo = "crac_taskicon";
   scrollOffset[0] = "0.50 0.50";
};
singleton Material(credits_touched : icon_taskbar_base_taskicon)
{
   mapTo = "crto_taskicon";
   scrollOffset[0] = "0.50 0.25";
};
//---credits----------------------------------------------------
singleton Material(newstuff_up : icon_taskbar_base_taskicon)
{
   mapTo = "nsup_taskicon";
   scrollOffset[0] = "0.75 0.00";
};
singleton Material(newstuff_active : icon_taskbar_base_taskicon)
{
   mapTo = "nsac_taskicon";
   scrollOffset[0] = "0.75 0.50";
};
singleton Material(newstuff_touched : icon_taskbar_base_taskicon)
{
   mapTo = "nsto_taskicon";
   scrollOffset[0] = "0.75 0.25";
};
//*********************************
// timer
//*********************************
singleton Material(bubbleUI_front_active_mat)
{
//   mapTo = "timer_sequence";
//   diffuseMap[0] = "art/cube/interface/timer_sequence.png";
   mapTo = "bubbleUI_front_active";
   diffuseMap[0] = "art/cube/interface/bubbleUI_front_active.png";



   emissive[0] = "1";
   castShadows = "0";
   translucent = "1";
   translucentZWrite = "0";
   alphaRef = "18";
/*   
   animFlags[0] = "0x00000010";
   scrollOffset[0] = "0 0";
   sequenceFramePerSec[0] = "20";
   sequenceSegmentSize[0] = "0.0078125";
   */
   pixelSpecular[0] = "1";
   useAnisotropic[0] = "1";
   translucentBlendOp = "Sub";
};


singleton Material(bubbleui_back_mat : bubbleUI_front_active_mat)
{
   mapTo = "bubbleUI_back";
   diffuseMap[0] = "art/cube/interface/bubbleUI_back";
};

singleton Material(bubbleui_front_inactive_mat : bubbleUI_front_active_mat)
{
   mapTo = "bubbleUI_front_inactive";
   diffuseMap[0] = "art/cube/interface/bubbleUI_front_inactive";
};

singleton Material(bubbleui_timer_mat : bubbleUI_front_active_mat)
{
   mapTo = "bubbleUI_timer";
   diffuseMap[0] = "art/cube/interface/timer_sequence.png";
   
};

singleton Material(curtain_mat)
{
//   mapTo = "timer_sequence";
//   diffuseMap[0] = "art/cube/interface/timer_sequence.png";
   mapTo = "curtain_diffuse.png";
   diffuseMap[0] = "art/cube/interface/curtain_diffuse.png";
   pixelSpecular[0] = "1";
   doubleSided = true;
};



//**********************************************************************
// bubble titles
//**********************************************************************
singleton Material(bubbleui_title_mat : bubbleUI_front_active_mat)
{
   mapTo = "bubbleui_title";
   diffuseMap[0] = "art/cube/interface/bubbleui_title.png";
};
singleton Material(bubbleui_title_bad:             bubbleui_title_mat){  mapTo = "title_bad";            diffuseMap[0] = "art/cube/interface/titles/bad.png";};
singleton Material(bubbleui_title_aastaghorn:      bubbleui_title_mat){  mapTo = "title_aastaghorn";     diffuseMap[0] = "art/cube/interface/titles/bottlebrushstaghorn.png";};
singleton Material(bubbleui_title_afstaghorn:      bubbleui_title_mat){  mapTo = "title_afstaghorn";     diffuseMap[0] = "art/cube/interface/titles/staghorn.png";};
singleton Material(bubbleui_title_ahplate:         bubbleui_title_mat){  mapTo = "title_ahplate";        diffuseMap[0] = "art/cube/interface/titles/platecoral.png";};
singleton Material(bubbleui_title_anstaghorn:      bubbleui_title_mat){  mapTo = "title_anstaghorn";     diffuseMap[0] = "art/cube/interface/titles/smallstaghorn.png";};
singleton Material(bubbleui_title_basslet:         bubbleui_title_mat){  mapTo = "title_basslet";        diffuseMap[0] = "art/cube/interface/titles/basslet.png";};
singleton Material(bubbleui_title_bluetang:        bubbleui_title_mat){  mapTo = "title_bluetang";       diffuseMap[0] = "art/cube/interface/titles/bluetang.png";};
singleton Material(bubbleui_title_brain:           bubbleui_title_mat){  mapTo = "title_brain";          diffuseMap[0] = "art/cube/interface/titles/braincoral";};
singleton Material(bubbleui_title_catspawcoral:    bubbleui_title_mat){  mapTo = "title_catspawcoral";   diffuseMap[0] = "art/cube/interface/titles/catspawcoral.png";};
singleton Material(bubbleui_title_clam:            bubbleui_title_mat){  mapTo = "title_clam";           diffuseMap[0] = "art/cube/interface/titles/clam.png";};
singleton Material(bubbleui_title_cleaner:         bubbleui_title_mat){  mapTo = "title_cleaner";        diffuseMap[0] = "art/cube/interface/titles/cleaner.png";};
singleton Material(bubbleui_title_clownfish:       bubbleui_title_mat){  mapTo = "title_clownfish";      diffuseMap[0] = "art/cube/interface/titles/clownfish.png";};
singleton Material(bubbleui_title_cucumber:        bubbleui_title_mat){  mapTo = "title_cucumber";       diffuseMap[0] = "art/cube/interface/titles/cucumber.png";};
singleton Material(bubbleui_title_cuttlefish:      bubbleui_title_mat){  mapTo = "title_cuttlefish";     diffuseMap[0] = "art/cube/interface/titles/cuttlefish.png";};
singleton Material(bubbleui_title_dendro:          bubbleui_title_mat){  mapTo = "title_dendro";         diffuseMap[0] = "art/cube/interface/titles/dendro.png";};
singleton Material(bubbleui_title_dugong:          bubbleui_title_mat){  mapTo = "title_dugong";         diffuseMap[0] = "art/cube/interface/titles/dugong.png";};
singleton Material(bubbleui_title_fanworm:         bubbleui_title_mat){  mapTo = "title_fanworm";        diffuseMap[0] = "art/cube/interface/titles/fanworm.png";};
singleton Material(bubbleui_title_greenalgae:      bubbleui_title_mat){  mapTo = "title_greenalgae";     diffuseMap[0] = "art/cube/interface/titles/algae.png";};
singleton Material(bubbleui_title_harlequinshrimp: bubbleui_title_mat){  mapTo = "title_harlequinshrimp";diffuseMap[0] = "art/cube/interface/titles/harlequinshrimp.png";};
singleton Material(bubbleui_title_hermit:          bubbleui_title_mat){  mapTo = "title_hermit";         diffuseMap[0] = "art/cube/interface/titles/hermit.png";};
singleton Material(bubbleui_title_jellyfish:       bubbleui_title_mat){  mapTo = "title_jellyfish";      diffuseMap[0] = "art/cube/interface/titles/jellyfish.png";};
singleton Material(bubbleui_title_lionfish:        bubbleui_title_mat){  mapTo = "title_lionfish";       diffuseMap[0] = "art/cube/interface/titles/lionfish.png";};
singleton Material(bubbleui_title_maganemone:      bubbleui_title_mat){  mapTo = "title_maganemone";     diffuseMap[0] = "art/cube/interface/titles/maganemone.png";};
singleton Material(bubbleui_title_manta:           bubbleui_title_mat){  mapTo = "title_manta";          diffuseMap[0] = "art/cube/interface/titles/manta.png";};
singleton Material(bubbleui_title_moorish:         bubbleui_title_mat){  mapTo = "title_moorish";        diffuseMap[0] = "art/cube/interface/titles/moorish.png";};
singleton Material(bubbleui_title_moray:           bubbleui_title_mat){  mapTo = "title_moray";          diffuseMap[0] = "art/cube/interface/titles/moray.png";};
singleton Material(bubbleui_title_nudibranch:      bubbleui_title_mat){  mapTo = "title_nudibranch";     diffuseMap[0] = "art/cube/interface/titles/nudibranch.png";};
singleton Material(bubbleui_title_parrotfish:      bubbleui_title_mat){  mapTo = "title_parrotfish";     diffuseMap[0] = "art/cube/interface/titles/parrotfish.png";};
singleton Material(bubbleui_title_porcelaincrab:   bubbleui_title_mat){  mapTo = "title_porcelaincrab";  diffuseMap[0] = "art/cube/interface/titles/porcelaincrab.png";};
singleton Material(bubbleui_title_porcupinefish:   bubbleui_title_mat){  mapTo = "title_porcupinefish";  diffuseMap[0] = "art/cube/interface/titles/porcupinefish.png";};
singleton Material(bubbleui_title_porites:         bubbleui_title_mat){  mapTo = "title_porites";        diffuseMap[0] = "art/cube/interface/titles/porites.png";};
singleton Material(bubbleui_title_potatocod:       bubbleui_title_mat){  mapTo = "title_potatocod";      diffuseMap[0] = "art/cube/interface/titles/pcod.png";};
singleton Material(bubbleui_title_ray:             bubbleui_title_mat){  mapTo = "title_ray";            diffuseMap[0] = "art/cube/interface/titles/ray.png";};
singleton Material(bubbleui_title_remora:          bubbleui_title_mat){  mapTo = "title_remora";         diffuseMap[0] = "art/cube/interface/titles/remora.png";};
singleton Material(bubbleui_title_sardinella:      bubbleui_title_mat){  mapTo = "title_sardinella";     diffuseMap[0] = "art/cube/interface/titles/sardinella.png";};
singleton Material(bubbleui_title_seafan:          bubbleui_title_mat){  mapTo = "title_seafan";         diffuseMap[0] = "art/cube/interface/titles/seafan.png";};
singleton Material(bubbleui_title_seagrass:        bubbleui_title_mat){  mapTo = "title_seagrass";       diffuseMap[0] = "art/cube/interface/titles/seagrass.png";};
singleton Material(bubbleui_title_seahorse:        bubbleui_title_mat){  mapTo = "title_seahorse";       diffuseMap[0] = "art/cube/interface/titles/seahorse.png";};
singleton Material(bubbleui_title_shark:           bubbleui_title_mat){  mapTo = "title_shark";          diffuseMap[0] = "art/cube/interface/titles/greyshark.png";};
singleton Material(bubbleui_title_snapper:         bubbleui_title_mat){  mapTo = "title_snapper";        diffuseMap[0] = "art/cube/interface/titles/snapper.png";};
singleton Material(bubbleui_title_starfish:        bubbleui_title_mat){  mapTo = "title_starfish";       diffuseMap[0] = "art/cube/interface/titles/starfish.png";};
singleton Material(bubbleui_title_trevally:        bubbleui_title_mat){  mapTo = "title_trevally";       diffuseMap[0] = "art/cube/interface/titles/trevally.png";};
singleton Material(bubbleui_title_tubeanemone:     bubbleui_title_mat){  mapTo = "title_tubeanemone";    diffuseMap[0] = "art/cube/interface/titles/tubeanemone.png";};
singleton Material(bubbleui_title_tunicate:        bubbleui_title_mat){  mapTo = "title_tunicate";       diffuseMap[0] = "art/cube/interface/titles/tunicate.png";};
singleton Material(bubbleui_title_turtle:          bubbleui_title_mat){  mapTo = "title_turtle";         diffuseMap[0] = "art/cube/interface/titles/turtle.png";};
singleton Material(bubbleui_title_tuskfish:        bubbleui_title_mat){  mapTo = "title_tuskfish";       diffuseMap[0] = "art/cube/interface/titles/tuskfish.png";};
singleton Material(bubbleui_title_urchin:          bubbleui_title_mat){  mapTo = "title_urchin";         diffuseMap[0] = "art/cube/interface/titles/urchin.png";};
singleton Material(bubbleui_title_whale:           bubbleui_title_mat){  mapTo = "title_whale";          diffuseMap[0] = "art/cube/interface/titles/whale.png";};
singleton Material(bubbleui_title_whaleshark:      bubbleui_title_mat){  mapTo = "title_whaleshark";     diffuseMap[0] = "art/cube/interface/titles/whaleshark.png";};
singleton Material(bubbleui_title_sunfish:         bubbleui_title_mat){  mapTo = "title_sunfish";        diffuseMap[0] = "art/cube/interface/titles/sunfish.png";};
singleton Material(bubbleui_title_hammerhead:      bubbleui_title_mat){  mapTo = "title_hammerhead";     diffuseMap[0] = "art/cube/interface/titles/hammerhead.png";};
singleton Material(bubbleui_title_tigershark:      bubbleui_title_mat){  mapTo = "title_tigershark";     diffuseMap[0] = "art/cube/interface/titles/tigershark.png";};
singleton Material(bubbleui_title_pufferfish:      bubbleui_title_mat){  mapTo = "title_pufferfish";     diffuseMap[0] = "art/cube/interface/titles/porcupinefish.png";};



//**********************************************************************
// animated bubble timer
//**********************************************************************
singleton Material(timer_sequence_mat : bubbleUI_front_active_mat)
{
   mapTo = "timer_sequence";
   diffuseMap[0] = "timer_sequence.png";

   animFlags[0] = "0x00000001";
   scrollDir[0] = "0.103 0.241";
   scrollOffset[0] = "0.0 0.0";
};

$OFFSET = (5/2048);

singleton Material(timer0000 : timer_sequence_mat){   mapTo = "ti000_sequence";  scrollOffset[0] = 000/128-$OFFSET SPC  0;};
singleton Material(timer0001 : timer_sequence_mat){   mapTo = "ti001_sequence";  scrollOffset[0] = 001/128-$OFFSET SPC  0;};
singleton Material(timer0002 : timer_sequence_mat){   mapTo = "ti002_sequence";  scrollOffset[0] = 002/128-$OFFSET SPC  0;};
singleton Material(timer0003 : timer_sequence_mat){   mapTo = "ti003_sequence";  scrollOffset[0] = 003/128-$OFFSET SPC  0;};
singleton Material(timer0004 : timer_sequence_mat){   mapTo = "ti004_sequence";  scrollOffset[0] = 004/128-$OFFSET SPC  0;};
singleton Material(timer0005 : timer_sequence_mat){   mapTo = "ti005_sequence";  scrollOffset[0] = 005/128-$OFFSET SPC  0;};
singleton Material(timer0006 : timer_sequence_mat){   mapTo = "ti006_sequence";  scrollOffset[0] = 006/128-$OFFSET SPC  0;};
singleton Material(timer0007 : timer_sequence_mat){   mapTo = "ti007_sequence";  scrollOffset[0] = 007/128-$OFFSET SPC  0;};
singleton Material(timer0008 : timer_sequence_mat){   mapTo = "ti008_sequence";  scrollOffset[0] = 008/128-$OFFSET SPC  0;};
singleton Material(timer0009 : timer_sequence_mat){   mapTo = "ti009_sequence";  scrollOffset[0] = 009/128-$OFFSET SPC  0;};
singleton Material(timer0010 : timer_sequence_mat){   mapTo = "ti010_sequence";  scrollOffset[0] = 010/128-$OFFSET SPC  0;};
singleton Material(timer0011 : timer_sequence_mat){   mapTo = "ti011_sequence";  scrollOffset[0] = 011/128-$OFFSET SPC  0;};
singleton Material(timer0012 : timer_sequence_mat){   mapTo = "ti012_sequence";  scrollOffset[0] = 012/128-$OFFSET SPC  0;};
singleton Material(timer0013 : timer_sequence_mat){   mapTo = "ti013_sequence";  scrollOffset[0] = 013/128-$OFFSET SPC  0;};
singleton Material(timer0014 : timer_sequence_mat){   mapTo = "ti014_sequence";  scrollOffset[0] = 014/128-$OFFSET SPC  0;};
singleton Material(timer0015 : timer_sequence_mat){   mapTo = "ti015_sequence";  scrollOffset[0] = 015/128-$OFFSET SPC  0;};
singleton Material(timer0016 : timer_sequence_mat){   mapTo = "ti016_sequence";  scrollOffset[0] = 016/128-$OFFSET SPC  0;};
singleton Material(timer0017 : timer_sequence_mat){   mapTo = "ti017_sequence";  scrollOffset[0] = 017/128-$OFFSET SPC  0;};
singleton Material(timer0018 : timer_sequence_mat){   mapTo = "ti018_sequence";  scrollOffset[0] = 018/128-$OFFSET SPC  0;};
singleton Material(timer0019 : timer_sequence_mat){   mapTo = "ti019_sequence";  scrollOffset[0] = 019/128-$OFFSET SPC  0;};

singleton Material(timer0020 : timer_sequence_mat){   mapTo = "ti020_sequence";  scrollOffset[0] = 020/128-$OFFSET SPC  0;};
singleton Material(timer0021 : timer_sequence_mat){   mapTo = "ti021_sequence";  scrollOffset[0] = 021/128-$OFFSET SPC  0;};
singleton Material(timer0022 : timer_sequence_mat){   mapTo = "ti022_sequence";  scrollOffset[0] = 022/128-$OFFSET SPC  0;};
singleton Material(timer0023 : timer_sequence_mat){   mapTo = "ti023_sequence";  scrollOffset[0] = 023/128-$OFFSET SPC  0;};
singleton Material(timer0024 : timer_sequence_mat){   mapTo = "ti024_sequence";  scrollOffset[0] = 024/128-$OFFSET SPC  0;};
singleton Material(timer0025 : timer_sequence_mat){   mapTo = "ti025_sequence";  scrollOffset[0] = 025/128-$OFFSET SPC  0;};
singleton Material(timer0026 : timer_sequence_mat){   mapTo = "ti026_sequence";  scrollOffset[0] = 026/128-$OFFSET SPC  0;};
singleton Material(timer0027 : timer_sequence_mat){   mapTo = "ti027_sequence";  scrollOffset[0] = 027/128-$OFFSET SPC  0;};
singleton Material(timer0028 : timer_sequence_mat){   mapTo = "ti028_sequence";  scrollOffset[0] = 028/128-$OFFSET SPC  0;};
singleton Material(timer0029 : timer_sequence_mat){   mapTo = "ti029_sequence";  scrollOffset[0] = 029/128-$OFFSET SPC  0;};

singleton Material(timer0030 : timer_sequence_mat){   mapTo = "ti030_sequence";  scrollOffset[0] = 030/128-$OFFSET SPC  0;};
singleton Material(timer0031 : timer_sequence_mat){   mapTo = "ti031_sequence";  scrollOffset[0] = 031/128-$OFFSET SPC  0;};
singleton Material(timer0032 : timer_sequence_mat){   mapTo = "ti032_sequence";  scrollOffset[0] = 032/128-$OFFSET SPC  0;};
singleton Material(timer0033 : timer_sequence_mat){   mapTo = "ti033_sequence";  scrollOffset[0] = 033/128-$OFFSET SPC  0;};
singleton Material(timer0034 : timer_sequence_mat){   mapTo = "ti034_sequence";  scrollOffset[0] = 034/128-$OFFSET SPC  0;};
singleton Material(timer0035 : timer_sequence_mat){   mapTo = "ti035_sequence";  scrollOffset[0] = 035/128-$OFFSET SPC  0;};
singleton Material(timer0036 : timer_sequence_mat){   mapTo = "ti036_sequence";  scrollOffset[0] = 036/128-$OFFSET SPC  0;};
singleton Material(timer0037 : timer_sequence_mat){   mapTo = "ti037_sequence";  scrollOffset[0] = 037/128-$OFFSET SPC  0;};
singleton Material(timer0038 : timer_sequence_mat){   mapTo = "ti038_sequence";  scrollOffset[0] = 038/128-$OFFSET SPC  0;};
singleton Material(timer0039 : timer_sequence_mat){   mapTo = "ti039_sequence";  scrollOffset[0] = 039/128-$OFFSET SPC  0;};

singleton Material(timer0040 : timer_sequence_mat){   mapTo = "ti040_sequence";  scrollOffset[0] = 040/128-$OFFSET SPC  0;};
singleton Material(timer0041 : timer_sequence_mat){   mapTo = "ti041_sequence";  scrollOffset[0] = 041/128-$OFFSET SPC  0;};
singleton Material(timer0042 : timer_sequence_mat){   mapTo = "ti042_sequence";  scrollOffset[0] = 042/128-$OFFSET SPC  0;};
singleton Material(timer0043 : timer_sequence_mat){   mapTo = "ti043_sequence";  scrollOffset[0] = 043/128-$OFFSET SPC  0;};
singleton Material(timer0044 : timer_sequence_mat){   mapTo = "ti044_sequence";  scrollOffset[0] = 044/128-$OFFSET SPC  0;};
singleton Material(timer0045 : timer_sequence_mat){   mapTo = "ti045_sequence";  scrollOffset[0] = 045/128-$OFFSET SPC  0;};
singleton Material(timer0046 : timer_sequence_mat){   mapTo = "ti046_sequence";  scrollOffset[0] = 046/128-$OFFSET SPC  0;};
singleton Material(timer0047 : timer_sequence_mat){   mapTo = "ti047_sequence";  scrollOffset[0] = 047/128-$OFFSET SPC  0;};
singleton Material(timer0048 : timer_sequence_mat){   mapTo = "ti048_sequence";  scrollOffset[0] = 048/128-$OFFSET SPC  0;};
singleton Material(timer0049 : timer_sequence_mat){   mapTo = "ti049_sequence";  scrollOffset[0] = 049/128-$OFFSET SPC  0;};

singleton Material(timer0050 : timer_sequence_mat){   mapTo = "ti050_sequence";  scrollOffset[0] = 050/128-$OFFSET SPC  0;};
singleton Material(timer0051 : timer_sequence_mat){   mapTo = "ti051_sequence";  scrollOffset[0] = 051/128-$OFFSET SPC  0;};
singleton Material(timer0052 : timer_sequence_mat){   mapTo = "ti052_sequence";  scrollOffset[0] = 052/128-$OFFSET SPC  0;};
singleton Material(timer0053 : timer_sequence_mat){   mapTo = "ti053_sequence";  scrollOffset[0] = 053/128-$OFFSET SPC  0;};
singleton Material(timer0054 : timer_sequence_mat){   mapTo = "ti054_sequence";  scrollOffset[0] = 054/128-$OFFSET SPC  0;};
singleton Material(timer0055 : timer_sequence_mat){   mapTo = "ti055_sequence";  scrollOffset[0] = 055/128-$OFFSET SPC  0;};
singleton Material(timer0056 : timer_sequence_mat){   mapTo = "ti056_sequence";  scrollOffset[0] = 056/128-$OFFSET SPC  0;};
singleton Material(timer0057 : timer_sequence_mat){   mapTo = "ti057_sequence";  scrollOffset[0] = 057/128-$OFFSET SPC  0;};
singleton Material(timer0058 : timer_sequence_mat){   mapTo = "ti058_sequence";  scrollOffset[0] = 058/128-$OFFSET SPC  0;};
singleton Material(timer0059 : timer_sequence_mat){   mapTo = "ti059_sequence";  scrollOffset[0] = 059/128-$OFFSET SPC  0;};

singleton Material(timer0060 : timer_sequence_mat){   mapTo = "ti060_sequence";  scrollOffset[0] = 060/128-$OFFSET SPC  0;};
singleton Material(timer0061 : timer_sequence_mat){   mapTo = "ti061_sequence";  scrollOffset[0] = 061/128-$OFFSET SPC  0;};
singleton Material(timer0062 : timer_sequence_mat){   mapTo = "ti062_sequence";  scrollOffset[0] = 062/128-$OFFSET SPC  0;};
singleton Material(timer0063 : timer_sequence_mat){   mapTo = "ti063_sequence";  scrollOffset[0] = 063/128-$OFFSET SPC  0;};
singleton Material(timer0064 : timer_sequence_mat){   mapTo = "ti064_sequence";  scrollOffset[0] = 064/128-$OFFSET SPC  0;};
singleton Material(timer0065 : timer_sequence_mat){   mapTo = "ti065_sequence";  scrollOffset[0] = 065/128-$OFFSET SPC  0;};
singleton Material(timer0066 : timer_sequence_mat){   mapTo = "ti066_sequence";  scrollOffset[0] = 066/128-$OFFSET SPC  0;};
singleton Material(timer0067 : timer_sequence_mat){   mapTo = "ti067_sequence";  scrollOffset[0] = 067/128-$OFFSET SPC  0;};
singleton Material(timer0068 : timer_sequence_mat){   mapTo = "ti068_sequence";  scrollOffset[0] = 068/128-$OFFSET SPC  0;};
singleton Material(timer0069 : timer_sequence_mat){   mapTo = "ti069_sequence";  scrollOffset[0] = 069/128-$OFFSET SPC  0;};

singleton Material(timer0070 : timer_sequence_mat){   mapTo = "ti070_sequence";  scrollOffset[0] = 070/128-$OFFSET SPC  0;};
singleton Material(timer0071 : timer_sequence_mat){   mapTo = "ti071_sequence";  scrollOffset[0] = 071/128-$OFFSET SPC  0;};
singleton Material(timer0072 : timer_sequence_mat){   mapTo = "ti072_sequence";  scrollOffset[0] = 072/128-$OFFSET SPC  0;};
singleton Material(timer0073 : timer_sequence_mat){   mapTo = "ti073_sequence";  scrollOffset[0] = 073/128-$OFFSET SPC  0;};
singleton Material(timer0074 : timer_sequence_mat){   mapTo = "ti074_sequence";  scrollOffset[0] = 074/128-$OFFSET SPC  0;};
singleton Material(timer0075 : timer_sequence_mat){   mapTo = "ti075_sequence";  scrollOffset[0] = 075/128-$OFFSET SPC  0;};
singleton Material(timer0076 : timer_sequence_mat){   mapTo = "ti076_sequence";  scrollOffset[0] = 076/128-$OFFSET SPC  0;};
singleton Material(timer0077 : timer_sequence_mat){   mapTo = "ti077_sequence";  scrollOffset[0] = 077/128-$OFFSET SPC  0;};
singleton Material(timer0078 : timer_sequence_mat){   mapTo = "ti078_sequence";  scrollOffset[0] = 078/128-$OFFSET SPC  0;};
singleton Material(timer0079 : timer_sequence_mat){   mapTo = "ti079_sequence";  scrollOffset[0] = 079/128-$OFFSET SPC  0;};
singleton Material(timer0079 : timer_sequence_mat){   mapTo = "ti079_sequence";  scrollOffset[0] = 079/128-$OFFSET SPC  0;};


singleton Material(timer0080 : timer_sequence_mat){   mapTo = "ti080_sequence";  scrollOffset[0] = 080/128-$OFFSET SPC  0;};
singleton Material(timer0081 : timer_sequence_mat){   mapTo = "ti081_sequence";  scrollOffset[0] = 081/128-$OFFSET SPC  0;};
singleton Material(timer0082 : timer_sequence_mat){   mapTo = "ti082_sequence";  scrollOffset[0] = 082/128-$OFFSET SPC  0;};
singleton Material(timer0083 : timer_sequence_mat){   mapTo = "ti083_sequence";  scrollOffset[0] = 083/128-$OFFSET SPC  0;};
singleton Material(timer0084 : timer_sequence_mat){   mapTo = "ti084_sequence";  scrollOffset[0] = 084/128-$OFFSET SPC  0;};
singleton Material(timer0085 : timer_sequence_mat){   mapTo = "ti085_sequence";  scrollOffset[0] = 085/128-$OFFSET SPC  0;};
singleton Material(timer0086 : timer_sequence_mat){   mapTo = "ti086_sequence";  scrollOffset[0] = 086/128-$OFFSET SPC  0;};
singleton Material(timer0087 : timer_sequence_mat){   mapTo = "ti087_sequence";  scrollOffset[0] = 087/128-$OFFSET SPC  0;};
singleton Material(timer0088 : timer_sequence_mat){   mapTo = "ti088_sequence";  scrollOffset[0] = 088/128-$OFFSET SPC  0;};
singleton Material(timer0089 : timer_sequence_mat){   mapTo = "ti089_sequence";  scrollOffset[0] = 089/128-$OFFSET SPC  0;};

singleton Material(timer0090 : timer_sequence_mat){   mapTo = "ti090_sequence";  scrollOffset[0] = 090/128-$OFFSET SPC  0;};
singleton Material(timer0091 : timer_sequence_mat){   mapTo = "ti091_sequence";  scrollOffset[0] = 091/128-$OFFSET SPC  0;};
singleton Material(timer0092 : timer_sequence_mat){   mapTo = "ti092_sequence";  scrollOffset[0] = 092/128-$OFFSET SPC  0;};
singleton Material(timer0093 : timer_sequence_mat){   mapTo = "ti093_sequence";  scrollOffset[0] = 093/128-$OFFSET SPC  0;};
singleton Material(timer0094 : timer_sequence_mat){   mapTo = "ti094_sequence";  scrollOffset[0] = 094/128-$OFFSET SPC  0;};
singleton Material(timer0095 : timer_sequence_mat){   mapTo = "ti095_sequence";  scrollOffset[0] = 095/128-$OFFSET SPC  0;};
singleton Material(timer0096 : timer_sequence_mat){   mapTo = "ti096_sequence";  scrollOffset[0] = 096/128-$OFFSET SPC  0;};
singleton Material(timer0097 : timer_sequence_mat){   mapTo = "ti097_sequence";  scrollOffset[0] = 097/128-$OFFSET SPC  0;};
singleton Material(timer0098 : timer_sequence_mat){   mapTo = "ti098_sequence";  scrollOffset[0] = 098/128-$OFFSET SPC  0;};
singleton Material(timer0099 : timer_sequence_mat){   mapTo = "ti099_sequence";  scrollOffset[0] = 099/128-$OFFSET SPC  0;};

singleton Material(timer0100 : timer_sequence_mat){   mapTo = "ti100_sequence";  scrollOffset[0] = 100/128-$OFFSET SPC  0;};
singleton Material(timer0101 : timer_sequence_mat){   mapTo = "ti101_sequence";  scrollOffset[0] = 101/128-$OFFSET SPC  0;};
singleton Material(timer0102 : timer_sequence_mat){   mapTo = "ti102_sequence";  scrollOffset[0] = 102/128-$OFFSET SPC  0;};
singleton Material(timer0103 : timer_sequence_mat){   mapTo = "ti103_sequence";  scrollOffset[0] = 103/128-$OFFSET SPC  0;};
singleton Material(timer0104 : timer_sequence_mat){   mapTo = "ti104_sequence";  scrollOffset[0] = 104/128-$OFFSET SPC  0;};
singleton Material(timer0105 : timer_sequence_mat){   mapTo = "ti105_sequence";  scrollOffset[0] = 105/128-$OFFSET SPC  0;};
singleton Material(timer0106 : timer_sequence_mat){   mapTo = "ti106_sequence";  scrollOffset[0] = 106/128-$OFFSET SPC  0;};
singleton Material(timer0107 : timer_sequence_mat){   mapTo = "ti107_sequence";  scrollOffset[0] = 107/128-$OFFSET SPC  0;};
singleton Material(timer0108 : timer_sequence_mat){   mapTo = "ti108_sequence";  scrollOffset[0] = 108/128-$OFFSET SPC  0;};
singleton Material(timer0109 : timer_sequence_mat){   mapTo = "ti109_sequence";  scrollOffset[0] = 109/128-$OFFSET SPC  0;};

singleton Material(timer0110 : timer_sequence_mat){   mapTo = "ti110_sequence";  scrollOffset[0] = 110/128-$OFFSET SPC  0;};
singleton Material(timer0111 : timer_sequence_mat){   mapTo = "ti111_sequence";  scrollOffset[0] = 111/128-$OFFSET SPC  0;};
singleton Material(timer0112 : timer_sequence_mat){   mapTo = "ti112_sequence";  scrollOffset[0] = 112/128-$OFFSET SPC  0;};
singleton Material(timer0113 : timer_sequence_mat){   mapTo = "ti113_sequence";  scrollOffset[0] = 113/128-$OFFSET SPC  0;};
singleton Material(timer0114 : timer_sequence_mat){   mapTo = "ti114_sequence";  scrollOffset[0] = 114/128-$OFFSET SPC  0;};
singleton Material(timer0115 : timer_sequence_mat){   mapTo = "ti115_sequence";  scrollOffset[0] = 115/128-$OFFSET SPC  0;};
singleton Material(timer0116 : timer_sequence_mat){   mapTo = "ti116_sequence";  scrollOffset[0] = 116/128-$OFFSET SPC  0;};
singleton Material(timer0117 : timer_sequence_mat){   mapTo = "ti117_sequence";  scrollOffset[0] = 117/128-$OFFSET SPC  0;};
singleton Material(timer0118 : timer_sequence_mat){   mapTo = "ti118_sequence";  scrollOffset[0] = 118/128-$OFFSET SPC  0;};
singleton Material(timer0119 : timer_sequence_mat){   mapTo = "ti119_sequence";  scrollOffset[0] = 119/128-$OFFSET SPC  0;};

singleton Material(timer0120 : timer_sequence_mat){   mapTo = "ti120_sequence";  scrollOffset[0] = 120/128-$OFFSET SPC  0;};
singleton Material(timer0121 : timer_sequence_mat){   mapTo = "ti121_sequence";  scrollOffset[0] = 121/128-$OFFSET SPC  0;};
singleton Material(timer0122 : timer_sequence_mat){   mapTo = "ti122_sequence";  scrollOffset[0] = 122/128-$OFFSET SPC  0;};
singleton Material(timer0123 : timer_sequence_mat){   mapTo = "ti123_sequence";  scrollOffset[0] = 123/128-$OFFSET SPC  0;};
singleton Material(timer0124 : timer_sequence_mat){   mapTo = "ti124_sequence";  scrollOffset[0] = 124/128-$OFFSET SPC  0;};
singleton Material(timer0125 : timer_sequence_mat){   mapTo = "ti125_sequence";  scrollOffset[0] = 125/128-$OFFSET SPC  0;};
singleton Material(timer0126 : timer_sequence_mat){   mapTo = "ti126_sequence";  scrollOffset[0] = 126/128-$OFFSET SPC  0;};
singleton Material(timer0127 : timer_sequence_mat){   mapTo = "ti127_sequence";  scrollOffset[0] = 127/128-$OFFSET SPC  0;};
singleton Material(timer0128 : timer_sequence_mat){   mapTo = "ti128_sequence";  scrollOffset[0] = 128/128-$OFFSET SPC  0;};


