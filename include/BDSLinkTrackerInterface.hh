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
  void SetNoNeutralParticles(bool val) { noNeutralParticles = val; }
  bool GetNoNeutralParticles() const { return noNeutralParticles; }

  void ClearXtrackData();

  ~BDSLinkTrackerInterface() = default;

  static BDSLinkTrackerInterface* GetInstance(std::string bdsimConfigFileIn,
                                              int referenceParticlePDGIn = 11,
                                              double referenceKineticEnergyIn = 100,
                                              double relativeEnergyCutIn = 0.01,
                                              int seedIn = 1234,
                                              int referenceIonChargeIn = 1,
                                              bool batchModeIn = true);
  static BDSLinkTrackerInterface* GetInstance();

  std::string GetBDSIMConfigFile() {return bdsimConfigFile;}
  int GetReferenceParticlePDG() {return referenceParticlePDG;}
  double GetReferenceParticleKineticEnergy() {return referenceKineticEnergy;}
  double GetRelativeEnergyCut() {return relativeEnergyCut;}
  int GetSeed() {return seed;}
  int GetReferenceIonCharge() {return referenceIonCharge;}
  bool GetBatchMode() {return batchMode;}
  double GetMinimumKineticEnergy() {return minimumKineticEnergy;}

  BDSLinkBunch* GetBunchLink() {return linkBunch;}
  BDSIMLink* GetBDSIMLink() {return linkBDSIM;}

  BDSParticleDefinition* GetReferenceParticleDefinition() const;
  void SetReferenceParticleDefinition(BDSParticleDefinition *rpd) {referenceParticleDefinition = rpd;}

  BDSParticleDefinition* prepareBDSParticleDefition(int pdg,
                                                    double momentum,
                                                    double kineticEnergy,
                                                    int ionCharge);
  void AddParticle(double x, double y, double px, double py,
                   double ct, double deltap, double chi,
                   double chargeRatio, double s,
                   int trackid, int pdgid);
  void AddParticles(std::vector<double> x, std::vector<double> y,
                    std::vector<double> px, std::vector<double> py,
                    std::vector<double> ct, std::vector<double> deltap,
                    std::vector<double> chi, std::vector<double> chargeRatio,
                    std::vector<double> s, std::vector<int> trackid,
                    std::vector<int> pdgid);

  // TODO is this needed?
  std::vector<bool>& GetParticleActiveState() { return particleActiveState; }

protected:
    bool noNeutralParticles = true;  // default value
  // singleton pointer
  static BDSLinkTrackerInterface* singleton;

  // singleton pattern
  BDSLinkTrackerInterface() = default;
  BDSLinkTrackerInterface(const BDSLinkTrackerInterface&) = delete;
  BDSLinkTrackerInterface& operator=(const BDSLinkTrackerInterface&) = delete;

  // singleton private constructor
  BDSLinkTrackerInterface(std::string bdsimConfigFileIn = "trackerInterface.gmad",
                          int referenceParticlePDGIn = 11,
                          double referenceKineticEnergyIn = 100,
                          double relativeEnergyCutIn = 0.01,
                          int seedIn = 1234,
                          int referenceIonChargeIn = 1,
                          bool batchModeIn = true);

  // configuration parameters
  std::string bdsimConfigFile;
  int referenceParticlePDG;
  double referenceKineticEnergy;
  double relativeEnergyCut;
  int seed;
  int referenceIonCharge;
  bool batchMode;

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