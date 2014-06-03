import sys
import glob
import excons
from excons.tools import arnold

env = excons.MakeBaseEnv()

agStateSrcs = ["agState/agColorState.cpp",
               "agState/agFloatState.cpp",
               "agState/agIntState.cpp",
               "agState/agVectorState.cpp"]

withoutState = (int(ARGUMENTS.get("without-state", "0")) != 0)

defs = []
incs = []
if not withoutState:
   defs.append("USE_AGSTATE")
   incs.append("agState")

prjs = [
  {"name": "agUtils",
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "srcs": glob.glob("src/*.cpp") + ([] if withoutState else agStateSrcs),
   "custom": [arnold.Require]
  }
]

env.Append(CPPFLAGS=" -Wno-unused-parameter")

excons.DeclareTargets(env, prjs)
