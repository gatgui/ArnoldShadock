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
   AiParameterPnt(SSTR::rotation_pivot, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::scale, 1.0f, 1.0f, 1.0f);
   AiParameterPnt(SSTR::scale_pivot, 0.0f, 0.0f, 0.0f);
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
      AiM4Translation(data->T, &T);
   }

   if (!AiNodeIsLinked(node, SSTR::rotation))
   {
      AtVector R = AiNodeGetVec(node, SSTR::rotation);

      data->Rx_set = true;
      data->Ry_set = true;
      data->Rz_set = true;

      AiM4RotationX(data->Rx, data->angleScale * R.x);
      AiM4RotationY(data->Ry, data->angleScale * R.y);
      AiM4RotationZ(data->Rz, data->angleScale * R.z);
   }
   else
   {
      data->Rx_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_x))
      {
         data->Rx_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         AiM4RotationX(data->Rx, data->angleScale * R.x);
      }

      data->Ry_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_y))
      {
         data->Ry_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         AiM4RotationY(data->Ry, data->angleScale * R.y);
      }

      data->Rz_set = false;
      if (!AiNodeIsLinked(node, SSTR::rotation_dot_z))
      {
         data->Rz_set = true;
         AtVector R = AiNodeGetVec(node, SSTR::rotation);
         AiM4RotationZ(data->Rz, data->angleScale * R.z);
      }
   }

   data->Rp_set = false;
   if (!AiNodeIsLinked(node, SSTR::rotation_pivot))
   {
      data->Rp_set = true;
      AtPoint P = AiNodeGetPnt(node, SSTR::rotation_pivot);
      AiM4Translation(data->Rp, &P);
      P = -P;
      AiM4Translation(data->iRp, &P);
   }

   data->S_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale))
   {
      data->S_set = true;
      AtVector S = AiNodeGetVec(node, SSTR::scale);
      AiM4Scaling(data->S, &S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, SSTR::scale_pivot))
   {
      data->Sp_set = true;
      AtPoint P = AiNodeGetPnt(node, SSTR::scale_pivot);
      AiM4Translation(data->Sp, &P);
      P = -P;
      AiM4Translation(data->iSp, &P);
   }

   if (data->Rx_set && data->Ry_set && data->Rz_set)
   {
      AtMatrix tmp;

      switch (data->rotationOrder)
      {
      case RO_XYZ:
         AiM4Mult(tmp, data->Ry, data->Rx);
         AiM4Mult(data->R, data->Rz, tmp);
         break;
      case RO_XZY:
         AiM4Mult(tmp, data->Rz, data->Rx);
         AiM4Mult(data->R, data->Ry, tmp);
         break;
      case RO_YXZ:
         AiM4Mult(tmp, data->Rx, data->Ry);
         AiM4Mult(data->R, data->Rz, tmp);
         break;
      case RO_YZX:
         AiM4Mult(tmp, data->Rz, data->Ry);
         AiM4Mult(data->R, data->Rx, tmp);
         break;
      case RO_ZXY:
         AiM4Mult(tmp, data->Rx, data->Rz);
         AiM4Mult(data->R, data->Ry, tmp);
         break;
      case RO_ZYX:
      default:
         AiM4Mult(tmp, data->Ry, data->Rz);
         AiM4Mult(data->R, data->Rx, tmp);
      }

      if (data->Rp_set)
      {
         AiM4Mult(tmp, data->R, data->iRp);
         AiM4Mult(data->Rf, data->Rp, tmp);
      }
   }

   if (data->S_set && data->Sp_set)
   {
      AtMatrix tmp;
      AiM4Mult(tmp, data->S, data->iSp);
      AiM4Mult(data->Sf, data->Sp, tmp);
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
      AiM4Translation(T, &t);
   }
   else
   {
      AiM4Copy(T, data->T);
   }
   
   // Compute rotation matrix
   
   if (!data->Rz_set || !data->Ry_set || !data->Rz_set)
   {
      r = AiShaderEvalParamVec(p_rotation);
      
      if (!data->Rx_set)
      {
         AiM4RotationX(Rx, data->angleScale * r.x);
      }
      else
      {
         AiM4Copy(Rx, data->Rx);
      }
      
      if (!data->Ry_set)
      {
         AiM4RotationY(Ry, data->angleScale * r.y);
      }
      else
      {
         AiM4Copy(Ry, data->Ry);
      }
      
      if (!data->Rz_set)
      {
         AiM4RotationZ(Rz, data->angleScale * r.z);
      }
      else
      {
         AiM4Copy(Rz, data->Rz);
      }
      
      switch (data->rotationOrder)
      {
      case RO_XYZ:
         AiM4Mult(tmp, Ry, Rx);
         AiM4Mult(R, Rz, tmp);
         break;
      case RO_XZY:
         AiM4Mult(tmp, Rz, Rx);
         AiM4Mult(R, Ry, tmp);
         break;
      case RO_YXZ:
         AiM4Mult(tmp, Rx, Ry);
         AiM4Mult(R, Rz, tmp);
         break;
      case RO_YZX:
         AiM4Mult(tmp, Rz, Ry);
         AiM4Mult(R, Rx, tmp);
         break;
      case RO_ZXY:
         AiM4Mult(tmp, Rx, Rz);
         AiM4Mult(R, Ry, tmp);
         break;
      case RO_ZYX:
      default:
         AiM4Mult(tmp, Ry, Rz);
         AiM4Mult(R, Rx, tmp);
      }
   }
   else
   {
      if (data->Rp_set)
      {
         AiM4Copy(R, data->Rf);
         computeR = false;
      }
      else
      {
         AiM4Copy(R, data->R);
      }
   }
   
   if (!data->Rp_set)
   {
      p = AiShaderEvalParamVec(p_rotation_pivot);
      ip = -p;
      
      AiM4Translation(P, &p);
      AiM4Translation(iP, &ip);
      
      AiM4Mult(tmp, R, iP);
      AiM4Mult(R, P, tmp);
   }
   else if (computeR)
   {
      AiM4Mult(tmp, R, data->iRp);
      AiM4Mult(R, data->Rp, tmp);
   }
   
   // Compute scale matrix
   
   if (!data->S_set)
   {
      s = AiShaderEvalParamVec(p_scale);
      AiM4Scaling(S, &s);
   }
   else
   {
      if (data->Sp_set)
      {
         AiM4Copy(S, data->Sf);
         computeS = false;
      }
      else
      {
         AiM4Copy(S, data->S);
      }
   }
   
   if (!data->Sp_set)
   {
      p = AiShaderEvalParamVec(p_scale_pivot);
      ip = -p;
      
      AiM4Translation(P, &p);
      AiM4Translation(iP, &ip);
      
      AiM4Mult(tmp, S, iP);
      AiM4Mult(S, P, tmp);
   }
   else if (computeS)
   {
      AiM4Mult(tmp, S, data->iSp);
      AiM4Mult(S, data->Sp, tmp);
   }
   
   sg->out.pMTX = (AtMatrix*)AiShaderGlobalsQuickAlloc(sg, sizeof(AtMatrix));
   
   switch (data->transformOrder)
   {
   case TO_SRT:
      AiM4Mult(tmp, R, S);
      AiM4Mult(*(sg->out.pMTX), T, tmp);
      break;
   case TO_STR:
      AiM4Mult(tmp, T, S);
      AiM4Mult(*(sg->out.pMTX), R, tmp);
      break;
   case TO_RST:
      AiM4Mult(tmp, S, R);
      AiM4Mult(*(sg->out.pMTX), T, tmp);
      break;
   case TO_RTS:
      AiM4Mult(tmp, T, R);
      AiM4Mult(*(sg->out.pMTX), S, tmp);
      break;
   case TO_TSR:
      AiM4Mult(tmp, S, T);
      AiM4Mult(*(sg->out.pMTX), R, tmp);
      break;
   case TO_TRS:
   default:
      AiM4Mult(tmp, R, T);
      AiM4Mult(*(sg->out.pMTX), S, tmp);
   }
}
