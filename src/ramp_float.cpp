#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RampFloatMtd);

enum RampFloatParams
{
   p_input = 0,
   p_positions,
   p_values,
   p_interpolations,
   p_default_value
};

node_parameters
{
   AtArray *def;
   
   AiParameterFlt("input", 0.0f);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_FLOAT);
   AiArraySetFlt(def, 0, 0.0f);
   AiArraySetFlt(def, 1, 1.0f);
   AiParameterArray("positions", def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_FLOAT);
   AiArraySetFlt(def, 0, 0.0f);
   AiArraySetFlt(def, 1, 1.0f);
   AiParameterArray("values", def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_INT);
   AiArraySetInt(def, 0, RI_Linear);
   AiArraySetInt(def, 1, RI_Linear);
   AiParameterArray("interpolations", def);
   
   AiParameterFlt("default_value", 0.0f);
   
   AiMetaDataSetStr(mds, NULL, "ramps", "values");
   AiMetaDataSetStr(mds, NULL, "ramps.values.positions", "positions");
   AiMetaDataSetStr(mds, NULL, "ramps.values.interpolations", "interpolations");
}

struct RampFloatData
{
   bool valid;
   unsigned int nkeys;
   AtArray *positions;
   bool linked_positions;
   AtArray *values;
   bool linked_values;
   AtArray *interpolations;
   bool linked_interpolations;
   unsigned int nshuffles;
   unsigned int **shuffles;
};

node_initialize
{
   RampFloatData *data = (RampFloatData*) AiMalloc(sizeof(RampFloatData));
   
   data->valid = false;
   data->nkeys = 0;
   data->positions = 0;
   data->linked_positions = false;
   data->values = 0;
   data->linked_values = false;
   data->interpolations = 0;
   data->linked_interpolations = false;
   data->nshuffles = 0;
   data->shuffles = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   RampFloatData *data = (RampFloatData*) AiNodeGetLocalData(node);
   
   data->valid = false;
   data->nkeys = 0;
   
   if (data->shuffles)
   {
      for (unsigned int i=0; i<data->nshuffles; ++i)
      {
         AiFree(data->shuffles[i]);
      }
      AiFree(data->shuffles);
      data->shuffles = 0;
   }
   data->nshuffles = 0;
   
   data->positions = AiNodeGetArray(node, "positions");
   data->linked_positions = AiNodeIsLinked(node, "positions");
   
   data->values = AiNodeGetArray(node, "values");
   data->linked_values = AiNodeIsLinked(node, "values");
   
   data->interpolations = AiNodeGetArray(node, "interpolations");
   data->linked_interpolations = AiNodeIsLinked(node, "interpolations");
   
   if (data->positions->nelements > 0 ||
       data->positions->nelements == data->values->nelements ||
       data->positions->nelements == data->interpolations->nelements)
   {
      data->nkeys = data->positions->nelements;
      
      data->nshuffles = (data->linked_positions ? GetRenderThreadsCount() : 1);
      data->shuffles = (unsigned int **) AiMalloc(data->nshuffles * sizeof(unsigned int*));
      
      for (unsigned int i=0; i<data->nshuffles; ++i)
      {
         data->shuffles[i] = (unsigned int*) AiMalloc(data->nkeys * sizeof(unsigned int));
      }
      
      if (!data->linked_positions)
      {
         SortRampPositions(data->positions, data->shuffles[0]);
      }
      
      data->valid = true;
   }
   else
   {
      AiMsgWarning("[ramp_float] Invalid ramp parameters");
   }
}

node_finish
{
   RampFloatData *data = (RampFloatData*) AiNodeGetLocalData(node);
   
   for (unsigned int i=0; i<data->nshuffles; ++i)
   {
      AiFree(data->shuffles[i]);
   }
   AiFree(data->shuffles);
   
   AiFree(data);
}

shader_evaluate
{
   RampFloatData *data = (RampFloatData*) AiNodeGetLocalData(node);
   
   if (!data->valid)
   {
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
      return;
   }
   
   AtArray *p = (data->linked_positions ? AiShaderEvalParamArray(p_values) : data->positions);
   AtArray *v = (data->linked_values ? AiShaderEvalParamArray(p_values) : data->values);
   AtArray *i = (data->linked_interpolations ? AiShaderEvalParamArray(p_values) : data->interpolations);
   
   if (v->nelements != data->nkeys || p->nelements != data->nkeys || i->nelements != data->nkeys)
   {
      AiMsgWarning("[ramp_float] Array size mismatch");
      sg->out.FLT = AiShaderEvalParamFlt(p_default_value);
   }
   else
   {
      float input = AiShaderEvalParamFlt(p_input);
      int si = 0;
      
      if (data->linked_positions)
      {
         si = sg->tid;
         SortRampPositions(i, data->shuffles[si]);
      }
      
      FloatRamp(p, v, i, RI_Linear, data->shuffles[si], input, sg->out.FLT);
   }
}
