import sys
import glob
import excons
from excons.tools import arnold

env = excons.MakeBaseEnv()

agStateSrcs = glob.glob("agState/ag?*State.cpp")

agNoisesSrcs = glob.glob("agNoises/src/ln_*.cpp") + \
               glob.glob("agNoises/src/libnoise/*.cpp") + \
               glob.glob("agNoises/src/stegu/*.cpp")

agSeExprSrcs = ["agSeExpr/src/agSeExpr.cpp"]

withoutState = (int(ARGUMENTS.get("without-state", "0")) != 0)
withoutNoises = (int(ARGUMENTS.get("without-noises", "0")) != 0)
withoutSeExpr = (int(ARGUMENTS.get("without-seexpr", "0")) != 0)
shdprefix = ARGUMENTS.get("shaders-prefix", "gas_")

defs = []
incs = []
libs = []
extra_srcs = []

if not withoutState:
  defs.append("USE_AGSTATE")
  incs.append("agState")
  extra_srcs += agStateSrcs

if not withoutNoises:
  defs.append("USE_AGNOISES")
  incs.append("agNoises/src")
  extra_srcs += agNoisesSrcs

if not withoutSeExpr:
  ARGUMENTS["static"] = "1"
  SConscript("agSeExpr/SeExpr/SConstruct")
  defs.append("USE_AGSEEXPR")
  incs.append("agSeExpr")
  libs.append("SeExpr")
  extra_srcs += agSeExprSrcs

prjs = [
  {"name": "agShaderLib",
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "libs": libs,
   "srcs": glob.glob("src/*.cpp") + extra_srcs,
   "custom": [arnold.Require]
  }
]

env.Append(CPPFLAGS=" -Wno-unused-parameter -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)

excons.DeclareTargets(env, prjs)

Default("agShaderLib")
