#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(BuildMatrixMtd);

enum BuildMatrixParams
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

enum TransformOrder
{
   TO_SRT = 0,
   TO_STR,
   TO_RST,
   TO_RTS,
   TO_TSR,
   TO_TRS
};

static const char* TransformOrderNames[] =
{
   "SRT",
   "STR",
   "RST",
   "RTS",
   "TSR",
   "TRS",
   NULL
};

enum RotationOrder
{
   RO_XYZ = 0,
   RO_XZY,
   RO_YXZ,
   RO_YZX,
   RO_ZXY,
   RO_ZYX
};

static const char* RotationOrderNames[] =
{
   "XYZ",
   "XZY",
   "YXZ",
   "YZX",
   "ZXY",
   "ZYX",
   NULL
};

enum AngleUnits
{
   AU_Degrees = 0,
   AU_Radians
};

static const char* AngleUnitsNames[] =
{
   "degrees",
   "radians",
   NULL
};

node_parameters
{
   AiParameterEnum("transform_order", TO_SRT, TransformOrderNames);
   AiParameterVec("translation", 0.0f, 0.0f, 0.0f);
   AiParameterEnum("rotation_order", RO_XYZ, RotationOrderNames);
   AiParameterEnum("angle_units", AU_Degrees, AngleUnitsNames);
   AiParameterVec("rotation", 0.0f, 0.0f, 0.0f);
   AiParameterPnt("rotation_pivot", 0.0f, 0.0f, 0.0f);
   AiParameterVec("scale", 1.0f, 1.0f, 1.0f);
   AiParameterPnt("scale_pivot", 0.0f, 0.0f, 0.0f);
   
   AiMetaDataSetBool(mds, "transform_order", "linkable", false);
   AiMetaDataSetBool(mds, "rotation_order", "linkable", false);
   AiMetaDataSetBool(mds, "angle_units", "linkable", false);
}

struct BuildMatrixData
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
   BuildMatrixData *data = (BuildMatrixData*) AiMalloc(sizeof(BuildMatrixData));

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
}

node_update
{
   BuildMatrixData *data = (BuildMatrixData*) AiNodeGetLocalData(node);
   
   data->transformOrder = (TransformOrder) AiNodeGetInt(node, "transform_order");
   data->rotationOrder = (RotationOrder) AiNodeGetInt(node, "rotation_order");
   data->angleScale = (AiNodeGetInt(node, "angle_units") == AU_Radians ? AI_RTOD : 1.0f);
   
   data->T_set = false;
   if (!AiNodeIsLinked(node, "translation"))
   {
      data->T_set = true;
      AtVector T = AiNodeGetVec(node, "translation");
      AiM4Translation(data->T, &T);
   }

   if (!AiNodeIsLinked(node, "rotation"))
   {
      AtVector R = AiNodeGetVec(node, "rotation");

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
      if (!AiNodeIsLinked(node, "rotation.x"))
      {
         data->Rx_set = true;
         AtVector R = AiNodeGetVec(node, "rotation");
         AiM4RotationX(data->Rx, data->angleScale * R.x);
      }

      data->Ry_set = false;
      if (!AiNodeIsLinked(node, "rotation.y"))
      {
         data->Ry_set = true;
         AtVector R = AiNodeGetVec(node, "rotation");
         AiM4RotationY(data->Ry, data->angleScale * R.y);
      }

      data->Rz_set = false;
      if (!AiNodeIsLinked(node, "rotation.z"))
      {
         data->Rz_set = true;
         AtVector R = AiNodeGetVec(node, "rotation");
         AiM4RotationZ(data->Rz, data->angleScale * R.z);
      }
   }

   data->Rp_set = false;
   if (!AiNodeIsLinked(node, "rotation_pivot"))
   {
      data->Rp_set = true;
      AtPoint P = AiNodeGetPnt(node, "rotation_pivot");
      AiM4Translation(data->Rp, &P);
      P = -P;
      AiM4Translation(data->iRp, &P);
   }

   data->S_set = false;
   if (!AiNodeIsLinked(node, "scale"))
   {
      data->S_set = true;
      AtVector S = AiNodeGetVec(node, "scale");
      AiM4Scaling(data->S, &S);
   }

   data->Sp_set = false;
   if (!AiNodeIsLinked(node, "scale_pivot"))
   {
      data->Sp_set = true;
      AtPoint P = AiNodeGetPnt(node, "scale_pivot");
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
   BuildMatrixData *data = (BuildMatrixData*) AiNodeGetLocalData(node);
   AiFree(data);
}

shader_evaluate
{
   BuildMatrixData *data = (BuildMatrixData*) AiNodeGetLocalData(node);
   
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
