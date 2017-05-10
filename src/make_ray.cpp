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

AI_SHADER_NODE_EXPORT_METHODS(MakeRayMtd);

enum MakeRayParams
{
   p_type = 0,
   p_origin,
   p_direction,
   p_maxdist
};

node_parameters
{
   AiParameterEnum(SSTR::type, RT_Generic, RayTypeNames);
   AiParameterPnt(SSTR::origin, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::direction, 0.0f, 0.0f, 0.0f);
   AiParameterFlt("maxdist", AI_BIG);
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
   AtRay *ray = (AtRay*) AiShaderGlobalsQuickAlloc(sg, sizeof(AtRay));
   
   unsigned int type = RayTypeValues[AiShaderEvalParamInt(p_type)];
   AtPoint origin = AiShaderEvalParamPnt(p_origin);
   AtPoint dir = AiShaderEvalParamVec(p_direction);
   float maxdist = AiShaderEvalParamFlt(p_maxdist);
   
   AiMakeRay(ray, type, &origin, (AiV3IsZero(dir) ? NULL : &dir), maxdist, sg);
   AiStateSetMsgPtr(SSTR::agsb_ray, ray);
   
   sg->out.VEC = ray->dir;
}
