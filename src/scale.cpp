#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(ScaleMtd);

enum ScaleParams
{
   p_input = 0,
   p_scale_pivot,
   p_scale
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::scale, 1.0f, 1.0f, 1.0f);
   AiParameterPnt(SSTR::scale_pivot, 0.0f, 0.0f, 0.0f);
}

struct NodeData
{
   AtMatrix S;
   AtMatrix Sp;
   AtMatrix iSp;
   AtMatrix Sf; // final scale matrix -> iSp * S * Sp

   bool S_set;
   bool Sp_set;
};

node_initialize
{
   NodeData *data = new NodeData();
   AddMemUsage<NodeData>();

   data->S_set = false;
   data->Sp_set = false;

   AiNodeSetLocalData(node, data);
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->S_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale))
   {
      data->S_set = true;
      AtVector S = AiNodeGetVec(node, SSTR::scale);
      AiM4Scaling(data->S, &S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale_pivot))
   {
      data->Sp_set = true;
      AtPoint P = AiNodeGetPnt(node, SSTR::scale_pivot);
      AiM4Translation(data->Sp, &P);
      P = -P;
      AiM4Translation(data->iSp, &P);
   }
   
   if (data->S_set && data->Sp_set)
   {
      AtMatrix tmp;
      AiM4Mult(tmp, data->S, data->iSp);
      AiM4Mult(data->Sf, data->Sp, tmp);
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
   
   AtVector p, ip, s;
   AtMatrix S, P, iP, tmp;
   bool computeS = true;
   
   if (!data->S_set)
   {
      s = AiShaderEvalParamVec(p_scale);
      AiM4Scaling(S, &s);
   }
   else
   {
      if (data->Sp_set)
      {
         AiM4Copy(S, data->Sf);
         computeS = false;
      }
      else
      {
         AiM4Copy(S, data->S);
      }
   }
   
   if (!data->Sp_set)
   {
      p = AiShaderEvalParamVec(p_scale_pivot);
      ip = -p;
      
      AiM4Translation(P, &p);
      AiM4Translation(iP, &ip);
      
      AiM4Mult(tmp, S, iP);
      AiM4Mult(S, P, tmp);
   }
   else if (computeS)
   {
      AiM4Mult(tmp, S, data->iSp);
      AiM4Mult(S, data->Sp, tmp);
   }
   
   AtVector v = AiShaderEvalParamVec(p_input);
   
   AiM4VectorByMatrixMult(&(sg->out.VEC), S, &v);
}
