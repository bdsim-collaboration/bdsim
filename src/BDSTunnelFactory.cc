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
#include "BDSDebug.hh"
#include "BDSGeometryComponent.hh"
#include "BDSTunnelFactory.hh"
#include "BDSTunnelFactoryBase.hh"
#include "BDSTunnelFactoryCircular.hh"
#include "BDSTunnelFactoryElliptical.hh"
#include "BDSTunnelFactoryRectangular.hh"
#include "BDSTunnelFactoryRectAboveGround.hh"
#include "BDSTunnelFactorySquare.hh"
#include "BDSTunnelSection.hh"
#include "BDSTunnelType.hh"

#include "globals.hh"                        // geant4 globals / types

BDSTunnelFactory* BDSTunnelFactory::instance = nullptr;

BDSTunnelFactory* BDSTunnelFactory::Instance()
{
  if (!instance)
    {instance = new BDSTunnelFactory();}
  return instance;
}

BDSTunnelFactory::BDSTunnelFactory()
{
  circular        = new BDSTunnelFactoryCircular();
  elliptical      = new BDSTunnelFactoryElliptical();
  rectangular     = new BDSTunnelFactoryRectangular();
  square          = new BDSTunnelFactorySquare();
  rectaboveground = new BDSTunnelFactoryRectAboveGround();
}

BDSTunnelFactory::~BDSTunnelFactory()
{
  delete circular;
  delete elliptical;
  delete rectangular;
  delete square;
  delete rectaboveground;
  instance = nullptr;
}

BDSTunnelFactoryBase* BDSTunnelFactory::GetAppropriateFactory(BDSTunnelType tunnelType)
{
  switch (tunnelType.underlying())
    {
    case BDSTunnelType::circular:
      {return circular;        break;}
    case BDSTunnelType::elliptical:
      {return elliptical;      break;}
    case BDSTunnelType::rectangular:
      {return rectangular;     break;}
    case BDSTunnelType::square:
      {return square;          break;}
    case BDSTunnelType::rectaboveground:
      {return rectaboveground; break;}
    default:
      {
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "unknown tunnel type \"" << tunnelType
	       << "\" - using circular tunnel factory by default" << G4endl;
#endif
	return circular;
	break;
      }
    }
}

BDSTunnelSection* BDSTunnelFactory::CreateTunnelSection(BDSTunnelType tunnelType,
							G4String      name,
							G4double      length,
							G4double      tunnelThickness,
							G4double      tunnelSoilThickness,
							G4Material*   tunnelMaterial,
							G4Material*   tunnelSoilMaterial,
							G4bool        tunnelFloor,
							G4double      tunnelFloorOffset,
							G4double      tunnel1,
							G4double      tunnel2,
							G4bool        visible)
{
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSection(name, length, tunnelThickness,
				      tunnelSoilThickness, tunnelMaterial,
				      tunnelSoilMaterial, tunnelFloor,
				      tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactory::CreateTunnelSectionAngledIn(BDSTunnelType tunnelType,
								G4String      name,
								G4double      length,
								G4double      angleIn,
								G4double      tunnelThickness,
								G4double      tunnelSoilThickness,
								G4Material*   tunnelMaterial,
								G4Material*   tunnelSoilMaterial,
								G4bool        tunnelFloor,
								G4double      tunnelFloorOffset,
								G4double      tunnel1,
								G4double      tunnel2,
								G4bool        visible)
{
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSectionAngledIn(name, length, angleIn, tunnelThickness,
					      tunnelSoilThickness, tunnelMaterial,
					      tunnelSoilMaterial, tunnelFloor,
					      tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactory::CreateTunnelSectionAngledOut(BDSTunnelType tunnelType,
								 G4String      name,
								 G4double      length,
								 G4double      angleOut,
								 G4double      tunnelThickness,
								 G4double      tunnelSoilThickness,
								 G4Material*   tunnelMaterial,
								 G4Material*   tunnelSoilMaterial,
								 G4bool        tunnelFloor,
								 G4double      tunnelFloorOffset,
								 G4double      tunnel1,
								 G4double      tunnel2,
								 G4bool        visible)
{
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSectionAngledOut(name, length, angleOut, tunnelThickness,
					       tunnelSoilThickness, tunnelMaterial,
					       tunnelSoilMaterial, tunnelFloor,
					       tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactory::CreateTunnelSectionAngledInOut(BDSTunnelType tunnelType,
								   G4String      name,
								   G4double      length,
								   G4double      angleIn,
								   G4double      angleOut,
								   G4double      tunnelThickness,
								   G4double      tunnelSoilThickness,
								   G4Material*   tunnelMaterial,
								   G4Material*   tunnelSoilMaterial,
								   G4bool        tunnelFloor,
								   G4double      tunnelFloorOffset,
								   G4double      tunnel1,
								   G4double      tunnel2,
								   G4bool        visible)
{
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSectionAngledInOut(name, length, angleIn, angleOut,
						 tunnelThickness, tunnelSoilThickness, tunnelMaterial,
						 tunnelSoilMaterial, tunnelFloor,
						 tunnelFloorOffset, tunnel1, tunnel2, visible);
}

BDSTunnelSection* BDSTunnelFactory::CreateTunnelSectionAngled(BDSTunnelType tunnelType,
							      G4String      name,
							      G4double      length,
							      G4ThreeVector inputFace,
							      G4ThreeVector outputFace,
							      G4double      tunnelThickness,
							      G4double      tunnelSoilThickness,
							      G4Material*   tunnelMaterial,
							      G4Material*   tunnelSoilMaterial,
							      G4bool        tunnelFloor,
							      G4double      tunnelFloorOffset,
							      G4double      tunnel1,
							      G4double      tunnel2,
							      G4bool        visible)
{
  BDSTunnelFactoryBase* factory = GetAppropriateFactory(tunnelType);
  return factory->CreateTunnelSectionAngled(name, length, inputFace, outputFace,
					    tunnelThickness, tunnelSoilThickness, tunnelMaterial,
					    tunnelSoilMaterial, tunnelFloor,
					    tunnelFloorOffset, tunnel1, tunnel2, visible);
}
