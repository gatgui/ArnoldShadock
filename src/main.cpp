#include <ai.h>
#include <cstring>

// ---

static int gsShaderCount = 0;

#define DECLARE_SHADER(name)\
   extern AtNodeMethods* name##Mtd;\
   static int name = gsShaderCount++

#define DECLARE_FLOAT_SHADER(name) DECLARE_SHADER(name##Float)

#define DECLARE_COLOR_SHADER(name) DECLARE_SHADER(name##Color)

#define DECLARE_VECTOR_SHADER(name) DECLARE_SHADER(name##Vector)

#define DECLARE_FCV_SHADER(name)\
   DECLARE_FLOAT_SHADER(name);\
   DECLARE_COLOR_SHADER(name);\
   DECLARE_VECTOR_SHADER(name)

#define REGISTER_SHADER(s_name, a_name, a_type)\
   else if (i == s_name)\
   {\
      node->name = #a_name;\
      node->node_type = AI_NODE_SHADER;\
      node->output_type = a_type;\
      node->methods = s_name##Mtd;\
      strcpy(node->version, AI_VERSION);\
      return true;\
   }

#define REGISTER_FLOAT_SHADER(s_name, a_name) REGISTER_SHADER(s_name##Float, a_name##_float, AI_TYPE_FLOAT)

#define REGISTER_COLOR_SHADER(s_name, a_name) REGISTER_SHADER(s_name##Color, a_name##_color, AI_TYPE_RGB)

#define REGISTER_VECTOR_SHADER(s_name, a_name) REGISTER_SHADER(s_name##Vector, a_name##_vector, AI_TYPE_VECTOR)

#define REGISTER_FCV_SHADER(s_name, a_name)\
   REGISTER_FLOAT_SHADER(s_name, a_name)\
   REGISTER_COLOR_SHADER(s_name, a_name)\
   REGISTER_VECTOR_SHADER(s_name, a_name)

// ---

DECLARE_FCV_SHADER(Abs);
DECLARE_FCV_SHADER(Add);
DECLARE_FCV_SHADER(Ceil);
DECLARE_FCV_SHADER(Clamp);
DECLARE_FCV_SHADER(Compare);
DECLARE_SHADER(ConvertFloatToBool);
DECLARE_SHADER(ConvertFloatToInt);
DECLARE_SHADER(ConvertVectorToColor);
DECLARE_SHADER(ConvertColorToVector);
DECLARE_FCV_SHADER(Divide);
DECLARE_FCV_SHADER(Floor);
DECLARE_FCV_SHADER(Inverse);
DECLARE_FCV_SHADER(Max);
DECLARE_FCV_SHADER(Min);
DECLARE_FCV_SHADER(MultiSelect);
DECLARE_FCV_SHADER(Multiply);
DECLARE_FCV_SHADER(Negate);
DECLARE_FCV_SHADER(Remap);
DECLARE_FCV_SHADER(Round);
DECLARE_FCV_SHADER(Select);
DECLARE_FCV_SHADER(Sign);
DECLARE_FCV_SHADER(SmoothStep);
DECLARE_FCV_SHADER(Subtract);

node_loader
{
   if (i >= gsShaderCount)
   {
      return false;
   }
   REGISTER_FCV_SHADER(Abs, absolute)
   REGISTER_FCV_SHADER(Add, add)
   REGISTER_FCV_SHADER(Ceil, ceil)
   REGISTER_FCV_SHADER(Clamp, clamp)
   REGISTER_FCV_SHADER(Compare, compare)
   REGISTER_SHADER(ConvertFloatToBool, convert_float_to_bool, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ConvertFloatToInt, convert_float_to_int, AI_TYPE_INT)
   REGISTER_SHADER(ConvertVectorToColor, convert_vector_to_color, AI_TYPE_RGB)
   REGISTER_SHADER(ConvertColorToVector, convert_color_to_vector, AI_TYPE_VECTOR)
   REGISTER_FCV_SHADER(Divide, divide)
   REGISTER_FCV_SHADER(Floor, floor)
   REGISTER_FCV_SHADER(Inverse, inverse)
   REGISTER_FCV_SHADER(Max, max)
   REGISTER_FCV_SHADER(Min, min)
   REGISTER_FCV_SHADER(MultiSelect, multi_select)
   REGISTER_FCV_SHADER(Multiply, multiply)
   REGISTER_FCV_SHADER(Negate, negate)
   REGISTER_FCV_SHADER(Remap, remap)
   REGISTER_FCV_SHADER(Round, round)
   REGISTER_FCV_SHADER(Select, select)
   REGISTER_FCV_SHADER(Sign, sign)
   REGISTER_FCV_SHADER(SmoothStep, smoothstep)
   REGISTER_FCV_SHADER(Subtract, subtract)
   return false;
}

/*
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
extern AtNodeMethods* ConvertFloatToIntMtd;
extern AtNodeMethods* ConvertFloatToBoolMtd;
extern AtNodeMethods* ClampFloatMtd;
extern AtNodeMethods* ClampColorMtd;
extern AtNodeMethods* ClampVectorMtd;
extern AtNodeMethods* SmoothStepFloatMtd;
extern AtNodeMethods* SmoothStepColorMtd;
extern AtNodeMethods* SmoothStepVectorMtd;
extern AtNodeMethods* MixFloatMtd;
extern AtNodeMethods* MixColorMtd;
extern AtNodeMethods* MixVectorMtd;
extern AtNodeMethods* RemapFloatMtd;
extern AtNodeMethods* RemapColorMtd;
extern AtNodeMethods* RemapVectorMtd;

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
    CONVERT_FLOAT_TO_INT,
    CONVERT_FLOAT_TO_BOOL,
    CLAMP_FLOAT,
    CLAMP_COLOR,
    CLAMP_VECTOR,
    SMOOTHSTEP_FLOAT,
    SMOOTHSTEP_COLOR,
    SMOOTHSTEP_VECTOR,
    MIX_FLOAT,
    MIX_COLOR,
    MIX_VECTOR,
    REMAP_FLOAT,
    REMAP_COLOR,
    REMAP_VECTOR
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
   else if (i == CONVERT_FLOAT_TO_INT)
   {
      node->name = "convert_float_to_int";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_INT;
      node->methods = ConvertFloatToIntMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == CONVERT_FLOAT_TO_BOOL)
   {
      node->name = "convert_float_to_bool";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_BOOLEAN;
      node->methods = ConvertFloatToBoolMtd;
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
   else if (i == CLAMP_COLOR)
   {
      node->name = "clamp_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = ClampColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == CLAMP_VECTOR)
   {
      node->name = "clamp_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = ClampVectorMtd;
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
   else if (i == SMOOTHSTEP_COLOR)
   {
      node->name = "smoothstep_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = SmoothStepColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == SMOOTHSTEP_VECTOR)
   {
      node->name = "smoothstep_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = SmoothStepVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MIX_FLOAT)
   {
      node->name = "mix_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = MixFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MIX_COLOR)
   {
      node->name = "mix_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = MixColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == MIX_VECTOR)
   {
      node->name = "mix_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = MixVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == REMAP_FLOAT)
   {
      node->name = "remap_float";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_FLOAT;
      node->methods = RemapFloatMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == REMAP_COLOR)
   {
      node->name = "remap_color";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_RGB;
      node->methods = RemapColorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else if (i == REMAP_VECTOR)
   {
      node->name = "remap_vector";
      node->node_type = AI_NODE_SHADER;
      node->output_type = AI_TYPE_VECTOR;
      node->methods = RemapVectorMtd;
      strcpy(node->version, AI_VERSION);
      return true;
   }
   else
   {
      return false;
   }
}
*/
