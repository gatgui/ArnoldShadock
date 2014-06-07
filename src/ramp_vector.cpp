#include "common.h"
#include <iostream>

AI_SHADER_NODE_EXPORT_METHODS(RampVectorMtd);

enum RampVectorParams
{
   p_input = 0,
   p_positions,
   p_values,
   p_interpolations,
   p_default_value
};

node_parameters
{
   AiParameterFlt("input", 0.0f);
   AiParameterArray("positions", AiArray(2, 1, AI_TYPE_FLOAT, 0.0f, 1.0f));
   AiParameterArray("values", AiArray(2, 1, AI_TYPE_VECTOR, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));
   AiParameterArray("interpolations", AiArray(2, 1, AI_TYPE_INT, IT_Linear, IT_Linear));
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
   
   // only allow values to be linked
   AiMetaDataSetBool(mds, "positions", "linkable", false);
   AiMetaDataSetBool(mds, "interpolations", "linkable", false);
}

struct RampVectorData
{
   bool valid;
   AtArray *positions;
   AtArray *interpolations;
   unsigned int *shuffle;
};

node_initialize
{
   RampVectorData *data = (RampVectorData*) AiMalloc(sizeof(RampVectorData));
   
   data->valid = false;
   data->positions = 0;
   data->interpolations = 0;
   data->shuffle = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   RampVectorData *data = (RampVectorData*) AiNodeGetLocalData(node);
   
   AtNode *opts = AiUniverseGetOptions();
   int nthreads = AiNodeGetInt(opts, "threads");
   
   data->valid = false;
   
   if (data->shuffle)
   {
      AiFree(data->shuffle);
      data->shuffle = 0;
   }
   
   data->positions = AiNodeGetArray(node, "positions");
   data->interpolations = AiNodeGetArray(node, "interpolations");
   
   if (data->positions &&
       data->interpolations &&
       data->positions->nelements == data->interpolations->nelements)
   {
      data->shuffle = (unsigned int*) AiMalloc(data->positions->nelements * sizeof(unsigned int));
      
      SortPositions(data->positions, data->shuffle);
      
      data->valid = true;
   }
   else
   {
      AiMsgWarning("[ramp_vector] Invalid positions and/or interpolations");
      
      data->positions = 0;
      data->interpolations = 0;
   }
   
   std::cout << "[ramp_vector] Threads Count: " << nthreads << std::endl;
}

node_finish
{
   RampVectorData *data = (RampVectorData*) AiNodeGetLocalData(node);
   AiFree(data->shuffle);
   AiFree(data);
}

shader_evaluate
{
   RampVectorData *data = (RampVectorData*) AiNodeGetLocalData(node);
   
   if (!data->valid)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
      return;
   }
   
   AtArray *v = AiShaderEvalParamArray(p_values);
   
   if (v->nelements != data->positions->nelements)
   {
      AiMsgWarning("[ramp_vector] Array size mismatch");
      sg->out.VEC = AiShaderEvalParamFlt(p_default_value);
      return;
   }
   
   EvalVectorRamp(data->positions, v, data->interpolations, IT_Linear,
                  data->shuffle, AiShaderEvalParamFlt(p_input),
                  sg->out.VEC);
}
