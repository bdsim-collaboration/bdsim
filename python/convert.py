import pandas
def SamplerHits2Dataframe(hc) :
    '''

    :param hc: SamplerHits collection from BDSIM
    :return: pandas.DataFrame of the SamplerHits collection

    Function to convert a bdsim Samplerhits collection into a pandas.DataFrame for debugging. Generally do not use for analysis

    '''

    x = []
    y = []
    z = []
    xp = []
    yp = []
    zp = []
    s = []
    T = []
    totalEnergy = []
    weight = []
    Momentum = []
    Position = []

    A = []
    beamlineIndex = []
    charge = []
    eventID = []
    externalParentID = []
    externalParticleID = []
    mass = []
    momentum = []
    nElectrons = []
    parentID = []
    pdgID = []
    rigidity = []
    samplerID = []
    trackID = []
    turnsTaken = []
    Z = []

    for i in range(0,hc.entries()) :
        coords = hc[i].coords
        x.append(coords.x)
        y.append(coords.y)
        z.append(coords.z)
        xp.append(coords.xp)
        yp.append(coords.yp)
        zp.append(coords.zp)
        s.append(coords.s)
        T.append(coords.T)
        Momentum.append(coords.Momentum())
        Position.append(coords.Position())
        totalEnergy.append(coords.totalEnergy)
        weight.append(weight)

        A.append(hc[i].A)
        beamlineIndex.append(hc[i].beamlineIndex)
        charge.append(hc[i].charge)
        eventID.append(hc[i].eventID)
        externalParentID.append(hc[i].externalParentID)
        externalParticleID.append(hc[i].externalParticleID)
        mass.append(hc[i].mass)
        momentum.append(hc[i].momentum)
        nElectrons.append(hc[i].nElectrons)
        parentID.append(hc[i].parentID)
        pdgID.append(hc[i].pdgID)
        rigidity.append(hc[i].rigidity)
        samplerID.append(hc[i].samplerID)
        trackID.append(hc[i].trackID)
        turnsTaken.append(hc[i].turnsTaken)
        Z.append(hc[i].Z)

    return pandas.DataFrame({"x":x,
                             "y":y,
                             "z":z,
                             "xp":xp,
                             "yp":yp,
                             "zp":zp,
                             "s":s,
                             "T":T,
                             "Momentum":Momentum,
                             "Position":Position,
                             "totalEnergy":totalEnergy,
                             "weight":weight,
                             "A":A,
                             "beamlineIndex":beamlineIndex,
                             "charge":charge,
                             "eventID":eventID,
                             "externalParentID":externalParentID,
                             "externalParticleID":externalParticleID,
                             "mass":mass,
                             "momentum":momentum,
                             "nElectrons":nElectrons,
                             "parentID":parentID,
                             "pdgID":pdgID,
                             "rigidity":rigidity,
                             "samplerID":samplerID,
                             "trackID":trackID,
                             "turnsTaken":turnsTaken,
                             "Z":Z})
