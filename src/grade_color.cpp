#include "common.h"

AI_SHADER_NODE_EXPORT_METHODS(GradeColorMtd);

enum GradeColorParams
{
   p_input,
   p_black,
   p_white,
   p_lift,
   p_gain
};

node_parameters
{
   AiParameterRGB("input", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("black", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("white", 1.0f, 1.0f, 1.0f);
   AiParameterRGB("lift", 0.0f, 0.0f, 0.0f);
   AiParameterRGB("gain", 1.0f, 1.0f, 1.0f);
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
   AtRGB input = AiShaderEvalParamRGB(p_input);
   AtRGB black = AiShaderEvalParamRGB(p_black);
   AtRGB white = AiShaderEvalParamRGB(p_white);
   AtRGB lift = AiShaderEvalParamRGB(p_lift);
   AtRGB gain = AiShaderEvalParamRGB(p_gain);
   
   AtRGB dWB = white - black;
   AtRGB dGL = gain - lift;
   
   sg->out.RGB = ((input - black) / dWB) * dGL + lift;
}
