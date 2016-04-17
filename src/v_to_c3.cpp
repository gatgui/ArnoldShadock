#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(VToC3Mtd);

enum VToC3Params
{
   p_input = 0,
   p_mode
};

enum VectorToColorMode
{
   VTC_RAW = 0,
   VTC_HSV,
   VTC_HSL
};

static const char* VectorToColorModeNames[] =
{
   "raw",
   "hsv",
   "hsl",
   NULL
};


node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::mode, VTC_RAW, VectorToColorModeNames);
   
   AiMetaDataSetBool(mds, SSTR::mode, SSTR::linkable, false);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtVector input = AiShaderEvalParamVec(p_input);
   
   VectorToColorMode mode = (VectorToColorMode) AiShaderEvalParamInt(p_mode);
   
   switch (mode)
   {
   case VTC_HSV:
      {
         float C = input.z * input.y;
         float h = input.x * 6.0f;
         float X = C * (1.0f - fabsf(fmodf(h, 2.0f) - 1.0f));
         
         if (h < 1.0f)
         {
            sg->out.RGB.r = C;
            sg->out.RGB.g = X;
            sg->out.RGB.b = 0.0f;
         }
         else if (h < 2.0f)
         {
            sg->out.RGB.r = X;
            sg->out.RGB.g = C;
            sg->out.RGB.b = 0.0f;
         }
         else if (h < 3.0f)
         {
            sg->out.RGB.r = 0.0f;
            sg->out.RGB.g = C;
            sg->out.RGB.b = X;
         }
         else if (h < 4.0f)
         {
            sg->out.RGB.r = 0.0f;
            sg->out.RGB.g = X;
            sg->out.RGB.b = C;
         }
         else if (h < 5.0f)
         {
            sg->out.RGB.r = X;
            sg->out.RGB.g = 0.0f;
            sg->out.RGB.b = C;
         }
         else
         {
            sg->out.RGB.r = C;
            sg->out.RGB.g = 0.0f;
            sg->out.RGB.b = X;
         }
         
         float m = input.z - C;
         
         sg->out.RGB.r += m;
         sg->out.RGB.g += m;
         sg->out.RGB.b += m;
      }
      break;
   case VTC_HSL:
      {
         float C;
         
         if (input.z <= 0.5f)
         {
            C = 2.0f * input.z * input.y;
         }
         else
         {
            C = (2.0f - 2.0f * input.z) * input.y;
         }
         
         float h = input.x * 6.0f;
         float X = C * (1.0f - fabsf(fmodf(h, 2.0f) - 1));
         
         if (h < 1.0f)
         {
            sg->out.RGB.r = C;
            sg->out.RGB.g = X;
            sg->out.RGB.b = 0.0f;
         }
         else if (h < 2.0f)
         {
            sg->out.RGB.r = X;
            sg->out.RGB.g = C;
            sg->out.RGB.b = 0.0f;
         }
         else if (h < 3.0f)
         {
            sg->out.RGB.r = 0.0f;
            sg->out.RGB.g = C;
            sg->out.RGB.b = X;
         }
         else if (h < 4.0f)
         {
            sg->out.RGB.r = 0.0f;
            sg->out.RGB.g = X;
            sg->out.RGB.b = C;
         }
         else if (h < 5.0f)
         {
            sg->out.RGB.r = X;
            sg->out.RGB.g = 0.0f;
            sg->out.RGB.b = C;
         }
         else
         {
            sg->out.RGB.r = C;
            sg->out.RGB.g = 0.0f;
            sg->out.RGB.b = X;
         }
         
         float m = input.z - 0.5f * C;
         
         sg->out.RGB.r += m;
         sg->out.RGB.g += m;
         sg->out.RGB.b += m;
      }
      break;
   case VTC_RAW:
   default:
      sg->out.RGB.r = input.x;
      sg->out.RGB.g = input.y;
      sg->out.RGB.b = input.z;
   }
}
