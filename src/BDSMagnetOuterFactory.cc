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
#include "BDSBeamPipe.hh"
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuter.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterFactoryBase.hh"
#include "BDSMagnetOuterFactoryCylindrical.hh"
#include "BDSMagnetOuterFactoryPolesCircular.hh"
#include "BDSMagnetOuterFactoryPolesFacet.hh"
#include "BDSMagnetOuterFactoryPolesFacetCrop.hh"
#include "BDSMagnetOuterFactoryPolesSquare.hh"
#include "BDSMagnetOuterFactoryLHCLeft.hh"
#include "BDSMagnetOuterFactoryLHCRight.hh"
#include "BDSMagnetOuterFactoryNone.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMaterials.hh"
#include "BDSWarning.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Box.hh"
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

BDSMagnetOuterFactory* BDSMagnetOuterFactory::instance = nullptr;

BDSMagnetOuterFactory* BDSMagnetOuterFactory::Instance()
{
  if (!instance)
    {instance = new BDSMagnetOuterFactory();}
  return instance;
}

BDSMagnetOuterFactory::BDSMagnetOuterFactory()
{
  lengthSafetyLarge = BDSGlobalConstants::Instance()->LengthSafetyLarge();
  none           = new BDSMagnetOuterFactoryNone();
  cylindrical    = new BDSMagnetOuterFactoryCylindrical();
  polescircular  = new BDSMagnetOuterFactoryPolesCircular();
  polessquare    = new BDSMagnetOuterFactoryPolesSquare();
  polesfacet     = new BDSMagnetOuterFactoryPolesFacet();
  polesfacetcrop = new BDSMagnetOuterFactoryPolesFacetCrop();
  lhcright       = new BDSMagnetOuterFactoryLHCRight();
  lhcleft        = new BDSMagnetOuterFactoryLHCLeft();
  sensitiveOuter = BDSGlobalConstants::Instance()->SensitiveOuter();
}

BDSMagnetOuterFactory::~BDSMagnetOuterFactory()
{
  delete none;
  delete cylindrical;
  delete polescircular;
  delete polessquare;
  delete polesfacet;
  delete polesfacetcrop;
  delete lhcright;
  delete lhcleft;
  instance = nullptr;
}

BDSMagnetOuterFactoryBase* BDSMagnetOuterFactory::GetAppropriateFactory(BDSMagnetGeometryType magnetTypeIn)
{
  switch (magnetTypeIn.underlying())
    {
    case BDSMagnetGeometryType::none:
      {return none; break;}
    case BDSMagnetGeometryType::cylindrical:
      {return cylindrical; break;}
    case BDSMagnetGeometryType::polescircular:
      {return polescircular; break;}
    case BDSMagnetGeometryType::polessquare:
      {return polessquare; break;}
    case BDSMagnetGeometryType::polesfacet:
      {return polesfacet; break;}
    case BDSMagnetGeometryType::polesfacetcrop:
      {return polesfacetcrop; break;}
    case BDSMagnetGeometryType::lhcleft:
      {return lhcleft; break;}
    case BDSMagnetGeometryType::lhcright:
      {return lhcright; break;}
    case BDSMagnetGeometryType::external:
      {return nullptr; break;}
    default:
      {
        throw BDSException(__METHOD_NAME__, "unknown type \"" + magnetTypeIn.ToString() + "\"");
        break;
      }
    }
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateMagnetOuter(BDSMagnetType       magnetType,
                                                         BDSMagnetOuterInfo* outerInfo,
                                                         G4double            outerLength,
                                                         G4double            containerLength,
                                                         BDSBeamPipe*        beamPipe)
{
  BDSMagnetOuter* outer = nullptr;

  G4String name                      = outerInfo->name;
  BDSMagnetGeometryType geometryType = outerInfo->geometryType;

  if (geometryType == BDSMagnetGeometryType::external)
    {
      outer = CreateExternal(name, outerInfo, outerLength, containerLength, beamPipe);
      G4double loadedLength = outer->GetExtent().DZ();
      if (loadedLength > outerLength)
        {
          BDS::Warning(__METHOD_NAME__, "External geometry of length " + std::to_string(loadedLength/CLHEP::m)
                             + "m\nappears to be too long for magnet of length " + std::to_string(outerLength/CLHEP::m) + "m. ");
        }
      return outer;
    }

  // Check dimensions
  if (geometryType != BDSMagnetGeometryType::none)
    {CheckOuterBiggerThanBeamPipe(name, outerInfo, beamPipe);}

  BDSMagnetOuterFactoryBase* factory = GetAppropriateFactory(geometryType);
  
  switch(magnetType.underlying())
    {
    case BDSMagnetType::decapole:
      {
        outer = factory->CreateDecapole(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::vkicker:
      {
        outer = factory->CreateKicker(name, outerLength, beamPipe, containerLength, outerInfo, true);
        break;
      }
    case BDSMagnetType::hkicker:
      {
        outer = factory->CreateKicker(name, outerLength, beamPipe, containerLength, outerInfo, false);
        break;
      }
    case BDSMagnetType::muonspoiler:
      {
        outer = factory->CreateMuonSpoiler(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::octupole:
      {
        outer = factory->CreateOctupole(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::quadrupole:
      {
        outer = factory->CreateQuadrupole(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::rfcavity:
      {
        outer = factory->CreateRfCavity(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::sectorbend:
      {
        outer = factory->CreateSectorBend(name, outerLength, beamPipe,
                                          containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::rectangularbend:
      {
        outer = factory->CreateRectangularBend(name, outerLength, beamPipe,
                                               containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::sextupole:
      {
        outer = factory->CreateSextupole(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::solenoid:
      {
        outer = factory->CreateSolenoid(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::multipole:
      {
        outer = factory->CreateMultipole(name, outerLength, beamPipe, containerLength, outerInfo);
        break;
      }
    case BDSMagnetType::thinmultipole:
    case BDSMagnetType::dipolefringe:
    case BDSMagnetType::undulator:
    case BDSMagnetType::rmatrix:
    case BDSMagnetType::paralleltransporter:
      {break;} // leave as nullptr - no outer geometry for dipole fringe or thin multipole
    default:
      G4cout << __METHOD_NAME__ << "unknown magnet type " << magnetType << " - no outer volume built" << G4endl;
      break;
    }
  return outer;
}

BDSMagnetOuter* BDSMagnetOuterFactory::CreateExternal(const G4String&     name,
                                                      BDSMagnetOuterInfo* info,
                                                      G4double          /*length*/,
                                                      G4double            magnetContainerLength,
                                                      BDSBeamPipe*        beampipe)
{
  std::map<G4String, G4Colour*> defaultMap = {
    {"coil", BDSColours::Instance()->GetColour("coil")},
    {"bend", BDSColours::Instance()->GetColour("sectorbend")},
    {"yoke", BDSColours::Instance()->GetColour("quadrupole")},
    {"quad", BDSColours::Instance()->GetColour("quadrupole")},
    {"sext", BDSColours::Instance()->GetColour("sextupole")},
    {"oct",  BDSColours::Instance()->GetColour("octupole")},
    {"dec",  BDSColours::Instance()->GetColour("decapole")}
  };
  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(name,
                                                                            info->geometryTypeAndPath,
                                                                            &defaultMap,
                                                                            info->autoColour,
                                                                            0, 0,
                                                                            nullptr,
                                                                            sensitiveOuter,
                                                                            BDSSDType::energydep,
                                                                            BDSSDType::energydepvacuum,
                                                                            false,
                                                                            nullptr,
                                                                            false,
                                                                            info->detectSchema);

  BDSExtent bpExtent = beampipe->GetExtent();
  BDSExtent magInner = geom->GetInnerExtent();
  
  if (magInner.TransverselyLessThan(bpExtent))
    {
      std::stringstream ss, ss2, ss3;
      ss << info->geometryTypeAndPath;
      ss2 << magInner;
      ss3 << bpExtent;
      std::string sss = ss.str();
      sss += " will not fit around beam pipe\nin element \"" + name + "\" and could potentially overlap with it\n";
      sss += "Determined extents to be:\n";
      sss += "External geometry inner: " + ss2.str() + "\n";
      sss += "Beam pipe outer        : " + ss3.str() + "\n";
      sss += "Check for overlaps with /geometry/test/run";
      BDS::Warning(__METHOD_NAME__, sss);
    }
    
  BDSGeometryComponent* container = CreateContainerForExternal(name, magnetContainerLength, geom, beampipe);
  
  BDSMagnetOuter* outer = new BDSMagnetOuter(geom, container);
  return outer;
}

BDSGeometryComponent* BDSMagnetOuterFactory::CreateContainerForExternal(const G4String&      name,
                                                                        G4double             length,
                                                                        BDSGeometryExternal* external,
                                                                        BDSBeamPipe*         beampipe)
{
  G4ThreeVector  inputFace = beampipe->InputFaceNormal();
  G4ThreeVector outputFace = beampipe->OutputFaceNormal();

  BDSExtent outer = external->GetExtent();
  G4VSolid* containerSolid;
  BDSExtent containerExt;
  if ((inputFace.z() > -1) || (outputFace.z() < 1))
    {// use a cut tubs for angled face
      G4double posR = std::hypot(outer.XPos(),outer.YPos());
      G4double negR = std::hypot(outer.XNeg(),outer.YNeg());
      G4double magnetContainerRadius = std::max(posR, negR) + 1*CLHEP::mm; // generous margin
      containerSolid = new G4CutTubs(name + "_container_solid",   // name
                                     0,                           // inner radius
                                     magnetContainerRadius,       // outer radius
                                     length * 0.5,                // z half length
                                     0,                           // starting angle
                                     CLHEP::twopi,                // sweep angle
                                     inputFace,
                                     outputFace);
      containerExt = BDSExtent(magnetContainerRadius, magnetContainerRadius, 0.5*length);
    }
  else
    {// flat faces so use a box
      G4double radius = outer.MaximumAbsTransverse() + 1*CLHEP::mm; // generous margin
      containerSolid = new G4Box(name + "_container_solid", // name
                                 radius,
                                 radius,
                                 length*0.5);
      containerExt = BDSExtent(radius, radius, length*0.5);
    }

  G4Material* worldMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->WorldMaterial());
  G4LogicalVolume* containerLV = new G4LogicalVolume(containerSolid,
                                                     worldMaterial,
                                                     name + "_container_lv");

  containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->ContainerVisAttr());

  BDSGeometryComponent* container = new BDSGeometryComponent(containerSolid,
                                                             containerLV,
                                                             containerExt);
  
  return container;
}
                                                      
void BDSMagnetOuterFactory::CheckOuterBiggerThanBeamPipe(const G4String&           name,
                                                         const BDSMagnetOuterInfo* outerInfo,
                                                         const BDSBeamPipe*        beamPipe) const
{
  G4double outerHorizontal = outerInfo->horizontalWidth;
  G4double outerVertical   = outerInfo->horizontalWidth * outerInfo->vhRatio;
  BDSExtent bpExtent = beamPipe->GetExtent();
  // 1x lsl between beam pipe and magnet container
  // 1x lsl between magnet container and yoke
  // 1x lsl for minimum yoke width -> takes us to minimum horizontalWidth required
  G4double  margin = 3*lengthSafetyLarge;
  if (outerHorizontal < bpExtent.DX()+margin || outerVertical < bpExtent.DY()+margin)
    {
      std::string msg =  "Magnet outer dimensions too small to encompass beam pipe for element " + name + "\n";
      msg += "magnet horizontal full width -> " + std::to_string(outerHorizontal) + "mm\n";
      msg += "magnet vertical full width   -> " + std::to_string(outerVertical) + "mm\n";
      msg += "Beam pipe width (mm): " + std::to_string(bpExtent.DX()) + ", height : ";
      msg += std::to_string(bpExtent.DY());
      throw BDSException(__METHOD_NAME__, msg);
    }
}
