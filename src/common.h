#ifndef __agUtils_common_h__
#define __agUtils_common_h__

#include <ai.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>

extern const char* CompareOperatorNames[];

enum CompareOperator
{
   CO_EQ = 0,
   CO_NEQ,
   CO_LT,
   CO_LTE,
   CO_GT,
   CO_GTE
};

// ---

inline float SmoothStep(float v)
{
   // 6t^5 - 15t^4 + 10t^3 (Perlin)
   return (v * v * v * (v * (v * 6.0f - 15.0f) + 10.0f));
}

inline AtRGB SmoothStep(const AtRGB &v)
{
   return AiColor(SmoothStep(v.r), SmoothStep(v.g), SmoothStep(v.b));
}

inline AtVector SmoothStep(const AtVector &v)
{
   AtVector rv;
   AiV3Create(rv, SmoothStep(v.x), SmoothStep(v.y), SmoothStep(v.z));
   return rv;
}

inline float NormalizeToRange(float in, float range_min, float range_max)
{
   return CLAMP((in - range_min) / (range_max - range_min), 0.0f, 1.0f);
}

inline AtVector NormalizeToRange(const AtVector &in, const AtVector &range_min, const AtVector &range_max)
{
   AtVector rv;
   
   rv.x = CLAMP((in.x - range_min.x) / (range_max.x - range_min.x), 0.0f, 1.0f);
   rv.y = CLAMP((in.y - range_min.y) / (range_max.y - range_min.y), 0.0f, 1.0f);
   rv.z = CLAMP((in.z - range_min.z) / (range_max.z - range_min.z), 0.0f, 1.0f);
   
   return rv;
}

inline AtVector NormalizeToRange(const AtVector &in, float range_min, float range_max)
{
   AtVector rv;
   float tmp = 1.0f / (range_max - range_min);
   
   rv.x = CLAMP((in.x - range_min) * tmp, 0.0f, 1.0f);
   rv.y = CLAMP((in.y - range_min) * tmp, 0.0f, 1.0f);
   rv.z = CLAMP((in.z - range_min) * tmp, 0.0f, 1.0f);
   
   return rv;
}

inline AtRGB NormalizeToRange(const AtRGB &in, const AtRGB &range_min, const AtRGB &range_max)
{
   AtRGB rv;
   
   rv.r = CLAMP((in.r - range_min.r) / (range_max.r - range_min.r), 0.0f, 1.0f);
   rv.g = CLAMP((in.g - range_min.g) / (range_max.g - range_min.g), 0.0f, 1.0f);
   rv.b = CLAMP((in.b - range_min.b) / (range_max.b - range_min.b), 0.0f, 1.0f);
   
   return rv;
}

inline AtRGB NormalizeToRange(const AtRGB &in, float range_min, float range_max)
{
   AtRGB rv;
   float tmp = 1.0f / (range_max - range_min);
   
   rv.r = CLAMP((in.r - range_min) * tmp, 0.0f, 1.0f);
   rv.g = CLAMP((in.g - range_min) * tmp, 0.0f, 1.0f);
   rv.b = CLAMP((in.b - range_min) * tmp, 0.0f, 1.0f);
   
   return rv;
}

inline float ExpandToRange(float in, float range_min, float range_max)
{
   return (range_min + in * (range_max - range_min));
}

inline AtVector ExpandToRange(const AtVector &in, float range_min, float range_max)
{
   AtVector rv;
   
   rv.x = range_min + in.x * (range_max - range_min);
   rv.y = range_min + in.y * (range_max - range_min);
   rv.z = range_min + in.z * (range_max - range_min);
   
   return rv;
}

inline AtVector ExpandToRange(const AtVector &in, const AtVector &range_min, const AtVector &range_max)
{
   AtVector rv;
   
   rv.x = range_min.x + in.x * (range_max.x - range_min.x);
   rv.y = range_min.y + in.y * (range_max.y - range_min.y);
   rv.z = range_min.z + in.z * (range_max.z - range_min.z);
   
   return rv;
}

inline AtRGB ExpandToRange(const AtRGB &in, float range_min, float range_max)
{
   AtRGB rv;
   
   rv.r = range_min + in.r * (range_max - range_min);
   rv.g = range_min + in.g * (range_max - range_min);
   rv.b = range_min + in.b * (range_max - range_min);
   
   return rv;
}

inline AtRGB ExpandToRange(const AtRGB &in, const AtRGB &range_min, const AtRGB &range_max)
{
   AtRGB rv;
   
   rv.r = range_min.r + in.r * (range_max.r - range_min.r);
   rv.g = range_min.g + in.g * (range_max.g - range_min.g);
   rv.b = range_min.b + in.b * (range_max.b - range_min.b);
   
   return rv;
}

#endif
