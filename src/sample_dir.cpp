#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(SampleDirMtd);

enum SampleDirParams
{
   p_sample = 0,
   p_local_frame,
   p_custom_frame,
   p_frame_rotation,
   p_angle_units
};

node_parameters
{
   AiParameterPnt2("sample", 0.0f, 0.0f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
   AtPoint2 sample = AiShaderEvalParamPnt2(p_sample);
   // sample.x = (sin(theta))^2
   // sample.y = phi / 2 * PI
   
   float sinTheta = sqrtf(sample.x);
   float cosTheta = sqrtf(1.0f - sample.x);
   float phi = 2.0f * AI_PI * sample.y;
   
   AtVector D;
   
   D.x = sinTheta * cosf(phi);
   D.y = sinTheta * sinf(phi);
   D.z = cosf(phi);
   
   sg->out.VEC = D;
}
