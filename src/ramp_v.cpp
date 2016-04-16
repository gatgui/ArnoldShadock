#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(RampVMtd);

enum RampVParams
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
   AiParameterArray(SSTR::positions, def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_VECTOR);
   AiArraySetVec(def, 0, AI_V3_ZERO);
   AiArraySetVec(def, 1, AI_V3_ONE);
   AiParameterArray(SSTR::values, def);
   
   def = AiArrayAllocate(2, 1, AI_TYPE_INT);
   AiArraySetInt(def, 0, RI_Linear);
   AiArraySetInt(def, 1, RI_Linear);
   AiParameterArray(SSTR::interpolations, def);
   
   AiParameterVec("default_value", 0.0f, 0.0f, 0.0f);
   
   //AiMetaDataSetStr(mds, NULL, "ramps", SSTR::values);
   //AiMetaDataSetStr(mds, NULL, "ramps.values.positions", SSTR::positions);
   //AiMetaDataSetStr(mds, NULL, "ramps.values.interpolations", SSTR::interpolations);
}

struct NodeData
{
   bool valid;
   unsigned int nkeys;
   AtArray *positions;
   bool evalPositions;
   AtArray *values;
   bool evalValues;
   AtArray *interpolations;
   bool evalInterpolations;
   unsigned int nshuffles;
   unsigned int **shuffles;
};

node_initialize
{
   NodeData *data = (NodeData*) AiMalloc(sizeof(NodeData));
   AddMemUsage<NodeData>();
   
   data->valid = false;
   data->nkeys = 0;
   data->positions = 0;
   data->evalPositions = false;
   data->values = 0;
   data->evalValues = false;
   data->interpolations = 0;
   data->evalInterpolations = false;
   data->nshuffles = 0;
   data->shuffles = 0;
   
   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
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
   
   data->positions = AiNodeGetArray(node, SSTR::positions);
   data->evalPositions = AiNodeIsLinked(node, SSTR::positions);
   
   data->values = AiNodeGetArray(node, SSTR::values);
   data->evalValues = AiNodeIsLinked(node, SSTR::values);
   
   data->interpolations = AiNodeGetArray(node, SSTR::interpolations);
   data->evalInterpolations = AiNodeIsLinked(node, SSTR::interpolations);
   
   if (data->positions->nelements > 0 ||
       data->positions->nelements == data->values->nelements ||
       data->positions->nelements == data->interpolations->nelements)
   {
      data->nkeys = data->positions->nelements;
      
      data->nshuffles = (data->evalPositions ? GetRenderThreadsCount() : 1);
      data->shuffles = (unsigned int **) AiMalloc(data->nshuffles * sizeof(unsigned int*));
      
      for (unsigned int i=0; i<data->nshuffles; ++i)
      {
         data->shuffles[i] = (unsigned int*) AiMalloc(data->nkeys * sizeof(unsigned int));
      }
      
      if (!data->evalPositions)
      {
         SortRampPositions(data->positions, data->shuffles[0]);
      }
      
      data->valid = true;
   }
   else
   {
      AiMsgWarning("[ramp_vector] Invalid ramp parameters");
   }
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   for (unsigned int i=0; i<data->nshuffles; ++i)
   {
      AiFree(data->shuffles[i]);
   }
   AiFree(data->shuffles);
   
   delete data;
   SubMemUsage<NodeData>();
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   if (!data->valid)
   {
      sg->out.VEC = AiShaderEvalParamVec(p_default_value);
      return;
   }
   
   AtArray *p = (data->evalPositions ? AiShaderEvalParamArray(p_values) : data->positions);
   AtArray *v = (data->evalValues ? AiShaderEvalParamArray(p_values) : data->values);
   AtArray *i = (data->evalInterpolations ? AiShaderEvalParamArray(p_values) : data->interpolations);
   
   if (v->nelements != data->nkeys || p->nelements != data->nkeys || i->nelements != data->nkeys)
   {
      AiMsgWarning("[ramp_vector] Array size mismatch");
      sg->out.VEC = AiShaderEvalParamFlt(p_default_value);
   }
   else
   {
      float input = AiShaderEvalParamFlt(p_input);
      int si = 0;
      
      if (data->evalPositions)
      {
         si = sg->tid;
         SortRampPositions(i, data->shuffles[si]);
      }
      
      VectorRamp(p, v, i, RI_Linear, data->shuffles[si], input, sg->out.VEC);
   }
}
