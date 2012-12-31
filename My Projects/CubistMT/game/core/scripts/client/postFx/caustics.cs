//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


singleton GFXStateBlockData( PFX_CausticsStateBlock : PFX_DefaultStateBlock )
{
   blendDefined = true;
   blendEnable = true; 
   blendSrc = GFXBlendOne;
   blendDest = GFXBlendOne;
   
   samplersDefined = true;
   samplerStates[0] = SamplerClampLinear;
   samplerStates[1] = SamplerWrapLinear;
   samplerStates[2] = SamplerWrapLinear;
};

singleton ShaderData( PFX_CausticsShader )
{   
   DXVertexShaderFile 	= "shaders/common/postFx/postFxV.hlsl";
   DXPixelShaderFile 	= "shaders/common/postFx/caustics/causticsP.hlsl";
         
   //OGLVertexShaderFile  = "shaders/common/postFx/gl//postFxV.glsl";
   //OGLPixelShaderFile   = "shaders/common/postFx/gl/passthruP.glsl";
      
   samplerNames[0] = "$prepassTex";
   samplerNames[1] = "$causticsTex0";
   samplerNames[2] = "$causticsTex1";
   
   pixVersion = 3.0;
};

singleton ShaderData( PFX_CausticsShaderXUp : PFX_CausticsShader )
{     
   defines = "XUP";
};

singleton PostEffect( CausticsPFX )
{
   requirements = "None";
   isEnabled = !$pref::Cube::causticsXUp;
   renderTime = "PFXBeforeBin";
   renderBin = "ObjTranslucentBin";      
   //renderPriority = 0.1;
      
   shader = PFX_CausticsShader;
   stateBlock = PFX_CausticsStateBlock;
   texture[0] = "#prepass";
   texture[1] = "textures/caustics_0";
   texture[2] = "textures/caustics_1";
   target = "$backBuffer";
   
};
singleton PostEffect( CausticsXUpPFX : CausticsPFX )
{
   isEnabled = $pref::Cube::causticsXUp;
   shader = PFX_CausticsShaderXUp;
};


$CausticsPFX::refTime = getSimTime();

function CausticsPFX::setShaderConsts(%this)
{
   //echo($Sim::time - %this.timeStart);
   //echo(%this.timeConst);
   %this.setShaderConst( "$refTime", $CausticsPFX::refTime ); 
//   if($pref::Cube::monitorRotation == -90)
//      %this.setShaderConst( "$swapXY", 1 );
//   else 
//      %this.setShaderConst( "$swapXY", 0 ); 

   
   
}

