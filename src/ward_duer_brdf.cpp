#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(WardDuerBrdfMtd);

enum WardDuerBrdfParams
{
   p_roughness_x = 0,
   p_roughness_y,
   p_u,
   p_u_space,
   p_v,
   p_v_space,
   p_N,
   p_N_space
};

node_parameters
{
   AiParameterFlt("roughness_x", 0.467f);
   AiParameterFlt("roughness_y", 0.467f);
   AiParameterVec("u", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("u_space", CS_World, CoordinateSpaceNames);
   AiParameterVec("v", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("v_space", CS_World, CoordinateSpaceNames);
   AiParameterVec("N", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("N_space", CS_World, CoordinateSpaceNames);
   
   AiMetaDataSetBool(mds, "u_space", "linkable", false);
   AiMetaDataSetBool(mds, "v_space", "linkable", false);
   AiMetaDataSetBool(mds, "N_space", "linkable", false);
}

struct NodeData
{
   bool u_is_connected;
   CoordinateSpace u_space;
   bool v_is_connected;
   CoordinateSpace v_space;
   bool N_is_connected;
   CoordinateSpace N_space;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->u_is_connected = AiNodeIsLinked(node, "u");
   data->u_space = (CoordinateSpace) AiNodeGetInt(node, "u_space");
   data->v_is_connected = AiNodeIsLinked(node, "v");
   data->v_space = (CoordinateSpace) AiNodeGetInt(node, "v_space");
   data->N_is_connected = AiNodeIsLinked(node, "N");
   data->N_space = (CoordinateSpace) AiNodeGetInt(node, "N_space");
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
}

shader_evaluate
{
   NodeData *node_data = (NodeData*) AiNodeGetLocalData(node);
   BRDFData *brdf_data = (BRDFData*) AiShaderGlobalsQuickAlloc(sg, sizeof(BRDFData));
   
   float rx = AiShaderEvalParamFlt(p_roughness_x);
   float ry = AiShaderEvalParamFlt(p_roughness_y);
   AtVector N, u, v;
   
   if (node_data->N_is_connected)
   {
      N = AiShaderEvalParamVec(p_N);
      if (node_data->N_space == CS_Object)
      {
         AtVector No = N;
         AiM4VectorByMatrixMult(&N, sg->M, &No);
         N = AiV3Normalize(N);
      }
   }
   else
   {
      N = sg->N;
   }
   
   AiBuildLocalFramePolar(&u, &v, &N);
   
   if (node_data->u_is_connected)
   {
      u = AiShaderEvalParamVec(p_u);
      if (node_data->u_space == CS_Object)
      {
         AtVector uo = u;
         AiM4VectorByMatrixMult(&u, sg->M, &uo);
         u = AiV3Normalize(u);
      }
   }
   
   if (node_data->v_is_connected)
   {
      v = AiShaderEvalParamVec(p_v);
      if (node_data->v_space == CS_Object)
      {
         AtVector vo = v;
         AiM4VectorByMatrixMult(&v, sg->M, &vo);
         v = AiV3Normalize(v);
      }
   }
   
   brdf_data->evalSample = AiWardDuerMISSample;
   brdf_data->evalBrdf = AiWardDuerMISBRDF;
   brdf_data->evalPdf = AiWardDuerMISPDF;
   brdf_data->data = AiWardDuerMISCreateData(sg, &u, &v, rx, ry);
   
   sg->out.PTR = brdf_data;
}
