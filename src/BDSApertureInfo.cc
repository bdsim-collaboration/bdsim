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
#include "BDSApertureInfo.hh"
#include "BDSApertureType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include <cmath>

BDSApertureInfo::BDSApertureInfo(BDSApertureType apertureTypeIn,
				 G4double        aper1In,
				 G4double        aper2In,
				 G4double        aper3In,
				 G4double        aper4In,
				 G4String        nameForError):
  apertureType(apertureTypeIn),
  aper1(aper1In),
  aper2(aper2In),
  aper3(aper3In),
  aper4(aper4In)
{
  CheckApertureInfo(nameForError);
}

BDSApertureInfo::BDSApertureInfo(G4String apertureTypeIn,
				 G4double aper1In,
				 G4double aper2In,
				 G4double aper3In,
				 G4double aper4In,
				 G4String nameForError):
  aper1(aper1In),
  aper2(aper2In),
  aper3(aper3In),
  aper4(aper4In)
{
  apertureType = BDS::DetermineApertureType(apertureTypeIn);
  CheckApertureInfo(nameForError);
}
  
BDSApertureInfo::BDSApertureInfo(BDSApertureInfo* defaultInfo,
				 G4String         apertureTypeIn,
				 G4double         aper1In,
				 G4double         aper2In,
				 G4double         aper3In,
				 G4double         aper4In,
				 G4String         nameForError)
{
  if (apertureTypeIn.empty())
    {apertureType = defaultInfo->apertureType;}
  else 
    {apertureType = BDS::DetermineApertureType(apertureTypeIn);}

  if (!BDS::IsFinite(aper1In))
    {aper1 = defaultInfo->aper1;}
  else
    {aper1 = aper1In;}
  if (!BDS::IsFinite(aper2In))
    {aper2 = defaultInfo->aper2;}
  else
    {aper2 = aper2In;}
  if (!BDS::IsFinite(aper3In))
    {aper3 = defaultInfo->aper3;}
  else
    {aper3 = aper3In;}
  if (!BDS::IsFinite(aper4In))
    {aper4 = defaultInfo->aper4;}
  else
    {aper4 = aper4In;}
  
  CheckApertureInfo(nameForError);
}
  
void BDSApertureInfo::CheckApertureInfo(const G4String& nameForError)
{
  try
    {
      switch (apertureType.underlying())
	{
	case BDSApertureType::circular:
	  {InfoOKForCircular();    break;}
	case BDSApertureType::elliptical:
	  {InfoOKForElliptical();  break;}
	case BDSApertureType::rectangular:
	  {InfoOKForRectangular(); break;}
	case BDSApertureType::lhc:
	  {InfoOKForLHC();         break;}
	case BDSApertureType::lhcdetailed:
	  {InfoOKForLHCDetailed(); break;}
	case BDSApertureType::rectellipse:
	  {InfoOKForRectEllipse(); break;}
	case BDSApertureType::racetrack:
	  {InfoOKForRaceTrack();   break;}
	case BDSApertureType::octagonal:
	  {InfoOKForOctagonal();   break;}
	case BDSApertureType::clicpcl:
	  {InfoOKForClicPCL();     break;}
	default:
	  InfoOKForCircular();
	}
    }
  catch (BDSException& exception)
    {
      exception.SetName(nameForError);
      throw;
    }
}

BDSExtent BDSApertureInfo::Extent() const
{
  G4double extX = 0;
  G4double extY = 0;
  switch (apertureType.underlying())
    {
    case BDSApertureType::circular:
    case BDSApertureType::circularvacuum:
      {
	extX = aper1;
	extY = aper1;
	break;
      }
    case BDSApertureType::elliptical:
    case BDSApertureType::rectangular:
    case BDSApertureType::octagonal:
      {
	extX = aper1;
	extY = aper2;
	break;
      }
    case BDSApertureType::lhc:
    case BDSApertureType::lhcdetailed:
    case BDSApertureType::rectellipse:
      {
	extX = std::min(aper1, aper3);
	extY = std::min(aper2, aper3);
	break;
      }
    case BDSApertureType::racetrack:
      {
	extX = aper1 + aper3;
	extY = aper2 + aper3;
	break;
      }
    case BDSApertureType::clicpcl:
      {// this one is asymmetric so done separately
	G4double extentX     = aper1;
	G4double extentYLow  = -std::abs(aper3);
	G4double extentYHigh = aper2 + aper4;
	BDSExtent ext = BDSExtent(-extentX,     extentX,
				  extentYLow,   extentYHigh,
				  0,0);
	return ext;
	break;
      }
    default:break;
    }
  BDSExtent ext = BDSExtent(extX, extY, 0);
  return ext;
}

G4double BDSApertureInfo::IndicativeRadius() const
{
  BDSExtent ext = Extent();
  return ext.MaximumAbsTransverse();
}

void BDSApertureInfo::CheckRequiredParametersSet(G4bool setAper1,
						 G4bool setAper2,
						 G4bool setAper3,
						 G4bool setAper4)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << "aper1: " << aper1 << " check it? " << setAper1 << G4endl;
  G4cout << "aper2: " << aper2 << " check it? " << setAper2 << G4endl;
  G4cout << "aper3: " << aper3 << " check it? " << setAper3 << G4endl;
  G4cout << "aper4: " << aper4 << " check it? " << setAper4 << G4endl;
#endif
  if (setAper1 && !BDS::IsFinite(aper1))
    {throw BDSException("","\"aper1\" not set, but required to be.");}
  if (setAper2 && !BDS::IsFinite(aper2))
    {throw BDSException("","\"aper2\" not set, but required to be.");}
  if (setAper3 && !BDS::IsFinite(aper3))
    {throw BDSException("","\"aper3\" not set, but required to be.");}
  if (setAper4 && !BDS::IsFinite(aper4))
    {throw BDSException("","\"aper4\" not set, but required to be.");}
}

void BDSApertureInfo::InfoOKForCircular()
{
  CheckRequiredParametersSet(true, false, false, false);
}

void BDSApertureInfo::InfoOKForElliptical()
{
  CheckRequiredParametersSet(true, true, false, false);
}

void BDSApertureInfo::InfoOKForRectangular()
{
  CheckRequiredParametersSet(true, true, false, false);
}

void BDSApertureInfo::InfoOKForLHC()
{
  CheckRequiredParametersSet(true, true, true, false);

  if ((aper3 > aper1) && (aper2 < aper3))
    {throw BDSException("\"aper3\" > \"aper1\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape");}

  if ((aper3 > aper2) && (aper1 < aper3))
    {throw BDSException("\"aper3\" > \"aper2\" (or \"beamPipeRadius\") for lhc aperture model - will not produce desired shape");}
}

void BDSApertureInfo::InfoOKForLHCDetailed()
{
  InfoOKForLHC();
}

void BDSApertureInfo::InfoOKForRectEllipse()
{
  CheckRequiredParametersSet(true, true, true, true);

  // TODO
  
  /*
  //treat rectangle as point coordinates.  If this point is inside ellipse,
  //rectangle is too small -> error should just use elliptical aperture
  G4double test = std::hypot((aper1/aper3),(aper2/aper4));
  if (test < 1)
    {
      //rectangle too small
      G4cerr << __METHOD_NAME__ << "WARNING - combination of \"aper1\" and \"aper2\" too small"
	     << " rectangle is entirely inside ellipse - simply use elliptical aperture" << G4endl;
	     throw BDSException(__METHOD_NAME__, "");
    }
  */
  //check if ellipse is b
  //if ((aper1 > aper3) && (aper2 > aper4))
  //  {;}
}

void BDSApertureInfo::InfoOKForRaceTrack()
{
  CheckRequiredParametersSet(true, true, true, false);
}

void BDSApertureInfo::InfoOKForOctagonal()
{
  CheckRequiredParametersSet(true, true, true, true);

  if (aper3 >= aper1)
    {throw BDSException("aper3 is >= aper1 - invalid for an octagonal aperture");}
  if (aper4 >= aper2)
    {throw BDSException("aper4 is >= aper2 - invalid for an octagonal aperture");}
}

void BDSApertureInfo::InfoOKForClicPCL()
{
  CheckRequiredParametersSet(true, true, true, false);
}
