#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(C3ToVMtd);

enum C3ToVParams
{
   p_input = 0,
   p_mode
};

enum ColorToVectorMode
{
   CTV_RAW = 0,
   CTV_HSV,
   CTV_HSL
};

static const char* ColorToVectorModeNames[] =
{
   "raw",
   "hsv",
   "hsl",
   NULL
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::mode, CTV_RAW, ColorToVectorModeNames);
}

node_initialize
{
   AiNodeSetLocalData(node, AiMalloc(sizeof(int)));
   AddMemUsage<int>();
}

node_update
{
   int *data = (int*) AiNodeGetLocalData(node);
   *data = AiNodeGetInt(node, SSTR::mode);
}

node_finish
{
   AiFree(AiNodeGetLocalData(node));
   SubMemUsage<int>();
}

shader_evaluate
{
   static float sNormalizeHue = 60.0f / 360.0f;
   
   AtRGB input = AiShaderEvalParamRGB(p_input);
   ColorToVectorMode mode = (ColorToVectorMode) *((int*) AiNodeGetLocalData(node));
   
   switch (mode)
   {
   case CTV_HSV:
      {
         float M = MAX3(input.r, input.g, input.b);
         float m = MIN3(input.r, input.g, input.b);
         float C = M - m;
         
         float hue = 0.0f;
         float sat = 0.0f;
         float val = 0.0f;
         
         val = M;
         
         if (M >= AI_EPSILON)
         {
            sat = C / M;
         }
         
         if (sat >= AI_EPSILON)
         {
            if (M == input.r)
            {
               hue = (input.g - input.b) / C;
            }
            else if (M == input.g)
            {
               hue = (input.b - input.r) / C + 2.0f;
            }
            else
            {
               hue = (input.r - input.g) / C + 4.0f;
            }
            
            hue *= sNormalizeHue;
            
            if (hue < 0.0f)
            {
               hue += 1.0f;
            }
         }
         
         sg->out.VEC.x = hue;
         sg->out.VEC.y = sat;
         sg->out.VEC.z = val;
      }
      break;
   case CTV_HSL:
      {
         float M = MAX3(input.r, input.g, input.b);
         float m = MIN3(input.r, input.g, input.b);
         float C = M - m;
         
         float hue = 0.0f;
         float sat = 0.0f;
         float lum = 0.0f;
         
         lum = 0.5f * (m + M);
         
         if (C >= AI_EPSILON)
         {
            if (lum <= 0.5f)
            {
               sat = C / (2.0f * lum);
            }
            else
            {
               sat = C / (2.0f * (1.0f - lum));
            }
            
            if (M == input.r)
            {
               hue = (input.g - input.b) / C;
            }
            else if (M == input.g)
            {
               hue = (input.b - input.r) / C + 2.0f;
            }
            else
            {
               hue = (input.r - input.g) / C + 4.0f;
            }
            
            hue *= sNormalizeHue;
            
            if (hue < 0.0f)
            {
               hue += 1.0f;
            }
         }
         
         sg->out.VEC.x = hue;
         sg->out.VEC.y = sat;
         sg->out.VEC.z = lum;
      }
      break;
   case CTV_RAW:
   default:
      sg->out.VEC.x = input.r;
      sg->out.VEC.y = input.g;
      sg->out.VEC.z = input.b;
   }
}
