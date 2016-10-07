#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SssMultiMtd);

enum SssMultiParams
{
   p_method = 0,
   p_radius0,
   p_albedo0,
   p_weight0,
   p_radius1,
   p_albedo1,
   p_weight1,
   p_radius2,
   p_albedo2,
   p_weight2,
   p_radius_scale
};

enum SssMethod
{
   SM_Cubic = 0,
   SM_Gaussian,
   SM_Empirical
};

static const char* SssMethodNames[] = {"cubic", "gaussian", "empirical", NULL};

node_parameters
{
   AiParameterEnum(SSTR::method, SM_Cubic, SssMethodNames);
   AiParameterVec("radius0", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("albedo0", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("weight0", 1.0f, 1.0f, 1.0f);
   AiParameterVec("radius1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("albedo1", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("weight1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("radius2", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("albedo2", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("weight2", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("radius_scale", 1.0f);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::method);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

inline void EvalLayer(AtShaderGlobals *sg, AtNode *node, int inputIndex, bool isEmpirical, float radiusScale,
                      unsigned int &outputIndex, float *radiuses, float *albedos, AtColor *weights)
{
   bool evalAlbedo = true;
   int offset = inputIndex * 3;
   AtVector radius = AiShaderEvalParamVec(p_radius0 + offset);
   AtColor weight = AiShaderEvalParamRGB(p_weight0 + offset);
   AtColor albedo = AI_RGB_BLACK;
   
   radius *= radiusScale;
   
   // But for empirical method, albedo is mixed in the final weight
   // If radius is uniform on all channels, only generate a single layer
   if (!isEmpirical && fabsf(radius.x - radius.y) < AI_EPSILON && fabsf(radius.x - radius.z) < AI_EPSILON)
   {
      if (radius.x > 0.0f && !AiColorIsZero(weight))
      {
         albedo = AiShaderEvalParamRGB(p_albedo0 + offset);
         if (!AiColorIsZero(albedo))
         {
            radiuses[outputIndex] = radius.x;
            albedos[outputIndex] = 1.0f;
            weights[outputIndex] = albedo * weight;
            ++outputIndex;
         }
      }
   }
   else
   {
      if (radius.x > 0.0f && weight.r > 0.0f)
      {
         if (evalAlbedo)
         {
            albedo = AiShaderEvalParamRGB(p_albedo0 + offset);
            evalAlbedo = false;
         }
         if (albedo.r > 0.0f)
         {
            radiuses[outputIndex] = radius.x;
            albedos[outputIndex] = albedo.r;
            weights[outputIndex] = AiColor(weight.r, 0.0f, 0.0f);
            ++outputIndex;
         }
      }
      if (radius.y > 0.0f && weight.g > 0.0f)
      {
         if (evalAlbedo)
         {
            albedo = AiShaderEvalParamRGB(p_albedo0 + offset);
            evalAlbedo = false;
         }
         if (albedo.g > 0.0f)
         {
            radiuses[outputIndex] = radius.y;
            albedos[outputIndex] = albedo.g;
            weights[outputIndex] = AiColor(0.0f, weight.g, 0.0f);
            ++outputIndex;
         }
      }
      if (radius.z > 0.0f && weight.b > 0.0f)
      {
         if (evalAlbedo)
         {
            albedo = AiShaderEvalParamRGB(p_albedo0 + offset);
            evalAlbedo = false;
         }
         if (albedo.b > 0.0f)
         {
            radiuses[outputIndex] = radius.z;
            albedos[outputIndex] = albedo.b;
            weights[outputIndex] = AiColor(0.0f, 0.0f, weight.b);
            ++outputIndex;
         }
      }
   }
}

shader_evaluate
{
   SssMethod method = (SssMethod) *((int*) AiNodeGetLocalData(node));
   
   float radiusScale = AiShaderEvalParamFlt(p_radius_scale);

   if (radiusScale <= 0.0f)
   {
      sg->out.RGB = AI_RGB_BLACK;
      return;
   }

   float radiuses[9];
   float albedos[9];
   AtColor weights[9];
   unsigned int numLayers = 0;

   EvalLayer(sg, node, 0, method != SM_Empirical, radiusScale, numLayers, radiuses, albedos, weights);
   EvalLayer(sg, node, 1, method != SM_Empirical, radiusScale, numLayers, radiuses, albedos, weights);
   EvalLayer(sg, node, 2, method != SM_Empirical, radiusScale, numLayers, radiuses, albedos, weights);

   if (numLayers == 0)
   {
      sg->out.RGB = AI_RGB_BLACK;
      return;
   }
   
   switch (method)
   {
   case SM_Gaussian:
      // mix albedos in weights
      for (unsigned int i=0; i<numLayers; ++i)
      {
         weights[i] *= albedos[i];
      }
      sg->out.RGB = AiBSSRDFGaussian(sg, radiuses, weights, numLayers);
      break;
   case SM_Empirical:
      {
         AtColor direct = AI_RGB_BLACK;
         AtColor indirect = AI_RGB_BLACK;
         AiBSSRDFEmpirical(sg, direct, indirect, radiuses, albedos, weights, numLayers);
         sg->out.RGB = direct + indirect;
      }
      break;
   case SM_Cubic:
   default:
      // mix albedos in weights
      for (unsigned int i=0; i<numLayers; ++i)
      {
         weights[i] *= albedos[i];
      }
      sg->out.RGB = AiBSSRDFCubic(sg, radiuses, weights, numLayers);
      break;
   }
}
