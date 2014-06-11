import sys
import glob
import excons
from excons.tools import arnold

env = excons.MakeBaseEnv()

withState = (int(ARGUMENTS.get("with-state", "1")) != 0)
withNoises = (int(ARGUMENTS.get("with-noises", "1")) != 0)
withSeExpr = (int(ARGUMENTS.get("with-seexpr", "1")) != 0)
withAnimCurve = (int(ARGUMENTS.get("with-animcurve", "1")) != 0)
withUserDataRamp = (int(ARGUMENTS.get("with-userdataramp", "1")) != 0)
shdprefix = ARGUMENTS.get("shaders-prefix", "gas_")

defs = []
incs = []
libs = []
extra_srcs = []

if withState:
  defs.append("USE_AGSTATE")
  incs.append("agState")
  extra_srcs += glob.glob("agState/ag?*State.cpp")

if withNoises:
  defs.append("USE_AGNOISES")
  incs.append("agNoises/src")
  extra_srcs += glob.glob("agNoises/src/ln_*.cpp") + \
                glob.glob("agNoises/src/libnoise/*.cpp") + \
                glob.glob("agNoises/src/stegu/*.cpp")

if withSeExpr:
  ARGUMENTS["static"] = "1"
  SConscript("agSeExpr/SeExpr/SConstruct")
  defs.append("USE_AGSEEXPR")
  incs.append("agSeExpr")
  libs.append("SeExpr")
  extra_srcs += ["agSeExpr/src/agSeExpr.cpp"]

if withAnimCurve:
  defs.extend(["USE_AGANIMCURVE", "GMATH_STATIC"])
  incs.append("agAnimCurve/gmath/include")
  extra_srcs += ["agAnimCurve/src/agAnimCurve.cpp",
                 "agAnimCurve/gmath/src/lib/curve.cpp",
                 "agAnimCurve/gmath/src/lib/polynomial.cpp",
                 "agAnimCurve/gmath/src/lib/vector.cpp"]
  
if withUserDataRamp:
  defs.append("USE_AGUSERDATARAMP")
  incs.append("agUserDataRamp/src")
  extra_srcs += glob.glob("agUserDataRamp/src/agUserData*.cpp")

prjs = [
  {"name": "agShadingBlocks",
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

Default("agShadingBlocks")
