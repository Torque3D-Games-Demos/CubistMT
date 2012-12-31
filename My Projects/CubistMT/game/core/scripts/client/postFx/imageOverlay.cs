

$ImageOverlayPostFX::diffuse = "art/cube/sets/reef_full2_diffuse.png";
$ImageOverlayPostFX::normal = "art/cube/sets/reef_full2_normal.png";
//$ImageOverlayPostFX::diffuse = "art/cube/sets/dugong_diffuse.png";
//$ImageOverlayPostFX::normal = "art/cube/sets/dugong_diffuse.png";

singleton ShaderData( ImageOverlayShader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/imageOverlayP.hlsl";
   
//   samplerNames[0] = "$prepassTex";
//   samplerNames[0] = "$backBuffer";
   samplerNames[0] = "$inTex";

   samplerNames[1] = "$diffuseTex";
   pixVersion = 3.0;   
};

singleton ShaderData( ImageOverlay2Shader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/imageOverlay2P.hlsl";
   
   samplerNames[0] = "$prepassTex";
   samplerNames[1] = "$backBuffer";
   samplerNames[2] = "$diffuseTex";
//   defines = "DIFFUSE_PASS";
   pixVersion = 3.0;   
};
singleton ShaderData( ImageOverlay3Shader )
{
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/imageOverlay3P.hlsl";
   
//   samplerNames[0] = "$prepassTex";
//   samplerNames[0] = "$backBuffer";
   samplerNames[0] = "$outTex";
 //  samplerNames[1] = "$diffuseTex";
   pixVersion = 3.0;   
};

singleton GFXStateBlockData( ImageOverlayStateBlock : PFX_DefaultStateBlock )
{
   
      zDefined = true;
      zEnable =false;
      zWriteEnable = false;
//   blendDefined = true;
//   blendEnable = false;

   samplersDefined = true;
   samplerStates[0] = SamplerClampPoint;
 //  samplerStates[1] = SamplerClampPoint;   
   samplerStates[1] = SamplerClampLinear;
};

singleton GFXStateBlockData( ImageOverlay2StateBlock : PFX_DefaultStateBlock )
{
   
      zDefined = true;
   zEnable = true;
   zWriteEnable = true;

   samplersDefined = true;
   samplerStates[0] = SamplerClampPoint;
   samplerStates[1] = SamplerClampPoint;   
   samplerStates[2] = SamplerClampLinear;
};
/*
singleton PostEffect( ImageOverlayPostFX )
{
   isEnabled = true;
   allowReflectPass = false;
   
   renderTime = "PFXBeforeBin";
   renderBin = "ObjTranslucentBin"; 
//   renderBin = "BeginBin"; 
//   renderTime = "PFXAfterBin";
//   renderTime = "PFXAfterDiffuse";
//   renderTime = "PFXBeforeBin";
//   renderBin = "AL_LightBinMgr";   
   
   renderPriority = 0.1;
      
   shader = ImageOverlayShader;
   stateBlock = ImageOverlayStateBlock;
   
  // texture[0] = "#prepass";
   texture[0] = "$inTex";
   texture[1] = $ImageOverlayPostFX::normal;
   
  // targetDepthStencil = "#depth";
 // target = "#prepassbuf";
  target = "$outTex";
         targetFormat = "GFXFormatR16G16B16A16F";
//    target = "$backBuffer";
//   requirements = "PrePassDepth";

   new PostEffect( )
   {
      renderPriority = 0.11;
      shader = ImageOverlay3Shader;
      stateBlock = ImageOverlayStateBlock;
      texture[0] = "$outTex";
    //  texture[0] = "$backBuffer";
    //  texture[1] = $ImageOverlayPostFX::normal;
     target = "#prepass";
   //    target = "$backBuffer";
   //   requirements = "PrePassDepth";
   };
};

*/

function ImageOverlayPostFX::preProcess( %this )
{

  // if ( %this.texture[1] !$= $ImageOverlayPostFX::normal )
      %this.setTexture( 1, $ImageOverlayPostFX::normal );         
}

function ImageOverlayPostFX::setShaderConsts( %this )
{
 //  echo("ImageOverlayPostFX::setShaderConsts");
}
/*
singleton PostEffect( ImageOverlay2PostFX )
{
   isEnabled = true;
   allowReflectPass = false;
   
   renderTime = "PFXBeforeBin";
   renderBin = "ObjTranslucentBin"; 
//   renderBin = "BeginBin"; 
//   renderTime = "PFXAfterBin";
//   renderTime = "PFXAfterDiffuse";
//   renderTime = "PFXBeforeBin";
//   renderBin = "AL_LightBinMgr";   
   renderPriority = 0.1;
      
   shader = ImageOverlay2Shader;
   stateBlock = ImageOverlay2StateBlock;
   
   texture[0] = "#prepass";
   texture[1] = "$backBuffer";
   texture[2] = $ImageOverlayPostFX::diffuse;  
   target = "$backBuffer";
   requirements = "PrePassDepth";
};
*/
function ImageOverlay2PostFX::preProcess( %this )
{

//   if ( %this.texture[1] !$= $ImageOverlayPostFX::diffuse )
      %this.setTexture( 2, $ImageOverlayPostFX::diffuse );         
}
function ImageOverlay2PostFX::setShaderConsts( %this )
{
 //  echo("ImageOverlayPostFX::setShaderConsts");
}
