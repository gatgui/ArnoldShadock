/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleDirMtd);

enum SampleDirParams
{
   p_sample = 0
};

node_parameters
{
   AiParameterVec2("sample", 0.0f, 0.0f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtVector2 sample = AiShaderEvalParamVec2(p_sample);
   // sample.x = (sin(theta))^2
   // sample.y = phi / 2 * PI
   
   float sinTheta = sqrtf(sample.x);
   float cosTheta = sqrtf(1.0f - sample.x);
   float phi = 2.0f * AI_PI * sample.y;
   
   AtVector D;
   
   D.x = sinTheta * cosf(phi);
   D.y = sinTheta * sinf(phi);
   D.z = cosTheta;
   
   sg->out.VEC() = D;
}
