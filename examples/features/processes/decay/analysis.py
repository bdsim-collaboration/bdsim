import pybdsim

from collections import defaultdict


def Analysis(f):
    d = pybdsim.Data.Load(f)

    nproducts = defaultdict(set)
    for event in d.GetEventTree():
        nproducts[event.d1.n].add(tuple(event.d1.partID))

    for k,v in nproducts.items():
        print(k, v)
                
