/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "BDSElectronOccupancy.hh"
#include "BDSElectronQuantumLevel.hh"
#include "BDSUtilities.hh"

#include "G4Types.hh"

#include <algorithm>
#include <cmath>


BDSElectronOccupancy::BDSElectronOccupancy(G4int maxn):
  totalElectrons(0)
{
  G4double spinUp = 0.5;
  G4double spinDown = -0.5;
  for (G4int i = 1; i <= maxn; i++)
    {
      for (G4int j = 0; j < i; j++)
        {
          G4double jSpinOrbit1 = std::abs(j+spinUp);
          G4double jSpinOrbit2 = std::abs(j+spinDown);
          if (jSpinOrbit1 == jSpinOrbit2)
            {stateList.push_back(new BDSElectronQuantumLevel(i, j, jSpinOrbit1));}
          else
            {
              stateList.push_back(new BDSElectronQuantumLevel(i, j, jSpinOrbit2));
              stateList.push_back(new BDSElectronQuantumLevel(i, j, jSpinOrbit1));
            }
        }
    }
  std::sort(stateList.begin(),stateList.end(),CompareEnergy);
}

BDSElectronOccupancy::~BDSElectronOccupancy()
{
  for (auto* eql : stateList)
    {delete eql;}
  stateList.clear();
}

void BDSElectronOccupancy::CreateNewLevel(G4int n, G4int l, G4int j)
{
  stateList.push_back(new BDSElectronQuantumLevel(n, l, j));
}

G4bool BDSElectronOccupancy::CompareEnergy(const BDSElectronQuantumLevel* level1, const BDSElectronQuantumLevel* level2)
{
  return (level1->GetLevelEnergy() > level2->GetLevelEnergy());
}

void BDSElectronOccupancy::PopulateLevels()
{
  AddElectrons(totalElectrons);
}

void BDSElectronOccupancy::AddElectrons(G4int number)
{
  G4int currentElectrons = number;
  for (auto* eql : stateList)
    {
      if (currentElectrons > 0)
        {
          G4int max = eql->GetMaxOccupancy();
          G4int current = eql->GetCurrentOccupancy();
          if (max > current)
            {
              if ((max - current) < currentElectrons)
                {
                  eql->AddElectrons(max - current);
                  currentElectrons = currentElectrons - (max - current);
                }
              else
                {
                  eql->AddElectrons(currentElectrons);
                  currentElectrons = 0;
                }
            }
        }
    }
}

void BDSElectronOccupancy::AddElectrons(G4int n, G4int l, G4double j, G4int number)
{
  G4int currentNumber = number;
  for (auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
        {
          if (currentNumber > 0)
            {
              G4int currentAvailable = eql->GetMaxOccupancy() - eql->GetCurrentOccupancy();
              if (currentAvailable > currentNumber)
                {
                  eql->AddElectrons(currentNumber);
                  currentNumber=0;
                }
              //else error
            }
        }
    }
}

void BDSElectronOccupancy::RemoveElectrons(G4int n, G4int l, G4double j, G4int number)
{
  G4int currentNumber = number;
  for (auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
        {
          if (currentNumber>0)
            {
              if (eql->GetCurrentOccupancy() > 0)
                {
                  eql->RemoveElectrons(number);
                  currentNumber=currentNumber-number;
                }
            }
        }
    }
}

void BDSElectronOccupancy::SetStateLifetime(G4int n, G4int l, G4double lifetime)
{
  for (auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l)
        {
          if (eql->GetCurrentOccupancy() > 0)
            {eql->SetExcitedLifetime(lifetime);}
        }
    }
}

void BDSElectronOccupancy::SetTimeOfExciation(G4double timeOfExcitationIn, G4int n, G4int l, G4double j)
{
  for (auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
        {eql->SetTimeOfExcitement(timeOfExcitationIn);}
    }
}

G4bool BDSElectronOccupancy::StatePopulated(G4int n, G4int l, G4double j) const
{
  for (const auto* eql : stateList)
  {
    if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
    {
      if (eql->GetCurrentOccupancy() > 0)
      {return true;}
    }
    return false;
  }
  return false;
}

G4double BDSElectronOccupancy::GetStateLifetime(G4int n, G4int l, G4double j) const
{
  for (const auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
        {return eql->GetExcitedLifetime();}
    }
  return 0;
}

G4double BDSElectronOccupancy::GetTimeOfExcitation(G4int n, G4int l, G4double j) const
{
  for (const auto* eql : stateList)
    {
      if (eql->GetnPrincipleNumnber() == n && eql->GetlAngularNumber() == l && eql->GetjSpinOrbitCoupling() == j)
        {return eql->GetTimeOfExcitement();}
    }
  return 0;
}
