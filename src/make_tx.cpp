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
#include <errno.h>
#endif

AI_SHADER_NODE_EXPORT_METHODS(MakeTxMtd);

#define MAX_FILENAME 1024

enum MakeTxWrap
{
   W_BLACK = 0,
   W_CLAMP,
   W_PERIODIC,
   W_MIRROR,
   W_DEFAULT,
   W_S
};

enum MakeTxFilter
{
   F_BOX = 0,
   F_TRIANGLE,
   F_GAUSSIAN,
   F_CATROM,
   F_BLACKMAN_HARRIS,
   F_SINC,
   F_LANCZOS3,
   F_RADIAL_LANCZOS3,
   F_MITCHELL,
   F_BSPLINE,
   F_DISK
};

enum MakeTxFormat
{
   D_UINT8 = 0,
   D_SINT8,
   D_UINT16,
   D_SINT16,
   D_HALF,
   D_FLOAT,
   D_INPUT
};

enum MakeTxMode
{
   M_IF_NEWER = 0,
   M_ALWAYS,
   M_REUSE,
   M_DISABLE
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

struct MakeTxData
{
   bool process_in_eval;

   char **tx_filenames;
   int num_tx_filenames;

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

static bool GetTxName(const char *in_filename, char *tx_filename, size_t sz)
{
   size_t inlen = strlen(in_filename);

   const char *p0 = strrchr(in_filename, '.');

   if (p0 && !strcmp(p0, ".tx"))
   {
      if (inlen < sz)
      {
         strcpy(tx_filename, in_filename);
         return true;
      }
      else
      {
         tx_filename[0] = '\0';
         return false;
      }
   }

   char *dst = 0;

   if (p0)
   {
      size_t extlen = (in_filename + inlen) - p0;
      if (inlen - extlen + 3 >= sz)
      {
         tx_filename[0] = '\0';
         return false;
      }
      strcpy(tx_filename, in_filename);
      dst = tx_filename + (p0 - in_filename);
   }
   else
   {
      if (inlen + 3 >= sz)
      {
         tx_filename[0] = '\0';
         return false;
      }
      dst = tx_filename + inlen;
   }

   dst[0] = '.';
   dst[1] = 't';
   dst[2] = 'x';
   dst[3] = '\0';

   return true;
}

static bool MakeTx(MakeTxData *data, const char *in_filename, const char *outfilename)
{
   struct stat st0, st1;

   if (stat(in_filename, &st0) == 0)
   {
      bool txExists = (stat(outfilename, &st1) == 0);

      if (!txExists || st0.st_mtime > st1.st_mtime || data->mode == M_ALWAYS)
      {
         //std::string cmd = "maketx -o ";
         //cmd += outfilename;

         std::vector<std::string> args;

         args.push_back("maketx");
         args.push_back("-o");
         args.push_back(outfilename);

         if (data->wrapt == W_S)
         {
            if (data->wraps != W_DEFAULT && data->wraps != W_S)
            {
               //cmd += " --wrap ";
               //cmd += MakeTxWrapNames[data->wraps];
               args.push_back("--wrap");
               args.push_back(MakeTxWrapNames[data->wraps]);
            }
         }
         else
         {
            if (data->wraps != W_DEFAULT && data->wraps != W_S)
            {
               //cmd += " --swrap ";
               //cmd += MakeTxWrapNames[data->wraps];
               args.push_back("--swrap");
               args.push_back(MakeTxWrapNames[data->wraps]);
            }
            if (data->wrapt != W_DEFAULT)
            {
               //cmd += " --twrap ";
               //cmd += MakeTxWrapNames[data->wrapt];
               args.push_back("--twrap");
               args.push_back(MakeTxWrapNames[data->wrapt]);
            }
         }

         if (data->format != D_INPUT)
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
         //cmd += in_filename;
         args.push_back(in_filename);
         

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
            //cmd += outfilename;

            // There are at least 3 arguments already for maketx command line
            argv[0] = (char*) "exiftool";
            argv[1] = (char*) "-XMP:all=";
            argv[2] = (char*) outfilename;
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
               std::string tmp = outfilename;
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
   AiParameterSTR("filename", "");
   AiParameterENUM("format", D_INPUT, MakeTxFormatNames);
   AiParameterENUM("wraps", W_DEFAULT, MakeTxWrapNames);
   AiParameterENUM("wrapt", W_S, MakeTxWrapNames);
   AiParameterINT("tile", 0);
   AiParameterBOOL("resize", false);
   AiParameterBOOL("mipmap", true);
   AiParameterENUM("filter", F_BOX, MakeTxFilterNames);
   AiParameterBOOL("stripxmp", true);
   AiParameterBOOL("oiioopt", true);
   AiParameterENUM("mode", M_IF_NEWER, MakeTxModeNames);
   
   //AiMetaDataSetBool(mds, NULL, "maya.hide", true);
   
   AiMetaDataSetStr(mds, NULL, "maya.name", "aiMakeTx");
   //AiMetaDataSetInt(mds, NULL, "maya.id", 0x00115D19);
   AiMetaDataSetStr(mds, NULL, "maya.classification", "utility/general");
   AiMetaDataSetBool(mds, NULL, "maya.swatch", false);

   AiMetaDataSetBool(mds, "filename", "filepath", true);
   AiMetaDataSetBool(mds, "output", "filepath", true);
}

node_initialize
{
}

node_update
{
   Finish(node);

   MakeTxData *data = (MakeTxData*) AiMalloc(sizeof(MakeTxData));

   new (&(data->converted)) ConvertedMap();

   AiCritSecInit(&(data->mutex));

   data->format = AiNodeGetInt(node, "format");
   data->resize = AiNodeGetBool(node, "resize");
   data->mipmap = AiNodeGetBool(node, "mipmap");
   data->tile = AiNodeGetInt(node, "tile");
   data->filter = AiNodeGetInt(node, "filter");
   data->wraps = AiNodeGetInt(node, "wraps");
   data->wrapt = AiNodeGetInt(node, "wrapt");
   data->stripxmp = AiNodeGetBool(node, "stripxmp");
   data->oiioopt = AiNodeGetBool(node, "oiioopt");
   data->mode = AiNodeGetInt(node, "mode");

   if (!AiNodeGetLink(node, "filename"))
   {
      data->process_in_eval = false;
      
      data->num_tx_filenames = 1;
      data->tx_filenames = (char**) AiMalloc(data->num_tx_filenames * sizeof(char*));
      data->tx_filenames[0] = (char*) AiMalloc(MAX_FILENAME * sizeof(char));
      
      bool useinput = true;
      
      const char *in_filename = AiNodeGetStr(node, "filename");
      
      if (data->mode != M_DISABLE && GetTxName(in_filename, data->tx_filenames[0], MAX_FILENAME))
      {
         if (data->mode == M_REUSE)
         {
            AiMsgInfo("[MakeTx] %s: Reuse %s", AiNodeGetName(node), data->tx_filenames[0]);
            struct stat inst, txst;
            if (stat(in_filename, &inst) == 0 && stat(data->tx_filenames[0], &txst) == 0)
            {
               useinput = (inst.st_mtime > txst.st_mtime);
            }
         }
         else
         {
            AiMsgInfo("[MakeTx] %s: Convert input %s", AiNodeGetName(node), in_filename);
            useinput = !MakeTx(data, in_filename, data->tx_filenames[0]);
         }
      }
      else
      {
         if (data->mode == M_DISABLE)
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
         strncpy(data->tx_filenames[0], in_filename, MAX_FILENAME);
      }
   }
   else
   {
      if (data->mode == M_IF_NEWER || data->mode == M_ALWAYS)
      {
         AiMsgInfo("[MakeTx] %s: Convertion delayed to shader evaluation time.", AiNodeGetName(node));
      }
      data->process_in_eval = true;
      
      AtNode *opts = AiUniverseGetOptions();
      data->num_tx_filenames = AiNodeGetInt(opts, "threads");
      data->tx_filenames = (char**) AiMalloc(data->num_tx_filenames * sizeof(char*));
      
      for (int i=0; i<data->num_tx_filenames; ++i)
      {
         data->tx_filenames[i] = (char*) AiMalloc(MAX_FILENAME * sizeof(char));
      }
   }

   AiNodeSetLocalData(node, data);
}

node_finish
{
   MakeTxData *data = (MakeTxData*) AiNodeGetLocalData(node);

   if (data != NULL)
   {
      data->converted.clear();
      data->converted.~ConvertedMap();

      for (int i=0; i<data->num_tx_filenames; ++i)
      {
         AiFree(data->tx_filenames[i]);
      }
      AiFree(data->tx_filenames);
      AiFree(data);

      AiCritSecClose(&(data->mutex));
   }
}

shader_evaluate
{
   MakeTxData *data = (MakeTxData*) AiNodeGetLocalData(node);

   if (data->process_in_eval)
   {
      const char *in_filename = AiShaderEvalParamStr(p_filename);
      
      if (data->mode != M_DISABLE && GetTxName(in_filename, data->tx_filenames[sg->tid], MAX_FILENAME))
      {
         bool success = false;
         
         AiCritSecEnter(&(data->mutex));
         
         ConvertedMap::iterator it = data->converted.find(in_filename);
         
         if (it != data->converted.end())
         {
            success = it->second;
         }
         else
         {
            if (data->mode == M_REUSE)
            {
               struct stat inst, txst;
               if (stat(in_filename, &inst) == 0 && stat(data->tx_filenames[sg->tid], &txst) == 0)
               {
                  success = (inst.st_mtime <= txst.st_mtime);
               }
            }
            else
            {
               AiMsgInfo("[MakeTx] %s: Convert input %s", AiNodeGetName(node), in_filename);
               success = MakeTx(data, in_filename, data->tx_filenames[sg->tid]);
            }
            data->converted[in_filename] = success;
         }
         
         AiCritSecLeave(&(data->mutex));
         
         sg->out.STR = (success ? data->tx_filenames[sg->tid] : in_filename);
      }
      else
      {
         //AiMsgInfo("[MakeTx] %s: Disabled", AiNodeGetName(node));
         sg->out.STR = in_filename;
      }
   }
   else
   {
      sg->out.STR = data->tx_filenames[0];
   }
}
