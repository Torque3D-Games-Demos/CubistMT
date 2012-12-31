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

#include "../postFx.hlsl"

uniform sampler2D frameSampler : register( s0 );
uniform sampler2D backBuffer : register( s1 );

uniform float3 camForward;
uniform float3 lightDirection;
uniform float2 screenSunPos;
uniform float2 oneOverTargetSize;
uniform int numSamples;
uniform float density;
uniform float weight;
uniform float decay;
uniform float exposure;

float4 main( PFXVertToPix IN ) : COLOR0  
{  
    float4 texCoord = float4( IN.uv0.xy, 0, 0 );        
    
    // Store initial sample.
    half3 color = (half3)tex2D( frameSampler, texCoord.xy ).rgb;  
	
	// Store original bb color.
	float4 bbCol = tex2D( backBuffer, IN.uv1 );
//	float4 bbCol = tex2D( backBuffer, texCoord.xy );

    // Set up illumination decay factor.
#define UNDERWATER
#ifndef UNDERWATER
    half illuminationDecay = 1.0;  
		
	float amount = saturate( dot( -lightDirection, camForward ) );
#else
//	half illuminationDecay = 1;  	
//	float amount = 50;
	
//	half illuminationDecay = 0.5;  	
	//float amount = 200;
//	half illuminationDecay =5;  	
//	half illuminationDecay =1.5;  	
	half illuminationDecay =0.9;  	
//	float amount = 100;
	float amount = saturate( dot( -lightDirection, camForward ) )*4;
//	amount /= amount;

#endif
	
		
	int samples = numSamples * amount;
	
	if ( samples <= 0 )
	   return bbCol;	

	// Calculate vector from pixel to light source in screen space.
    half2 deltaTexCoord = (half2)( texCoord.xy - screenSunPos );  
  //  half2 deltaTexCoord = (half2)( texCoord.xy - float2(0.5,-10) );  
 //   half2 deltaTexCoord = (half2)( texCoord.xy - float2(screenSunPos.x-0.2, screenSunPos.y-0.7) );  

    // Divide by number of samples and scale by control factor.  
#ifndef UNDERWATER
    deltaTexCoord *= 1.0 / (half)samples * density; 
#else
   // deltaTexCoord *= 1.0 / (half)(samples) * (density*4); 
   // deltaTexCoord *= 1.0 / (half)(samples) * (density*40); 
 //   deltaTexCoord *= 1.0 / (half)(samples*1.9) * (density); 
    deltaTexCoord *= 1.0 / (half)(samples*2) * (density); 
#endif
	
    // Evaluate summation from Equation 3 NUM_SAMPLES iterations.  
    for ( int i = 0; i < samples; i++ )  
    {  
#ifdef UNDERWATER
	//	float falloff = (float(samples-i)/float(samples));
	//	if(falloff<0.001)
	//		break;
#endif
		
        // Step sample location along ray.
        texCoord.xy -= deltaTexCoord;  

        // Retrieve sample at new location.
        half3 sample = (half3)tex2Dlod( frameSampler, texCoord );  

        // Apply sample attenuation scale/decay factors.
     //   sample *= illuminationDecay * weight;
        sample *= illuminationDecay;
	//	sample *= sample * sample;

        // Accumulate combined color.
        color += sample*sample*sample;

        // Update exponential decay factor.
#ifndef UNDERWATER
        illuminationDecay *= decay;
#else
      //  illuminationDecay *= decay*(falloff*falloff);
     //   illuminationDecay *= decay*(falloff);
        illuminationDecay *= decay;
#endif
    }       
   
    //return saturate( amount ) * color * Exposure;
	//return bbCol * decay;
	
    // Output final color with a further scale control factor.      
#ifndef UNDERWATER
    return saturate( amount ) * float4( color * (exposure), 1 ) + bbCol;
#else
    return saturate( amount ) * float4( color * (exposure), 1 ) + bbCol;
#endif
}  
