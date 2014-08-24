#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MicrofacetNormalMtd);

enum MicrofacetNormalParams
{
   p_sample = 0,
   p_distribution,
   p_distribution_param
};

enum MicrofacetDistribution
{
   MD_Beckmann = 0,
   MD_Phong,
   MD_GGX
};

static const char* MicrofacetDistributionNames[] = {"beckmann", "phong", "ggx", NULL};

node_parameters
{
   AiParameterPnt2("sample", 0.0f, 0.0f);
   AiParameterEnum("distribution", MD_Beckmann, MicrofacetDistributionNames);
   AiParameterFlt("distribution_param", 0.0f);
   
   AiMetaDataSetBool(mds, "distribution", "linkable", false);
}

struct MicrofacetNormalData
{
   MicrofacetDistribution distribution;
   bool distribution_param_is_linked;
   float distribution_param;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(MicrofacetNormalData)));
}

node_update
{
   MicrofacetNormalData *data = (MicrofacetNormalData*) AiNodeGetLocalData(node);
   
   data->distribution = (MicrofacetDistribution) AiNodeGetInt(node, "distribution");
   data->distribution_param_is_linked = AiNodeIsLinked(node, "distribution_param");
   if (!data->distribution_param_is_linked)
   {
      data->distribution_param = AiNodeGetFlt(node, "distribution_param");
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   MicrofacetNormalData *data = (MicrofacetNormalData*) AiNodeGetLocalData(node);
   
   AtPoint2 sample = AiShaderEvalParamPnt2(p_sample);
   
   float param = (data->distribution_param_is_linked
                  ? AiShaderEvalParamFlt(p_distribution_param)
                  : data->distribution_param);
   
   float cosTheta, sinTheta;
   float phi = 2.0f * AI_PI * sample.y;
   
   switch (data->distribution)
   {
   case MD_Phong:
      {
         cosTheta = powf(sample.x, 1.0f / (param + 2.0f));
         sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
      }
      break;
   case MD_GGX:
      {
         float tanTheta = param * sqrtf(sample.x) / sqrtf(1.0f - sample.x);
         cosTheta = 1.0f / sqrtf(1.0f + tanTheta * tanTheta);
         sinTheta = cosTheta * tanTheta;
      }
      break;
   case MD_Beckmann:
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
