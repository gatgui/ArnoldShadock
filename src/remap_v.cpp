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

AI_SHADER_NODE_EXPORT_METHODS(RemapVMtd);

enum RemapVParams
{
   p_input = 0,
   p_input_min,
   p_input_max,
   p_output_min,
   p_output_max
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_min", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input_max", 1.0f, 1.0f, 1.0f);
   AiParameterVec("output_min", 0.0f, 0.0f, 0.0f);
   AiParameterVec("output_max", 1.0f, 1.0f, 1.0f);
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
   AtVector input = AiShaderEvalParamVec(p_input);
   AtVector input_min = AiShaderEvalParamVec(p_input_min);
   AtVector input_max = AiShaderEvalParamVec(p_input_max);
   AtVector output_min = AiShaderEvalParamVec(p_output_min);
   AtVector output_max = AiShaderEvalParamVec(p_output_max);
   
   sg->out.VEC() = ExpandToRange(NormalizeToRange(input, input_min, input_max), output_min, output_max);
}
