import sys
import glob
import re
import os
import excons
import shutil
from excons.tools import arnold

excons.SetArgument("no-arch", 1)

env = excons.MakeBaseEnv()

version = "0.5.1"

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

minMayaId = None
maxMayaId = None
mayaIds = {} # mayaId -> line number

def make_mtd():
  # Can use pre-processor like directive to check for arnold version:
  #   #if arnold >= 4.2
  #   ...
  #   #endif
  nodeexp = re.compile(r"^(\s*\[\s*node\s+)([^]]+)(\s*\]\s*)$")
  attrexp = re.compile(r"^(\s+\[\s*attr\s+)([^]]+)(\s*\]\s*)$")
  mdexp = re.compile(r"(\s*)([^\s]+)\s+([^\s]+)\s+([^\s]+)\s*$")
  ppexp = re.compile(r"^\s*#(.*)\s*$")
  ifexp = re.compile(r"^if\s+([^\s]+)\s+([<>=!]+)\s+([^\s]+)$")
  verexp = re.compile(r"^(\d+)(.(\d+)(.(\d+)(.(\d+))?)?)?$")
  idexp = re.compile(r"^(\s*)maya.id\s+INT\s+0x([a-fA-F0-9]+)\s*$")
  
  arnold_ver = arnold.Version(asString=False)

  dlines = []

  def mayaIdStr(n):
    return "0x" + ("%08x" % n).upper()

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
    global minMayaId, maxMayaId, mayaIds
    
    if not os.path.isfile(path):
      return

    sf = open(path, "r")

    has_maya_name = False
    maya_name = None
    node_heading = ""
    attr_heading = ""
    first_attr = -1
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
          if not has_maya_name and maya_name is not None:
            extra_line = node_heading + "maya.name STRING \"" + maya_name + "\"\n\n"
            if first_attr >= 0:
              dlines.insert(first_attr, extra_line)
            else:
              while len(dlines) > 0:
                last = dlines.pop()
                if len(last.strip()) > 0:
                  dlines.append(last)
                  break
              dlines.append("\n" + extra_line)
          
          has_maya_name = False
          node_heading = line.split("[")[0] + " " # at least indent by one space
          first_attr = -1
          base_name = remap.get(m.group(2), m.group(2))
          node_name = shdprefix + base_name
          maya_name = re.sub(r"_+$", "", shdprefix) + "".join(map(lambda x: x[0].upper() + x[1:], base_name.split("_")))
          dlines.append("%s%s%s" % (m.group(1), node_name, m.group(3)))
        
        else:
          m = idexp.match(line)
          if m:
            mid = int(m.group(2), 16)
            
            if minMayaId is None:
              minMayaId = mid
            elif mid < minMayaId:
              minMayaId = mid
            
            if maxMayaId is None:
              maxMayaId = mid
            elif mid > maxMayaId:
              maxMayaId = mid
            
            if mid in mayaIds:
              print("ID 0x%s already in use!" % m.group(2))
            else:
              mayaIds[mid] = len(dlines)
            
            line = "%smaya.id INT 0x%s\n" % (m.group(1), m.group(2))
          
          else:
            if len(line.strip()) > 0:
              m = attrexp.match(line)
              if m:
                if first_attr < 0:
                  first_attr = len(dlines)
              else:
                m = mdexp.match(line)
                if m:
                  attr_heading = m.group(1)
                  if first_attr < 0:
                    node_heading = attr_heading
                  attr_name = m.group(2)
                  attr_type = m.group(3)
                  if attr_name == "maya.name":
                    has_maya_name = True
                    if maya_name is not None:
                      print("has name! %s" % maya_name)
                      line = attr_heading + attr_name + " " + attr_type + " \"" + maya_name + "\"\n"
          
          dlines.append(line)

    if not has_maya_name and maya_name is not None:
      extra_line = node_heading + "maya.name STRING \"" + maya_name + "\"\n\n"
      if first_attr >= 0:
        dlines.insert(first_attr, extra_line)
      else:
        while len(dlines) > 0:
          last = dlines.pop()
          if len(last.strip()) > 0:
            dlines.append(last)
            break
        dlines.append("\n" + extra_line)

    sf.close()

  append_file_content("src/agShadingBlocks.mtd")

  if withState:
    dlines.append("\n")
    append_file_content("agState/src/agState.mtd", remap={"vector_state": "globals_v",
                                                          "float_state": "globals_f",
                                                          "color_state": "globals_c3",
                                                          "integer_state": "globals_i",
                                                          "matrix_state": "globals_m",
                                                          "node_state": "globals_n"})

  if withNoises:
    dlines.append("\n")
    append_file_content("agNoises/src/agNoises.mtd", remap={"ln_factal": "fractal_noise",
                                                            "ln_voronoi": "voronoi_noise",
                                                            "ln_distort_point": "distort_point"})

  if withSeExpr:
    dlines.append("\n")
    append_file_content("agSeExpr/src/seexpr.mtd")

  if withAnimCurve:
    dlines.append("\n")
    append_file_content("agAnimCurve/src/agAnimCurve.mtd", remap={"anim_curve": "curve"})

  if withUserDataRamp:
    dlines.append("\n")
    append_file_content("agUserDataRamp/src/agUserDataRamp.mtd", remap={"userDataFloatRamp": "shape_attr_ramp_f",
                                                                        "userDataColorRamp": "shape_attr_ramp_c3",
                                                                        "userDataVectorRamp": "shape_attr_ramp_v"})
  
  # Modify maya node IDs
  print("Maya Node ID range: %s - %s" % (mayaIdStr(minMayaId), mayaIdStr(maxMayaId)))
  baseid = excons.GetArgument("maya-base-nodeid", None)
  rev = False
  
  if baseid is not None:
    if re.match(r"^(0x)?[a-fA-F0-9]+$", baseid):
      baseid = int(baseid, 16)
    elif re.match(r"^\d+$", baseid):
      baseid = int(baseid)
    else:
      baseid = None
    
    rev = (excons.GetArgument("maya-increasing-nodeid", 0, int) == 0)
    
    if baseid:
      for mid, lno in mayaIds.iteritems():
        if rev:
          nid = baseid - (maxMayaId - mid)
        else:
          nid = baseid + (mid - minMayaId)
        heading = dlines[lno].split("maya.id")[0]
        dlines[lno] = "%smaya.id INT %s\n" % (heading, mayaIdStr(nid))
  
  df = open("agShadingBlocks.mtd", "w")
  for line in dlines:
    df.write(line)
  df.write("\n")
  df.close()



arniver = arnold.Version(asString=False)
if arniver[0] < 4 or (arniver[0] == 4 and (arniver[1] < 2 or (arniver[1] == 2 and arniver[2] < 12))):
  print("agShadingBlocks requires at least Arnold 4.2.12.0")
  sys.exit(1)

defs = []
incs = []
libs = []
extra_srcs = []
instfiles = {"arnold": ["agShadingBlocks.mtd"]}

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
  instfiles["maya"] = instfiles.get("maya", []) + ["agSeExpr/maya/aiSeexprTemplate.py"]

if withAnimCurve:
  defs.extend(["USE_AGANIMCURVE"])
  extra_srcs += ["agAnimCurve/src/agAnimCurve.cpp"]

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
   "prefix": "arnold",
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "libs": libs,
   "srcs": glob.glob("src/*.cpp") + extra_srcs,
   "install": instfiles,
   "custom": [RequireGmath, arnold.Require],
   "post": [check_symbols]
  }
]

env.Append(CPPFLAGS=" -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)
if sys.platform != "win32":
  env.Append(CPPFLAGS=" -Wno-unused-parameter")

excons.DeclareTargets(env, prjs)

dist_env, ver_dir = excons.EcosystemDist(env, "agShadingBlocks.env", {"agShadingBlocks": ""})
dist_env.Install(ver_dir, "agShadingBlocks.mtd")
if withSeExpr:
  dist_env.Install(ver_dir+"/ae", "agSeExpr/maya/aiSeexprTemplate.py")

Default(["agShadingBlocks"])

