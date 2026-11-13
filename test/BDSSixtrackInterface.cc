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
#include "BDSBunchSixTrackLink.hh"
#include "BDSException.hh"
#include "BDSIMLink.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Types.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <vector>

std::string CleanFortranString(char* str, size_t count);

//std::vector<CollimationParticle> input_particles;
//std::vector<CollimationParticle> output_particles;
//std::vector<CollimationEnergyDeposition> EnergyDepositionConfiguration;
//std::set<int> keep_ids;



BDSIMLink* bds = nullptr;
BDSBunchSixTrackLink* stp = nullptr;

extern "C"
void g4_collimation_init(double* /*ReferenceE*/,
			 int*    /*seed*/,
			 double* /*recut*/,
			 double* /*aecut*/,
			 double* /*rcut*/,
			 double* /*rangecut_mm*/,
			 double* /*v0*/,
			 char*   /*PhysicsSelect*/,
			 bool*   /*g4_debug*/,
			 bool*   /*g4_keep_stable*/,
			 bool*   /*DoEnergyDeposition*/)
{
  stp = new BDSBunchSixTrackLink();
  bds = new BDSIMLink(stp);

  std::vector<std::string> arguments = {"--file=lhccrystals.gmad","--file=lhccrystals.gmad", "--vis_debug", "--batch"};
  //std::vector<std::string> arguments = {"--file=lhccrystals.gmad","--batch"};
  std::vector<char*> argv;
  for (const auto& arg : arguments)
    {argv.push_back((char*)arg.data());}
  argv.push_back(nullptr);

  try
    {bds->Initialise(argv.size() - 1, argv.data());}
  catch (const std::exception& e)
    {std::cout << e.what() << std::endl; exit(1);}
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl; exit(1);}
}

extern "C"
void g4_add_collimator(char*   name,
		       char*   material,
		       double* lengthIn,
		       double* apertureIn,
		       double* rotationIn,
		       double* xOffsetIn,
		       double* yOffsetIn)
{
  //  keep 48 value in sync with mNameLen in common_modules.f90
  std::string collimatorName = CleanFortranString(name, 48);
  std::string materialName   = CleanFortranString(material, 4);
  
  G4double length   = *lengthIn   * CLHEP::m;
  G4double aperture = *apertureIn * CLHEP::m;
  G4double rotation = *rotationIn * CLHEP::rad;
  G4double xOffset  = *xOffsetIn;
  G4double yOffset  = *yOffsetIn;
  G4cout << "TEST " << collimatorName << " " << materialName << " " << length << " " << aperture << " " <<  rotation << " " << xOffset << " " << yOffset << G4endl;
  bds->AddLinkCollimator(collimatorName,
			 materialName,
			 length,
			 aperture,
			 rotation,
			 xOffset,
			 yOffset);
}

extern "C"
void g4_terminate()
{
  delete bds;
  bds = nullptr;
  delete stp;
  stp = nullptr;
}

extern "C"
void g4_set_collimator(char* name)
{
  //  keep 48 value in sync with mNameLen in common_modules.f90
  std::string collimatorName = CleanFortranString(name, 48);
  bds->SelectLinkElement(collimatorName);
}

extern "C"
void g4_add_particle(double*  xIn,
		     double*  yIn,
		     double*  xpIn,
		     double*  ypIn,
		     double*  totalEnergyIn,
		     int32_t* pdgIDIn,
		     int16_t* nzz,
		     int16_t* naa,
		     int16_t* nqq,
		     double*  /*massIn*/,
		     double*  /*sigmv*/,
		     double*  /*sx*/,
		     double*  /*sy*/,
		     double*  /*sz*/)
{
  //G4double mass        = (*massIn) * CLHEP::MeV;
  G4double totalEnergy = (*totalEnergyIn) * CLHEP::GeV;
  //G4double momMag      = std::sqrt(totalEnergy*totalEnergy - mass*mass);
  G4double xp          = (*xpIn);
  G4double yp          = (*ypIn);
  G4double zp          = BDSBunch::CalculateZp(xp,yp,1);
  BDSParticleCoordsFull coords = BDSParticleCoordsFull((*xIn) * CLHEP::m,
						       (*yIn) * CLHEP::m,
						       0,
						       xp,
						       yp,
						       zp,
						       0,
						       0,
						       totalEnergy,
						       1);

  long long int pdgID = (long long int)(*pdgIDIn);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particleDef = particleTable->FindParticle(pdgID);
  if (!particleDef)
    {throw BDSException("BDSBunchUserFile> Particle \"" + std::to_string(pdgID) + "\" not found");}

  BDSIonDefinition* ionDef = nullptr;
  if (BDS::IsIon(particleDef))
    {
      G4int a = (G4int)(*naa);
      G4int z = (G4int)(*nzz);
      G4double q = (G4double)(*nqq) * CLHEP::eplus;
      ionDef = new BDSIonDefinition(a,z,q);
    }
	      
  // Wrap in our class that calculates momentum and kinetic energy.
  // Requires that one of E, Ek, P be non-zero (only one).
  BDSParticleDefinition* particleDefinition = nullptr;
  try
    {
      particleDefinition = new BDSParticleDefinition(particleDef, totalEnergy, 0, 0, 1, ionDef);
    }
  catch (const BDSException& e)
    {// if we throw an exception the object is invalid for the delete on the next loop
      particleDefinition = nullptr; // reset back to nullptr for safe delete
      return;
    }

  if (particleDefinition)
    {stp->AddParticle(particleDefinition, coords);}
  //WARNING: at this stage in SixTrack the units have been converted to GeV, m, and rad!
  //The particle energy input is the TOTAL energy
  //mass (i.e. nucm) is already in MeV!
  /*
  double x_in = (*x) * CLHEP::m;
  double y_in = (*y) * CLHEP::m;
  
  //We want px and py, not the angle!
  //p_in is the TOTAL momentum of the particle
  
  double e_in = (*e) * CLHEP::GeV;
  double p_in = sqrt((e_in*e_in) - (*mass * *mass));
  
  // x' = p_x / p_in
  // -> p_x = p_in * x'
  //p_in will now be in MeV, xp, yp will be in rad -> units are good!
  double px_in = p_in * (*xp);
  double py_in = p_in * (*yp);
  
  // p_z^2 = p_in^2 - p_x^2 - p_y^2
  double p_z = sqrt( (p_in*p_in) - (px_in*px_in) - (py_in*py_in) );
  
  CollimationParticle in_particle;
  in_particle.x = x_in;
  in_particle.y = y_in;
  
  in_particle.px = px_in;
  in_particle.py = py_in;
  in_particle.pz = p_z;
  in_particle.p = p_in;
  
  in_particle.e = e_in;
  in_particle.pdgid = *pdgid;
  in_particle.z = *nzz;
  in_particle.a = *naa;
  in_particle.q = *nqq;
  in_particle.m = *mass;
  in_particle.id = input_particles.size();
  
  in_particle.sx = *sx;
  in_particle.sy = *sy;
  in_particle.sz = *sz;
  
  input_particles.push_back(in_particle);
  */
}

extern "C"
void g4_collimate()
{
  bds->BeamOn((G4int)stp->Size());
}

extern "C"
void g4_collimate_return(int*     j,
			 double*  x,
			 double*  y,
			 double*  xp,
			 double*  yp,
			 double*  e,
			 int32_t* pdgid,
			 double*  m,
			 int16_t* z,
			 int16_t* a,
			 int16_t* q,
			 double*  sigmv,
			 int*     /*part_hit*/,
			 int*     /*part_abs*/,
			 double*  /*part_impact*/,
			 double*  /*part_indiv*/,
			 double*  /*part_linteract*/,
			 double*  sx,
			 double*  sy,
			 double*  sz)
{
  if (*j == 0)
    {return;}
  /*
    part_hit(j), part_abs(j), part_impact(j), part_indiv(j),
    & part_linteract(j))
    !++  Output information:
    !++
    !++  PART_HIT(MAX_NPART)     Hit flag for last hit (10000*element# + turn#)
    !++  PART_ABS(MAX_NPART)     Abs flag (10000*element# + turn#)
    !++  PART_IMPACT(MAX_NPART)  Impact parameter (0 for inner face)
    !++  PART_INDIV(MAX_NPART)   Divergence of impacting particles
  */
  
  //Here the units have been converted back to GeV and m (in the tracking action)
  const BDSHitsCollectionSamplerLink* hits = bds->SamplerHits();
  BDSHitSamplerLink* hit = (*hits)[*j];
  const BDSParticleCoordsFull& coords = hit->coords;
  *x  = coords.x / CLHEP::m;
  *y  = coords.y / CLHEP::m;
  //Remember, sixtrack xp, yp are p_x / p_total
  *xp = coords.xp;
  *yp = coords.yp;
  *e  = coords.totalEnergy / CLHEP::GeV;
  *pdgid  = (int32_t)hit->pdgID;
  *z = (int16_t)hit->Z;
  *a = (int16_t)hit->A;
  *q = (int16_t)hit->charge;
  
  //nucm is in MeV on both sides
  *m  = hit->mass;
  
  //Spin
  *sx = 0;
  *sy = 0;
  *sz = 1;
  
  //time, must be converted for using with sigmv
  *sigmv  = coords.T / CLHEP::s;
}

std::string CleanFortranString(char* str, size_t count)
{
  // Fortran string nightmares
  std::string whitespace(" \t\n\r");
  std::string sstring(str,count);
  
  size_t lpos = sstring.find_last_not_of(whitespace);
  if (lpos != std::string::npos)
    {sstring.erase(lpos+1);}

  std::transform(sstring.begin(), sstring.end(), sstring.begin(), ::toupper);
  // Fortran string happy place
  return sstring;
}

extern "C"
void g4_get_particle_count(int* g4_npart)
{
  int count = bds->SamplerHits()->entries();
  std::cout << "Returning " << count << std::endl;
  *g4_npart = count;
}

extern "C"
void g4_collimation_clear()
{
  bds->ClearSamplerHits();
  stp->ClearParticles();
}

extern "C"
void g4_keep_id(int* /*id*/)
{
  //keep_ids.insert(*id);
}

extern "C"
void g4_add_edep(char* /*name_in*/,
		 int* /*type*/,
		 double* /*xstep*/,
		 double* /*ystep*/,
		 double* /*zstep*/,
		 double* /*xmax*/,
		 double* /*ymax*/,
		 double* /*zmax*/,
		 double* /*xbigstep*/,
		 double* /*ybigstep*/,
		 double* /*zbigstep*/,
		 double* /*xbigmax*/,
		 double* /*ybigmax*/,
		 double* /*zbigmax*/)
{;}
