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
   
   AiParameterFlt(SSTR::time, 0.0f);
   AiParameterEnum(SSTR::mode, SNM_Default, ShadingNormalModeNames);
   AiParameterBool(SSTR::faceforward, false);
   AiParameterEnum(SSTR::space, SNS_World, ShadingNormalSpaceNames);
   AiParameterMtx(SSTR::custom_space, id);
   
   AiMetaDataSetBool(mds, SSTR::mode, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::faceforward, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::space, SSTR::linkable, false);
}

struct NodeData
{
   bool evalTime;
   float time;
   ShadingNormalMode mode;
   bool faceforward;
   ShadingNormalSpace space;
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
   
   data->mode = (ShadingNormalMode) AiNodeGetInt(node, SSTR::mode);
   
   data->faceforward = AiNodeGetBool(node, SSTR::faceforward);
   
   data->space = (ShadingNormalSpace) AiNodeGetInt(node, SSTR::space);
   
   data->evalCustomSpace = AiNodeIsLinked(node, SSTR::custom_space);
   
   if (data->space == SNS_Custom && !data->evalCustomSpace)
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
   
   AtPoint P, N, Ns, Ng, outN;
   
   AiShaderGlobalsGetPositionAtTime(sg, (data->evalTime ? AiShaderEvalParamFlt(p_time) : data->time), &P, &N, &Ng, &Ns);
   
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
         
         if (data->evalCustomSpace)
         {
            // custom space -> world matrix 
            AtMatrix *tmp = AiShaderEvalParamMtx(p_custom_space);
            // world -> custom space matrix
            AtMatrix customSpace;
            AiM4Invert(customSpace, *tmp);
            
            AiM4VectorByMatrixMult(&csN, customSpace, &outN);
         }
         else
         {
            AiM4VectorByMatrixMult(&csN, data->customSpace, &outN);
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
