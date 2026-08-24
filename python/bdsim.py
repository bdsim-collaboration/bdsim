import subprocess as _subprocess
try:
    import rpyc as _rpyc
except ImportError:
    _rpyc = None

class BDSIMServer :
    def __init__(self, iport = 1234):
        self.iport = iport
        self.rpyc = _subprocess.Popen(["rpyc_classic", "-m","oneshot", "-p",str(self.iport)])

    def close(self):
        pass

class BDSIMClient :
    def __init__(self, iport = 1234):
        self.iport = iport
        self.conn = None

    def bdsim_module(self):
        if _rpyc is None:
            raise ImportError(
                "BDSIMClient requires the optional 'rpyc' Python package"
            )
        self.conn = _rpyc.classic.connect("localhost",port=self.iport)
        self.conn.execute("import bdsim")
        return self.conn.namespace['bdsim']

    def close(self):

        if self.conn :
            self.conn.close()
