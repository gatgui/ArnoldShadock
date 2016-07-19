#ifndef __agShadingBlocks_common_h__
#define __agShadingBlocks_common_h__

#include <ai.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <gmath/color.h>
#include "strings.h"

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

extern const char* RampInterpolationNames[];

enum RampInterpolation
{
   RI_None = 0,
   RI_Linear,
   RI_Smooth,
   RI_Spline
};

enum TransformOrder
{
   TO_SRT = 0,
   TO_STR,
   TO_RST,
   TO_RTS,
   TO_TSR,
   TO_TRS
};

extern const char* TransformOrderNames[];

enum RotationOrder
{
   RO_XYZ = 0,
   RO_XZY,
   RO_YXZ,
   RO_YZX,
   RO_ZXY,
   RO_ZYX
};

extern const char* RotationOrderNames[];

enum AngleUnits
{
   AU_Degrees = 0,
   AU_Radians
};

extern const char* AngleUnitsNames[];

enum CombineMode
{
   CM_Add = 0,
   CM_Avg,
   CM_Max,
   CM_Min
};

extern const char* CombineModeNames[];

enum RayType
{
   RT_Camera = 0,
   RT_Shadow,
   RT_Reflected,
   RT_Refracted,
   RT_Diffuse,
   RT_Glossy,
   RT_Generic
};

extern const char* RayTypeNames[];
extern AtUInt16 RayTypeValues[];

enum GammaMode
{
   GM_Expand = 0,
   GM_Compress
};

extern const char* GammaModeNames[];

enum GammaFunction
{
   GF_Power22 = 0,
   GF_Power24,
   GF_sRGB,
   GF_Rec709,
   GF_LogC,
   GF_Cineon,
   GF_Rec2020
};

extern const char* GammaFunctionNames[];

enum LogCExposureLevel
{
   EL_160 = 0,
   EL_200,
   EL_250,
   EL_320,
   EL_400,
   EL_500,
   EL_640,
   EL_800,
   EL_1000,
   EL_1280,
   EL_1600
};

extern const char* LogCExposureLevelNames[];

enum ColorSpace
{
   CS_Rec709 = 0,
   CS_NTSC,
   CS_SMPTE,
   CS_CIE,
   CS_UHDTV,
   CS_DCIP3,
   CS_AdobeRGB,
   CS_AdobeWide,
   CS_AlexaWide
};

extern const char* ColorSpaceNames[];
extern const gmath::ColorSpace* ColorSpaces[];

// gmath::ChromaticAdaptationTransform
extern const char* ChromaticAdaptationTransformNames[];

enum LocalFrame
{
   LF_Nf_dPdu = 0,
   LF_Nf_dPdv,
   LF_polar,
   LF_shirley,
   LF_custom
};

extern const char* LocalFrameNames[];

enum MicrofacetDistribution
{
   MD_Beckmann = 0,
   MD_GGX
};

extern const char* MicrofacetDistributionNames[];

enum VolumeInterp
{
   VI_closest = 0,
   VI_linear,
   VI_cubic
};

extern const char* VolumeInterpNames[];

enum EvalOrder
{
   EO_input_first = 0,
   EO_input_last
};

extern const char* EvalOrderNames[];

// ---

struct BRDFData
{
   AtBRDFEvalSampleFunc evalSample;
   AtBRDFEvalBrdfFunc evalBrdf;
   AtBRDFEvalPdfFunc evalPdf;
   void *data;
};

// ---

struct HitData
{
   void *ptr;
   bool isSG;
};

// ---

template <typename T>
inline void AddMemUsage()
{
   AiAddMemUsage(AtInt64(sizeof(T)), "shading_blocks");
}

template <typename T>
inline void SubMemUsage()
{
   AiAddMemUsage(-AtInt64(sizeof(T)), "shading_blocks");
}

inline int GetRenderThreadsCount()
{
   return AiNodeGetInt(AiUniverseGetOptions(), SSTR::threads);
}

template <unsigned int DIM, typename T>
inline T MaxComponent(const T *val)
{
   T maxval = val[0];
   for (unsigned int i=1; i<DIM; ++i)
   {
      if (val[i] > maxval)
      {
         maxval = val[i];
      }
   }
   return maxval;
}

void SortRampPositions(AtArray *a, unsigned int *shuffle);

void FloatRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, float &out);
void VectorRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, AtVector &out);
void ColorRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, AtRGB &out);

inline float SmoothStep(float v)
{
   // 6t^5 - 15t^4 + 10t^3 (Perlin)
   return (v * v * v * (v * (v * 6.0f - 15.0f) + 10.0f));
}

inline AtRGB SmoothStep(const AtRGB &v)
{
   AtRGB rv;
   rv.r = SmoothStep(v.r);
   rv.g = SmoothStep(v.g);
   rv.b = SmoothStep(v.b);
   return rv;
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

// ---

struct UVData
{
   float u, v;
   float dudx, dudy;
   float dvdx, dvdy;
   AtVector dPdu, dPdv;

   UVData(AtShaderGlobals *sg);

   void store(AtShaderGlobals *sg);
   void restore(AtShaderGlobals *sg);
};

inline void ScaleUV(const AtPoint2 &pivot, const AtPoint2 &s, AtPoint2 &uv)
{
   uv -= pivot;
   uv.x *= s.x;
   uv.y *= s.y;
   uv += pivot;
}

inline void RotateUV(const AtPoint2 &pivot, float cosA, float sinA, AtPoint2 &uv)
{
   AtPoint2 tmp = uv - pivot;

   uv.x = pivot.x + cosA * tmp.x - sinA * tmp.y;
   uv.y = pivot.y + cosA * tmp.y + sinA * tmp.x;
}

inline void TranslateUV(const AtPoint2 &t, AtPoint2 &uv)
{
   uv += t;
}

void ComputeSurfaceScreenDerivatives(AtShaderGlobals *sg, AtVector &dPdx, AtVector &dPdy);

void ComputeSurfaceUVDerivatives(AtShaderGlobals *sg, const AtVector &dPdx, const AtVector &dPdy);

// ---

template <int dim>
class LinearSystem
{
public:

   LinearSystem(float eps=0.000001f)
      : EPS(eps)
   {
      for (int i=0; i<dim; ++i)
      {
         memset(matrix[i], 0, (dim+1)*sizeof(float));
      }
   }

   LinearSystem(const LinearSystem<dim> &rhs)
      : EPS(rhs.EPS)
   {
      for (int i=0; i<dim; ++i)
      {
         memcpy(matrix[i], rhs.matrix[i], (dim+1)*sizeof(float));
      }
   }

   ~LinearSystem()
   {
   }

   LinearSystem<dim>& operator=(const LinearSystem<dim> &rhs)
   {
      if (this != &rhs)
      {
         for (int i=0; i<dim; ++i)
         {
            memcpy(matrix[i], rhs.matrix[i], (dim+1)*sizeof(float));
         }
         EPS = rhs.EPS;
      }
      return *this;
   }

   float* operator[](int i)
   {
      return matrix[i];
   }

   const float* operator[](int i) const
   {
      return matrix[i];
   }

   // result size is dim
   bool solve(float result[dim])
   {
      int i, j, k;
      int ccol = 0;
      int crow = 0;
      int mrow = 0;
      float cval;
      float mval;

      for (i=0; i<dim; ++i)
      {
         memcpy(rmatrix[i], matrix[i], (dim+1)*sizeof(float));
      }

      // gauss-jordan reduction
      for (ccol=0; ccol<dim; ++ccol)
      {
         mrow = ccol;
         mval = cval = fabs(rmatrix[mrow][ccol]);

         // select pivot
         for (crow=ccol+1; crow<dim; ++crow)
         {
            cval = fabs(rmatrix[crow][ccol]);
            if (cval > mval)
            {
               mval = cval;
               mrow = crow;
            }
         }

         // check if pivot is valid
         if (mval >= EPS)
         {
            float factor = 1.0f / rmatrix[mrow][ccol];

            swap(ccol, mrow);

            for (crow=ccol; crow<dim; ++crow)
            {
               multiply(crow, factor);
            }

            for (crow=ccol+1; crow<dim; ++crow)
            {
               subtractMultiplied(crow, rmatrix[crow][ccol], ccol);
            }
         }
         else
         {
            // maximum value on this column is zero, cannot reduce
            // could swap column here maybe
         }
      }

      // backsubstitute

      memset(result, 0, dim*sizeof(float));

      j = dim-1;

      if (fabs(rmatrix[j][j]) >= EPS)
      {
         result[j] = rmatrix[j][dim];
      }

      while (--j >= 0)
      {
         result[j] = rmatrix[j][dim];
         for (k=dim-1; k>j; --k)
         {
            result[j] -= rmatrix[j][k] * result[k];
         }
      }

      return true;
   }

private:

   void swap(int i, int j)
   {
      float tmp[dim+1];
      memcpy(tmp, rmatrix[i], (dim+1)*sizeof(float));
      memcpy(rmatrix[i], rmatrix[j], (dim+1)*sizeof(float));
      memcpy(rmatrix[j], tmp, (dim+1)*sizeof(float));
   }

   void multiply(int i, float s)
   {
      for (int j=0; j<=dim; ++j)
      {
         rmatrix[i][j] *= s;
      }
   }

   void subtractMultiplied(int i, float s, int j)
   {
      for (int k=0; k<=dim; ++k)
      {
         rmatrix[i][k] -= s * rmatrix[j][k];
      }
   }

private:

   float matrix[dim][dim+1]; //rows : augmented
   float rmatrix[dim][dim+1]; // rmatrix is the matrix to be modified
   float EPS;
};

#endif
