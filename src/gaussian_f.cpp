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

AI_SHADER_NODE_EXPORT_METHODS(GaussianFMtd);

enum GaussianFParams
{
   p_input = 0,
   p_amplitude,
   p_center,
   p_width, // standard devisation
   p_offset
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterFlt("amplitude", 1.0f);
   AiParameterFlt("center", 0.0f);
   AiParameterFlt("width", 1.0f);
   AiParameterFlt("offset", 0.0f);
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
   float input = AiShaderEvalParamFlt(p_input);
   float amplitude = AiShaderEvalParamFlt(p_amplitude);
   float center = AiShaderEvalParamFlt(p_center);
   float width = AiShaderEvalParamFlt(p_width);
   float offset = AiShaderEvalParamFlt(p_offset);
   
   float tmp = input - center;
   
   sg->out.FLT = offset + amplitude * expf(-(tmp * tmp) / (2.0f * width * width));
}
