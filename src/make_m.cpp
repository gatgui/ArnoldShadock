/*
MIT License
Copyright (c) 2014~ Gaetan Guidet
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeMMtd);

enum MakeMParams
{
   p_transform_order = 0,
   p_translation,
   p_rotation_order,
   p_angle_units,
   p_rotation_pivot,
   p_rotation,
   p_scale_pivot,
   p_scale
};

node_parameters
{
   AiParameterEnum(SSTR::transform_order, TO_SRT, TransformOrderNames);
   AiParameterVec(SSTR::translation, 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::rotation_order, RO_XYZ, RotationOrderNames);
   AiParameterEnum(SSTR::angle_units, AU_Degrees, AngleUnitsNames);
   AiParameterVec(SSTR::rotation, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::rotation_pivot, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::scale, 1.0f, 1.0f, 1.0f);
   AiParameterVec(SSTR::scale_pivot, 0.0f, 0.0f, 0.0f);
}

struct MakeMData
{
   AtMatrix T;
   AtMatrix Rx;
   AtMatrix Ry;
   AtMatrix Rz;
   AtMatrix Rp;
   AtMatrix iRp;
   AtMatrix R;  // composed rotate matrix -> Rz * Ry * Rx for ZYX rotation order
   AtMatrix Rf; // final rotate matrix -> iRp * R * Rp
   AtMatrix S;
   AtMatrix Sp;
   AtMatrix iSp;
   AtMatrix Sf; // final scale matrix -> iSp * S * Sp

   bool T_set;
   bool Rx_set;
   bool Ry_set;
   bool Rz_set;
   bool Rp_set;
   bool S_set;
   bool Sp_set;
   
   float angleScale;
   
   TransformOrder transformOrder;
   RotationOrder rotationOrder;
};

node_initialize
{
   MakeMData *data = new MakeMData();

   data->T_set = false;
   data->Rx_set = false;
   data->Ry_set = false;
   data->Rz_set = false;
   data->Rp_set = false;
   data->S_set = false;
   data->Sp_set = false;
   data->angleScale = 1.0f;
   data->transformOrder = TO_SRT;
   data->rotationOrder = RO_XYZ;

   AiNodeSetLocalData(node, data);
   
   AddMemUsage<MakeMData>();
}

node_update
{
   MakeMData *data = (MakeMData*) AiNodeGetLocalData(node);
   
   data->transformOrder = (TransformOrder) AiNodeGetInt(node, SSTR::transform_order);
   data->rotationOrder = (RotationOrder) AiNodeGetInt(node, SSTR::rotation_order);
   data->angleScale = (AiNodeGetInt(node, SSTR::angle_units) == AU_Radians ? AI_RTOD : 1.0f);
   
   data->T_set = false;
   if (!AiNodeIsLinked(node, SSTR::translation))
   {
      data->T_set = true;
      AtVector T = AiNodeGetVec(node, SSTR::translation);
      data->T = AiM4Translation(T);
   }

   if (!AiNodeIsLinked(node, SSTR::rotation))
   {
      AtVector R = AiNodeGetVec(node, SSTR::rotation);

      data->Rx_set = true;
      data->Ry_set = true;
      data->Rz_set = true;

      data->Rx = AiM4RotationX(data->angleScale * R.x);
      data->Ry = AiM4RotationY(data->angleScale * R.y);
      data->Rz = AiM4RotationZ(data->angleScale * R.z);
   }
   else
   {
      data->Rx_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_x))
      {
         data->Rx_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         data->Rx = AiM4RotationX(data->angleScale * R.x);
      }

      data->Ry_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_y))
      {
         data->Ry_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         data->Ry = AiM4RotationY(data->angleScale * R.y);
      }

      data->Rz_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_z))
      {
         data->Rz_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         data->Rz = AiM4RotationZ(data->angleScale * R.z);
      }
   }

   data->Rp_set = false;
   if (!AiNodeIsLinked(node, SSTR::rotation_pivot))
   {
      data->Rp_set = true;
      AtVector P = AiNodeGetVec(node, SSTR::rotation_pivot);
      data->Rp = AiM4Translation(P);
      P = -P;
      data->iRp = AiM4Translation(P);
   }

   data->S_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale))
   {
      data->S_set = true;
      AtVector S = AiNodeGetVec(node, SSTR::scale);
      data->S = AiM4Scaling(S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale_pivot))
   {
      data->Sp_set = true;
      AtVector P = AiNodeGetVec(node, SSTR::scale_pivot);
      data->Sp = AiM4Translation(P);
      P = -P;
      data->iSp = AiM4Translation(P);
   }

   if (data->Rx_set && data->Ry_set && data->Rz_set)
   {
      AtMatrix tmp;

      switch (data->rotationOrder)
      {
      case RO_XYZ:
         tmp = AiM4Mult(data->Ry, data->Rx);
         data->R = AiM4Mult(data->Rz, tmp);
         break;
      case RO_XZY:
         tmp = AiM4Mult(data->Rz, data->Rx);
         data->R = AiM4Mult(data->Ry, tmp);
         break;
      case RO_YXZ:
         tmp = AiM4Mult(data->Rx, data->Ry);
         data->R = AiM4Mult(data->Rz, tmp);
         break;
      case RO_YZX:
         tmp = AiM4Mult(data->Rz, data->Ry);
         data->R = AiM4Mult(data->Rx, tmp);
         break;
      case RO_ZXY:
         tmp = AiM4Mult(data->Rx, data->Rz);
         data->R = AiM4Mult(data->Ry, tmp);
         break;
      case RO_ZYX:
      default:
         tmp = AiM4Mult(data->Ry, data->Rz);
         data->R = AiM4Mult(data->Rx, tmp);
      }

      if (data->Rp_set)
      {
         tmp = AiM4Mult(data->R, data->iRp);
         data->Rf = AiM4Mult(data->Rp, tmp);
      }
   }

   if (data->S_set && data->Sp_set)
   {
      AtMatrix tmp;
      tmp = AiM4Mult(data->S, data->iSp);
      data->Sf = AiM4Mult(data->Sp, tmp);
   }
}

node_finish
{
   MakeMData *data = (MakeMData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<MakeMData>();
}

shader_evaluate
{
   MakeMData *data = (MakeMData*) AiNodeGetLocalData(node);
   
   AtVector p, ip, r, t, s;
   AtMatrix T, R, Rx, Ry, Rz, S, P, iP, tmp;
   bool computeR = true;
   bool computeS = true;
   
   // Compute translation matrix
   
   if (!data->T_set)
   {
      t = AiShaderEvalParamVec(p_translation);
      T = AiM4Translation(t);
   }
   else
   {
      T = data->T;
   }
   
   // Compute rotation matrix
   
   if (!data->Rz_set || !data->Ry_set || !data->Rz_set)
   {
      r = AiShaderEvalParamVec(p_rotation);
      
      if (!data->Rx_set)
      {
         Rx = AiM4RotationX(data->angleScale * r.x);
      }
      else
      {
         Rx = data->Rx;
      }
      
      if (!data->Ry_set)
      {
         Ry = AiM4RotationY(data->angleScale * r.y);
      }
      else
      {
         Ry = data->Ry;
      }
      
      if (!data->Rz_set)
      {
         Rz = AiM4RotationZ(data->angleScale * r.z);
      }
      else
      {
         Rz = data->Rz;
      }
      
      switch (data->rotationOrder)
      {
      case RO_XYZ:
         tmp = AiM4Mult(Ry, Rx);
         R = AiM4Mult(Rz, tmp);
         break;
      case RO_XZY:
         tmp = AiM4Mult(Rz, Rx);
         R = AiM4Mult(Ry, tmp);
         break;
      case RO_YXZ:
         tmp = AiM4Mult(Rx, Ry);
         R = AiM4Mult(Rz, tmp);
         break;
      case RO_YZX:
         tmp = AiM4Mult(Rz, Ry);
         R = AiM4Mult(Rx, tmp);
         break;
      case RO_ZXY:
         tmp = AiM4Mult(Rx, Rz);
         R = AiM4Mult(Ry, tmp);
         break;
      case RO_ZYX:
      default:
         tmp = AiM4Mult(Ry, Rz);
         R = AiM4Mult(Rx, tmp);
      }
   }
   else
   {
      if (data->Rp_set)
      {
         R = data->Rf;
         computeR = false;
      }
      else
      {
         R = data->R;
      }
   }
   
   if (!data->Rp_set)
   {
      p = AiShaderEvalParamVec(p_rotation_pivot);
      ip = -p;
      
      P = AiM4Translation(p);
      iP = AiM4Translation(ip);
      
      tmp = AiM4Mult(R, iP);
      R = AiM4Mult(P, tmp);
   }
   else if (computeR)
   {
      tmp = AiM4Mult(R, data->iRp);
      R = AiM4Mult(data->Rp, tmp);
   }
   
   // Compute scale matrix
   
   if (!data->S_set)
   {
      s = AiShaderEvalParamVec(p_scale);
      S = AiM4Scaling(s);
   }
   else
   {
      if (data->Sp_set)
      {
         S = data->Sf;
         computeS = false;
      }
      else
      {
         S = data->S;
      }
   }
   
   if (!data->Sp_set)
   {
      p = AiShaderEvalParamVec(p_scale_pivot);
      ip = -p;
      
      P = AiM4Translation(p);
      iP = AiM4Translation(ip);
      
      tmp = AiM4Mult(S, iP);
      S = AiM4Mult(P, tmp);
   }
   else if (computeS)
   {
      tmp = AiM4Mult(S, data->iSp);
      S = AiM4Mult(data->Sp, tmp);
   }
   
   sg->out.pMTX() = (AtMatrix*)AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   switch (data->transformOrder)
   {
   case TO_SRT:
      tmp = AiM4Mult(R, S);
      *(sg->out.pMTX()) = AiM4Mult(T, tmp);
      break;
   case TO_STR:
      tmp = AiM4Mult(T, S);
      *(sg->out.pMTX()) = AiM4Mult(R, tmp);
      break;
   case TO_RST:
      tmp = AiM4Mult(S, R);
      *(sg->out.pMTX()) = AiM4Mult(T, tmp);
      break;
   case TO_RTS:
      tmp = AiM4Mult(T, R);
      *(sg->out.pMTX()) = AiM4Mult(S, tmp);
      break;
   case TO_TSR:
      tmp = AiM4Mult(S, T);
      *(sg->out.pMTX()) = AiM4Mult(R, tmp);
      break;
   case TO_TRS:
   default:
      tmp = AiM4Mult(R, T);
      *(sg->out.pMTX()) = AiM4Mult(S, tmp);
   }
}
