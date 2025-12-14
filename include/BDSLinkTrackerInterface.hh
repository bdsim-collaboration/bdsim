#ifndef BDSLINKTRACKERINTERFACE_H
#define BDSLINKTRACKERINTERFACE_H

#include <string>

class G4ParticleTable;
class G4IonTable;
class BDSLinkBunch;
class BDSIMLink;
class BDSParticleDefinition;

class BDSLinkTrackerInterface {
public:

  static BDSLinkTrackerInterface* GetInstance(std::string bdsimConfigFileIn,
                                              int referenceParticlePDGIn = 11,
                                              double referenceKineticEnergyIn = 100,
                                              double relativeEnergyCutIn = 0.01,
                                              int seedIn = 1234,
                                              int referenceIonChargeIn = 1,
                                              bool batchModeIn = true);
  static BDSLinkTrackerInterface* GetInstance();

  ~BDSLinkTrackerInterface();
  void Reset();

  BDSParticleDefinition* PrepareBDSParticleDefinition(int pdg,
                                                      double totalEnergy,
                                                      double momentum,
                                                      double kineticEnergy,
                                                      int ionCharge);
  BDSParticleDefinition* PrepareBDSParticleDefinition_Bjorn(int pdg,
                                                            double momentum,
                                                            double kineticEnergy,
                                                            int ionCharge);

  BDSParticleDefinition* GetReferenceParticleDefinition() {return referenceParticleDefinition;}
  void SetReferenceParticleDefinition(BDSParticleDefinition *rpd) {referenceParticleDefinition = rpd;}

  std::string GetBDSIMConfigFile() {return bdsimConfigFile;}
  int GetReferenceParticlePDG() {return referenceParticlePDG;}
  double GetReferenceParticleKineticEnergy() {return referenceKineticEnergy;}
  double GetRelativeEnergyCut() {return relativeEnergyCut;}
  int GetSeed() {return seed;}
  int GetReferenceIonCharge() {return referenceIonCharge;}
  bool GetBatchMode() {return batchMode;}
  double GetMinimumKineticEnergy() {return minimumKineticEnergy;}

  void SetNoNeutralParticles(bool val) { noNeutralParticles = val; }
  bool GetNoNeutralParticles() const { return noNeutralParticles; }

  BDSLinkBunch* GetBunchLink() {return linkBunch;}
  BDSIMLink* GetBDSIMLink() {return linkBDSIM;}

  void AddParticle(double x, double y, double px, double py,
                   double ct, double deltap, double chi,
                   double chargeRatio, double s,
                   int trackid, int pdgid);
  void AddParticle(double x, double y, double px, double py,
                   double ct, double pc, double s,
                   int trackid, int pdgid);
  void AddParticles(std::vector<double> x, std::vector<double> y,
                    std::vector<double> px, std::vector<double> py,
                    std::vector<double> ct, std::vector<double> deltap,
                    std::vector<double> chi, std::vector<double> chargeRatio,
                    std::vector<double> s, std::vector<int> trackid,
                    std::vector<int> pdgid);
  void AddParticles(std::vector<double> x, std::vector<double> y,
                    std::vector<double> px, std::vector<double> py,
                    std::vector<double> pz, std::vector<double> t,
                    std::vector<double> s, std::vector<int> trackid,
                    std::vector<int> pdgid);

  void ClearData();

  // TODO is this needed?
  std::vector<bool>& GetParticleActiveState() { return particleActiveState; }

protected:

  // singleton private constructor
  BDSLinkTrackerInterface(std::string bdsimConfigFileIn = "trackerInterface.gmad",
                          int referenceParticlePDGIn = 11,
                          double referenceKineticEnergyIn = 100,
                          double relativeEnergyCutIn = 0.01,
                          int seedIn = 1234,
                          int referenceIonChargeIn = 1,
                          bool batchModeIn = true);

  // singleton pointer
  static BDSLinkTrackerInterface* singleton;

  // singleton pattern
  BDSLinkTrackerInterface() = default;
  BDSLinkTrackerInterface(const BDSLinkTrackerInterface&) = delete;
  BDSLinkTrackerInterface& operator=(const BDSLinkTrackerInterface&) = delete;

  // configuration parameters
  std::string bdsimConfigFile;
  int referenceParticlePDG;
  double referenceKineticEnergy;
  double relativeEnergyCut;
  int seed;
  int referenceIonCharge;
  bool batchMode;

  // other parameters
  bool noNeutralParticles = true;  // default value

  // derived parmeters
  double minimumKineticEnergy;

  // Handy G4 objects
  G4ParticleTable *g4particle_table;
  G4IonTable *g4ion_table;

  // link objects
  BDSLinkBunch *linkBunch;
  BDSIMLink *linkBDSIM;

  // reference particle
  BDSParticleDefinition* referenceParticleDefinition;

  // todo, what is this for?
  std::vector<bool> particleActiveState;
};

#endif