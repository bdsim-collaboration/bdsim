import pandas

def Bunch2Dataframe(bunch6d) :
    '''

    :param bunch6d: Input RF_Track bunch6d
    :return: DataFrame

    Function to convert a RF_Track.bunch6d into a pandas.DataFrame for debugging. Generally do not use for analysis

    '''
    id = []
    lifetime = []
    mass = []
    Pc = []
    Q = []
    S_lost = []
    t = []
    x = []
    xp = []
    y = []
    yp = []
    N = []
    pdg_id = []

    for i in range(0,bunch6d.size()) :
        p = bunch6d[i]
        id.append(p.id)
        lifetime.append(p.lifetime)
        mass.append(p.mass)
        Pc.append(p.Pc)
        Q.append(p.Q)
        S_lost.append(p.S_lost)
        t.append(p.t)
        x.append(p.x)
        xp.append(p.xp)
        y.append(p.y)
        yp.append(p.yp)
        N.append(p.N)
        pdg_id.append(p.pdg_id)

    return pandas.DataFrame({"id":id,
                             "lifetime":lifetime,
                             "mass":mass,
                             "Pc":Pc,
                             "N":N,
                             "Q":Q,
                             "S_lost":S_lost,
                             "t":t,
                             "x":x,
                             "xp":xp,
                             "y":y,
                             "yp":yp,
                             "pdg_id":pdg_id})



