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

AI_SHADER_NODE_EXPORT_METHODS(MicrofacetNormalMtd);

enum MicrofacetNormalParams
{
   p_sample = 0,
   p_distribution,
   p_distribution_param
};

enum NormalDistribution
{
   ND_Phong = 0,
   ND_Beckmann,
   ND_GGX
};

static const char* NormalDistributionNames[] = 
{
   "Phong",
   "Beckmann",
   "GGX",
   NULL
};

node_parameters
{
   AiParameterPnt2("sample", 0.0f, 0.0f);
   AiParameterEnum(SSTR::distribution, ND_Beckmann, NormalDistributionNames);
   AiParameterFlt(SSTR::distribution_param, 0.0f);
}

struct MicrofacetNormalData
{
   NormalDistribution distribution;
   bool evalDistributionParam;
   float distributionParam;
};

node_initialize
{
   AiNodeSetLocalData(node, new MicrofacetNormalData());
   AddMemUsage<MicrofacetNormalData>();
}

node_update
{
   MicrofacetNormalData *data = (MicrofacetNormalData*) AiNodeGetLocalData(node);
   
   data->distribution = (NormalDistribution) AiNodeGetInt(node, SSTR::distribution);
   data->evalDistributionParam = AiNodeIsLinked(node, SSTR::distribution_param);
   if (!data->evalDistributionParam)
   {
      data->distributionParam = AiNodeGetFlt(node, SSTR::distribution_param);
   }
}

node_finish
{
   MicrofacetNormalData *data = (MicrofacetNormalData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<MicrofacetNormalData>();
}

shader_evaluate
{
   MicrofacetNormalData *data = (MicrofacetNormalData*) AiNodeGetLocalData(node);
   
   AtPoint2 sample = AiShaderEvalParamPnt2(p_sample);
   
   float param = (data->evalDistributionParam ? AiShaderEvalParamFlt(p_distribution_param) : data->distributionParam);
   
   float cosTheta, sinTheta;
   float phi = 2.0f * AI_PI * sample.y;
   
   // Eurographics Symposium on Rendering (2007)
   // Microfacet Models for Refraction through Rough Surfaces
   // Bruce Walter, Stephan R. Marschner, Hongsong Li, Kenneth E. Torrance
   
   switch (data->distribution)
   {
   case ND_Phong:
      {
         cosTheta = powf(sample.x, 1.0f / (param + 2.0f));
         sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
      }
      break;
   case ND_GGX:
      {
         float tanTheta = param * sqrtf(sample.x) / sqrtf(1.0f - sample.x);
         cosTheta = 1.0f / sqrtf(1.0f + tanTheta * tanTheta);
         sinTheta = cosTheta * tanTheta;
      }
      break;
   case ND_Beckmann:
   default:
      {
         float squaredTanTheta = -(param * param) * logf(1.0f - sample.x);
         float tanTheta = sqrtf(squaredTanTheta);
         cosTheta = 1.0f / sqrtf(1.0f + squaredTanTheta);
         sinTheta = cosTheta * tanTheta;
      }
      break;
   }
   
   sg->out.VEC.x = cosf(phi) * sinTheta;
   sg->out.VEC.y = sinf(phi) * sinTheta;
   sg->out.VEC.z = cosTheta;
}
