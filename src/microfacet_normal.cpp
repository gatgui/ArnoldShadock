#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MicrofacetNormalMtd);

enum MicrofacetNormalParams
{
   p_sample = 0,
   p_distribution,
   p_distribution_param
};

node_parameters
{
   AiParameterPnt2("sample", 0.0f, 0.0f);
   AiParameterEnum(SSTR::distribution, MD_Beckmann, MicrofacetDistributionNames);
   AiParameterFlt(SSTR::distribution_param, 0.0f);
   
   AiMetaDataSetBool(mds, SSTR::distribution, SSTR::linkable, false);
}

struct NodeData
{
   MicrofacetDistribution distribution;
   bool evalDistributionParam;
   float distributionParam;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->distribution = (MicrofacetDistribution) AiNodeGetInt(node, SSTR::distribution);
   data->evalDistributionParam = AiNodeIsLinked(node, SSTR::distribution_param);
   if (!data->evalDistributionParam)
   {
      data->distributionParam = AiNodeGetFlt(node, SSTR::distribution_param);
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   AtPoint2 sample = AiShaderEvalParamPnt2(p_sample);
   
   float param = (data->evalDistributionParam ? AiShaderEvalParamFlt(p_distribution_param) : data->distributionParam);
   
   float cosTheta, sinTheta;
   float phi = 2.0f * AI_PI * sample.y;
   
   switch (data->distribution)
   {
   // case MD_Phong:
   //    {
   //       cosTheta = powf(sample.x, 1.0f / (param + 2.0f));
   //       sinTheta = sqrtf(1.0f - cosTheta * cosTheta);
   //    }
   //    break;
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
