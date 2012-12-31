//////////////////////////////////////////////////////////////

#include "./postFx.hlsl"

uniform float    accumTime;

float4 main(	PFXVertToPix IN,
				uniform sampler2D inputTex : register(S0) ) : COLOR
{

    float2 screeen_center = (0.5,0.5);
	float backbuffer_edge_coef = 0.98; // tricky,based on power
	float reduction = 128;
	
	float power = 1.0;
	
	float2 len = abs(IN.uv0 - screeen_center)/5;
    float space = length(len);

    // invert the origin of space
    space = abs(1.0 - space);
        
    //here we need to calculate the distance of distortion on each screen pixel
    float sin = cos(accumTime / 2 / space);
    sin = clamp(sin, 1, 1);
    
    // left or right distortion,i used right
    float right = cos(accumTime / 2 / space);     
	
    // here we need to slow down the distortion via the reduction level
    sin *= power/reduction;
    
	// let us do some actual PP rendering
	// here we need to scale up the sampled backbuff frame,we need to hide right since we know the backbuffer is not tiled
    // then we apply our new offset,calculated via the ripple formula

    float4 color = tex2D(inputTex, IN.uv0 * backbuffer_edge_coef+(sin*right));           
	return color;

}