#!/usr/bin/env python

import numpy as np

N = 512

#[print("%.20f," % c) for c in np.sin((np.pi*np.arange(16))/16)]


outstr = "__constant float cos" + str(N) + "[" + str(N) + "] = {\n"

outstr += ", ".join(["%.20f" % c for c in np.cos(np.pi*np.arange(N)/N)])

outstr += "\n};\n"


outstr += "__constant float sin" + str(N) + "[" + str(N) + "] = {\n"

outstr += ", ".join(["%.20f" % c for c in np.sin(np.pi*np.arange(N)/N)])

outstr += "\n};\n"


with open("sincos" + str(N) + ".h", "w") as fh:
    fh.write(outstr)
