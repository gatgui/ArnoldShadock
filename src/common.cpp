#include "common.h"

const char* CompareOperatorNames[] =
{
   "equal",
   "not_equal",
   "less_than",
   "less_than_equal",
   "greater_than",
   "greater_than_equal",
   NULL
};

const char* RampInterpolationNames[] =
{
   "None",
   "Linear",
   "Smooth",
   "Spline",
   NULL
};

static void GetArrayElement(AtArray *a, unsigned int i, float &e)
{
   e = AiArrayGetFlt(a, i);
}

static void GetArrayElement(AtArray *a, unsigned int i, AtVector &e)
{
   e = AiArrayGetVec(a, i);
}

static void GetArrayElement(AtArray *a, unsigned int i, AtRGB &e)
{
   e = AiArrayGetRGB(a, i);
}

template <typename T>
static void RampT(AtArray *p, AtArray *v, AtArray *it, RampInterpolation defi, unsigned int *shuffle, float t, T &result)
{
   unsigned int inext = p->nelements;

   for (unsigned int i = 0; (i < p->nelements); ++i)
   {
      if (t < AiArrayGetFlt(p, shuffle[i]))
      {
         inext = i;
         break;
      }
   }

   if (inext >= p->nelements)
   {
      GetArrayElement(v, shuffle[p->nelements - 1], result);
      return;
   }

   int iprev = inext - 1;
   
   if (iprev < 0)
   {
      GetArrayElement(v, shuffle[0], result);
      return;
   }

   float p1 = AiArrayGetFlt(p, shuffle[iprev]);
   float p2 = AiArrayGetFlt(p, shuffle[inext]);

   T v1, v2;
   GetArrayElement(v, shuffle[iprev], v1);
   GetArrayElement(v, shuffle[inext], v2);

   float dp = p2 - p1;
   float u = (t - p1) / dp;

   int itype = defi;
   if (it)
   {
      itype = AiArrayGetInt(it, shuffle[iprev]);
      if (itype < RI_None || itype > RI_Spline)
      {
         itype = defi;
      }
   }

   switch (itype)
   {
   case RI_None:
      result = v1;
      break;

   case RI_Linear:
      result = v1 + u * (v2 - v1);
      break;

   case RI_Smooth:
      {
         float u2 = u * u;
         float u3 = u * u2;
         float a =  2.0f * u3 - 3.0f * u2 + 1.0f;
         float b = -2.0f * u3 + 3.0f * u2 ;
         result = a * v1 + b * v2;
      }
      break;

   case RI_Spline:
      {
         T t1, t2;

         float u2 = u * u;
         float u3 = u * u2;

         float a =  2.0f * u3 - 3.0f * u2 + 1.0f; // p1
         float b = -2.0f * u3 + 3.0f * u2; // p2
         float c = u3 - 2.0f * u2 + u; // t1
         float d = u3 - u2; // t2
         
         // Catmull-rom type tangents:
         float p0, p3, sx;
         T v0, v3, sy;

         // Compute start tangent
         if (iprev == 0)
         {
            p0 = p1 - dp;
            v0 = v1;
         }
         else
         {
            p0 = AiArrayGetFlt(p, shuffle[iprev-1]);
            GetArrayElement(v, shuffle[iprev-1], v0);
         }
         sx = p2 - p0;
         if (sx < AI_EPSILON)
         {
            sx = AI_EPSILON;
         }
         sy = v2 - v0;
         t1 = sy / sx;
         
         // Compute end tangent
         if (inext+1 >= p->nelements)
         {
            p3 = p2 + dp;
            v3 = v2;
         }
         else
         {
            p3 = AiArrayGetFlt(p, shuffle[inext+1]);
            GetArrayElement(v, shuffle[inext+1], v3);
         }
         sx = p3 - p1;
         if (sx < AI_EPSILON)
         {
            sx = AI_EPSILON;
         }
         sy = v3 - v1;
         t2 = sy / sx;
         
         t1 *= dp;
         t2 *= dp;

         result = (a * v1 + b * v2 + c * t1 + d * t2);
      }
      break;

   default:
      result = v1;
   }
}

bool SortRampPositions(AtArray *a, unsigned int *shuffle)
{
   bool modified = false;

   if (a && shuffle && a->nelements > 0)
   {
      float p0, p1;
      int tmp;

      bool swapped = true;
      unsigned int n = a->nelements;

      for (unsigned int i=0; i<n; ++i)
      {
         shuffle[i] = i;
      }

      while (swapped)
      {
         swapped = false;
         n -= 1;
         for (unsigned int i=0; i<n; ++i)
         {
            p0 = AiArrayGetFlt(a, shuffle[i]);
            p1 = AiArrayGetFlt(a, shuffle[i + 1]);
            if (p0 > p1)
            {
               swapped = true;
               modified = true;

               tmp = shuffle[i];
               shuffle[i] = shuffle[i + 1];
               shuffle[i + 1] = tmp;
            }
         }
      }
   }

   return modified;
}

void FloatRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, float &out)
{
   RampT(p, v, i, defi, s, t, out);
}

void VectorRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, AtVector &out)
{
   RampT(p, v, i, defi, s, t, out);
}

void ColorRamp(AtArray *p, AtArray *v, AtArray *i, RampInterpolation defi, unsigned int *s, float t, AtRGB &out)
{
   RampT(p, v, i, defi, s, t, out);
}

// ---

UVData::UVData(AtShaderGlobals *sg)
{
   store(sg);
}

void UVData::store(AtShaderGlobals *sg)
{
   u = sg->u;
   v = sg->v;
   dudx = sg->dudx;
   dudy = sg->dudy;
   dvdx = sg->dvdx;
   dvdy = sg->dvdy;
   dPdu = sg->dPdu;
   dPdv = sg->dPdv;
}

void UVData::restore(AtShaderGlobals *sg)
{
   sg->u = u;
   sg->v = v;
   sg->dudx = dudx;
   sg->dudy = dudy;
   sg->dvdx = dvdx;
   sg->dvdy = dvdy;
   sg->dPdu = dPdu;
   sg->dPdv = dPdv;
}

void ComputeSurfaceScreenDerivatives(AtShaderGlobals *sg, AtVector &dPdx, AtVector &dPdy)
{
   dPdx = sg->dudx * sg->dPdu + sg->dvdx * sg->dPdv;
   dPdy = sg->dudy * sg->dPdu + sg->dvdy * sg->dPdv;
}

void ComputeSurfaceUVDerivatives(AtShaderGlobals *sg, const AtVector &dPdx, const AtVector &dPdy)
{
   // (1) dPdx = dudx * dPdu + dvdx * dPdv
   // (2) dPdy = dudy * dPdu + dvdy * dPdv

   // Note 1: sg's dPdu and dPdv are left unchanged when the system cannot be solved
   // Note 2: Could scale all equations to gain precision
   
   float rv[6];
   LinearSystem<6> lsys;

   lsys[0][0] = sg->dudx; lsys[0][3] = sg->dvdx; lsys[0][6] = dPdx.x;
   lsys[1][1] = sg->dudx; lsys[1][4] = sg->dvdx; lsys[1][6] = dPdx.y;
   lsys[2][2] = sg->dudx; lsys[2][5] = sg->dvdx; lsys[2][6] = dPdx.z;
   lsys[3][0] = sg->dudy; lsys[3][3] = sg->dvdy; lsys[3][6] = dPdy.x;
   lsys[4][1] = sg->dudy; lsys[4][4] = sg->dvdy; lsys[4][6] = dPdy.y;
   lsys[5][2] = sg->dudy; lsys[5][5] = sg->dvdy; lsys[5][6] = dPdy.z;

   if (lsys.solve(rv))
   {
      sg->dPdu.x = rv[0];
      sg->dPdu.y = rv[1];
      sg->dPdu.z = rv[2];
      sg->dPdv.x = rv[3];
      sg->dPdv.y = rv[4];
      sg->dPdv.z = rv[5];
   }
   else
   {
      AiMsgWarning("Could no re-compute dPdu and dPdv");
   }
}

