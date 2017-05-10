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

AI_SHADER_NODE_EXPORT_METHODS(TransformMtd);

enum TransformParams
{
   p_matrix = 0,
   p_vector,
   p_as_point
};

node_parameters
{
   AtMatrix id;
   AiM4Identity(id);
   AiParameterMtx(SSTR::matrix, id);
   AiParameterVec("vector", 0.0f, 0.0f, 0.0f);
   AiParameterBool(SSTR::as_point, false);
}

struct TransformData
{
   bool evalMatrix;
   AtMatrix matrix;
   bool asPoint;
};

node_initialize
{
   AiNodeSetLocalData(node, new TransformData());
   AddMemUsage<TransformData>();
}

node_update
{
   TransformData *data = (TransformData*) AiNodeGetLocalData(node);
   data->asPoint = AiNodeGetBool(node, SSTR::as_point);
   data->evalMatrix = AiNodeIsLinked(node, SSTR::matrix);
   if (!data->evalMatrix)
   {
      AiNodeGetMatrix(node, SSTR::matrix, data->matrix);
   }
}

node_finish
{
   TransformData *data = (TransformData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<TransformData>();
}

shader_evaluate
{
   TransformData *data = (TransformData*) AiNodeGetLocalData(node);

   AtMatrix *mtx = (data->evalMatrix ? AiShaderEvalParamMtx(p_matrix) : &(data->matrix));
   AtVector vec = AiShaderEvalParamVec(p_vector);

   if (data->asPoint)
   {
      AiM4PointByMatrixMult(&(sg->out.PNT), *mtx, &vec);
   }
   else
   {
      AiM4VectorByMatrixMult(&(sg->out.VEC), *mtx, &vec);
   }
}
