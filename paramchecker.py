import os
import re
import sys
import glob

if __name__ == "__main__":
    srcs = glob.glob("src/*.cpp")
    srcs.sort()
    
    allowed = set(["default", "_default", "operator", "_operator"])
    penum = re.compile(r"^\s*p_([^\s,=]+)")
    pdecl = re.compile(r"^\s*AiParameter[^(]+\((?:(?:\"([^\"]+)\")|SSTR::([^\s,]+))")
    
    for src in srcs:
        with open(src, "r") as f:
            lines = f.readlines()
        
        process = False
        pnames = []
        pdecls = []
        
        for line in lines:
            if "AI_SHADER_NODE_EXPORT_METHODS" in line:
                process = True
            
            elif process:
                m = penum.search(line)
                if m is not None:
                    pnames.append(m.group(1))
                
                else:
                    m = pdecl.search(line)
                    if m is not None:
                        if m.group(1):
                            name = m.group(1)
                        else:
                            name = m.group(2)
                        pdecls.append(name)
        
        if pnames != pdecls:
            n = set(pnames)
            d = set(pdecls)
            diff = filter(lambda x: x not in allowed, d.symmetric_difference(n))
            if len(diff) > 0:
                print("'%s' is invalid" % src)
                print("  Enums: %s" % pnames)
                print("  Decls: %s" % pdecls)
                break