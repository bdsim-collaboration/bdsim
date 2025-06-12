import bdsim
import numpy as _np

class TrackerInterface :
    def __init__(self,
                 bdsimConfigFile="trackerInterface.gmad",
                 referenceParticlePDG = 11,
                 referenceKineticEnergy = 100,
                 relativeEnergyCut = 0.01,
                 seed = 12345,
                 referenceIonCharge = 1,
                 batchMode = False):

        self._particle_table = bdsim.G4ParticleTable.GetParticleTable()
        self._ion_table = self._particle_table.GetIonTable()

        self._stp_link = bdsim.BDSBunchSixTrackLink()
        self._bds_link = bdsim.BDSIMLink(self._stp_link)

        bdsim_args = ["bdsim",
                      "--file="+bdsimConfigFile,
                      "--seed="+str(seed),
                      "--output=None"]

        if batchMode :
            bdsim_args.append("--batch")
        else :
            bdsim_args.append("--vis_mac=vis.mac")

        referenceKineticEnergy = referenceKineticEnergy

        if relativeEnergyCut < 1e-6:
            relativeEnergyCut = 1.0
        minimumKineticEnergy = relativeEnergyCut * referenceKineticEnergy

        print(referenceKineticEnergy, minimumKineticEnergy)
        self._bds_link.Initialise(argv = bdsim_args,
                                  usualPrintOut = True,
                                  minimumKineticEnergy = minimumKineticEnergy/bdsim.clhep.GeV,
                                  protonsAndIonsOnly = False)

        self._refParticleDefinition = self.prepareBDSParticleDefition(pdg = referenceParticlePDG,
                                                                      momentum = 0,
                                                                      kineticEnergy = referenceKineticEnergy * bdsim.clhep.GeV,
                                                                      ionCharge = referenceIonCharge);

    def prepareBDSParticleDefition(self, pdg, momentum, kineticEnergy, ionCharge):
        particleDefGeant = None
        particleDefintion = None

        if pdg < 1000000000 : # Not an ion
            particleDefGeant = self._particle_table.FindParticle(pdg)
            particleDefinition = bdsim.BDSParticleDefinition(particleDefGeant, 0,
                                                             kineticEnergy, momentum, 1, None);
        else : # Ions
            particleDefGeant = self._ion_table.GetIon(pdg)

            if ionCharge == 0 :
                ionCharge = particleDefGeant.GetAtomicNumber()
            else :
                pass

            ionDef = bdsim.BDSIonDefinition(particleDefGeant.GetAtomicMass(),
                                            particleDefGeant.GetAtomicNumber(),
                                            ionCharge)
            mass = self._ion_table.GetIonMass(ionDef.Z(), ionDef.A())
            charge = ionDef.Charge()

            bdsimPartName = "ion " + str(ionDef.A())+ \
                            " " + str(ionDef.Z())+ \
                            " " + str(charge)

            particleDefinition = bdsim.BDSParticleDefinition(bdsimPartName, mass, charge, 0,
                                                             kineticEnergy, momentum, 1, ionDef, pdg)


        return particleDefinition

    @property
    def bds_link(self):
        return self._bds_link

    @property
    def stp_link(self):
        return self._stp_link

    @property
    def particle_table(self):
        return self._particle_table

    @property
    def ion_table(self):
        return self._ion_table

    @property
    def ref_particle(self):
        return self._refParticleDefinition

    def addParticlePython(self,
                          x, y, px, py,
                          ct, deltap, chi,
                          chargeRatio, s,
                          trackid, pdgID
                          ):

        q = chargeRatio * self._refParticleDefinition.Charge()
        mass_ratio = chargeRatio / chi
        p = self._refParticleDefinition.Momentum() * (deltap + 1) * mass_ratio

        pdg = 0
        if pdgID == 0 :
            pdg = self._refParticleDefinition.PDGID()
        else :
            pdg = pdgID

        partDef = self.prepareBDSParticleDefition(pdg, p, 0, q);
        t = - ct * bdsim.clhep.m / (self._refParticleDefinition.Beta() * bdsim.clhep.c_light)
        oneplusdelta = (1 + deltap)
        xp = px / oneplusdelta
        yp = py / oneplusdelta
        zp = _np.sqrt(1 - xp**2 - yp**2)

        coords = bdsim.BDSParticleCoordsFull(x * bdsim.clhep.m,
                                             y * bdsim.clhep.m,
                                             0,
                                             xp,
                                             yp,
                                             zp,
                                             t,
                                             0,
                                             partDef.TotalEnergy(),
                                             1)

        return self._stp_link.AddParticle(partDef, coords, trackid, trackid)
    def addParticlesPython(self, pdgIn = [],
                           xIn = [], yIn = [],
                           xpIn = [], ypIn = [],
                           zIn = [], tIn = [],
                           sIn =[], totalEnergyIn = [],
                           weightIn = []):

        '''Add particles to link, using a python loop. Not to be used in large scale production. Here
        to test all the binding code'''
        for i in range(len(pdgIn)) :
            pdg = pdgIn[i]
            particleDefGeant = self._particle_table.FindParticle(pdg);

def test_TrackerInterface() :
    ti = TrackerInterface()

    # test electron
    electron = ti.prepareBDSParticleDefition(11, 100*bdsim.clhep.GeV, 0, 0)

    # test ion
    ion  = ti.prepareBDSParticleDefition(1000501000,100, 0, 0)


    icol1 = ti.bds_link.AddLinkCollimatorJaw("col1",
                                             "G4_Fe",
                                             0.25*bdsim.clhep.m,
                                             0.0*bdsim.clhep.m,
                                             0.0*bdsim.clhep.m,
                                             0,
                                             0,
                                             0)


    iCol2 = ti.bds_link.AddLinkCollimatorJaw("col2",
                                             "G4_Fe",
                                             1.0*bdsim.clhep.m,
                                             0.0*bdsim.clhep.m,
                                             0.0*bdsim.clhep.m,
                                             0,
                                             0,
                                             0)

    e = bdsim.Element()
    e.name = "drift1"
    e.type = bdsim.elementtype.ElementType.DRIFT
    e.l = 1.0

    iDri1 = ti.bds_link.AddLinkElement(e)

    # add single particle
    pd = ti.addParticlePython(x=0, y=0, px=0, py=0, ct=0, deltap=0, chi = 1,
                              chargeRatio=1 , s=0,
                              trackid=1 , pdgID=11)


    ti.bds_link.SelectLinkElement(0)
    ti.bds_link.BeamOn(1)

    sh = ti.bds_link.SamplerHits()
    print("number of sampler hits", sh.entries())
    sh.PrintAllHits()


    return ti

if __name__ == "__main__":
    test_TrackerInterface()
