#include "shadergen:/autogenConditioners.h"  
#include "./postFx.hlsl"  
#include "../torque.hlsl"

//uniform sampler2D prepassTex : register( s0 );
//uniform sampler2D backBuffer : register( s1 );
uniform sampler2D outTex :register(S10);
//uniform float3    eyePosWorld;
//uniform float4    rtParams0;

//uniform float OneOverGamma;

//struct PS_OUTPUT
//{
//    float4 Color[4] : COLOR0;
//    float  Depth  : DEPTH;
//};
struct PS_OUTPUT 
{
   float4 color     : COLOR0;
 //   float  depth  : DEPTH;
};


PS_OUTPUT main( PFXVertToPix IN )
{
	PS_OUTPUT OUT;

    float4 diffuse = tex2D( outTex, IN.uv0).rgba;
	clip(0.999-diffuse.a);
	OUT.color = diffuse;
//	OUT.color = float4(0,1,1,1);

	return OUT;
	
}