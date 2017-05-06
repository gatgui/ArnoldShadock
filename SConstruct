import sys
import glob
import re
import os
import excons
import shutil
from excons.tools import arnold

env = excons.MakeBaseEnv()

version = "0.6.0"

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
    append_file_content("ArnoldStateShaders/src/state.mtd.in", remap={"${prefix}state_v": "globals_v",
                                                                      "${prefix}state_f": "globals_f",
                                                                      "${prefix}state_c3": "globals_c3",
                                                                      "${prefix}state_i": "globals_i",
                                                                      "${prefix}state_m": "globals_m",
                                                                      "${prefix}state_n": "globals_n"})

  if withNoises:
    dlines.append("\n")
    append_file_content("ArnoldNoiseShaders/src/noise.mtd.in", remap={"${prefix}fractal": "fractal_noise",
                                                                      "${prefix}voronoi": "voronoi_noise",
                                                                      "${prefix}distort_point": "distort_point"})

  if withSeExpr:
    dlines.append("\n")
    append_file_content("SeExprArnold/src/seexpr.mtd")

  if withAnimCurve:
    dlines.append("\n")
    append_file_content("ArnoldAnimCurve/src/anim_curve.mtd.in", remap={"${prefix}anim_curve": "curve"})

  if withUserDataRamp:
    dlines.append("\n")
    append_file_content("ArnoldUserDataRamp/src/user_data_ramp.mtd.in", remap={"${prefix}user_data_ramp_f": "shape_attr_ramp_f",
                                                                               "${prefix}user_data_ramp_c3": "shape_attr_ramp_c3",
                                                                               "${prefix}user_data_ramp_v": "shape_attr_ramp_v"})
  
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
prjs = []
extra_srcs = []
instfiles = {"arnold": ["agShadingBlocks.mtd"]}

if withState:
  defs.append("WITH_STATE_SHADERS")
  prjs.append({"name": "state_shaders",
               "type": "staticlib",
               "srcs": glob.glob("ArnoldStateShaders/src/state_*.cpp"),
               "custom": [arnold.Require]})
  libs.append("state_shaders")

if withNoises:
  defs.append("WITH_NOISE_SHADERS")
  incs.append("ArnoldNoiseShaders/src")
  prjs.append({"name": "noise_shaders",
               "type": "staticlib",
               "srcs": ["ArnoldNoiseShaders/src/fractal.cpp",
                        "ArnoldNoiseShaders/src/distort_point.cpp",
                        "ArnoldNoiseShaders/src/voronoi.cpp",
                        "ArnoldNoiseShaders/src/common.cpp",] + \
                       glob.glob("ArnoldNoiseShaders/src/libnoise/*.cpp") + \
                       glob.glob("ArnoldNoiseShaders/src/stegu/*.cpp"),
               "custom": [arnold.Require]})
  libs.append("noise_shaders")

if withSeExpr:
  excons.Call("SeExprArnold/SeExpr", targets=["SeExpr"], overrides={"static": "1"})
  defs.append("WITH_SEEXPR_SHADER")
  prjs.append({"name": "seexpr_shader",
               "type": "staticlib",
               "srcs": ["SeExprArnold/src/seexpr.cpp"],
               "custom": [arnold.Require]})
  libs.extend(["seexpr_shader", "SeExpr"])
  instfiles["maya"] = instfiles.get("maya", []) + ["SeExprArnold/maya/aiSeexprTemplate.py"]

if withAnimCurve:
  defs.extend(["WITH_ANIMCURVE_SHADER"])
  prjs.append({"name": "animcurve_shader",
               "type": "staticlib",
               "srcs": ["ArnoldAnimCurve/src/anim_curve.cpp"],
               "custom": [arnold.Require]})
  libs.append("animcurve_shader")

if withUserDataRamp:
  defs.append("WITH_USERDATARAMP_SHADERS")
  incs.append("ArnoldUserDataRamp/src")
  prjs.append({"name": "userdataramp_shaders",
               "type": "staticlib",
               "srcs": glob.glob("ArnoldUserDataRamp/src/user_data_ramp*.cpp") + \
                       ["ArnoldUserDataRamp/src/common.cpp"],
               "custom": [arnold.Require]})
  libs.append("userdataramp_shaders")

make_mtd()

excons.Call("gmath", overrides={"static": "1"}, imp=["RequireGmath"])

prjs.append(
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
  })

env.Append(CPPFLAGS=" -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)
if sys.platform != "win32":
  env.Append(CPPFLAGS=" -Wno-unused-parameter")

excons.AddHelpOptions(agShadingBlocks="""AGSHADINGBLOCKS OPTIONS
  with-state=0|1             : Include state querying shaders. [1]
  with-noises=0|1            : Include noise shaders. [1]
  with-seexpr=0|1            : Include SeExpr shader. [1]
  with-animcurve=0|1         : Include animation curve shader. [1]
  with-userdataramp=0|1      : Include user data based ramp shaders. [1]
  shaders-prefix=<str>       : Set shaders prefix. ['gas_']
  maya-base-nodeid=<int>     : Override maya node ids by setting a new node base id. [-]
  maya-increasing-nodeid=0|1 : Whether node ids are generated by incrementing or decrementing the base id. [0]""")

excons.DeclareTargets(env, prjs)

dist_env, ver_dir = excons.EcosystemDist(env, "agShadingBlocks.env", {"agShadingBlocks": ""})
dist_env.Install(ver_dir, "agShadingBlocks.mtd")
if withSeExpr:
  dist_env.Install(ver_dir+"/ae", "SeExprArnold/maya/aiSeexprTemplate.py")

Default(["agShadingBlocks"])
