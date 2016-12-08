#include "common.h"
#include <string>
#include <vector>

AI_SHADER_NODE_EXPORT_METHODS(TokenPathMtd);

#define PATH_MAX_LENGTH 4096

enum TokenPathParams
{
   p_filename = 0,
   p_frame
};

enum TokenKey
{
   TK_attr_string = 0,
   TK_attr_integer,
   TK_utile,
   TK_vtile,
   TK_tile,
   TK_udim,
   TK_frame
   //TK_shape_name,
   //TK_shape_path,
   //TK_shape_name_nons,
   //TK_shape_path_nons
};

struct Part
{
   bool isToken;
   TokenKey token;
   std::string sarg1; // for token, this will contain the token argument if any
   std::string sarg2;
   int iarg1;
   int iarg2;
};

typedef std::vector<Part> Parts;

struct TokenPathData
{
   bool evalPath;
   bool evalFrame;
   float frame;
   Parts parts;
   
   int numOutPaths;
   char* *outPaths; // final path per thread
   char constPath[PATH_MAX_LENGTH];
   
   TokenPathData()
   {
      evalPath = false;
      evalFrame = false;
      frame = 0.0f;
      numOutPaths = 0;
      outPaths = 0;
   }
   
   ~TokenPathData()
   {
      clear();
   }
   
   void clear()
   {
      for (int i=0; i<numOutPaths; ++i)
      {
         AiFree(outPaths[i]);
      }
      AiFree(outPaths);
      
      parts.clear();
      
      evalPath = false;
      evalFrame = false;
      frame = 0.0f;
      numOutPaths = 0;
      outPaths = 0;
      constPath[0] = '\0';
   }
};

node_parameters
{
   AiParameterStr(SSTR::filename, "");
   AiParameterFlt(SSTR::frame, 0.0f);
}

node_initialize
{
   AiNodeSetLocalData(node, new TokenPathData());
   AddMemUsage<TokenPathData>();
}

node_update
{
   TokenPathData *data = (TokenPathData*) AiNodeGetLocalData(node);
   
   data->clear();
   
   data->evalPath = AiNodeIsLinked(node, SSTR::filename);
   data->evalFrame = AiNodeIsLinked(node, SSTR::frame);
   
   if (!data->evalFrame)
   {
      data->frame = AiNodeGetFlt(node, SSTR::frame);
   }
   
   if (!data->evalPath)
   {
      int nthreads = GetRenderThreadsCount();
      std::string filename = AiNodeGetStr(node, SSTR::filename).c_str();
      
      bool invalid = false;
      std::string token;
      Part part;
      
      size_t p0 = 0;
      size_t p1 = filename.find('<', p0);
      size_t p2, p3, p4;
      
      while (p1 != std::string::npos)
      {
         // add non-token part
         if (p1 > p0)
         {
            part.isToken = false;
            part.sarg1 = filename.substr(p0, p1 - p0);
            data->parts.push_back(part);
            
            //AiMsgDebug("[token_path] Add string part: \"%s\"", part.sarg1.c_str());
         }
         
         p2 = filename.find('>', p1);
         if (p2 == std::string::npos)
         {
            AiMsgWarning("[token_path] Unclosed token starting at position %lu in \"%s\".", p1, filename.c_str());
            invalid = true;
            break;
         }
         
         p3 = filename.rfind('<', p2);
         if (p3 > p1)
         {
            AiMsgWarning("[token_path] Invalid character '<' in token starting at position %lu in \"%s\"", p1, filename.c_str());
            invalid = true;
            break;
         }
         
         // token from p1+1 to p2-1
         part.isToken = true;
         part.sarg1 = filename.substr(p1 + 1, p2 - p1 - 1);
         
         p3 = part.sarg1.find(':');
         if (p3 != std::string::npos)
         {
            token = part.sarg1.substr(0, p3);
            part.sarg1 = part.sarg1.substr(p3+1);
         }
         else
         {
            token = part.sarg1;
            part.sarg1 = "";
         }
         
         //AiMsgDebug("[token_path] Found \"%s\" token with argument: \"%s\".", token.c_str(), part.sarg1.c_str());
         
         if (token == "attr" || token == "sattr" || token == "iattr")
         {
            // <[is]?attr:name( default:val)?>
            part.token = (token == "iattr" ? TK_attr_integer : TK_attr_string);
            
            if (part.sarg1.length() == 0)
            {
               AiMsgWarning("[token_path] attr/sattr/iattr token expects a value.");
               invalid = true;
               break;
            }
            
            p4 = part.sarg1.find(' ');
            
            if (p4 != std::string::npos)
            {
               part.sarg2 = part.sarg1.substr(p4 + 1);
               part.sarg1 = part.sarg1.substr(0, p4);
               
               //AiMsgDebug("[token_path]   Attribute name: \"%s\"", part.sarg1.c_str());
               //AiMsgDebug("[token_path]   Remains: \"%s\"", part.sarg2.c_str());
               
               p4 = part.sarg2.find(':');
               if (p4 == std::string::npos || part.sarg2.substr(0, p4) != "default")
               {
                  AiMsgWarning("[token_path] Invalid extra argument to attr/sattr/iattr token.");
                  invalid = true;
                  break;
               }
               
               part.sarg2 = part.sarg2.substr(p4 + 1);
               
               if (part.token == TK_attr_integer)
               {
                  // Note that %d will work if string is something like "4.2"
                  if (sscanf(part.sarg2.c_str(), "%d", &(part.iarg2)) != 1)
                  {
                     AiMsgWarning("[token_path] Invalid default value for iattr token. Expected an integer.");
                     invalid = true;
                     break;
                  }
                  
                  //AiMsgDebug("[token_part]   Default int value: %d (%s)", part.iarg2, part.sarg2.c_str());
               }
               else
               {
                  //AiMsgDebug("[token_part]   Default string value: \"%s\"", part.sarg2.c_str());
               }
            }
         }
         else if (token == "utile" || token == "vtile" || token == "udim" || token == "tile")
         {
            // <utile|vtile|tile|udim(:offset)?>
            part.token = (token == "utile" ? TK_utile : (token == "vtile" ? TK_vtile : (token == "tile" ? TK_tile : TK_udim)));
            part.iarg1 = (part.token == TK_udim ? 10 : 1);
            
            if (part.sarg1.length() > 0)
            {
               if (sscanf(part.sarg1.c_str(), "%d", &(part.iarg1)) != 1)
               {
                  AiMsgWarning("[token_path] Invalid argument to %s token: Expected an integer.", token.c_str());
                  invalid = true;
                  break;
               }
               
               //AiMsgDebug("[token_path]   Offset: %d", part.iarg1);
            }
         }
         else if (token == "frame")
         {
            // <frame(:fpad(.sfpad)?)?>
            part.token = TK_frame;
            part.iarg1 = -1; // -1, 0 means no frame padding
            part.iarg2 = -1; // -1 means no subframe, 0 means no subframe padding
            
            if (part.sarg1.length() > 0)
            {
               p4 = part.sarg1.find(',');
               if (p4 != std::string::npos)
               {
                  if (sscanf(part.sarg1.c_str(), "%d,%d", &(part.iarg1), &(part.iarg2)) != 2)
                  {
                     AiMsgWarning("[token_path] Invalid padding values for frame token. Expected 2 integers.");
                     invalid = true;
                     break;
                  }
               }
               else
               {
                  if (sscanf(part.sarg1.c_str(), "%d", &(part.iarg1)) != 1)
                  {
                     AiMsgWarning("[token_path] Invalid padding value for frame token. Expected an integer.");
                     invalid = true;
                     break;
                  }
               }
               
               //AiMsgDebug("[token_path]   Frame padding: %d", part.iarg1);
               //AiMsgDebug("[token_path]   Subframe padding: %d", part.iarg2);
            }
         }
         else
         {
            AiMsgWarning("[token_path] Unknown token %s", token.c_str());
            invalid = true;
            break;
         }
         
         data->parts.push_back(part);
         
         p0 = p2 + 1;
         p1 = filename.find('<', p0);
      }
      
      if (invalid)
      {
         data->parts.clear();
      }
      else
      {
         // add last part if any
         part.isToken = false;
         part.sarg1 = filename.substr(p0);
         
         if (part.sarg1.length() > 0)
         {
            //AiMsgDebug("[token_path] Add string part: \"%s\"", part.sarg1.c_str());
            data->parts.push_back(part);
         }
         
         if (data->parts.size() == 1 && data->parts[0].isToken == false)
         {
            AiMsgDebug("[token_path] Path is constant");
            if (data->parts[0].sarg1.length() + 1 >= PATH_MAX_LENGTH)
            {
               AiMsgWarning("[token_path] Input file path is too long.");
               // Path is too long
               data->constPath[0] = '\0';
            }
            else
            {
               strcpy(data->constPath, data->parts[0].sarg1.c_str());
            }
            data->parts.clear();
         }
         else
         {
            // allocate path buffer per thread
            data->numOutPaths = nthreads;
            data->outPaths = (char**) AiMalloc(nthreads * sizeof(char*));
            for (int i=0; i<nthreads; ++i)
            {
               data->outPaths[i] = (char*) AiMalloc(PATH_MAX_LENGTH * sizeof(char));
            }
         }
      }
   }
}

node_finish
{
   TokenPathData *data = (TokenPathData*) AiNodeGetLocalData(node);
   delete data;
   SubMemUsage<TokenPathData>();
}

shader_evaluate
{
   TokenPathData *data = (TokenPathData*) AiNodeGetLocalData(node);
   
   if (data->parts.size() == 0)
   {
      if (data->evalPath)
      {
         sg->out.STR = AiShaderEvalParamStr(p_filename);
      }
      else
      {
         sg->out.STR = data->constPath;
      }
   }
   else
   {
      float frame = (data->evalFrame ? AiShaderEvalParamFlt(p_frame) : data->frame);
      
      char* &outPath = data->outPaths[sg->tid];
      
      size_t totalLen = 0, partLen = 0;
      bool success = true;
      char buffer[256];
      const char *partStr = 0;
      
      outPath[0] = '\0';
      
      for (Parts::const_iterator part = data->parts.begin(); part != data->parts.end(); ++part)
      {
         if (!part->isToken)
         {
            partLen = part->sarg1.length();
            
            if (totalLen + partLen + 1 >= PATH_MAX_LENGTH)
            {
               //AiMsgWarning("[token_path] Expanded file path is longer than %d, stop processing.", PATH_MAX_LENGTH);
               success = false;
               break;
            }
            
            memcpy(outPath + totalLen, part->sarg1.c_str(), partLen * sizeof(char));
         }
         else
         {
            switch (part->token)
            {
            case TK_attr_string:
               {
                  if (AiUDataGetStr(part->sarg1.c_str(), &partStr))
                  {
                     partLen = strlen(partStr);
                  }
                  else
                  {
                     partLen = part->sarg2.length();
                     if (partLen > 0)
                     {
                        partStr = part->sarg2.c_str();
                     }
                     else
                     {
                        success = false;
                        break;
                     }
                  }
               }
               break;
            case TK_attr_integer:
               {
                  int attrval = 0;
                  
                  if (AiUDataGetInt(part->sarg1.c_str(), &attrval))
                  {
                     sprintf(buffer, "%d", attrval);
                     partLen = strlen(buffer);
                  }
                  else if (part->sarg2.length() > 0)
                  {
                     sprintf(buffer, "%d", part->iarg2);
                     partLen = strlen(buffer);
                  }
                  else
                  {
                     success = false;
                     break;
                  }
                  
                  partStr = buffer;
               }
               break;
            case TK_utile:
               {
                  int u = int(floorf(sg->u)) + part->iarg1;
                  sprintf(buffer, "u%d", u);
                  partLen = strlen(buffer);
                  partStr = buffer;
               }
               break;
            case TK_vtile:
               {
                  int v = int(floorf(sg->v)) + part->iarg1;
                  sprintf(buffer, "v%d", v);
                  partLen = strlen(buffer);
                  partStr = buffer;
               }
               break;
            case TK_tile:
               {
                  int u = int(floorf(sg->u)) + part->iarg1;
                  int v = int(floorf(sg->v)) + part->iarg1;
                  sprintf(buffer, "u%d_v%d", u, v);
                  partLen = strlen(buffer);
                  partStr = buffer;
               }
               break;
            case TK_udim:
               {
                  int c = int(sg->u <= 0.0f ? floorf(sg->u) : ceilf(sg->u) - 1.0f);
                  int r = int(sg->v <= 0.0f ? floorf(sg->v) : ceilf(sg->v) - 1.0f);
                  int d = (part->iarg1 <= 0 ? 1 : part->iarg1);
                  int udim = 1001 + (c + r * d);
                  sprintf(buffer, "%d", udim);
                  partLen = strlen(buffer);
                  partStr = buffer;
               }
               break;
            case TK_frame:
               {
                  char fmt[16];
                  
                  int ff = int(floorf(frame));
                  
                  if (part->iarg2 >= 0)
                  {
                     if (part->iarg2 > 0)
                     {
                        // padded subframe
                        float scl = 1.0f;
                        for (int j=0; j<part->iarg2; ++j) scl *= 10.0f;
                        
                        int sf = int(floorf(0.5f + scl * (frame - float(ff))));
                        
                        if (part->iarg1 > 0)
                        {
                           // padded frame
                           sprintf(fmt, "%%0%dd.%%0%dd", part->iarg1, part->iarg2);
                        }
                        else
                        {
                           // non-padded frame
                           sprintf(fmt, "%%d.%%0%dd", part->iarg2);
                        }
                        
                        sprintf(buffer, fmt, ff, sf);
                     }
                     else
                     {
                        // subframe non-padded -> force 6 digits
                        if (part->iarg1 > 0)
                        {
                           sprintf(fmt, "%%0%d.6f", part->iarg1 + 1 + 6);
                           sprintf(buffer, fmt, frame);
                        }
                        else
                        {
                           // frame non-padded
                           sprintf(buffer, "%f", frame);
                        }
                     }
                  }
                  else
                  {
                     // only full frame
                     if (part->iarg1 > 0)
                     {
                        sprintf(fmt, "%%0%dd", part->iarg1);
                        sprintf(buffer, fmt, ff);
                     }
                     else
                     {
                        sprintf(buffer, "%d", ff);
                     }
                  }
                  
                  partLen = strlen(buffer);
                  partStr = buffer;
               }
               break;
            default:
               success = false;
               break;
            }
            
            if (success)
            {
               if (totalLen + partLen + 1 >= PATH_MAX_LENGTH)
               {
                  //AiMsgWarning("[token_path] Expanded file path is longer than %d, stop processing.", PATH_MAX_LENGTH);
                  break;
               }
               
               memcpy(outPath + totalLen, partStr, partLen * sizeof(char));
            }
         }
         
         if (!success)
         {
            break;
         }
         
         totalLen += partLen;
         outPath[totalLen] = '\0';
      }
      
      if (!success)
      {
         //AiMsgDebug("[token_path] Token replacement failed");
         outPath[0] = '\0';
      }
      
      sg->out.STR = outPath;
   }
}
