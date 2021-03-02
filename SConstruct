import sys
import glob
import re
import os
import excons
import shutil
import excons.config
from excons.tools import arnold

# SeExpr v3 requires c++11 compatible compiler
ARGUMENTS["use-c++11"] = ARGUMENTS.get("with-seexpr", "1")

env = excons.MakeBaseEnv()

arniver = arnold.Version(asString=False)
if arniver[0] < 5:
  print("shadock requires at least Arnold 5.0.0.0")
  sys.exit(1)

version = "1.1.1"

withState = (excons.GetArgument("with-state", 1, int) != 0)
withNoises = (excons.GetArgument("with-noises", 1, int) != 0)
withSeExpr = (excons.GetArgument("with-seexpr", 1, int) != 0)
withAnimCurve = (excons.GetArgument("with-animcurve", 1, int) != 0)
withUserDataRamp = (excons.GetArgument("with-userdataramp", 1, int) != 0)
shdprefix = excons.GetArgument("shaders-prefix", "shk_")

minMayaId = None
maxMayaId = None
mayaIds = {} # mayaId -> line number

def make_mtd_in():
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

  append_file_content("src/shadock.mtd")

  if withState:
    dlines.append("\n")
    append_file_content("ArnoldStateShaders/src/state.mtd.in", remap={"@PREFIX@state_v": "globals_v",
                                                                      "@PREFIX@state_f": "globals_f",
                                                                      "@PREFIX@state_c3": "globals_c3",
                                                                      "@PREFIX@state_i": "globals_i",
                                                                      "@PREFIX@state_m": "globals_m",
                                                                      "@PREFIX@state_n": "globals_n"})

  if withNoises:
    dlines.append("\n")
    append_file_content("ArnoldNoiseShaders/src/noise.mtd.in", remap={"@PREFIX@fractal": "fractal_noise",
                                                                      "@PREFIX@voronoi": "voronoi_noise",
                                                                      "@PREFIX@distort_point": "distort_point"})

  if withSeExpr:
    dlines.append("\n")
    append_file_content("SeExprArnold/src/seexpr.mtd.in", remap={"@PREFIX@seexpr": "seexpr"})

  if withAnimCurve:
    dlines.append("\n")
    append_file_content("ArnoldAnimCurve/src/anim_curve.mtd.in", remap={"@PREFIX@anim_curve": "curve"})

  if withUserDataRamp:
    dlines.append("\n")
    append_file_content("ArnoldUserDataRamp/src/user_data_ramp.mtd.in", remap={"@PREFIX@user_data_ramp_f": "shape_attr_ramp_f",
                                                                               "@PREFIX@user_data_ramp_c3": "shape_attr_ramp_c3",
                                                                               "@PREFIX@user_data_ramp_v": "shape_attr_ramp_v"})
  
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
  
  outpath = "shadock.mtd.in"
  
  df = open(outpath, "w")
  for line in dlines:
    df.write(line)
  df.write("\n")
  df.close()

  return outpath


def to_maya_name(name, capitalize=False):
  spl = name.split("_")
  name = spl[0] + "".join(map(lambda x: x[0].upper() + x[1:], spl[1:]))
  return (name[0].upper() + name[1:]) if capitalize else name


excons.Call("gmath", overrides={"static": "1"}, imp=["RequireGmath"])

aes = []
defs = []
incs = []
libs = []
prjs = []
reqs = [RequireGmath, arnold.Require]
extra_srcs = []
instfiles = {}
opts = {"PREFIX": shdprefix}

GenerateMtd = excons.config.AddGenerator(env, "mtd", opts)

mtd = GenerateMtd("shadock.mtd", make_mtd_in())
instfiles["arnold"] = mtd

tpls = []
for item in excons.glob("src/*.cpp"):
  bn = os.path.splitext(os.path.basename(item))[0]
  mbn = to_maya_name(bn, capitalize=True)
  tplin = "maya/%sTemplate.py.in" % mbn
  if not os.path.isfile(tplin):
    if bn not in ("common", "main", "strings"):
      print("No template for shader '%s'" % item)
  else:
    mn = to_maya_name(shdprefix + bn)
    opts["%s_MAYA_NODENAME" % mbn.upper()] = mn
    tpls.append(("maya/%sTemplate.py" % mn, tplin))

GenerateMayaAE = excons.config.AddGenerator(env, "mayaAE", opts)

aes = [GenerateMayaAE(x[0], x[1]) for x in tpls]

if withState:
  defs.append("WITH_STATE_SHADERS")
  prjs.append({"name": "state_shaders",
               "type": "staticlib",
               "prefix": "arnold-%s" % arnold.Version(compat=True),
               "bldprefix": "arnold-%s" % arnold.Version(compat=True),
               "srcs": glob.glob("ArnoldStateShaders/src/state_*.cpp"),
               "custom": [arnold.Require]})
  libs.append("state_shaders")
  for sn in ("state_i", "state_f", "state_v", "state_c3", "state_m", "state_n"):
    mbn = to_maya_name(sn, capitalize=True)
    mn = to_maya_name(shdprefix + sn.replace("state_", "globals_"))
    opts["%s_MAYA_NODENAME" % mbn.upper()] = mn
    aes += GenerateMayaAE("maya/%sTemplate.py" % mn, "ArnoldStateShaders/maya/%sTemplate.py.in" % mbn)

if withNoises:
  defs.append("WITH_NOISE_SHADERS")
  incs.append("ArnoldNoiseShaders/src")
  prjs.append({"name": "noise_shaders",
               "type": "staticlib",
               "prefix": "arnold-%s" % arnold.Version(compat=True),
               "bldprefix": "arnold-%s" % arnold.Version(compat=True),
               "srcs": ["ArnoldNoiseShaders/src/fractal.cpp",
                        "ArnoldNoiseShaders/src/distort_point.cpp",
                        "ArnoldNoiseShaders/src/voronoi.cpp",
                        "ArnoldNoiseShaders/src/common.cpp",] + \
                       glob.glob("ArnoldNoiseShaders/src/libnoise/*.cpp") + \
                       glob.glob("ArnoldNoiseShaders/src/stegu/*.cpp"),
               "custom": [arnold.Require]})
  libs.append("noise_shaders")
  for sn in ("fractal", "voronoi", "distort_point"):
    mbn = to_maya_name(sn, capitalize=True)
    mn = to_maya_name(shdprefix + sn + ("_noise" if sn != "distort_point" else ""))
    opts["%s_MAYA_NODENAME" % mbn.upper()] = mn
    aes += GenerateMayaAE("maya/%sTemplate.py" % mn, "ArnoldNoiseShaders/maya/%sTemplate.py.in" % mbn)

if withSeExpr:
  # allow caching of flags queried by SeExpr
  excons.Call("SeExprArnold/SeExpr", targets=["SeExpr2"], imp=["RequireSeExpr2"], keepflags=["llvm"])
  defs.append("WITH_SEEXPR_SHADER")
  prjs.append({"name": "seexpr_shader",
               "type": "staticlib",
               "prefix": "arnold-%s" % arnold.Version(compat=True),
               "bldprefix": "arnold-%s" % arnold.Version(compat=True),
               "srcs": ["SeExprArnold/src/seexpr.cpp"],
               "custom": [RequireSeExpr2, arnold.Require]})
  libs.extend(["seexpr_shader"])
  reqs.insert(0, RequireSeExpr2)
  mn = to_maya_name(shdprefix + "seexpr")
  opts["SEEXPR_MAYA_NODENAME"] = mn
  aes += GenerateMayaAE("maya/%sTemplate.py" % mn, "SeExprArnold/maya/SeexprTemplate.py.in")

if withAnimCurve:
  defs.extend(["WITH_ANIMCURVE_SHADER"])
  prjs.append({"name": "animcurve_shader",
               "type": "staticlib",
               "prefix": "arnold-%s" % arnold.Version(compat=True),
               "bldprefix": "arnold-%s" % arnold.Version(compat=True),
               "srcs": ["ArnoldAnimCurve/src/anim_curve.cpp"],
               "custom": [RequireGmath, arnold.Require]})
  libs.append("animcurve_shader")
  mbn = to_maya_name("anim_curve", capitalize=True)
  mn = to_maya_name(shdprefix + "curve")
  opts["%s_MAYA_NODENAME" % mbn.upper()] = mn
  aes += GenerateMayaAE("maya/%sTemplate.py" % mn, "ArnoldAnimCurve/maya/%sTemplate.py.in" % mbn)

if withUserDataRamp:
  defs.append("WITH_USERDATARAMP_SHADERS")
  incs.append("ArnoldUserDataRamp/src")
  prjs.append({"name": "userdataramp_shaders",
               "type": "staticlib",
               "prefix": "arnold-%s" % arnold.Version(compat=True),
               "bldprefix": "arnold-%s" % arnold.Version(compat=True),
               "srcs": glob.glob("ArnoldUserDataRamp/src/user_data_ramp*.cpp") + \
                       ["ArnoldUserDataRamp/src/common.cpp"],
               "custom": [arnold.Require]})
  libs.append("userdataramp_shaders")
  for sn in ("user_data_ramp_f", "user_data_ramp_v", "user_data_ramp_c3"):
    mbn = to_maya_name(sn, capitalize=True)
    mn = to_maya_name(shdprefix + sn.replace("user_data_ramp_", "shape_attr_ramp_"))
    opts["%s_MAYA_NODENAME" % mbn.upper()] = mn
    aes += GenerateMayaAE("maya/%sTemplate.py" % mn, "ArnoldUserDataRamp/maya/%sTemplate.py.in" % mbn)

instfiles["maya"] = aes



prjs.append(
  {"name": "shadock",
   "prefix": "arnold/%s" % arnold.Version(compat=True),
   "bldprefix": "arnold-%s" % arnold.Version(compat=True),
   "type": "dynamicmodule",
   "defs": defs,
   "incdirs": incs,
   "ext": arnold.PluginExt(),
   "libs": libs,
   "libdirs": ["%s/lib/arnold-%s" % (excons.OutputBaseDirectory(), arnold.Version(compat=True))],
   "srcs": glob.glob("src/*.cpp") + extra_srcs,
   "install": instfiles,
   "custom": reqs
  })


env.Append(CPPFLAGS=" -DSHADERS_PREFIX=\"\\\"%s\\\"\"" % shdprefix)
if sys.platform != "win32":
  env.Append(CPPFLAGS=" -Wno-unused-parameter")
else:
  env.Append(CPPFLAGS=" /wd4100") # unreferenced format parameter


excons.AddHelpOptions(shadock="""SHADOCK OPTIONS
  with-state=0|1             : Include state querying shaders. [1]
  with-noises=0|1            : Include noise shaders. [1]
  with-seexpr=0|1            : Include SeExpr shader. [1]
  with-animcurve=0|1         : Include animation curve shader. [1]
  with-userdataramp=0|1      : Include user data based ramp shaders. [1]
  shaders-prefix=<str>       : Set shaders prefix. ['shk_']
  maya-base-nodeid=<int>     : Override maya node ids by setting a new node base id. [-]
  maya-increasing-nodeid=0|1 : Whether node ids are generated by incrementing or decrementing the base id. [0]""")

excons.DeclareTargets(env, prjs)

dist_env, ver_dir = excons.EcosystemDist(env, "shadock.env", {"shadock": ""}, version=version)
dist_env.Install(ver_dir, mtd)
dist_env.Install(ver_dir + "/maya/ae", aes)

Default(["shadock"])

