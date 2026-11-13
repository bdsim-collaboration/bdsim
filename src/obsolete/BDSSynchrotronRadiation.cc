/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
*/

#include "BDSGlobalConstants.hh" 
#include "BDSSynchrotronRadiation.hh"
#include "BDSBeamline.hh"
#include "BDSDebug.hh"

#include "G4AffineTransform.hh"
#include "G4Field.hh"
#include "G4FieldManager.hh"
#include "G4Gamma.hh"
#include "G4ios.hh"
#include "G4PropagatorInField.hh"
#include "G4TransportationManager.hh"
#include "Randomize.hh" 
#include "CLHEP/Units/PhysicalConstants.h"

// #define BDSDEBUG 

BDSSynchrotronRadiation::BDSSynchrotronRadiation(const G4String& processName) : G4VDiscreteProcess(processName) { // initialization 
  nExpConst=5*CLHEP::fine_structure_const/(2*sqrt(3.0))/CLHEP::electron_mass_c2;
  CritEngFac=3./2.*CLHEP::hbarc/pow(CLHEP::electron_mass_c2,3);
  MeanFreePathCounter = 0;
}

G4VParticleChange* BDSSynchrotronRadiation::PostStepDoIt(const G4Track& trackData,
							 const G4Step& stepData) {
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Start " << G4endl;
#endif
  aParticleChange.Initialize(trackData);

  //Allow reweight of the synchrotron photons
  aParticleChange.SetSecondaryWeightByProcess(true);

  G4double eEnergy=trackData.GetTotalEnergy();
  
  //G4double R=BDSLocalRadiusOfCurvature;
  
  G4double NewKinEnergy = trackData.GetKineticEnergy();
  
  G4double GamEnergy=0;
  
  MeanFreePathCounter++;
  
  const G4DynamicParticle* aDynamicParticle=trackData.GetDynamicParticle();
  
  G4double gamma = aDynamicParticle->GetTotalEnergy()/aDynamicParticle->GetMass();
  
  if(gamma <= 1.0e3) {
#ifdef BDSDEBUG 
    G4cout << __METHOD_NAME__ << "Gamma<1000" << G4endl;
#endif
    return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
  }
  G4double particleCharge = aDynamicParticle->GetCharge();
  
  G4ThreeVector  FieldValue;
  const G4Field*   pField = 0 ;
  
  G4FieldManager* fieldMgr=nullptr;
  G4bool          fieldExertsForce = false;
  
  G4TransportationManager* transportMgr = G4TransportationManager::GetTransportationManager();
  G4PropagatorInField* fFieldPropagator = transportMgr->GetPropagatorInField();

  if( (particleCharge != 0.0) ) {
#ifdef BDSDEBUG 
    G4cout << __METHOD_NAME__ << "Particle charge != 0.0" << G4endl;
#endif
    fieldMgr = fFieldPropagator->FindAndSetFieldManager(trackData.GetVolume());
    if ( fieldMgr != 0 )
      {
	// If the field manager has no field, there is no field !	  
	fieldExertsForce = ( fieldMgr->GetDetectorField() != 0 );
      }
  }
  if(fieldExertsForce) {
#ifdef BDSDEBUG 
    G4cout << __METHOD_NAME__ << "fieldExertsForce==true" << G4endl;
#endif
      pField = fieldMgr->GetDetectorField() ;

      G4ThreeVector  globPosition = trackData.GetPosition() ;
      G4double       globPosVec[3], FieldValueVec[3] ;

      globPosVec[0] = globPosition.x() ;
      globPosVec[1] = globPosition.y() ;
      globPosVec[2] = globPosition.z() ;
      
      pField->GetFieldValue( globPosVec, FieldValueVec ) ;
      FieldValue = G4ThreeVector(FieldValueVec[0],
				 FieldValueVec[1],
				 FieldValueVec[2]);
      
      G4ThreeVector unitMomentum = aDynamicParticle->GetMomentumDirection();
      G4ThreeVector unitMcrossB = FieldValue.cross(unitMomentum);
      G4double perpB = unitMcrossB.mag() ;
      std::vector<G4DynamicParticle*> listOfSecondaries;
      if(perpB > 0.0) {
#ifdef BDSDEBUG 
	G4cout << __METHOD_NAME__ << "perpB>0 " << G4endl;
#endif
	//Loop over multiplicity
	for (int i=0; i<BDSGlobalConstants::Instance()->GetSynchPhotonMultiplicity(); i++){
#ifdef BDSDEBUG 
	  G4cout  << __METHOD_NAME__ << "SynchPhotonMultiplicity" << G4endl;
#endif
	  //if(fabs(R)==0)
	  G4double R=(aDynamicParticle->GetTotalMomentum()/CLHEP::GeV)/
	    (0.299792458*particleCharge*perpB);
	  GamEnergy=SynGenC(BDSGlobalConstants::Instance()->GetSynchLowX())*
	    CritEngFac*pow(eEnergy,3)/fabs(R);
	  
	  if(GamEnergy>0 && GamEnergy < NewKinEnergy) {
#ifdef BDSDEBUG 
	    G4cout << __METHOD_NAME__ << "GamEnergy in range" << G4endl;
#endif
	    if((BDSGlobalConstants::Instance()->GetSynchTrackPhotons())&&(GamEnergy>BDSGlobalConstants::Instance()->GetSynchLowGamE()) ) {
#ifdef BDSDEBUG 
	      G4cout << __METHOD_NAME__ << "TrackPhotons" << G4endl;
#endif
	      G4DynamicParticle* aGamma= new G4DynamicParticle(G4Gamma::Gamma(), 
							       trackData.GetMomentumDirection(),
							       GamEnergy);
	      listOfSecondaries.push_back(aGamma);
#ifdef BDSDEBUG 
	      G4cout << __METHOD_NAME__ << "Creating synchRad photon of energy : " << GamEnergy << G4endl;
#endif
	    }
	    
	    //BDSBeamline::const_iterator iBeam;
	    //G4double zpos=trackData.GetPosition().z();
	    
	    //   for(iBeam=BDSBeamline::Instance()->begin();
	    // 	  iBeam!=BDSBeamline::Instance()->end() && zpos>=(*iBeam)->GetZUpper(); 
	    // 	  iBeam++){}
	    
	    if(i==0 && MeanFreePathCounter==1) NewKinEnergy -= GamEnergy;
	    
	    if (NewKinEnergy > 0.) {
#ifdef BDSDEBUG 
	      G4cout << __METHOD_NAME__ << "NewKinEnergy>0, NewKinEnergy=" << NewKinEnergy << G4endl;
#endif
	      // Update the incident particle 
	      aParticleChange.ProposeEnergy(NewKinEnergy);
	    } 
	    else { 
	      aParticleChange.ProposeEnergy(0.);
	      aParticleChange.ProposeLocalEnergyDeposit(0.);
	      G4double charge= trackData.GetDynamicParticle()->GetCharge();
	      if (charge<0.) aParticleChange.ProposeTrackStatus(fStopAndKill);
	      else       aParticleChange.ProposeTrackStatus(fStopButAlive);
	    }
	  }
	}
	aParticleChange.SetNumberOfSecondaries(listOfSecondaries.size());
	
	for(unsigned int n=0;n<listOfSecondaries.size();n++) {
#ifdef BDSDEBUG 
	  G4cout << __METHOD_NAME__ << "Adding secondaries" << G4endl;
#endif              
	  aParticleChange.AddSecondary(listOfSecondaries[n]); 
#ifdef BDSDEBUG 
	  G4cout << __METHOD_NAME__ << "Added secondary particle...\n";
#endif
	}
	//Find out the weight of the gammas
	G4double gammaWeight = aParticleChange.GetParentWeight()/BDSGlobalConstants::Instance()->GetSynchPhotonMultiplicity();
	//Set the weights of the gammas
	for(unsigned int n=0;n<listOfSecondaries.size();n++){
	  aParticleChange.GetSecondary(n)->SetWeight(gammaWeight);
	}
      }
  }
  return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
}

BDSSynchrotronRadiation::~BDSSynchrotronRadiation(){
}

//--------------------------------------------------------------------
// routines from H. Burkhardt (Lep Note 632)
// C++ version
// synchrotron radiation photon spectrum generator
// returns photon energy in units of the critical energy
// xmin is the lower limit for the photon energy to be generated
//          see H.Burkhardt, LEP Note 632

G4double BDSSynchrotronRadiation::SynGenC(G4double xmin) { 
  static bool DoInit=true;
  static G4double a1,a2,c1,xlow,ratio,LastXmin=-1.;
  
  if((DoInit) | (xmin!=LastXmin)) { 
    DoInit=false;
    LastXmin=xmin;
    if(xmin>1.) xlow=xmin; else xlow=1.;
    // initialize constants used in the approximate expressions
    // for SYNRAD   (integral over the modified Bessel function K5/3)
    a1=SynRadC(1.e-38)/pow(1.e-38,-2./3.); // = 2**2/3 GAMMA(2/3)
    a2=SynRadC(xlow)/exp(-xlow);
    c1=pow(xmin,1./3.);
    // calculate the integrals of the approximate expressions
    if(xmin<1.) { // low and high approx needed
      G4double sum1ap=3.*a1*(1.-pow(xmin,1./3.)); //     integral xmin --> 1
      G4double sum2ap=a2*exp(-1.);                    //     integral 1 --> infin
      ratio=sum1ap/(sum1ap+sum2ap);
    }
    else {// only high approx needed
      ratio=0.;     //     generate only high energies using approx. 2
      a2=SynRadC(xlow)/exp(-xlow);
    }
  }
  // Init done, now generate
  G4double appr,exact,result;
  do { 
    if(G4UniformRand()<ratio) {          // use low energy approximation
      result=c1+(1.-c1)*G4UniformRand();
      result*=result*result;             // take to 3rd power;
      exact=SynRadC(result);
      appr=a1*pow(result,-2./3.);
    }
    else {                               // use high energy approximation
      result=xlow-log(G4UniformRand());
      exact=SynRadC(result);
      appr=a2*exp(-result);
    }
  }
  while(exact<appr*G4UniformRand());     // reject in proportion of approx
  return result;                         // result now exact spectrum with unity weight
}

/*   x :    energy normalized to the critical energy
     returns function value SynRadC   photon spectrum dn/dx
     (integral of modified 1/3 order Bessel function)
     principal: Chebyshev series see H.H.Umstaetter CERN/PS/SM/81-13 10-3-1981
     see also my LEP Note 632 of 12/1990
     converted to C++, H.Burkhardt 21-4-1996    */
G4double BDSSynchrotronRadiation::SynRadC(G4double x) {
  G4double synrad=0.;
  if((x>0.) & (x<800.)) { // otherwise result synrad remains 0
    if(x<6.) {
      G4double a,b,z;
      z=x*x/16.-2.;
      b=          .00000000000000000012;
      a=z*b  +    .00000000000000000460;
      b=z*a-b+    .00000000000000031738;
      a=z*b-a+    .00000000000002004426;
      b=z*a-b+    .00000000000111455474;
      a=z*b-a+    .00000000005407460944;
      b=z*a-b+    .00000000226722011790;
      a=z*b-a+    .00000008125130371644;
      b=z*a-b+    .00000245751373955212;
      a=z*b-a+    .00006181256113829740;
      b=z*a-b+    .00127066381953661690;
      a=z*b-a+    .02091216799114667278;
      b=z*a-b+    .26880346058164526514;
      a=z*b-a+   2.61902183794862213818;
      b=z*a-b+  18.65250896865416256398;
      a=z*b-a+  92.95232665922707542088;
      b=z*a-b+ 308.15919413131586030542;
      a=z*b-a+ 644.86979658236221700714;
      G4double p;
      p=.5*z*a-b+  414.56543648832546975110;
      a=          .00000000000000000004;
      b=z*a+      .00000000000000000289;
      a=z*b-a+    .00000000000000019786;
      b=z*a-b+    .00000000000001196168;
      a=z*b-a+    .00000000000063427729;
      b=z*a-b+    .00000000002923635681;
      a=z*b-a+    .00000000115951672806;
      b=z*a-b+    .00000003910314748244;
      a=z*b-a+    .00000110599584794379;
      b=z*a-b+    .00002581451439721298;
      a=z*b-a+    .00048768692916240683;
      b=z*a-b+    .00728456195503504923;
      a=z*b-a+    .08357935463720537773;
      b=z*a-b+    .71031361199218887514;
      a=z*b-a+   4.26780261265492264837;
      b=z*a-b+  17.05540785795221885751;
      a=z*b-a+  41.83903486779678800040;
      G4double q;
      q=.5*z*a-b+28.41787374362784178164;
      G4double y;
      G4double const twothird=2./3.;
      y=pow(x,twothird);
      synrad=(p/y-q*y-1.)*1.81379936423421784215530788143;
    }
    else {// 6 < x < 174
      G4double a,b,z;
      z=20./x-2.;
      a=      .00000000000000000001;
      b=z*a  -.00000000000000000002;
      a=z*b-a+.00000000000000000006;
      b=z*a-b-.00000000000000000020;
      a=z*b-a+.00000000000000000066;
      b=z*a-b-.00000000000000000216;
      a=z*b-a+.00000000000000000721;
      b=z*a-b-.00000000000000002443;
      a=z*b-a+.00000000000000008441;
      b=z*a-b-.00000000000000029752;
      a=z*b-a+.00000000000000107116;
      b=z*a-b-.00000000000000394564;
      a=z*b-a+.00000000000001489474;
      b=z*a-b-.00000000000005773537;
      a=z*b-a+.00000000000023030657;
      b=z*a-b-.00000000000094784973;
      a=z*b-a+.00000000000403683207;
      b=z*a-b-.00000000001785432348;
      a=z*b-a+.00000000008235329314;
      b=z*a-b-.00000000039817923621;
      a=z*b-a+.00000000203088939238;
      b=z*a-b-.00000001101482369622;
      a=z*b-a+.00000006418902302372;
      b=z*a-b-.00000040756144386809;
      a=z*b-a+.00000287536465397527;
      b=z*a-b-.00002321251614543524;
      a=z*b-a+.00022505317277986004;
      b=z*a-b-.00287636803664026799;
      a=z*b-a+.06239591359332750793;
      G4double p;
      p=.5*z*a-b    +1.06552390798340693166;
      //       G4double const pihalf=pi/2.;
      synrad=p*sqrt(CLHEP::halfpi/x)/exp(x);
    }
  }
  return synrad;
}

G4double  BDSSynchrotronRadiation::GetMeanFreePath(const G4Track& track,
						   G4double /*PreviousStepSize*/,
						   G4ForceCondition* ForceCondition) {  

  // Check if primary if not return large mean free path
  if(track.GetParentID() !=0) {
    // DBL_MAX
    return DBL_MAX;    
  }

  *ForceCondition = NotForced ;

  //   static G4FieldManager* lastFieldManager;

  G4double        MeanFreePath;
  G4FieldManager* TheFieldManager = track.GetVolume()->GetLogicalVolume()->GetFieldManager();
  
  /*
  G4double SynchOnZPos = (7.184+4.0) * CLHEP::m;
  if(track.GetPosition().z() + BDSGlobalConstants::Instance()->GetWorldSizeZ() < SynchOnZPos)
    return DBL_MAX;
  */
  if(TheFieldManager) {
    const G4Field* pField = TheFieldManager->GetDetectorField() ;
    G4ThreeVector  globPosition = track.GetPosition() ;
    G4double  globPosVec[3], FieldValueVec[3]={0.} ;
    globPosVec[0] = globPosition.x() ;
    globPosVec[1] = globPosition.y() ;
    globPosVec[2] = globPosition.z() ;
    
    if(pField)
      pField->GetFieldValue(globPosVec, FieldValueVec) ; 
	
    G4double Blocal= FieldValueVec[1];
    if ( FieldValueVec[0]!=0.)
      Blocal=sqrt(Blocal*Blocal+FieldValueVec[0]*FieldValueVec[0]);
       
    // DangerousStuffHere, 1 line
    if(track.GetMaterial()==BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial())
       && Blocal !=0 ) {
      G4ThreeVector InitMag=track.GetMomentum();
      
      // transform to the local coordinate frame      
      G4AffineTransform tf(track.GetTouchable()->GetHistory()->GetTopTransform().Inverse());
      const G4RotationMatrix Rot=tf.NetRotation();
      const G4ThreeVector Trans=-tf.NetTranslation();
      InitMag=Rot*InitMag; 

      G4double Rlocal=(InitMag.z()/CLHEP::GeV)/(0.299792458 * Blocal/CLHEP::tesla) *CLHEP::m;
      
      MeanFreePath = fabs(Rlocal)/(track.GetTotalEnergy()*nExpConst);
      MeanFreePath /= BDSGlobalConstants::Instance()->GetSynchMeanFreeFactor();
      
      // reset mean free path counter
      if(MeanFreePathCounter>=BDSGlobalConstants::Instance()->GetSynchMeanFreeFactor())
	    MeanFreePathCounter=0;

      if(InitMag.mag() < 100.0) {
	// DBL_MAX
	return DBL_MAX;
      }

#ifdef BDSDEBUG
      G4cout<< __METHOD_NAME__ << G4endl;
      G4cout<< __METHOD_NAME__ << "Track momentum=" << InitMag << G4endl;
      G4cout<< __METHOD_NAME__ << "Blocal="<<Blocal/CLHEP::tesla<<"  Rlocal="<<Rlocal/CLHEP::m<<G4endl;
      G4cout<< __METHOD_NAME__ << "Volume=" << track.GetVolume()->GetName() << G4endl;
      G4cout<< __METHOD_NAME__ << "mfp="<<MeanFreePath/CLHEP::m << G4endl;
#endif


      return MeanFreePath;
    }
    else {
      // DBL_MAX
      return DBL_MAX;
    }
  }
  else {
    // DBL_MAX
    return DBL_MAX;
  }
}


