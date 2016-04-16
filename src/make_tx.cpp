#include <ai.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
#else
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#endif
#include "strings.h"

AI_SHADER_NODE_EXPORT_METHODS(MakeTxMtd);

#define MAX_FILENAME 1024

enum MakeTxWrap
{
   Wrap_black = 0,
   Wrap_clamp,
   Wrap_periodic,
   Wrap_mirror,
   Wrap_default,
   Wrap_s
};

enum MakeTxFilter
{
   Filter_box = 0,
   Filter_triangle,
   Filter_gaussian,
   Filter_catrom,
   Filter_blackman_harris,
   Filter_sinc,
   Filter_lanczos3,
   Filter_radial_lanczos3,
   Filter_mitchell,
   Filter_bspline,
   Filter_disk
};

enum MakeTxFormat
{
   Format_uint8 = 0,
   Format_sint8,
   Format_uint16,
   Format_sint16,
   Format_half,
   Format_float,
   Format_input
};

enum MakeTxMode
{
   Mode_if_newer = 0,
   Mode_always,
   Mode_reuse,
   Mode_disable
};

enum MakeTxParams
{
   p_filename = 0,
   p_format,
   p_wraps,
   p_wrapt,
   p_tile,
   p_resize,
   p_mipmap,
   p_filter,
   p_stripxmp,
   p_oiioopt,
   p_mode
};

static const char* MakeTxWrapNames[] = {"black", "clamp", "periodic", "mirror", "default", "wraps", 0};
static const char* MakeTxFormatNames[]  = {"uint8", "sint8", "uint16", "sint16", "half", "float", "input", 0};
static const char* MakeTxFilterNames[]  = {"box", "triangle", "gaussian", "catrom", "blackman-harris", "sinc", "lanczos3", "radial-lanczos3", "mitchell", "bspline", "disk", 0};
static const char* MakeTxModeNames[] = {"if_newer", "always", "reuse", "disable", 0};

typedef std::map<std::string, bool> ConvertedMap;

struct NodeData
{
   bool processInEval;

   char **txFilenames;
   int numTxFilenames;

   AtCritSec mutex;
   ConvertedMap converted;

   // convert parameters
   int format;
   int tile;
   int wraps;
   int wrapt;
   int filter;
   bool resize;
   bool mipmap;
   bool stripxmp;
   bool oiioopt;
   int mode;
};

static std::string EscapeString(const std::string &s)
{
   if (s.find(' ') != std::string::npos)
   {
      return "\"" + s + "\"";
   }
   else
   {
      return s;
   }
}

static std::string GetCommandLine(int argc, char **argv)
{
   std::string cmd, arg;

   cmd = "";

   if (argc <= 0)
   {
      return cmd;
   }
   
   cmd += argv[0];
   
   for (int i=1; i<argc; ++i)
   {
      arg = argv[i];
      if (arg.length() == 0)
      {
         continue;
      }
      cmd += " " + EscapeString(arg);
   }
   
   return cmd;
}

static int ExecuteCommand(int argc, char **argv)
{
   #ifdef _WIN32
   
   PROCESS_INFORMATION pinfo;
   STARTUPINFO sinfo;
   
   ZeroMemory(&sinfo, sizeof(sinfo));
   
   sinfo.cb = sizeof(STARTUPINFO);
   sinfo.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
   sinfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
   sinfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
   sinfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
   sinfo.wShowWindow = SW_HIDE;
   
   std::string cmd = GetCommandLine(argc, argv);

   if (CreateProcess(NULL, (char*)cmd.c_str(), NULL, NULL, TRUE, 0, 0, NULL, &sinfo, &pinfo))
   {
      // Parent process
      HANDLE phdl = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pinfo.dwProcessId);
      if (phdl)
      {
         DWORD ret = WaitForSingleObject(phdl, INFINITE);
         int rv = (ret == WAIT_OBJECT_0 ? 0 : 1);
         CloseHandle(phdl);
         return rv;
      }
   }
   return 1;
   
   #else
   
   argv[argc] = 0;
   int pid = fork();
   if (pid == 0)
   {
      int rv = execvp(argv[0], argv);
      return (rv != -1 ? 0 : errno);
   }
   else
   {
      int status;
      int rpid = waitpid(pid, &status, 0);
      return (rpid == pid ? 0 : 1);
   }

   #endif
}

static bool GetTxName(const char *inFilename, char *txFilename, size_t sz)
{
   size_t inlen = strlen(inFilename);

   const char *p0 = strrchr(inFilename, '.');

   if (p0 && !strcmp(p0, ".tx"))
   {
      if (inlen < sz)
      {
         strcpy(txFilename, inFilename);
         return true;
      }
      else
      {
         txFilename[0] = '\0';
         return false;
      }
   }

   char *dst = 0;

   if (p0)
   {
      size_t extlen = (inFilename + inlen) - p0;
      if (inlen - extlen + 3 >= sz)
      {
         txFilename[0] = '\0';
         return false;
      }
      strcpy(txFilename, inFilename);
      dst = txFilename + (p0 - inFilename);
   }
   else
   {
      if (inlen + 3 >= sz)
      {
         txFilename[0] = '\0';
         return false;
      }
      dst = txFilename + inlen;
   }

   dst[0] = '.';
   dst[1] = 't';
   dst[2] = 'x';
   dst[3] = '\0';

   return true;
}

static bool MakeTx(NodeData *data, const char *inFilename, const char *outFilename)
{
   struct stat st0, st1;

   if (stat(inFilename, &st0) == 0)
   {
      bool txExists = (stat(outFilename, &st1) == 0);

      if (!txExists || st0.st_mtime > st1.st_mtime || data->mode == Mode_always)
      {
         //std::string cmd = "maketx -o ";
         //cmd += outFilename;

         std::vector<std::string> args;

         args.push_back("maketx");
         args.push_back("-o");
         args.push_back(outFilename);

         if (data->wrapt == Wrap_s)
         {
            if (data->wraps != Wrap_default && data->wraps != Wrap_s)
            {
               //cmd += " --wrap ";
               //cmd += MakeTxWrapNames[data->wraps];
               args.push_back("--wrap");
               args.push_back(MakeTxWrapNames[data->wraps]);
            }
         }
         else
         {
            if (data->wraps != Wrap_default && data->wraps != Wrap_s)
            {
               //cmd += " --swrap ";
               //cmd += MakeTxWrapNames[data->wraps];
               args.push_back("--swrap");
               args.push_back(MakeTxWrapNames[data->wraps]);
            }
            if (data->wrapt != Wrap_default)
            {
               //cmd += " --twrap ";
               //cmd += MakeTxWrapNames[data->wrapt];
               args.push_back("--twrap");
               args.push_back(MakeTxWrapNames[data->wrapt]);
            }
         }

         if (data->format != Format_input)
         {
            //cmd += " -d ";
            //cmd += MakeTxFormatNames[data->format];
            args.push_back("-d");
            args.push_back(MakeTxFormatNames[data->format]);
         }

         if (data->resize)
         {
            //cmd += " --resize";
            args.push_back("--resize");
         }

         if (!data->mipmap)
         {
            //cmd += " --nomipmap";
            args.push_back("--nomipmap");
         }

         if (data->tile > 0)
         {
            char tmp[32];
            //sprintf(tmp, " --tile %d %d", data->tile, data->tile);
            //cmd += tmp;
            sprintf(tmp, "%d", data->tile);
            args.push_back("--tile");
            args.push_back(tmp);
            args.push_back(tmp);
         }

         //cmd += " --filter ";
         //cmd += MakeTxFilterNames[data->filter];
         args.push_back("--filter");
         args.push_back(MakeTxFilterNames[data->filter]);

         if (data->oiioopt)
         {
            //cmd += " --oiio";
            args.push_back("--oiio");
         }

         if (txExists)
         {
            //cmd += " -u";
            args.push_back("-u");
         }

         //cmd += " ";
         //cmd += inFilename;
         args.push_back(inFilename);
         

         //AiMsgInfo("[MakeTx]   %s", cmd.c_str());
         //int ret = system(cmd.c_str());
         int argc = (int) args.size();
         char **argv = (char**) AiMalloc((argc + 1) * sizeof(char*));
         for (int i=0; i<argc; ++i)
         {
            argv[i] = (char*) args[i].c_str();
         }
         argv[argc] = 0;

         AiMsgInfo("[MakeTx]   %s", GetCommandLine(argc, argv).c_str());
         int ret = ExecuteCommand(argc, argv);

         if (ret != 0)
         {
            AiFree(argv);
            return false;
         }
         
         if (data->stripxmp)
         {
            //cmd = "exiftool -XMP:all= ";
            //cmd += outFilename;

            // There are at least 3 arguments already for maketx command line
            argv[0] = (char*) "exiftool";
            argv[1] = (char*) "-XMP:all=";
            argv[2] = (char*) outFilename;
            argv[3] = 0;

            AiMsgInfo("[MakeTx]   %s", GetCommandLine(3, argv).c_str());
            //ret = system(cmd.c_str());
            ret = ExecuteCommand(3, argv);

            if (ret != 0)
            {
               AiMsgWarning("[MakeTx]   Strip XMP metadata failed");
            }
            else
            {
               std::string tmp = outFilename;
               tmp += "_original";
               remove(tmp.c_str());
            }
         }

         AiFree(argv);
      }
      else
      {
         AiMsgInfo("[MakeTx]   Already converted.");
      }
      return true;
   }
   else
   {
      return false;
   }
}

node_parameters
{
   AiParameterSTR(SSTR::filename, "");
   AiParameterENUM(SSTR::format, Format_input, MakeTxFormatNames);
   AiParameterENUM(SSTR::wraps, Wrap_default, MakeTxWrapNames);
   AiParameterENUM(SSTR::wrapt, Wrap_s, MakeTxWrapNames);
   AiParameterINT(SSTR::tile, 0);
   AiParameterBOOL(SSTR::resize, false);
   AiParameterBOOL(SSTR::mipmap, true);
   AiParameterENUM(SSTR::filter, Filter_box, MakeTxFilterNames);
   AiParameterBOOL(SSTR::stripxmp, true);
   AiParameterBOOL(SSTR::oiioopt, true);
   AiParameterENUM(SSTR::mode, Mode_if_newer, MakeTxModeNames);
   
   AiMetaDataSetBool(mds, SSTR::filename, SSTR::filepath, true);
   AiMetaDataSetStr(mds, SSTR::filename, "houdini.type", "file:image");
   AiMetaDataSetBool(mds, SSTR::format, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::wraps, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::wrapt, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::tile, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::resize, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::mipmap, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::filter, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::stripxmp, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::oiioopt, SSTR::linkable, false);
   AiMetaDataSetBool(mds, SSTR::mode, SSTR::linkable, false);
}

node_initialize
{
}

node_update
{
   Finish(node);

   NodeData *data = new NodeData();

   AiCritSecInit(&(data->mutex));

   data->format = AiNodeGetInt(node, SSTR::format);
   data->resize = AiNodeGetBool(node, SSTR::resize);
   data->mipmap = AiNodeGetBool(node, SSTR::mipmap);
   data->tile = AiNodeGetInt(node, SSTR::tile);
   data->filter = AiNodeGetInt(node, SSTR::filter);
   data->wraps = AiNodeGetInt(node, SSTR::wraps);
   data->wrapt = AiNodeGetInt(node, SSTR::wrapt);
   data->stripxmp = AiNodeGetBool(node, SSTR::stripxmp);
   data->oiioopt = AiNodeGetBool(node, SSTR::oiioopt);
   data->mode = AiNodeGetInt(node, SSTR::mode);

   if (!AiNodeGetLink(node, SSTR::filename))
   {
      data->processInEval = false;
      
      data->numTxFilenames = 1;
      data->txFilenames = (char**) AiMalloc(data->numTxFilenames * sizeof(char*));
      data->txFilenames[0] = (char*) AiMalloc(MAX_FILENAME * sizeof(char));
      
      bool useinput = true;
      
      const char *inFilename = AiNodeGetStr(node, SSTR::filename);
      
      if (data->mode != Mode_disable && GetTxName(inFilename, data->txFilenames[0], MAX_FILENAME))
      {
         if (data->mode == Mode_reuse)
         {
            AiMsgInfo("[MakeTx] %s: Reuse %s", AiNodeGetName(node), data->txFilenames[0]);
            struct stat inst, txst;
            if (stat(inFilename, &inst) == 0 && stat(data->txFilenames[0], &txst) == 0)
            {
               useinput = (inst.st_mtime > txst.st_mtime);
            }
         }
         else
         {
            AiMsgInfo("[MakeTx] %s: Convert input %s", AiNodeGetName(node), inFilename);
            useinput = !MakeTx(data, inFilename, data->txFilenames[0]);
         }
      }
      else
      {
         if (data->mode == Mode_disable)
         {
            AiMsgInfo("[MakeTx] %s: Disabled", AiNodeGetName(node));
         }
         else
         {
            AiMsgWarning("[MakeTx] %s: Could not generate TX filename", AiNodeGetName(node));
         }
      }
      
      if (useinput)
      {
         strncpy(data->txFilenames[0], inFilename, MAX_FILENAME);
      }
   }
   else
   {
      if (data->mode == Mode_if_newer || data->mode == Mode_always)
      {
         AiMsgInfo("[MakeTx] %s: Convertion delayed to shader evaluation time.", AiNodeGetName(node));
      }
      data->processInEval = true;
      
      AtNode *opts = AiUniverseGetOptions();
      data->numTxFilenames = AiNodeGetInt(opts, SSTR::threads);
      data->txFilenames = (char**) AiMalloc(data->numTxFilenames * sizeof(char*));
      
      for (int i=0; i<data->numTxFilenames; ++i)
      {
         data->txFilenames[i] = (char*) AiMalloc(MAX_FILENAME * sizeof(char));
      }
   }

   AiNodeSetLocalData(node, data);
}

node_finish
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   if (data != NULL)
   {
      data->converted.clear();

      for (int i=0; i<data->numTxFilenames; ++i)
      {
         AiFree(data->txFilenames[i]);
      }
      AiFree(data->txFilenames);
      
      AiCritSecClose(&(data->mutex));

      delete data;
   }
}

shader_evaluate
{
   NodeData *data = (NodeData*) AiNodeGetLocalData(node);

   if (data->processInEval)
   {
      const char *inFilename = AiShaderEvalParamStr(p_filename);
      
      if (data->mode != Mode_disable && GetTxName(inFilename, data->txFilenames[sg->tid], MAX_FILENAME))
      {
         bool success = false;
         
         AiCritSecEnter(&(data->mutex));
         
         ConvertedMap::iterator it = data->converted.find(inFilename);
         
         if (it != data->converted.end())
         {
            success = it->second;
         }
         else
         {
            if (data->mode == Mode_reuse)
            {
               struct stat inst, txst;
               if (stat(inFilename, &inst) == 0 && stat(data->txFilenames[sg->tid], &txst) == 0)
               {
                  success = (inst.st_mtime <= txst.st_mtime);
               }
            }
            else
            {
               AiMsgInfo("[MakeTx] %s: Convert input %s", AiNodeGetName(node), inFilename);
               success = MakeTx(data, inFilename, data->txFilenames[sg->tid]);
            }
            data->converted[inFilename] = success;
         }
         
         AiCritSecLeave(&(data->mutex));
         
         sg->out.STR = (success ? data->txFilenames[sg->tid] : inFilename);
      }
      else
      {
         //AiMsgInfo("[MakeTx] %s: Disabled", AiNodeGetName(node));
         sg->out.STR = inFilename;
      }
   }
   else
   {
      sg->out.STR = data->txFilenames[0];
   }
}
