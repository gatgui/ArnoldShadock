#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ShadingNormalMtd);

enum ShadingNormalParams
{
   p_time = 0,
   p_mode,
   p_faceforward,
   p_space,
   p_custom_space
};

enum ShadingNormalMode
{
   SNM_Default = 0,
   SNM_Geometric,
   SNM_Smoothed
};

enum ShadingNormalSpace
{
   SNS_World = 0,
   SNS_Object,
   SNS_Custom
};

static const char* ShadingNormalModeNames[] =
{
   "default",
   "geometric",
   "smoothed",
   NULL
};

static const char* ShadingNormalSpaceNames[] =
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
   AiParameterEnum("mode", SNM_Default, ShadingNormalModeNames);
   AiParameterBool("faceforward", false);
   AiParameterEnum("space", SNS_World, ShadingNormalSpaceNames);
   AiParameterMtx("custom_space", id);
   
   AiMetaDataSetBool(mds, "mode", "linkable", false);
   AiMetaDataSetBool(mds, "faceforward", "linkable", false);
   AiMetaDataSetBool(mds, "space", "linkable", false);
}

struct ShadingNormalData
{
   bool time_is_linked;
   float time;
   ShadingNormalMode mode;
   bool faceforward;
   ShadingNormalSpace space;
   bool custom_space_is_linked;
   AtMatrix custom_space;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(ShadingNormalData)));
}

node_update
{
   ShadingNormalData *data = (ShadingNormalData*) AiNodeGetLocalData(node);
   
   data->time_is_linked = AiNodeIsLinked(node, "time");
   
   if (!data->time_is_linked)
   {
      data->time = AiNodeGetFlt(node, "time");
   }
   
   data->mode = (ShadingNormalMode) AiNodeGetInt(node, "mode");
   
   data->faceforward = AiNodeGetBool(node, "faceforward");
   
   data->space = (ShadingNormalSpace) AiNodeGetInt(node, "space");
   
   data->custom_space_is_linked = AiNodeIsLinked(node, "custom_space");
   
   if (data->space == SNS_Custom && !data->custom_space_is_linked)
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
   ShadingNormalData *data = (ShadingNormalData*) AiNodeGetLocalData(node);
   
   AtPoint P, N, Ns, Ng, outN;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->time_is_linked ? AiShaderEvalParamFlt(p_time) : data->time), &P, &N, &Ng, &Ns);
   
   switch (data->mode)
   {
   case SNM_Geometric:
      outN = Ng;
      break;
      
   case SNM_Smoothed:
      outN = Ns;
      break;
      
   default:
      outN = N;
   }
   
   if (data->faceforward)
   {
      AiFaceForward(&outN, sg->Rd);
   }
   
   switch (data->space)
   {
   case SNS_Object:
      sg->out.VEC = AiShaderGlobalsTransformNormal(sg, outN, AI_WORLD_TO_OBJECT);
      break;
      
   case SNS_Custom:
      {
         AtVector csN;
         
         if (data->custom_space_is_linked)
         {
            // custom space -> world matrix 
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix custom_space;
            AiM4Invert(custom_space, *tmp);
            
            AiM4VectorByMatrixMult(&csN, custom_space, &outN);
         }
         else
         {
            AiM4VectorByMatrixMult(&csN, data->custom_space, &outN);
         }
         
         sg->out.VEC = AiV3Normalize(csN);
      }
      break;
      
   case SNS_World:
   default:
      sg->out.VEC = outN;
      break;
   }
}
