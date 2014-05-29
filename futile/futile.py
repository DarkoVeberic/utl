def which(program):
    import os
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)
    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None


NORMAL = 0
PROCESS = 1
PARALLEL = 2

# open pipe, zipped, or unzipped file automagically
def zopen(filename, mode='r', buff=1024*1024, external=PARALLEL):
    # external == 0: normal zip libraries
    # external == 1: (zcat, gzip) or (bzcat, bzip2)
    # external == 2: (pigz -dc, pigz) or (pbzip2 -dc, pbzip2)
    if 'r' in mode and 'w' in mode:
        return None
    if filename.startswith('!'):
        import subprocess
        if 'r' in mode:
            return subprocess.Popen(filename[1:], shell=True, bufsize=buff,
                                    stdout=subprocess.PIPE).stdout
        elif 'w' in mode:
            return subprocess.Popen(filename[1:], shell=True, bufsize=buff,
                                    stdin=subprocess.PIPE).stdin
    elif filename.endswith('.bz2'):
        if external == NORMAL:
            import bz2
            return bz2.BZ2File(filename, mode, buff)
        elif external == PROCESS:
            if not which('bzip2'):
                return anyOpen(filename, mode, buff, NORMAL)
            if 'r' in mode:
                return anyOpen('!bzip2 -dc ' + filename, mode, buff)
            elif 'w' in mode:
                return anyOpen('!bzip2 >' + filename, mode, buff)
        elif external == PARALLEL:
            if not which('pbzip2'):
                return anyOpen(filename, mode, buff, PROCESS)
            if 'r' in mode:
                return anyOpen('!pbzip2 -dc ' + filename, mode, buff)
            elif 'w' in mode:
                return anyOpen('!pbzip2 >' + filename, mode, buff)
    elif filename.endswith('.gz'):
        if external == NORMAL:
            import gzip
            return gzip.GzipFile(filename, mode, buff)
        elif external == PROCESS:
            if not which('gzip'):
                return anyOpen(filename, mode, buff, NORMAL)
            if 'r' in mode:
                return anyOpen('!gzip -dc ' + filename, mode, buff)
            elif 'w' in mode:
                return anyOpen('!gzip >' + filename, mode, buff)
        elif external == PARALLEL:
            if not which('pigz'):
                return anyOpen(filename, mode, buff, PROCESS)
            if 'r' in mode:
                return anyOpen('!pigz -dc ' + filename, mode, buff)
            elif 'w' in mode:
                return anyOpen('!pigz >' + filename, mode, buff)
    elif filename.endswith('.xz'):
        if which('xz'):
            if 'r' in mode:
                return anyOpen('!xz -dc ' + filename, mode, buff)
            elif 'w' in mode:
                return anyOpen('!xz >' + filename, mode, buff)
    else:
        return open(filename, mode, buff)
    return None


def pickle_loader(pklFile):
    import cPickle as pkl
    try:
        while True:
            yield pkl.load(pklFile)
    except EOFError:
        pass


# t is in seconds
def nice_time(t):
    div = (60., 60., 24., 365.25)
    unit = ('', 's', 'min', 'h', 'day', 'year')
    sign = 1 if t >= 0 else -1
    tt = sign*t
    for i in xrange(len(div)):
        if tt < div[i]:
            break
        tt /= div[i]
    return "%.2f %s" % (sign*tt, unit[i+1])
