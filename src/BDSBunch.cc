#include "BDSBunch.hh"
#include "BDSBunchGaussian.hh"
#include "BDSBunchSquare.hh"
#include "BDSBunchCircle.hh"
#include "BDSBunchRing.hh"
#include "BDSBunchEShell.hh"
#include "BDSBunchTwiss.hh"
#include "BDSBunchUserFile.hh"
#include "BDSBunchComposite.hh"
#include "BDSDebug.hh"


BDSBunch::BDSBunch() {
  // Construct default reference type 
  distribType = std::string("reference");
  bdsBunch    = new BDSBunchInterface(); 
}

BDSBunch::~BDSBunch() {
  delete bdsBunch;
}

void BDSBunch::SetOptions(struct Options& opt) {
  // check options and construct corrrect bdsBunchInterface
  delete bdsBunch;

  if (opt.distribType == "reference") 
    bdsBunch = new BDSBunchInterface();
  else if(opt.distribType == "gauss" || opt.distribType == "gaussmatrix") 
    bdsBunch = new BDSBunchGaussian(); 
  else if(opt.distribType == "square") 
    bdsBunch = new BDSBunchSquare();
  else if(opt.distribType == "circle") 
    bdsBunch = new BDSBunchCircle();
  else if(opt.distribType == "ring") 
    bdsBunch = new BDSBunchRing();
  else if(opt.distribType == "eshell") 
    bdsBunch = new BDSBunchEShell();
  else if(opt.distribType == "gausstwiss") 
    bdsBunch = new BDSBunchTwiss();
  else if(opt.distribType == "userfile"){
    bdsBunch = new BDSBunchUserFile(opt);
  }
  else if(opt.distribType == "composite") {
    bdsBunch = new BDSBunchComposite();
  }
  else {
    G4cerr << "distribType not found " << opt.distribType << G4endl;
    exit(1);
  }
  bdsBunch->SetOptions(opt);
  return;
}


void BDSBunch::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
			       G4double& xp, G4double& yp, G4double& zp,
			       G4double& t , G4double&  E, G4double& weight) {
  bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
  return;
}


