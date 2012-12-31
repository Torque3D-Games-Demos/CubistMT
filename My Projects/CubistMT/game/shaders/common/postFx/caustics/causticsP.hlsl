//-----------------------------------------------------------------------------
// Torque 3D
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "../postFx.hlsl"
#include "shadergen:/autogenConditioners.h"

uniform float3    eyePosWorld;
uniform float4    rtParams0;
uniform float4    waterFogPlane;
uniform float     accumTime;
//uniform float      swapXY;

float4 main( PFXVertToPix IN, 
             uniform sampler2D prepassTex :register(S0),
             uniform sampler2D causticsTex0 :register(S1),
             uniform sampler2D causticsTex1 :register(S2),
             uniform float2 targetSize : register(C0) ) : COLOR
{   
   //Sample the pre-pass
   float2 prepassCoord = ( IN.uv0.xy * rtParams0.zw ) + rtParams0.xy;  
   float4 prePass = prepassUncondition( prepassTex, prepassCoord );
   
   //Get depth

//#define XUP
//#ifndef XUP
   float depth = prePass.w;  
//#else
//   float depth = prePass.z;
//#endif

   clip( 0.9999 - depth );

   //Get world position
   float3 pos = eyePosWorld + IN.wsEyeRay * depth;
   //return float4((-pos.z/100),0,0,1);
   
   //Use world position X and Y to calculate caustics UV 
   float2 causticsUV0;// = (abs(pos.xy * 0.25) % float2(1, 1));
   float2 causticsUV1;// = (abs(pos.xy * 0.2) % float2(1, 1));
   
//#ifndef XUP
// too tight need better textures
//      causticsUV0 = (abs(pos.xy * 1) % float2(1, 1));
//      causticsUV1 = (abs(pos.xy * 0.8) % float2(1, 1));
      causticsUV0 = (abs(pos.xy * 0.5) % float2(1, 1));
      causticsUV1 = (abs(pos.xy * 0.4) % float2(1, 1));
//#else
////      causticsUV0 = (abs(float2(pos.z, pos.y) * 0.25) % float2(1, 1));
////      causticsUV1 = (abs(float2(pos.z, pos.y) * 0.2) % float2(1, 1));
////      causticsUV0.x = 1.0 - causticsUV0.x;
////      causticsUV1.x = 1.0 - causticsUV1.x;
//      causticsUV0 = (abs(pos.xy * 1) % float2(1, 1));
//      causticsUV1 = (abs(pos.xy * 0.8) % float2(1, 1));

//#endif
   
   //Animate uvs
   float timeSin = sin(accumTime);
   causticsUV0.xy += float2(accumTime*0.1, timeSin*0.2);
   causticsUV1.xy -= float2(accumTime*0.15, timeSin*0.15);   
   
   
   //Sample caustics texture   
   float4 caustics = tex2D(causticsTex0, causticsUV0);   
   caustics *= tex2D(causticsTex1, causticsUV1);
   
   
   //Use normal Z to modulate caustics  
   float waterDepth;
   
//#ifndef XUP
      waterDepth = 1 - saturate(pos.z + waterFogPlane.w + 1);
  // return float4(waterDepth/20,0,0,1);
//#else
//      waterDepth = saturate((pos.z) + (-waterFogPlane.w));
//  // return float4(waterFogPlane.w,0,0,1);
//#endif

#ifndef XUP
   caustics *= saturate(prePass.z) * pow(1-depth, 16) * (waterDepth/2); 
#else
   caustics *= saturate(-prePass.x) * pow(1-depth, 16) * (waterDepth/2); 
#endif

 //return float4(-prePass.x,0,0,1);
     
   return caustics;   
}
