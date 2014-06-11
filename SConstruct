import sys
import glob
import excons
from excons.tools import arnold

env = excons.MakeBaseEnv()

agStateSrcs = glob.glob("agState/ag?*State.cpp")

agNoisesSrcs = glob.glob("agNoises/src/ln_*.cpp") + \
               glob.glob("agNoises/src/libnoise/*.cpp") + \
               glob.glob("agNoises/src/stegu/*.cpp")

withoutState = (int(ARGUMENTS.get("without-state", "0")) != 0)
withoutNoises = (int(ARGUMENTS.get("without-noises", "0")) != 0)
shdprefix = ARGUMENTS.get("shaders-prefix", "gas_")

defs = []
incs = []

if not withoutState:
  defs.append("USE_AGSTATE")
  incs.append("agState")

if not withoutNoises:
  defs.append("USE_AGNOISES")
  incs.append("agNoises/src")

prjs = [
  {"name": "agUtils",
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "srcs": glob.glob("src/*.cpp") + ([] if withoutState else agStateSrcs) + ([] if withoutNoises else agNoisesSrcs),
   "custom": [arnold.Require]
  }
]

env.Append(CPPFLAGS=" -Wno-unused-parameter -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)

excons.DeclareTargets(env, prjs)
