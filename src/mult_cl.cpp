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

AI_SHADER_NODE_EXPORT_METHODS(MultClMtd);

enum MultClParams
{
   p_input = 0,
   p_fweight,
   p_vweight,
   p_cweight
};

node_parameters
{
   AiParameterClosure("input");
   AiParameterFlt("fweight", 1.0f);
   AiParameterVec("vweight", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("cweight", 1.0f, 1.0f, 1.0f);
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
   float f = AiShaderEvalParamFlt(p_fweight);
   AtVector v = AiShaderEvalParamVec(p_vweight);
   AtRGB c = AiShaderEvalParamRGB(p_cweight);

   c.r *= (f * v.x);
   c.g *= (f * v.y);
   c.b *= (f * v.z);
   
   sg->out.CLOSURE() = AiShaderEvalParamClosure(p_input);
   sg->out.CLOSURE() *= c;
}