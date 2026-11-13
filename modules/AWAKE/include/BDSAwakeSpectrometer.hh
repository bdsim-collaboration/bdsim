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
/* BDSIM code.   
A scintillator screen.
Work in progress.  
*/

#ifndef BDSAwakeSpectrometer_h
#define BDSAwakeSpectrometer_h 

#include "globals.hh"
#include "BDSAcceleratorComponent.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"
#include "BDSAwakeMultilayerScreen.hh"
#include "BDSCCDCamera.hh"
#include "BDSScreenFrameRectangular.hh"

class BDSFieldInfo;
class BDSSpectrVacChamb;
class G4MagIntegratorStepper;

class BDSAwakeSpectrometer: public BDSAcceleratorComponent
{
public:
  BDSAwakeSpectrometer(G4String aName, 
                       G4double magnetOffsetX,
		       G4double length,
		       BDSFieldInfo* fieldInfo,
		       G4double poleStartZ,
		       G4String material,
		       G4double thickness,
		       G4double screenPsize,
		       G4double windowScreenGap,
		       G4double angle,
		       G4double windowThickness,
		       G4String windowMaterial,
		       G4double mountThickness,
		       G4String mountMaterial,
		       G4double screenEnd,
		       G4String spec,
		       G4double screenWidth);
  virtual ~BDSAwakeSpectrometer();

protected:
  virtual void CalculateLengths();

private:
  virtual void Build();
  //To build the magnet...
  void MagnetDefaults();
  void BuildMagnet();
  void PlaceMagnet();
  void BuildYoke();
  void PlaceYoke();
  void BuildPoles();
  void PlacePoles();
  void BuildCoils();
  void PlaceCoils();
  void BuildVacuumChamber();
  void PlaceVacuumChamber();
  //To build the camera...
  void BuildCamera();
  void PlaceCamera();
  //To build the screen...
  void BuildScreen();
    void BuildFrame();
    void PlaceFrame();
  void PlaceScreen();
  //To build the tunnel...
  void BuildAwakeSpectrometerTunnel();
  virtual void SetVisAttributes();
  virtual void BuildContainerLogicalVolume();
  void BuildCameraScoringPlane();
  void BuildScreenScoringPlane();
  /// Build Field
  void BuildField();

  /// Field info object
  BDSFieldInfo* _fieldInfo;
  
  // Geometrical objects and associated parameters:
  //Magnet logical volume to include magnet and surrounding area in order to include the vacuum chamber which will protude the magnet at the end/sides
  G4LogicalVolume* itsMagnetLog;
  G4ThreeVector itsMagnetSize;
  G4ThreeVector itsMagnetPos;
  G4ThreeVector itsYokeSize;  
  G4ThreeVector itsYokeUpperSize;  
  G4ThreeVector itsYokeMiddleSize; 
  G4ThreeVector itsYokeLowerSize;   
  G4ThreeVector itsYokePos;
  G4ThreeVector itsYokeUpperPos;
  G4ThreeVector itsYokeMiddlePos;
  G4ThreeVector itsYokeLowerPos;
  G4ThreeVector itsPolePos;
  G4LogicalVolume* itsYokeUpperLog;
  G4LogicalVolume* itsYokeMiddleLog;
  G4LogicalVolume* itsYokeLowerLog;
  G4ThreeVector itsMiddleCoilSize;  
  G4ThreeVector itsCoilSize;  
  G4ThreeVector itsCoilPos;
  G4ThreeVector itsUpperCoilPos;
  G4ThreeVector itsLowerCoilPos;
  G4ThreeVector itsUpperFrontCoilPos;
  G4ThreeVector itsLowerFrontCoilPos;
  G4ThreeVector itsUpperRearCoilPos;
  G4ThreeVector itsLowerRearCoilPos;
  G4ThreeVector itsUpperLeftCoilPos;
  G4ThreeVector itsLowerLeftCoilPos;
  G4LogicalVolume* itsCoilLog;
  G4LogicalVolume* itsMiddleCoilLog;
  G4ThreeVector itsAperture1Size;
  G4ThreeVector itsAperture2Size;
  G4double _magnetOffsetX;
  G4double itsPoleAperture;
  G4double itsCoilAperture;
  G4ThreeVector itsAperture1Pos;
  G4ThreeVector itsAperture2Pos;
  G4ThreeVector itsPoleSize;
  G4ThreeVector itsUpperPolePos;
  G4ThreeVector itsLowerPolePos;
  G4ThreeVector itsMagnetCentre;
  G4double itsPoleVerticalGap;
  G4double itsFieldWidth; //Width of the uniform magnetic field.
  G4double itsUpstreamCoilLength;
  G4double itsDownstreamCoilLength;

  G4VPhysicalVolume* itsCameraScoringPlanePhys;
  G4VPhysicalVolume* itsScreenScoringPlanePhys;
  G4LogicalVolume* itsCameraScoringPlaneLog;
  G4LogicalVolume* itsCameraScoringPlaneLog2;
  G4LogicalVolume* itsCameraScoringPlaneLog3;
  G4LogicalVolume* itsCameraScoringPlaneLog4;
  G4LogicalVolume* itsCameraScoringPlaneLog5;
  G4LogicalVolume* itsCameraScoringPlaneLog6;
  G4LogicalVolume* itsScreenScoringPlaneLog;
  G4LogicalVolume* itsScreenScoringPlaneLog2;
  G4VSolid* itsCameraScoringPlaneSolid;
  G4VSolid* itsScreenScoringPlaneSolid;

  G4LogicalVolume* itsInnerTunnelLogicalVolume;
  G4LogicalVolume* itsSoilTunnelLogicalVolume;
  G4UserLimits* itsTunnelUserLimits;
  G4UserLimits* itsSoilTunnelUserLimits;
  G4UserLimits* itsInnerTunnelUserLimits;

	//User limits for the whole element
	G4UserLimits* itsUserLimits;
                    
  //  G4Mag_UsualEqRhs* itsEqRhs;
  
private:
  G4RotationMatrix* _screenRotationMatrix;
  G4RotationMatrix* _vacRotationMatrix;
  G4RotationMatrix* _magRotationMatrix;


  //scoring plane
  G4double _totalThickness;
  G4double _screenThickness;
  G4double _screenHeight;
  G4double _screenWidth;
  G4double _scoringPlaneThickness;
    BDSScreenFrameRectangular* _frame;
    G4double _frameHeight;
    G4double _frameWidth;
    G4double _frameThicknessX;
    G4double _frameThicknessZ;
    G4double _frame_x_dim;
    G4double _frame_z_dim;
    G4double _frameCentreX;
    G4double _frameCentreZ;

  //Vacuum chamber dimensions
  BDSSpectrVacChamb* _vacChamb;
  G4int _vacuumChamberType;
  G4int _magnetGeometryType;
  G4double _strutSizeX;
  G4double _strutSizeZ;
  G4String _strutMaterial;
  G4double _vacWindowHeight;
  G4double _vacMylarThickness;
  G4double _vacKevlarThickness;
  G4double _vacThickness;
  G4double _vacInnerHeight;
  G4double _vacInnerWidth;
  G4double _vacHeight;
  G4double _vacLength;

  G4double _vacWidth1;
  G4double _vacDispX1;
  G4double _vacDispY1;
  G4double _vacDispZ1;

  G4double _vacWidth2;
  G4double _vacDispX2;
  G4double _vacDispY2;
  G4double _vacDispZ2;

  G4VisAttributes* _visAttFront;
  G4VisAttributes* _visAttScint;
  G4VisAttributes* _visAttBase;
  G4VisAttributes* _visAttSampler;
  G4VisAttributes* itsVisAttributes;

  G4String _scoringPlaneName;
  G4String _screenScoringPlaneName;
  G4String _screenSamplerName;
  G4String _screenSamplerName2;
  G4String _samplerName;
  G4String _samplerName2;
  G4String _samplerName3;
  G4String _samplerName4;
  G4String _samplerName5;
  G4String _samplerName6;
  BDSAwakeMultilayerScreen* _mlScreen;
  BDSCCDCamera* _camera;
  G4double _cameraScreenDist;
  G4String _material;
  G4double _thickness;
  G4double _screenPSize;
  G4double _windowScreenGap;
  G4double _screenAngle;
  G4double _windowThickness;
  G4String _windowMaterial;
  G4double _mountThickness;
  G4String _mountMaterial;

  G4double _screenEndZ;
  G4double _poleStartZ;
  G4double _screen_z_dim;
  G4double _screen_x_dim;
  G4double _startZPos;
  G4double _screenCentreZ;
  G4double _screenCentreX;
  //  BDS3DMagField* _magField;
  //Y component of the B field.

  G4double _windowOffsetX;
  G4double _windowOffsetXFromVCEdge;

  // added by JS
  G4double itsBmapXOffset, itsBmapZOffset;

  //Containter to hold pointers to all the logical volumes in the class
  std::vector<G4LogicalVolume*> _logVols;
  void SetUserLimits();
};

#endif
