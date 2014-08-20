#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SssMultiMtd);

enum SssMultiParams
{
   p_method = 0,
   p_num_layers,
   p_radius0,
   p_weight0,
   p_radius1,
   p_weight1,
   p_radius2,
   p_weight2,
   p_radius3,
   p_weight3,
   p_radius4,
   p_weight4
};

enum SssMethod
{
   SM_Cubic = 0,
   SM_Gaussian
};

static const char* SssMethodNames[] = {"cubic", "gaussian", NULL};

node_parameters
{
   AiParameterEnum("method", SM_Cubic, SssMethodNames);
   AiParameterUInt("num_layers", 1);
   AiParameterFlt("radius0", 1.0f);
   AiParameterRGB("weight0", 1.0f, 1.0f, 1.0f);
   AiParameterFlt("radius1", 0.0f);
   AiParameterRGB("weight1", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("radius2", 0.0f);
   AiParameterRGB("weight2", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("radius3", 0.0f);
   AiParameterRGB("weight3", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("radius4", 0.0f);
   AiParameterRGB("weight4", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "method", "linkable", false);
   AiMetaDataSetBool(mds, "num_layers", "linkable", false);
   AiMetaDataSetFlt(mds, "num_layers", "min", 1);
   AiMetaDataSetFlt(mds, "num_layers", "max", 5);
}

struct SssMultiData
{
   SssMethod method;
   unsigned int num_layers;
};

node_initialize
{
   SssMultiData *data = (SssMultiData*) AiMalloc(sizeof(SssMultiData));
   data->method = SM_Cubic;
   data->num_layers = 0;
}

node_update
{
   SssMultiData *data = (SssMultiData*) AiNodeGetLocalData(node);
   
   data->method = (SssMethod) AiNodeGetInt(node, "method");
   data->num_layers = AiNodeGetUInt(node, "num_layers");
   
   if (data->num_layers > 5)
   {
      data->num_layers = 5;
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   SssMultiData *data = (SssMultiData*) AiNodeGetLocalData(node);
   
   float r[5];
   AtColor w[5];
   
   for (unsigned int i=0, j=0; i<data->num_layers; ++i, j+=2)
   {
      r[i] = AiShaderEvalParamFlt(p_radius0 + j);
      w[i] = AiShaderEvalParamRGB(p_weight0 + j);
   }
   
   if (data->method == SM_Cubic)
   {
      sg->out.RGB = AiBSSRDFCubic(sg, r, w, data->num_layers);
   }
   else
   {
      sg->out.RGB = AiBSSRDFGaussian(sg, r, w, data->num_layers);
   }
}
