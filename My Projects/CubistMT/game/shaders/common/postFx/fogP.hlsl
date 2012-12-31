//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "shadergen:/autogenConditioners.h"
#include "./postFx.hlsl"
#include "./../torque.hlsl"

uniform sampler2D prepassTex : register(S0);
uniform sampler2D backbuffer : register(S1);
uniform float3    eyePosWorld;
uniform float4    fogColor;
uniform float3    fogData;
uniform float4    rtParams0;

float4 main( PFXVertToPix IN ) : COLOR
{   
   //float2 prepassCoord = ( IN.uv0.xy * rtParams0.zw ) + rtParams0.xy;   
   float depth = prepassUncondition( prepassTex, IN.uv0 ).w;
   float3 inColor = hdrDecode( tex2D( backbuffer, IN.uv0 ).rgb );
   //return float4( depth, 0, 0, 0.7 );
   
   // Skip fogging the extreme far plane so that 
   // the canvas clear color always appears.
   clip( 0.9999 - depth );
   
   float factor = computeSceneFog( eyePosWorld,
                                   eyePosWorld + ( IN.wsEyeRay * depth ),
                                   fogData.x, 
                                   fogData.y, 
                                   fogData.z );

//   return hdrEncode( float4( fogColor.rgb, 1.0 - saturate( factor ) ) ); 

	float3 outColor;
//	float fogAmt = 1.0 - saturate( factor ); 
//	float3 fogColor2 = float3(0.004,0.278,0.604);
//	fogColor2 *= fogColor;
//	outColor.r = lerp( inColor.r, fogColor2.r, clamp((fogAmt)/0.45, 0.0, 1.0) );
//	outColor.g = lerp( inColor.g, fogColor2.g, clamp((fogAmt)/0.9, 0.0, 1.0) );
//	outColor.b = lerp( inColor.b, fogColor2.b, clamp((fogAmt), 0.0, 1.0) );
	outColor = BlendUnderwaterFog(inColor, fogColor.rgb, factor);

	return hdrEncode( float4( outColor,1.0 ) );   
   
}