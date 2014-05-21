#include <ai.h>
#include <cstring>

extern AtNodeMethods* CompareFloatMtd;
extern AtNodeMethods* CompareColorMtd;
extern AtNodeMethods* SelectFloatMtd;
extern AtNodeMethods* SelectColorMtd;

enum
{
    COMPARE_FLOAT = 0,
    COMPARE_COLOR,
    SELECT_FLOAT,
    SELECT_COLOR
};

node_loader
{
   if (i == COMPARE_FLOAT)
   {
      node->name = "compare_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_BOOLEAN;
      node->methods = CompareFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == COMPARE_COLOR)
   {
      node->name = "compare_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_BOOLEAN;
      node->methods = CompareColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == SELECT_FLOAT)
   {
      node->name = "select_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = SelectFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == SELECT_COLOR)
   {
      node->name = "select_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = SelectColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else
   {
      return false;
   }
}
