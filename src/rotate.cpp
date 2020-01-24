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

AI_SHADER_NODE_EXPORT_METHODS(RotateMtd);

enum RotateParams
{
   p_input = 0,
   p_rotation_order,
   p_angle_units,
   p_rotation_pivot,
   p_rotation
};

node_parameters
{
   AiParameterVec("input", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::rotation_order, RO_XYZ, RotationOrderNames);
   AiParameterEnum(SSTR::angle_units, AU_Degrees, AngleUnitsNames);
   AiParameterVec(SSTR::rotation, 0.0f, 0.0f, 0.0f);
   AiParameterVec(SSTR::rotation_pivot, 0.0f, 0.0f, 0.0f);
}

struct RotateData
{
   AtMatrix Rx;
   AtMatrix Ry;
   AtMatrix Rz;
   AtMatrix Rp;
   AtMatrix iRp;
   AtMatrix R;  // composed rotate matrix -> Rz * Ry * Rx for ZYX rotation order
   AtMatrix Rf; // final rotate matrix -> iRp * R * Rp
   
   bool Rx_set;
   bool Ry_set;
   bool Rz_set;
   bool Rp_set;
   
   float angleScale;
   
   RotationOrder rotationOrder;
};

node_initialize
{
   RotateData *data = new RotateData();
   AddMemUsage<RotateData>();

   data->Rx_set = false;
   data->Ry_set = false;
   data->Rz_set = false;
   data->Rp_set = false;
   data->angleScale = 1.0f;
   data->rotationOrder = RO_XYZ;

   AiNodeSetLocalData(node, data);
}

node_update
{
   RotateData *data = (RotateData*) AiNodeGetLocalData(node);
   
   data->rotationOrder = (RotationOrder) AiNodeGetInt(node, SSTR::rotation_order);
   data->angleScale = (AiNodeGetInt(node, SSTR::angle_units) == AU_Radians ? AI_RTOD : 1.0f);
   
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
}

node_finish
{
   RotateData *data = (RotateData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<RotateData>();
}

shader_evaluate
{
   RotateData *data = (RotateData*) AiNodeGetLocalData(node);
   
   AtVector p, ip, r;
   AtMatrix R, Rx, Ry, Rz, P, iP, tmp;
   bool computeR = true;
   
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
   
   AtVector v = AiShaderEvalParamVec(p_input);
   
   sg->out.VEC() = AiM4VectorByMatrixMult(R, v);
}
