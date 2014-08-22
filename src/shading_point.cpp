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
   
   AiParameterFlt("time", 0.0f);
   AiParameterEnum("space", SPS_World, ShadingPointSpaceNames);
   AiParameterMtx("custom_space", id);
   
   AiMetaDataSetBool(mds, "space", "linkable", false);
}

struct ShadingPointData
{
   bool time_is_linked;
   float time;
   ShadingPointSpace space;
   bool custom_space_is_linked;
   AtMatrix custom_space;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(ShadingPointData)));
}

node_update
{
   ShadingPointData *data = (ShadingPointData*) AiNodeGetLocalData(node);
   
   data->time_is_linked = AiNodeIsLinked(node, "time");
   
   if (!data->time_is_linked)
   {
      data->time = AiNodeGetFlt(node, "time");
   }
   
   data->space = (ShadingPointSpace) AiNodeGetInt(node, "space");
   
   data->custom_space_is_linked = AiNodeIsLinked(node, "custom_space");
   
   if (data->space == SPS_Custom && !data->custom_space_is_linked)
   {
      // custom space -> world matrix
      AtMatrix tmp;
      AiNodeGetMatrix(node, "custom_space", tmp);
      // world -> custom space matrix
      AiM4Invert(data->custom_space, tmp);
   }
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   ShadingPointData *data = (ShadingPointData*) AiNodeGetLocalData(node);
   
   AtPoint outP;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->time_is_linked ? AiShaderEvalParamFlt(p_time) : data->time), &outP, 0, 0, 0);
   
   switch (data->space)
   {
   case SPS_Object:
      sg->out.PNT = AiShaderGlobalsTransformPoint(sg, outP, AI_WORLD_TO_OBJECT);
      break;
      
   case SPS_Custom:
      {
         if (data->custom_space_is_linked)
         {
            // custom space -> world matrix
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix custom_space;
            AiM4Invert(custom_space, *tmp);
            
            AiM4PointByMatrixMult(&(sg->out.PNT), custom_space, &outP);
         }
         else
         {
            AiM4PointByMatrixMult(&(sg->out.PNT), data->custom_space, &outP);
         }
      }
      break;
      
   case SPS_World:
   default:
      sg->out.PNT = outP;
      break;
   }
}
