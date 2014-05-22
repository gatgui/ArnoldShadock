#include <ai.h>
#include <cstring>

extern AtNodeMethods* CompareFloatMtd;
extern AtNodeMethods* CompareColorMtd;
extern AtNodeMethods* CompareVectorMtd;
extern AtNodeMethods* SelectFloatMtd;
extern AtNodeMethods* SelectColorMtd;
extern AtNodeMethods* SelectVectorMtd;
extern AtNodeMethods* MultiSelectFloatMtd;
extern AtNodeMethods* MultiSelectColorMtd;
extern AtNodeMethods* MultiSelectVectorMtd;
extern AtNodeMethods* UnaryMathFloatMtd;
extern AtNodeMethods* UnaryMathColorMtd;
extern AtNodeMethods* UnaryMathVectorMtd;
extern AtNodeMethods* BinaryMathFloatMtd;
extern AtNodeMethods* BinaryMathColorMtd;
extern AtNodeMethods* BinaryMathVectorMtd;
extern AtNodeMethods* FloatToIntMtd;
extern AtNodeMethods* FloatToBoolMtd;
extern AtNodeMethods* ClampFloatMtd;
extern AtNodeMethods* SmoothStepFloatMtd;

enum
{
    COMPARE_FLOAT = 0,
    COMPARE_COLOR,
    COMPARE_VECTOR,
    SELECT_FLOAT,
    SELECT_COLOR,
    SELECT_VECTOR,
    MULTI_SELECT_FLOAT,
    MULTI_SELECT_COLOR,
    MULTI_SELECT_VECTOR,
    UNARY_MATH_FLOAT,
    UNARY_MATH_COLOR,
    UNARY_MATH_VECTOR,
    BINARY_MATH_FLOAT,
    BINARY_MATH_COLOR,
    BINARY_MATH_VECTOR,
    FLOAT_TO_INT,
    FLOAT_TO_BOOL,
    CLAMP_FLOAT,
    SMOOTHSTEP_FLOAT
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
   else if (i == COMPARE_VECTOR)
   {
      node->name = "compare_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = CompareVectorMtd;
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
   else if (i == SELECT_VECTOR)
   {
      node->name = "select_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = SelectVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MULTI_SELECT_FLOAT)
   {
      node->name = "multi_select_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = MultiSelectFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MULTI_SELECT_COLOR)
   {
      node->name = "multi_select_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = MultiSelectColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MULTI_SELECT_VECTOR)
   {
      node->name = "multi_select_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = MultiSelectVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == UNARY_MATH_FLOAT)
   {
      node->name = "unary_math_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = UnaryMathFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == UNARY_MATH_COLOR)
   {
      node->name = "unary_math_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = UnaryMathColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == UNARY_MATH_VECTOR)
   {
      node->name = "unary_math_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = UnaryMathVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == BINARY_MATH_FLOAT)
   {
      node->name = "binary_math_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = BinaryMathFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == BINARY_MATH_COLOR)
   {
      node->name = "binary_math_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = BinaryMathColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == BINARY_MATH_VECTOR)
   {
      node->name = "binary_math_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = BinaryMathVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == FLOAT_TO_INT)
   {
      node->name = "float_to_int";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_INT;
      node->methods = FloatToIntMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == FLOAT_TO_BOOL)
   {
      node->name = "float_to_bool";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_BOOLEAN;
      node->methods = FloatToBoolMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == CLAMP_FLOAT)
   {
      node->name = "clamp_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = ClampFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == SMOOTHSTEP_FLOAT)
   {
      node->name = "smoothstep_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = SmoothStepFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else
   {
      return false;
   }
}
