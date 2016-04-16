#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadingPointMtd);

enum ShadingPointParams
{
   p_time = 0,
   p_space,
   p_custom_space
};

enum ShadingPointSpace
{
   SPS_World = 0,
   SPS_Object,
   SPS_Custom
};

static const char* ShadingPointSpaceNames[] =
{
   "world",
   "object",
   "custom",
   NULL
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   
   AiParameterFlt(SSTR::time, 0.0f);
   AiParameterEnum(SSTR::space, SPS_World, ShadingPointSpaceNames);
   AiParameterMtx(SSTR::custom_space, id);
   
   AiMetaDataSetBool(mds, SSTR::space, SSTR::linkable, false);
}

struct NodeData
{
   bool evalTime;
   float time;
   ShadingPointSpace space;
   bool evalCustomSpace;
   AtMatrix customSpace;
};

node_initialize
{
   AiNodeSetLocalData(node, new NodeData());
   AddMemUsage<NodeData>();
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->evalTime = AiNodeIsLinked(node, SSTR::time);
   
   if (!data->evalTime)
   {
      data->time = AiNodeGetFlt(node, SSTR::time);
   }
   
   data->space = (ShadingPointSpace) AiNodeGetInt(node, SSTR::space);
   
   data->evalCustomSpace = AiNodeIsLinked(node, SSTR::custom_space);
   
   if (data->space == SPS_Custom && !data->evalCustomSpace)
   {
      // custom space -> world matrix
      AtMatrix tmp;
      AiNodeGetMatrix(node, SSTR::custom_space, tmp);
      // world -> custom space matrix
      AiM4Invert(data->customSpace, tmp);
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
   
   AtPoint outP;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->evalTime ? AiShaderEvalParamFlt(p_time) : data->time), &outP, 0, 0, 0);
   
   switch (data->space)
   {
   case SPS_Object:
      sg->out.PNT = AiShaderGlobalsTransformPoint(sg, outP, AI_WORLD_TO_OBJECT);
      break;
      
   case SPS_Custom:
      {
         if (data->evalCustomSpace)
         {
            // custom space -> world matrix
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix customSpace;
            AiM4Invert(customSpace, *tmp);
            
            AiM4PointByMatrixMult(&(sg->out.PNT), customSpace, &outP);
         }
         else
         {
            AiM4PointByMatrixMult(&(sg->out.PNT), data->customSpace, &outP);
         }
      }
      break;
      
   case SPS_World:
   default:
      sg->out.PNT = outP;
      break;
   }
}
