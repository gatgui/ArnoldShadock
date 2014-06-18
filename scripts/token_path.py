import re
import os

_textureTokensExps = {}
_directoryFilesCache = {}
_texturePathExpsCache = {}

def _InitTextureTokenExps():
    global _textureTokensExps
    
    if len(_textureTokensExps) == 0:
        _textureTokensExps["t_udim"] = (re.compile("<udim(:\d+)?>"), "\d+")
        _textureTokensExps["t_tile"] = (re.compile("<tile(:\d+)?>"), "u\d+_v\d+")
        _textureTokensExps["t_utile"] = (re.compile("<utile(:\d+)?>"), "u\d+")
        _textureTokensExps["t_vtile"] = (re.compile("<vtile(:\d+)?>"), "v\d+")
        _textureTokensExps["t_sattr"] = (re.compile("<s?attr:[_a-zA-Z][\w_]*(\s+default:[^>]*)?>"), ".*")
        _textureTokensExps["t_iattr"] = (re.compile("<iattr:[_a-zA-Z][\w_]*(\s+default:(-\d)?\d*)?>"), "-?\d+")
        _textureTokensExps["t_frame"] = (re.compile("<frame(:(\d+)(,(\d+))?)?>"), None)
        _textureTokensExps["frame"] = (re.compile(r"\d+\\.(\w+)$"), None)

def ClearDirectoryFilesCache():
    global _directoryFilesCache
    
    _directoryFilesCache = {}

def ListTextures(path, isSequence=False, verbose=False):
    global _textureTokensExps
    global _directoryFilesCache
    global _texturePathExpsCache
    
    # Replace \ by /
    path = path.replace("\\", "/")
    
    fileDir = os.path.dirname(path)
    if not os.path.isdir(fileDir):
        return []
    
    fileExp = _texturePathExpsCache.get(path, None)
    
    if fileExp is None:
        if verbose:
            print("Compile new file pattern for \"%s\"" % path)
        # Make sure texture token expressions are initialized
        _InitTextureTokenExps()

        # Pattern replace
        filePattern = os.path.basename(path).replace(".", r"\.")

        for k, v in _textureTokensExps.iteritems():
            if "frame" in k:
                continue
            tokenExp, tokenPattern = v
            if tokenPattern:
                filePattern = tokenExp.sub(tokenPattern, filePattern)

        tokenFrame = False

        # Special handling for t_frame and frame
        tframeExp = _textureTokensExps.get("t_frame", None)
        if tframeExp:
            m = tframeExp[0].search(filePattern)
            while m:
                tokenFrame = True
                if m.group(1):
                    if m.group(3):
                        filePattern = filePattern[:m.start()] + ("(?P<frmnum>\d{%s}\.\d{%s})" % (m.group(2), m.group(4))) + filePattern[m.end():]
                    else:
                        filePattern = filePattern[:m.start()] + ("(?P<frmnum>\d{%s})" % m.group(2)) + filePattern[m.end():]
                else:
                    filePattern = filePattern[:m.start()] + "(?P<frmnum>\d+)" + filePattern[m.end():]
                m = tframeExp[0].search(filePattern)

        if isSequence and not tokenFrame:
            # Maya style image sequence
            frameExp = _textureTokensExps.get("frame", None)
            if frameExp:
                filePattern = frameExp[0].sub("(?P<frmnum>\d+)\.\g<1>", filePattern)
        
        # Compile final expression
        try:
            if verbose:
                print("Final pattern: \"%s\"" % filePattern)
            fileExp = re.compile(filePattern)
            _texturePathExpsCache[path] = fileExp
        except:
            return []
    else:
        if verbose:
            print("Reuse existing pattern for \"%s\": %s" % (path, fileExp.pattern))
        
    
    if fileExp.pattern == os.path.basename(path).replace(".", r"\."):
        if verbose:
            print("No tokens in \"%s\"" % path)
        return [path]
    
    try:
        lst = []
        
        fl = _directoryFilesCache.get(fileDir, None)
        if fl is None:
            fl = os.listdir(fileDir)
            fl.sort()
            if verbose:
                print("Listing files in texture directory: %s" % fileDir)
                for f in fl:
                    print("  %s" % f)
            _directoryFilesCache[fileDir] = fl
        else:
            if verbose:
                print("Reuse listed files in texture directory: %s" % fileDir)
        
        for f in fl:
            if fileExp.match(f):
                lst.append(fileDir + "/" + f)
        
        return lst

    except:
        return []

def HasMatchingTXTextures(path, isSequence, verbose=False):
    noextpath, ext = os.path.splitext(path)
    if ext.lower() == ".tx":
        return True 
    else:
        txpath = noextpath + ".tx"
        allFiles = ListTextures(path, isSequence, verbose=verbose)
        txFiles = ListTextures(txpath, isSequence, verbose=verbose)
        if len(allFiles) == len(txFiles):
            for i in xrange(len(allFiles)):
                if os.path.splitext(allFiles[i])[0] != os.path.splitext(txFiles[i])[0]:
                    return False
            return True
        else:
            return False

#
# Maya specifics
#
class Maya(object):
    @staticmethod
    def _MayaNodeTexturePath(node):
        import maya.cmds as cmds
        import maya.mel as mel
        
        if cmds.nodeType(node) == "file":
            path = cmds.getAttr(node+".fileTextureName")
            isSequence = cmds.getAttr(node+".useFrameExtension")
        
        elif cmds.attributeQuery("filename", node=node, exists=1):
            # aiImage might only support a subset of the texture tokens...
            path = cmds.getAttr(node+".filename")
            if cmds.nodeType(node) == "aiImage" and "<" in path:
                mel.warning("aiImage may only support a small subset of the texture tokens")
            isSequence = False
        
        else:
            path = None
            isSequence = False
        
        # Dirmap
        if path:
            tmp = cmds.dirmap(cd=path)
            if tmp:
                path = tmp
        
        return (path, isSequence)
    
    @staticmethod
    def ListMayaNodeTextures(node, verbose=False):
        path, isSequence = Maya._MayaNodeTexturePath(node)
        if path:
            return ListTextures(path, isSequence, verbose=verbose)
        else:
            return []
    
    @staticmethod
    def HasMayaNodeMatchingTXTextures(node, verbose=False):
        path, isSequence = Maya._MayaNodeTexturePath(node)
        if path:
            return HasMatchingTXTextures(path, isSequence, verbose=verbose)
        else:
            return False
