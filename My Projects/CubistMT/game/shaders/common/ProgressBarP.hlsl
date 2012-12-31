    #include "torque.hlsl"  
      
uniform sampler2D DiffSampler: register( S0 );  


struct ConnectData           
{           
   float4 Position        : POSITION;  
   float2 TexCoord        : TEXCOORD0;  
};     

float4 main( ConnectData IN  ) : COLOR    
{  
   float2 TexUV = IN.TexCoord.xy;  
   float4 Diff = tex2D(DiffSampler, TexUV);  
   if( TexUV.y<0.5f ){
      Diff.w = 0;
   }else{
      Diff.w = 1.0f;
   }
   return hdrEncode(Diff);  
}  