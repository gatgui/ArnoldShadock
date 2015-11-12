import sys
import glob
import re
import os
import excons
import shutil
from excons.tools import arnold

env = excons.MakeBaseEnv()

withState = (excons.GetArgument("with-state", 1, int) != 0)
withNoises = (excons.GetArgument("with-noises", 1, int) != 0)
withSeExpr = (excons.GetArgument("with-seexpr", 1, int) != 0)
withAnimCurve = (excons.GetArgument("with-animcurve", 1, int) != 0)
withUserDataRamp = (excons.GetArgument("with-userdataramp", 1, int) != 0)
shdprefix = excons.GetArgument("shaders-prefix", "gas_")

def check_symbols(*args, **kwargs):
  import subprocess

  try:

    _, arnilib = excons.GetDirs("arnold", libdirname=("bin" if sys.platform != "win32" else "lib"))

    envvar = ("PATH" if sys.platform == "win32" else ("DYLD_LIBRARY_PATH" if sys.platform == "darwin" else "LD_LIBRARY_PATH"))

    if not arnilib in os.environ.get(envvar, ""):
      os.environ[envvar] = os.environ.get(envvar, "") + os.pathsep + arnilib

    cmd = ["python", "-c", "import ctypes; ctypes.cdll.LoadLibrary('%s')" % kwargs["target"][0]]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    if p.returncode != 0:
      raise Exception(err)

  except Exception, e:
    msg = str(e)

    if "undefined symbol" in msg:
      print("\n!!! MISSING SYMBOLS FOUND !!!")
    else:
      print("\n!!! FAILED TO CHECK SYMBOLS !!!")

    print(msg)

    sys.exit(1)

def get_arnold_version():
  arnoldinc, _ = excons.GetDirs("arnold", libdirname=("bin" if sys.platform != "win32" else "lib"))

  ai_version = os.path.join(arnoldinc, "ai_version.h")

  varch, vmaj, vmin, vpatch = 0, 0, 0, 0

  if os.path.isfile(ai_version):
    defexp = re.compile(r"^\s*#define\s+AI_VERSION_(ARCH_NUM|MAJOR_NUM|MINOR_NUM|FIX)\s+([^\s]+)")
    f = open(ai_version, "r")
    for line in f.readlines():
      m = defexp.match(line)
      if m:
        which = m.group(1)
        if which == "ARCH_NUM":
          varch = int(m.group(2))
        elif which == "MAJOR_NUM":
          vmaj = int(m.group(2))
        elif which == "MINOR_NUM":
          vmin = int(m.group(2))
        elif which == "FIX":
          m = re.search(r"\d+", m.group(2))
          vpatch = (0 if m is None else int(m.group(0)))
    f.close()

  return (varch, vmaj, vmin, vpatch)

# arnold_ver is a 4 tuple (4, 2, 2, 0)
def make_mtd():
  nodeexp = re.compile(r"^(\s*\[\s*node\s+)([^]]+)(\s*\]\s*)$")
  ppexp = re.compile(r"^\s*#(.*)\s*$")
  ifexp = re.compile(r"^if\s+([^\s]+)\s+([<>=!]+)\s+([^\s]+)$")
  verexp = re.compile(r"^(\d+)(.(\d+)(.(\d+)(.(\d+))?)?)?$")

  arnold_ver = get_arnold_version()

  df = open("agShadingBlocks.mtd", "w")

  def parse_version(s):
    vm = verexp.match(s)
    if vm:
      varch = int(vm.group(1))
      vmaj, vmin, vpatch = 0, 0, 0
      if vm.group(2):
        vmaj = int(vm.group(3))
        if vm.group(4):
          vmin = int(vm.group(5))
          if vm.group(6):
            vpatch = vm.group(7)
      return (varch, vmaj, vmin, vpatch)
    else:
      return None

  def append_file_content(path, remap={}):
    if not os.path.isfile(path):
      return

    sf = open(path, "r")

    ignore_lines = False

    for line in sf.readlines():
      # Check for pre-processor line
      m = ppexp.match(line)
      if m:
        # Is it a conditional?
        pp = m.group(1)
        m = ifexp.match(pp)
        if m:
          # Check conditional variable (only 'arnold' supported for now)
          if m.group(1) == "arnold":
            # Parse required arnold version
            ver = parse_version(m.group(3))

            if ver:
              # Get comparison operator
              op = m.group(2)

              if op == "<=":
                ignore_lines = not (arnold_ver <= ver)

              elif op == ">=":
                ignore_lines = not (arnold_ver >= ver)

              elif op == "<":
                ignore_lines = not (arnold_ver < ver)

              elif op == ">":
                ignore_lines = not (arnold_ver > ver)

              elif op == "==":
                ignore_lines = not (arnold_ver == ver)

              elif op == "!=":
                ignore_lines = not (arnold_ver != ver)

              else:
                # unknown operator: evaluate condition to false
                ignore_lines = True
            else:
              # invalid version specification: evaluate condition to false
              ignore_lines = True
          else:
            # unknown condition variable: evaluates to false
            ignore_lines = True

        elif pp == "endif":
          # reset ignore state
          ignore_lines = False

      elif not ignore_lines:
        # Replace node name
        m = nodeexp.match(line)
        if m:
          df.write("%s%s%s%s" % (m.group(1), shdprefix, remap.get(m.group(2), m.group(2)), m.group(3)))
        else:
          df.write(line)

    sf.close()

  append_file_content("src/agShadingBlocks.mtd")

  if withState:
    df.write("\n")
    append_file_content("agState/src/agState.mtd", remap={"vector_state": "globals_v",
                                                          "float_state": "globals_f",
                                                          "color_state": "globals_c3",
                                                          "integer_state": "globals_i",
                                                          "matrix_state": "globals_m",
                                                          "node_state": "globals_n"})

  if withNoises:
    df.write("\n")
    append_file_content("agNoises/src/agNoises.mtd", remap={"ln_factal": "fractal_noise",
                                                            "ln_voronoi": "voronoi_noise",
                                                            "ln_distort_point": "distort_point"})

  if withSeExpr:
    df.write("\n")
    append_file_content("agSeExpr/src/seexpr.mtd")

  if withAnimCurve:
    df.write("\n")
    append_file_content("agAnimCurve/src/agAnimCurve.mtd", remap={"anim_curve": "curve"})

  if withUserDataRamp:
    df.write("\n")
    append_file_content("agUserDataRamp/src/agUserDataRamp.mtd", remap={"userDataFloatRamp": "shape_attr_ramp_f",
                                                                        "userDataColorRamp": "shape_attr_ramp_c3",
                                                                        "userDataVectorRamp": "shape_attr_ramp_v"})

  df.write("\n")
  df.close()


defs = []
incs = []
libs = []
extra_srcs = []

if withState:
  defs.append("USE_AGSTATE")
  incs.append("agState")
  extra_srcs += glob.glob("agState/src/ag?*State.cpp")

if withNoises:
  defs.append("USE_AGNOISES")
  incs.append("agNoises/src")
  extra_srcs += glob.glob("agNoises/src/ln_*.cpp") + \
                glob.glob("agNoises/src/libnoise/*.cpp") + \
                glob.glob("agNoises/src/stegu/*.cpp")

if withSeExpr:
  excons.SetArgument("static", 1)
  SConscript("agSeExpr/SeExpr/SConstruct")
  defs.append("USE_AGSEEXPR")
  incs.append("agSeExpr")
  libs.append("SeExpr")
  extra_srcs += ["agSeExpr/src/seexpr.cpp"]

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

make_mtd()

excons.SetArgument("static", 1)
SConscript("gmath/SConstruct")
Import("RequireGmath")

prjs = [
  {"name": "agShadingBlocks",
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "libs": libs,
   "srcs": glob.glob("src/*.cpp") + extra_srcs,
   "install": {"": "agShadingBlocks.mtd"},
   "custom": [RequireGmath(subdir="gmath"), arnold.Require],
   "post": [check_symbols]
  }
]

env.Append(CPPFLAGS=" -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)
if sys.platform != "win32":
  env.Append(CPPFLAGS=" -Wno-unused-parameter")

excons.DeclareTargets(env, prjs)

Default("agShadingBlocks")
