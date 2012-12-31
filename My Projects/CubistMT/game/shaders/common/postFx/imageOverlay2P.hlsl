#include "shadergen:/autogenConditioners.h"  
#include "./postFx.hlsl"  
#include "../torque.hlsl"

uniform sampler2D prepassTex : register( s0 );
uniform sampler2D backBuffer : register( s1 );
uniform sampler2D diffuseTex :register(S2);
uniform float3    eyePosWorld;
uniform float4    rtParams0;

//uniform float OneOverGamma;

//struct PS_OUTPUT
//{
//    float4 Color[4] : COLOR0;
//    float  Depth  : DEPTH;
//};
struct PS_OUTPUT 
{
   float4 color     : COLOR0;
    float  depth  : DEPTH;
};

//float4 main( PFXVertToPix IN ) : COLOR
PS_OUTPUT main( PFXVertToPix IN )
{
clip(-1);
	PS_OUTPUT OUT;
	float4 prepass = prepassUncondition( prepassTex, IN.uv0 );
	float depth = prepass.w;
//    float3 back = hdrDecode( tex2D( backBufferTex, IN.uv0).rgb );
//    float3 color;
    
   // color = lerp(back,diffuse,0.0);
//    color = diffuse;
   // Apply the color correction.
 //  color.r = tex1D( colorCorrectionTex, color.r ).r;
 //  color.g = tex1D( colorCorrectionTex, color.g ).g;
 //  color.b = tex1D( colorCorrectionTex, color.b ).b;

   // Apply gamma correction
 //   color.rgb = pow( abs(color.rgb), OneOverGamma );

	//out.color = color;
	//out.depth = color.r;
    //return out;    

  //  color.a=0.5;
  //  return float4( hdrEncode( diffuse ), 1 );  
//  OUT.Color[0] = float4( hdrEncode( diffuse ), 1 );    
//  OUT.Depth = hdrEncode( diffuse ).r;    


 //   float4 diffuse = hdrDecode(tex2D( diffuseTex, IN.uv0).rgba);
    float4 diffuse = tex2D( diffuseTex, IN.uv0).rgba;
	//	float depth2 = ((diffuse.a+0.01));
	clip( 0.999 - (1.0-diffuse.a) );
//	clip((diffuse.a-0.5)-depth);
//	if(depth > depth2 && depth2 > 0.1 )
	//if(depth > depth2)
	float depth2 =2.0-(((diffuse.a*2)-0.85));
//	if(depth > depth2 && depth2 > 0.1 )
		OUT.depth = depth2;
//		OUT.depth = depth;
//	else
//		clip(-1);

	OUT.color = float4( hdrEncode( diffuse.rgb ), 1 );
	return OUT;
//	return diffuse;
//	return float4( diffuse.rgb, diffuse.a );
}