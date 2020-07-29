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

AI_SHADER_NODE_EXPORT_METHODS(SmoothRangeSwitchClMtd);

enum SmoothRangeSwitchClParams
{
   p_value = 0,
   p_lower_bound,
   p_upper_bound,
   p_falloff,
   p_inside_range,
   p_outside_range
};

node_parameters
{
   AiParameterFlt("value", 1.0f);
   AiParameterFlt("lower_bound", 1.0f);
   AiParameterFlt("upper_bound", 1.0f);
   AiParameterFlt("falloff", 0.0f);
   AiParameterClosure("inside_range");
   AiParameterClosure("outside_range");
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
   float value = AiShaderEvalParamFlt(p_value);
   float lower = AiShaderEvalParamFlt(p_lower_bound);
   float upper = AiShaderEvalParamFlt(p_upper_bound);
   float falloff = AiShaderEvalParamFlt(p_falloff);
   
   if (lower <= value && value <= upper)
   {
      sg->out.RGB() = AiShaderEvalParamRGB(p_inside_range);
   }
   else if (value < lower-falloff || value > upper+falloff)
   {
      sg->out.RGB() = AiShaderEvalParamRGB(p_outside_range);
   }
   else
   {
      float blend;
      
      if (value <= lower)
      {
         blend = SmoothStep(NormalizeToRange(value, lower-falloff, lower));
      }
      else
      {
         blend = 1.0f - SmoothStep(NormalizeToRange(value, upper, upper+falloff));
      }
      
      if (blend <= 0.0f)
      {
         sg->out.CLOSURE() = AiShaderEvalParamClosure(p_outside_range);
      }
      else if (blend >= 1.0f)
      {
         sg->out.CLOSURE() = AiShaderEvalParamClosure(p_inside_range);
      }
      else
      {
         AtClosureList tmp = AiShaderEvalParamClosure(p_outside_range);
         tmp *= (1.0f - blend);

         sg->out.CLOSURE() = AiShaderEvalParamClosure(p_inside_range);
         sg->out.CLOSURE() *= blend;
         sg->out.CLOSURE().add(tmp);
      }
   }
}
