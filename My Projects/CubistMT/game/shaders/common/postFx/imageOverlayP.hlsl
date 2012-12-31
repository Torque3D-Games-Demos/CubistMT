#include "shadergen:/autogenConditioners.h"  
#include "./postFx.hlsl"  
#include "../torque.hlsl"

//uniform sampler2D prepassTex : register( s0 );
//uniform sampler2D backBuffer : register( s0 );
uniform sampler2D inTex : register( s0 );
uniform sampler2D diffuseTex :register(S1);
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
 //   float  depth  : DEPTH;
};


PS_OUTPUT main( PFXVertToPix IN )
{
	PS_OUTPUT OUT;
//clip(-1);
//float2 prepassCoord = ( IN.uv0.xy * rtParams0.zw ) + rtParams0.xy;
//	float4 prepass = prepassUncondition( prepassTex, IN.uv0 );
 //   float4 prepass = tex2D( prepassTex, IN.uv0);

//	float depth = prepass.w;
//	clip( 0.9999 - depth );

    float3 back = hdrDecode( tex2D( inTex, IN.uv0).rgb );
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


//    float4 diffuse = hdrDecode(tex2D( diffuseTex, IN.uv0).rgba);
//    return hdrEncode( diffuse );    
//    float4 diffuse = hdrDecode(tex2D( diffuseTex, IN.uv0).rgba);
    float4 diffuse = tex2D( diffuseTex, IN.uv0).rgba;
	//float depth2 = (1.0-diffuse.a-0.01);
	float depth2 = 1.0-((diffuse.a+0.5));
//	float depth2 = ((1.0-diffuse.a));
//	clip( 0.999 - (1.0-diffuse.a) );
//	clip(-1);

	// todo: this 0.1 value should be a lot lower
//	if(depth > depth2 && depth2 > 0.1 )
//	{
	//	prepass = float4( diffuse.rgb, (1-diffuse.a) );
	//	float prepass = float4( hdrEncode(diffuse.rgb), (((diffuse.a-0.5))) );
		//prepass = float4( 0,0,1, (diffuse.a-0.5)  );
		
//clip(-1);
	//clip((depth)-(depth2));
//	OUT.depth  =2.0-(((diffuse.a*2)-0.85));
	//OUT.depth=0;
//	OUT.depth  =1;
	//OUT.depth  = depth2;

//	return diffuse;
//	OUT.color = float4( diffuse.r, diffuse.g, diffuse.b, OUT.depth );
//	OUT.color = prepassCondition(prepass);
//	OUT.color = prepass;
//	clip(depth - depth2);
//	if(depth > depth2 && depth2 > 0.1 )
	//	OUT.color = float4( diffuse.a,diffuse.a,diffuse.a, diffuse.a );
		OUT.color = float4( back.rgb, diffuse.a );
	//	OUT.color = float4( hdrEncode(diffuse.rgb), diffuse.a );
//	else OUT.color = prepass;
	
//}
//	else clip(-1);


	return OUT;
	
}