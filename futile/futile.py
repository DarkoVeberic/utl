#!/usr/bin/env python

__author__ = ("Darko Veberic", "Philipp Meder")
__copyright__ = "Copyright (C) 2021 Darko Veberic"
__license__ = "GPLv3"
__version__ = "1.3"


def open_pipe(command, mode="r", buff=1024*1024):
    """ Open a pipe to a subprocess specified by 'command'.
    Using a wrapper around Popen instead of returning Popen.stdin/stdout
    SHOULD result in correct working 'with'-statements (waiting for the
    process to finish). """

    text = ("b" not in mode) or ("t" in mode)
    import subprocess
    import signal

    class PopenWrapper(subprocess.Popen):
        def __enter__(self):
            return self

        def __exit__(self, exc_type, exc_val, exc_tb):
            if self.stdin:
                self.stdin.close()
            if exc_type is not None:
                # An exception occurred: terminate the subprocess so it stops
                # trying to write into the pipe, then drain any buffered output
                # to avoid a deadlock on the pipe buffer.
                self.terminate()
                if self.stdout:
                    self.stdout.read()  # drain remaining output
            self.wait()
            return False

        def __iter__(self):
            return iter(self.stdout or self.stdin)

        def __next__(self):
            return next(self.stdout or self.stdin)

        def __getattribute__(self, __name: str):
            try:
                return super().__getattribute__(__name)
            except AttributeError:
                try:
                    return self.stdin.__getattribute__(__name)
                except (AttributeError, TypeError):
                    return self.stdout.__getattribute__(__name)

    if "r" in mode:
        return PopenWrapper(
            command, shell=True, bufsize=buff, text=text, stdout=subprocess.PIPE,
            preexec_fn=lambda: signal.signal(signal.SIGPIPE, signal.SIG_DFL)
        )
    if "w" in mode:
        return PopenWrapper(
            command, shell=True, bufsize=buff, text=text, stdin=subprocess.PIPE
        )
    return None


NORMAL = 0
PROCESS = 1
PARALLEL = 2


def open_bz2(filename, mode='r', buff=1024*1024, external=PARALLEL):
    if external == NORMAL:
        # the single-letter mode of bz2.open is by default binary, change it to correspond to the built-in open()
        m = mode
        if 'b' not in m and 't' not in m:
            m += 't'
        import bz2
        return bz2.open(filename, m)
    import shutil
    if external == PROCESS:
        if not shutil.which("bzip2"):
            return open_bz2(filename, mode, buff, NORMAL)
        if 'r' in mode:
            return open_pipe("bzip2 -dc " + filename, mode, buff)
        if 'w' in mode:
            return open_pipe("bzip2 >" + filename, mode, buff)
    if external == PARALLEL:
        if not shutil.which("pbzip2"):
            return open_bz2(filename, mode, buff, PROCESS)
        if 'r' in mode:
            return open_pipe("pbzip2 -dc " + filename, mode, buff)
        if 'w' in mode:
            return open_pipe("pbzip2 >" + filename, mode, buff)
    raise Exception("Unknown value of external option!")


def open_gz(filename, mode='r', buff=1024*1024, external=PARALLEL):
    if external == NORMAL:
        # the single-letter mode of gzip.open is by default binary, change it to correspond to the built-in open()
        m = mode
        if 'b' not in m and 't' not in m:
            m += 't'
        import gzip
        return gzip.open(filename, m)
    import shutil
    if external == PROCESS:
        if not shutil.which("gzip"):
            return open_gz(filename, mode, buff, NORMAL)
        if 'r' in mode:
            return open_pipe("gzip -dc " + filename, mode, buff)
        if 'w' in mode:
            return open_pipe("gzip >" + filename, mode, buff)
    if external == PARALLEL:
        if not shutil.which("pigz"):
            return open_gz(filename, mode, buff, PROCESS)
        if 'r' in mode:
            return open_pipe("pigz -dc " + filename, mode, buff)
        if 'w' in mode:
            return open_pipe("pigz >" + filename, mode, buff)
    raise Exception("Unknown value of external option!")


def open_xz(filename, mode='r', buff=1024*1024, external=PARALLEL):
    import shutil
    if shutil.which("xz"):
        if 'r' in mode:
            return open_pipe("xz -dc " + filename, mode, buff)
        if 'w' in mode:
            return open_pipe("xz >" + filename, mode, buff)
    raise Exception("xz not found!")


def zopen(filename, mode='r', buff=1024*1024, external=PARALLEL):
    """
    Open pipe, zipped, or unzipped file automagically.
    Additionally support the Mathematica leading bang notation
    (filename starts with "!") for executing commands in a pipe
    and making their output available.

    # external == 0: normal zip libraries
    # external == 1: (zcat, gzip) or (bzcat, bzip2) or xz
    # external == 2: pigz or pbzip2 or xz
    """
    if 'r' in mode and 'w' in mode:
        return None
    if filename.startswith('!'):
        return open_pipe(filename[1:], mode, buff)
    if filename.endswith('.bz2'):
        return open_bz2(filename, mode, buff, external)
    if filename.endswith('.gz'):
        return open_gz(filename, mode, buff, external)
    if filename.endswith('.xz'):
        return open_xz(filename, mode, buff, external)
    return open(filename, mode, buff)


def pickle_loader(pklFile):
    """ Add iterators to the pickle file so it can be used as a object container """
    import pickle as pkl
    try:
        while True:
            yield pkl.load(pklFile)
    except EOFError:
        pass


def nice_time(t):
    """ Reformat a time period given as number of seconds to a human readable string

    t is in seconds
    """
    div = (60, 60, 24, 365.25)
    unit = ('', 's', 'min', 'h', 'day', 'year')
    sign = 1 if t >= 0 else -1
    tt = sign * t
    for i in range(len(div)):
        if tt < div[i]:
            break
        tt /= div[i]
    return "%.2f %s" % (sign*tt, unit[i+1])


if __name__ == "__main__":

    def compare(a, b):
        if a != b:
            print(f"a=|{a}| not equal to b=|{b}|")
            exit(1)
        else:
            print("ok")

    basename = "/tmp/futile-test"
    text = "first line\n1 2 3 foo bar baz\n a b c\nZ 13 &^%$"
    file = basename + ".out"
    with open(file, "w") as f:
        print("write..", flush=True, end="")
        print(text, file=f)
    with open(file, "r") as f:
        print("read..compare..", flush=True, end="")
        compare(text, f.read().rstrip("\n"))
    import os
    import pickle
    os.remove(file)
    for z in ["", ".gz", ".bz2", ".xz"]:
        for e in (NORMAL, PROCESS, PARALLEL):
            file = basename + ".out" + z
            print(f"z=\"{z}\" e={e} write..", flush=True, end="")
            with zopen(file, "w", external=e) as f:
                print(text, file=f)
            print("read..compare..", flush=True, end="")
            with zopen(file, "r", external=e) as f:
                compare(text, f.read().rstrip("\n"))
            print("line iterator:")
            with zopen(file, "r", external=e) as f:
                for line in f:
                    print(line, end="")
            print()
            os.remove(file)

            # test pickling
            file = basename + ".pkl" + z
            print(f"z=\"{z}\" e={e} pickle..", flush=True, end="")
            with zopen(file, "wb", external=e) as f:
                pickle.dump(text, f)
            print("unpickle..compare..", flush=True, end="")
            with zopen(file, "rb", external=e) as f:
                pkl_text = pickle.load(f)
                compare(text, pkl_text.rstrip("\n"))
            print()
            os.remove(file)
