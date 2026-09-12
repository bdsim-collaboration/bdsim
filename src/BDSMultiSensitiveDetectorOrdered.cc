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
#include "BDSMultiSensitiveDetectorOrdered.hh"
#include "BDSSensitiveDetector.hh"

#include "globals.hh"
#include "G4VSensitiveDetector.hh"

BDSMultiSensitiveDetectorOrdered::BDSMultiSensitiveDetectorOrdered(const G4String& name):
  G4VSensitiveDetector(name)
{;}

BDSMultiSensitiveDetectorOrdered::~BDSMultiSensitiveDetectorOrdered()
{
  ClearSDs();
}

BDSMultiSensitiveDetectorOrdered::BDSMultiSensitiveDetectorOrdered(const BDSMultiSensitiveDetectorOrdered& rhs):
  G4VSensitiveDetector(rhs),
  sensitiveDetectors(rhs.sensitiveDetectors)
{;}

BDSMultiSensitiveDetectorOrdered&
BDSMultiSensitiveDetectorOrdered::operator=(const BDSMultiSensitiveDetectorOrdered& rhs)
{
  if (this != &rhs )
    {
      G4VSensitiveDetector::operator=(static_cast<const G4VSensitiveDetector&>(rhs));
      sensitiveDetectors = rhs.sensitiveDetectors;
    }
  return *this;
}

void BDSMultiSensitiveDetectorOrdered::clear()
{
  for (auto sd : sensitiveDetectors)
    {sd->clear();}
}

void BDSMultiSensitiveDetectorOrdered::DrawAll()
{
  for (auto sd : sensitiveDetectors)
    {sd->DrawAll();}
}

void BDSMultiSensitiveDetectorOrdered::PrintAll()
{
  for (auto sd : sensitiveDetectors)
    {sd->PrintAll();}
}

G4bool BDSMultiSensitiveDetectorOrdered::ProcessHits(G4Step* step,
						     G4TouchableHistory* /*row*/)
{
  std::vector<G4VHit*> hits;
  G4bool result = true;
  for (auto sd : sensitiveDetectors)
    {
      // here we don't use Hit() as that calls the default ProcessHits 
      G4bool sdStored = sd->HitOrdered(step, hits);
      if (sdStored)
	{hits.push_back(sd->last());}
      else
	{hits.push_back(nullptr);} // there was no lastHit as not stored
      result &= sdStored;
    }
  return result;
}

G4VSensitiveDetector* BDSMultiSensitiveDetectorOrdered::Clone() const
{
  BDSMultiSensitiveDetectorOrdered* newInst = new BDSMultiSensitiveDetectorOrdered(GetName());
  for (auto sd : sensitiveDetectors )
    {
      G4VSensitiveDetector* newSD = sd->Clone();
      BDSSensitiveDetector* newSDBDS = dynamic_cast<BDSSensitiveDetector*>(newSD);
      newInst->AddSD(newSDBDS);
    }
  return newInst;
}
