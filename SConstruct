import sys
import glob
import excons
from excons.tools import arnold

env = excons.MakeBaseEnv()

prjs = [
  {"name": "agUtils",
   "type": "dynamicmodule",
   "ext": arnold.PluginExt(),
   "srcs": glob.glob("src/*.cpp"),
   "custom": [arnold.Require]
  }
]

excons.DeclareTargets(env, prjs)
