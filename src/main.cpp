#include <ai.h>
#include <cstring>
#include <cstdlib>

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
DECLARE_FCV_SHADER(Bias);
DECLARE_FCV_SHADER(BiasAndGain);
DECLARE_FCV_SHADER(Ceil);
DECLARE_FCV_SHADER(Clamp);
DECLARE_FCV_SHADER(Compare);
DECLARE_SHADER(ConvertFloatToBool);
DECLARE_SHADER(ConvertFloatToInt);
DECLARE_SHADER(ConvertVectorToColor);
DECLARE_SHADER(ConvertColorToVector);
DECLARE_SHADER(ConvertColorToFloat);
DECLARE_SHADER(CrossProduct);
DECLARE_FCV_SHADER(Gain);
DECLARE_FCV_SHADER(Divide);
DECLARE_SHADER(DotProduct);
DECLARE_FCV_SHADER(Exp);
DECLARE_FCV_SHADER(Floor);
DECLARE_FCV_SHADER(Inverse);
DECLARE_FCV_SHADER(Log);
DECLARE_FCV_SHADER(Max);
DECLARE_FCV_SHADER(Min);
DECLARE_FCV_SHADER(Mix);
DECLARE_FCV_SHADER(Modulo);
DECLARE_FCV_SHADER(MultiSelect);
DECLARE_FCV_SHADER(Multiply);
DECLARE_FCV_SHADER(Negate);
DECLARE_SHADER(NormalizeVector);
DECLARE_FCV_SHADER(Pow);
DECLARE_FCV_SHADER(Remap);
DECLARE_FCV_SHADER(Round);
DECLARE_FCV_SHADER(Select);
DECLARE_FCV_SHADER(Sign);
DECLARE_FCV_SHADER(SmoothStep);
DECLARE_FCV_SHADER(Sqrt);
DECLARE_FCV_SHADER(Subtract);
DECLARE_FLOAT_SHADER(Trigonometry);
DECLARE_VECTOR_SHADER(Trigonometry);
DECLARE_SHADER(VectorLength);
DECLARE_SHADER(ShapeAttrBool);
DECLARE_SHADER(ShapeAttrInt);
DECLARE_SHADER(ShapeAttrFloat);
DECLARE_SHADER(ShapeAttrPoint2);
DECLARE_SHADER(ShapeAttrVector);
DECLARE_SHADER(ShapeAttrColor);
DECLARE_SHADER(ShapeAttrRGBA);
DECLARE_SHADER(ShapeAttrString);
#ifdef USE_AGSTATE
DECLARE_SHADER(agIntState);
DECLARE_SHADER(agFloatState);
DECLARE_SHADER(agColorState);
DECLARE_SHADER(agVectorState);
DECLARE_SHADER(agMatrixState);
#endif
DECLARE_SHADER(TransformVector);
DECLARE_SHADER(BuildMatrix);
DECLARE_SHADER(MultiplyMatrix);

node_loader
{
   if (i >= gsShaderCount)
   {
      return false;
   }
   REGISTER_FCV_SHADER(Abs, absolute)
   REGISTER_FCV_SHADER(Add, add)
   REGISTER_FCV_SHADER(Bias, bias)
   REGISTER_FCV_SHADER(BiasAndGain, bias_and_gain)
   REGISTER_FCV_SHADER(Ceil, ceil)
   REGISTER_FCV_SHADER(Clamp, clamp)
   REGISTER_FCV_SHADER(Compare, compare)
   REGISTER_SHADER(ConvertFloatToBool, convert_float_to_bool, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ConvertFloatToInt, convert_float_to_int, AI_TYPE_INT)
   REGISTER_SHADER(ConvertVectorToColor, convert_vector_to_color, AI_TYPE_RGB)
   REGISTER_SHADER(ConvertColorToVector, convert_color_to_vector, AI_TYPE_VECTOR)
   REGISTER_SHADER(ConvertColorToFloat, convert_color_to_float, AI_TYPE_FLOAT)
   REGISTER_SHADER(CrossProduct, cross_product, AI_TYPE_VECTOR)
   REGISTER_FCV_SHADER(Gain, gain)
   REGISTER_FCV_SHADER(Divide, divide)
   REGISTER_SHADER(DotProduct, dot_product, AI_TYPE_FLOAT)
   REGISTER_FCV_SHADER(Exp, exp)
   REGISTER_FCV_SHADER(Floor, floor)
   REGISTER_FCV_SHADER(Inverse, inverse)
   REGISTER_FCV_SHADER(Log, log)
   REGISTER_FCV_SHADER(Max, max)
   REGISTER_FCV_SHADER(Min, min)
   REGISTER_FCV_SHADER(Mix, mix)
   REGISTER_FCV_SHADER(Modulo, modulo)
   REGISTER_FCV_SHADER(MultiSelect, multi_select)
   REGISTER_FCV_SHADER(Multiply, multiply)
   REGISTER_FCV_SHADER(Negate, negate)
   REGISTER_SHADER(NormalizeVector, normalize_vector, AI_TYPE_VECTOR)
   REGISTER_FCV_SHADER(Pow, pow)
   REGISTER_FCV_SHADER(Remap, remap)
   REGISTER_FCV_SHADER(Round, round)
   REGISTER_FCV_SHADER(Select, select)
   REGISTER_FCV_SHADER(Sign, sign)
   REGISTER_FCV_SHADER(SmoothStep, smoothstep)
   REGISTER_FCV_SHADER(Sqrt, sqrt)
   REGISTER_FCV_SHADER(Subtract, subtract)
   REGISTER_FLOAT_SHADER(Trigonometry, trigonometry)
   REGISTER_VECTOR_SHADER(Trigonometry, trigonometry)
   REGISTER_SHADER(VectorLength, vector_length, AI_TYPE_FLOAT)
   REGISTER_SHADER(ShapeAttrBool, shape_attr_bool, AI_TYPE_BOOLEAN)
   REGISTER_SHADER(ShapeAttrInt, shape_attr_int, AI_TYPE_INT)
   REGISTER_SHADER(ShapeAttrFloat, shape_attr_float, AI_TYPE_FLOAT)
   REGISTER_SHADER(ShapeAttrPoint2, shape_attr_point2, AI_TYPE_POINT2)
   REGISTER_SHADER(ShapeAttrVector, shape_attr_vector, AI_TYPE_VECTOR)
   REGISTER_SHADER(ShapeAttrColor, shape_attr_color, AI_TYPE_RGB)
   REGISTER_SHADER(ShapeAttrRGBA, shape_attr_rgba, AI_TYPE_RGBA)
   REGISTER_SHADER(ShapeAttrString, shape_attr_string, AI_TYPE_STRING)
#ifdef USE_AGSTATE
   REGISTER_SHADER(agIntState, shader_globals_int, AI_TYPE_INT)
   REGISTER_SHADER(agFloatState, shader_globals_float, AI_TYPE_FLOAT)
   REGISTER_SHADER(agColorState, shader_globals_color, AI_TYPE_RGB)
   REGISTER_SHADER(agVectorState, shader_globals_vector, AI_TYPE_VECTOR)
   REGISTER_SHADER(agMatrixState, shader_globals_matrix, AI_TYPE_MATRIX)
#endif
   REGISTER_SHADER(TransformVector, transform_vector, AI_TYPE_VECTOR)
   REGISTER_SHADER(BuildMatrix, build_matrix, AI_TYPE_MATRIX)
   REGISTER_SHADER(MultiplyMatrix, multiply_matrix, AI_TYPE_MATRIX)
   return false;
}

