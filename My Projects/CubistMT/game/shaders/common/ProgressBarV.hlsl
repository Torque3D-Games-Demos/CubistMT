#include "shaders/common/hlslstructs.h"  
  
struct ConnectData         
{         
   float4 Position        : POSITION;
   float2 TexCoord        : TEXCOORD0;
}; 
 
ConnectData main( VertexIn_PNTTTB IN,     
                  uniform float4x4 modelview,
                  uniform float4x4 objTrans
)  
{  
   ConnectData OUT;

   OUT.TexCoord = IN.uv0;
   OUT.Position = mul(modelview, float4(IN.pos.xyz,1));  
   return OUT;  
}