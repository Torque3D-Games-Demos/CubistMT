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

//*****************************************************************************
// Shaders  ( For Custom Materials )
//*****************************************************************************

singleton ShaderData( ProgressBar )  
{  
   DXVertexShaderFile     = "shaders/common/ProgressBarV.hlsl";  
   DXPixelShaderFile      = "shaders/common/ProgressBarP.hlsl";     
       
   pixVersion = 3.0;  
};  


new ShaderData( BumpReflect )
{
   DXVertexShaderFile 	= "shaders/common/BumpReflectV.hlsl";
   DXPixelShaderFile 	= "shaders/common/BumpReflectP.hlsl";
   
//   OGLVertexShaderFile 	= "shaders/common/gl/planarReflectBumpV.glsl";
//   OGLPixelShaderFile 	= "shaders/common/gl/planarReflectBumpP.glsl";
              
   samplerNames[0] = "$diffuseMap";
   samplerNames[1] = "$bumpMap";
   samplerNames[2] = "$cubeMap";
   
   pixVersion = 3.0;
};

new ShaderData( BumpCubeDiff )
{
   DXVertexShaderFile 	= "shaders/common/bumpCubeDiffuseV.hlsl";
   DXPixelShaderFile 	= "shaders/common/bumpCubeDiffuseP.hlsl";
   pixVersion = 2.0;
   samplerNames[1] = "$diffuseMap";
   samplerNames[0] = "$bumpMap";
   samplerNames[3] = "$cubeMap";
};
