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
DECLARE_SHADER(ConvertColorToFloat);
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
   REGISTER_SHADER(ConvertColorToFloat, convert_color_to_float, AI_TYPE_FLOAT)
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

