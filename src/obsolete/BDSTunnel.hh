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
#ifndef BDS_TUNNEL_HH__
#define BDS_TUNNEL_HH__

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "../parser/element.h"
#include <stdexcept>


class BDSTunnel{
public:
  BDSTunnel(Element val1, G4double length, G4double angle);//, BDSAcceleratorComponent* val2);  
  ~BDSTunnel();

  void length(G4double val);
  void angle(G4double val); //The phi angle if the tunnel has a horizontal bend.
  void print();
  void motherVolume(G4LogicalVolume* val);  
  virtual void Build();
  //  void motherComponent(BDSAcceleratorComponent* val);

  double length() const;
  double radius() const;
  double floorBeamlineHeight() const;
  double beamlineCeilingHeight() const;
  double thickness() const;
  double soilThickness() const;
  double offsetX() const;
  double offsetY() const;
  double angle() const;
  G4VPhysicalVolume* physicalVolume() const;
  G4VPhysicalVolume* soilPhysicalVolume() const;
  std::vector<G4VPhysicalVolume*> multiplePhysicalVolumes();
  G4String name() const;

protected:
  BDSTunnel();


  void floorBeamlineHeight(G4double val);
  void beamlineCeilingHeight(G4double val);
  void thickness(G4double val);
  void soilThickness(G4double val);
  void offsetX(G4double val);


  void offsetY(G4double val);
  //Dimensions.  
  G4double _length;
  G4double _angle;
  G4double _offsetX;
  G4double _offsetY;
  G4double _floorBeamlineHeight;
  G4double _beamlineCeilingHeight;
  G4double _thickness;
  G4double _soilThickness;
  //Position/rotation
  G4ThreeVector _trans;
  G4ThreeVector _floorOffset;
  G4RotationMatrix* _rot;
  G4ThreeVector _nullThreeVector;
  G4RotationMatrix* _nullRotationMatrix;

  G4LogicalVolume* _motherVolume;
  //  BDSAcceleratorComponent* _motherComponent;
  G4String _motherName;
  //Materials.
  G4Material* _material;
  G4Material* _soilMaterial;
  G4Material* _cavityMaterial;

  G4VSolid* _solid;
  G4VSolid* _soilSolid;
  G4VSolid*  _cavity;
  G4VSolid * _floor;
  G4VSolid * _wallsSolid;
  G4VSolid * _innerSolid;
  G4LogicalVolume* _logicalVolume;

  
  /// Tunnel logical volumes
  G4LogicalVolume* _soilLogicalVolume;
  G4LogicalVolume* _cavityLogicalVolume;
  G4LogicalVolume*  _wallsLogicalVolume;
  /// Tunnel physical volumes
  G4VPhysicalVolume* _physiComp;
  G4VPhysicalVolume* _physiCompSoil;
  /// Tunnel user limits
  G4UserLimits* _userLimits;
  G4UserLimits* _soilUserLimits;

  virtual void SetVisAttributes();
  G4VisAttributes* _visAtt1;

private:
  void Defaults();
  void SetParameters(Element element);
  //  void SetParameters(BDSAcceleratorComponent* element);
    void radius(G4double val);
  G4double _radius;
  void CheckExceptions();
  virtual void BuildSolidVolumes();
  virtual void CalculateDimensions();
  virtual void PlaceLogicalVolumes();
  void multiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol);
  void BuildLogicalVolumes();
  void defaults();
  void SetUserLimits();
  G4bool _bBuildTunnel;
  std::vector<G4VPhysicalVolume*> _multiplePhysicalVolumes;
  G4VisAttributes* _visAtt;
  G4VisAttributes* _visAtt2;
};

inline void BDSTunnel::length(G4double val){_length = val;}
inline void BDSTunnel::floorBeamlineHeight(G4double val){ _floorBeamlineHeight = val;}
inline void BDSTunnel::beamlineCeilingHeight(G4double val){ _beamlineCeilingHeight = val;}
inline void BDSTunnel::thickness(G4double val){ _thickness = val;}
inline void BDSTunnel::soilThickness(G4double val){ _soilThickness = val;}
inline void BDSTunnel::offsetX(G4double val){ _offsetX = val;}
inline void BDSTunnel::offsetY(G4double val){ _offsetY = val;}
inline void BDSTunnel::angle(G4double val){_angle = val;} //The phi angle if the tunnel has a horizontal bend.

inline double BDSTunnel::length() const {return _length;}
inline double BDSTunnel::floorBeamlineHeight() const { return _floorBeamlineHeight;}
inline double BDSTunnel::beamlineCeilingHeight() const { return _beamlineCeilingHeight;}
inline double BDSTunnel::thickness() const { return _thickness;}
inline double BDSTunnel::soilThickness() const { return _soilThickness;}
inline double BDSTunnel::offsetX() const { return _offsetX;}
inline double BDSTunnel::offsetY() const { return _offsetY;}
inline double BDSTunnel::angle() const {return _angle;} //The phi angle if the tunnel has a horizontal bend.
inline G4VPhysicalVolume* BDSTunnel::physicalVolume() const { return _physiComp; }
inline G4VPhysicalVolume* BDSTunnel::soilPhysicalVolume() const { return _physiCompSoil; }
inline G4String BDSTunnel::name() const { return _motherName;}

inline void BDSTunnel::multiplePhysicalVolumes(G4VPhysicalVolume* aPhysVol)
{ _multiplePhysicalVolumes.push_back(aPhysVol);}

inline  std::vector<G4VPhysicalVolume*> BDSTunnel::multiplePhysicalVolumes()
{return _multiplePhysicalVolumes;}

#endif
