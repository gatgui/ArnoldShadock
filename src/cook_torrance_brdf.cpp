#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(CookTorranceBrdfMtd);

enum CookTorranceBrdfParams
{
   p_u = 0,
   p_v,
   p_roughness_x,
   p_roughness_y
};

node_parameters
{
   AiParameterVec("u", 0.0f, 0.0f, 0.0f);
   AiParameterVec("v", 0.0f, 0.0f, 0.0f);
   AiParameterFlt("roughness_x", 0.467f);
   AiParameterFlt("roughness_y", 0.467f);
}

struct NodeData
{
   bool u_is_connected;
   bool v_is_connected;
};

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(NodeData)));
}

node_update
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);
   
   data->u_is_connected = AiNodeIsLinked(node, "u");
   data->v_is_connected = AiNodeIsLinked(node, "v");
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
   AtVector u, v;
   
   if (node_data->u_is_connected)
   {
      u = AiShaderEvalParamVec(p_u);
   }
   else
   {
      u = AiV3Normalize(sg->dPdu);
   }
   
   if (node_data->v_is_connected)
   {
      v = AiShaderEvalParamVec(p_v);
   }
   else
   {
      v = AiV3Normalize(sg->dPdv);
   }
   
   brdf_data->evalSample = AiCookTorranceMISSample;
   brdf_data->evalBrdf = AiCookTorranceMISBRDF;
   brdf_data->evalPdf = AiCookTorranceMISPDF;
   brdf_data->data = AiCookTorranceMISCreateData(sg, &u, &v, rx, ry);
   
   sg->out.PTR = brdf_data;
}
