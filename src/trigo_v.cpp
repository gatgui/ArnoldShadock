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

AI_SHADER_NODE_EXPORT_METHODS(TrigoVMtd);

enum TrigoVParams
{
   p_input1 = 0,
   p_input2,
   p_operation,
   p_units
};

enum TrigonometryOperation
{
   TO_ACOS = 0,
   TO_ASIN,
   TO_ATAN,
   TO_ATAN2,
   TO_COS,
   TO_SIN,
   TO_TAN
};

extern const char* TrigonometryOperationNames[];

node_parameters
{
   AiParameterVec("input1", 0.0f, 0.0f, 0.0f);
   AiParameterVec("input2", 0.0f, 0.0f, 0.0f);
   AiParameterEnum(SSTR::operation, TO_COS, TrigonometryOperationNames);
   AiParameterEnum(SSTR::units, AU_Radians, AngleUnitsNames);
}

struct TrigoVData
{
   TrigonometryOperation operation;
   AngleUnits units;
};

node_initialize
{
   AiNodeSetLocalData(node, new TrigoVData());
   AddMemUsage<TrigoVData>();
}

node_update
{
   TrigoVData *data = (TrigoVData*) AiNodeGetLocalData(node);
   data->operation = (TrigonometryOperation) AiNodeGetInt(node, SSTR::operation);
   data->units = (AngleUnits) AiNodeGetInt(node, SSTR::units);
}

node_finish
{
   TrigoVData *data = (TrigoVData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<TrigoVData>();
}

shader_evaluate
{
   static float sDegToRad = AI_PI / 180.0f;
   static float sRadToDeg = 180.0f / AI_PI;
   
   TrigoVData *data = (TrigoVData*) AiNodeGetLocalData(node);
   
   AtVector input = AiShaderEvalParamVec(p_input1);
   
   switch (data->operation)
   {
   case TO_COS:
      {
         float f = (data->units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = cosf(f * input.x);
         sg->out.VEC.y = cosf(f * input.y);
         sg->out.VEC.z = cosf(f * input.z);
      }
      break;
   case TO_SIN:
      {
         float f = (data->units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = sinf(f * input.x);
         sg->out.VEC.y = sinf(f * input.y);
         sg->out.VEC.z = sinf(f * input.z);
      }
      break;
   case TO_TAN:
      {
         float f = (data->units == AU_Degrees ? sDegToRad : 1.0f);
         sg->out.VEC.x = tanf(f * input.x);
         sg->out.VEC.y = tanf(f * input.y);
         sg->out.VEC.z = tanf(f * input.z);
      }
      break;
   case TO_ACOS:
      {
         float f = (data->units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * acosf(input.x);
         sg->out.VEC.y = f * acosf(input.y);
         sg->out.VEC.z = f * acosf(input.z);
      }
      break;
   case TO_ASIN:
      {
         float f = (data->units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * asinf(input.x);
         sg->out.VEC.y = f * asinf(input.y);
         sg->out.VEC.z = f * asinf(input.z);
      }
      break;
   case TO_ATAN:
      {
         float f = (data->units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * atanf(input.x);
         sg->out.VEC.y = f * atanf(input.y);
         sg->out.VEC.z = f * atanf(input.z);
      }
      break;
   case TO_ATAN2:
      {
         AtVector input2 = AiShaderEvalParamVec(p_input2);
         float f = (data->units == AU_Degrees ? sRadToDeg : 1.0f);
         sg->out.VEC.x = f * atan2f(input.x, input2.x);
         sg->out.VEC.y = f * atan2f(input.y, input2.y);
         sg->out.VEC.z = f * atan2f(input.z, input2.z);
      }
      break;
   default:
      sg->out.VEC.x = 0.0f;
      sg->out.VEC.y = 0.0f;
      sg->out.VEC.z = 0.0f;
   }
}
