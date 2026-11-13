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
#include "BDSDicomFileCT.hh"
#include "BDSDicomFileMgr.hh"

#include "G4Exception.hh"
#include "G4GeometryTolerance.hh"
#include "G4String.hh"
#include "G4Types.hh"

#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmrt/drtimage.h"

#include <cmath>
#include <limits>
#include <string>
#include <vector>

BDSDicomFileCT::BDSDicomFileCT()
{
  InitialiseVariables();
  theFileMgr = BDSDicomFileMgr::GetInstance();
}

BDSDicomFileCT::BDSDicomFileCT(DcmDataset* dset):
  BDSDicomVFile(dset)
{
  InitialiseVariables();
  theFileMgr = BDSDicomFileMgr::GetInstance();
}

void BDSDicomFileCT::InitialiseVariables()
{
  fLocation = 0;
  fBitAllocated = 0;
  fRescaleSlope = 0;
  fRescaleIntercept = 0;
  fNoVoxelX = 0;
  fNoVoxelY = 0;
  fNoVoxelZ = 0;
  fMinX = 0;
  fMaxX = 0;
  fMinY = 0;
  fMaxY = 0;
  fMinZ = 0;
  fMaxZ = 0;
  fVoxelDimX = 0;
  fVoxelDimY = 0;
  fVoxelDimZ = 0;
  theFileMgr = nullptr;
}

void BDSDicomFileCT::ReadData()
{
  std::vector<double> dImagePositionPatient = Read1Data(theDataset, DCM_ImagePositionPatient, 3);
  fLocation = dImagePositionPatient[2];
  std::vector<double> dSliceThickness = Read1Data(theDataset, DCM_SliceThickness, 1);
  std::vector<double> dPixelSpacing = Read1Data(theDataset, DCM_PixelSpacing, 2);

  std::vector<double> dRows = Read1Data(theDataset, DCM_Rows, 1);
  std::vector<double> dColumns = Read1Data(theDataset, DCM_Columns, 1);
  fNoVoxelY = dRows[0];
  fNoVoxelX = dColumns[0];
  fNoVoxelZ = 1;

  fMinX = dImagePositionPatient[0]; // center of upper corner of pixel?
  fMaxX = dImagePositionPatient[0] + dColumns[0] * dPixelSpacing[0];

  fMinY = dImagePositionPatient[1];
  fMaxY = dImagePositionPatient[1] + dRows[0] * dPixelSpacing[1];

  fMinZ = dImagePositionPatient[2] - dSliceThickness[0] / 2.;
  fMaxZ = dImagePositionPatient[2] + dSliceThickness[0] / 2.;
  fVoxelDimX = dPixelSpacing[0];
  fVoxelDimY = dPixelSpacing[1];
  fVoxelDimZ = dSliceThickness[0];

  if (BDSDicomFileMgr::verbose >= debugVerb)
  {
    G4cout << " DicomVFileImage::ReadData:  fNoVoxel " << fNoVoxelX << " " << fNoVoxelY << " " << fNoVoxelZ << G4endl;
    G4cout << " DicomVFileImage::ReadData:  fMin " << fMinX << " " << fMinY << " " << fMinZ << G4endl;
    G4cout << " DicomVFileImage::ReadData:  fMax " << fMaxX << " " << fMaxY << " " << fMaxZ << G4endl;
    G4cout << " DicomVFileImage::ReadData:  fVoxelDim " << fVoxelDimX << " " << fVoxelDimY << " " << fVoxelDimZ << G4endl;
  }

  std::vector<double> dImageOrientationPatient = Read1Data(theDataset, DCM_ImageOrientationPatient, 6);
  fOrientationRows = G4ThreeVector(dImageOrientationPatient[0],
                                   dImageOrientationPatient[1],
                                   dImageOrientationPatient[2]);
  fOrientationColumns = G4ThreeVector(dImageOrientationPatient[3],
                                      dImageOrientationPatient[4],
                                      dImageOrientationPatient[5]);

  if (fOrientationRows != G4ThreeVector(1, 0, 0) || fOrientationColumns != G4ThreeVector(0, 1, 0))
  {
    G4cerr << " OrientationRows " << fOrientationRows << " OrientationColumns "
           << fOrientationColumns << G4endl;
    G4Exception("DicomVFileImage::ReadData",
                "DFCT0002",
                JustWarning,
                "OrientationRows must be (1,0,0) and OrientationColumns (0,1,0), please contact GAMOS authors");
  }
  fBitAllocated = Read1Data(theDataset, DCM_BitsAllocated, 1)[0];
  if (BDSDicomFileMgr::verbose >= 4)
  {
    G4cout << " BIT ALLOCATED " << fBitAllocated << G4endl;
  }

  std::vector<double> dRescaleSlope = Read1Data(theDataset, DCM_RescaleSlope, 1);
  if (dRescaleSlope.size() == 1)
  {
    fRescaleSlope = dRescaleSlope[0];
  }
  else
  {
    fRescaleSlope = 1;
  }
  std::vector<double> dRescaleIntercept = Read1Data(theDataset, DCM_RescaleIntercept, 1);
  if (dRescaleIntercept.size() == 1)
  {
    fRescaleIntercept = dRescaleIntercept[0];
  }
  else
  {
    fRescaleIntercept = 1;
  }

  ReadPixelData();
}

void BDSDicomFileCT::ReadPixelData()
{
  //  READING THE PIXELS :
  OFCondition result = EC_Normal;
  //---- CHECK IF DATA IS COMPRESSED
  DcmElement *element = nullptr;
  result = theDataset->findAndGetElement(DCM_PixelData, element);
  if (result.bad() || element == nullptr)
  {
    G4Exception("ReadData",
                "findAndGetElement(DCM_PixelData, ",
                FatalException,
                ("Element PixelData not found: " + G4String(result.text())).c_str());
  }
  DcmPixelData *dpix = nullptr;
  dpix = OFstatic_cast(DcmPixelData *, element);
  // If we have compressed data, we must utilize DcmPixelSequence
  //   in order to access it in raw format, e. g. for decompressing it
  //   with an external library.
  DcmPixelSequence *dseq = nullptr;
  E_TransferSyntax xferSyntax = EXS_Unknown;
  const DcmRepresentationParameter *rep = nullptr;
  // Find the key that is needed to access the right representation of the data within DCMTK
  dpix->getOriginalRepresentationKey(xferSyntax, rep);
  // Access original data representation and get result within pixel sequence
  result = dpix->getEncapsulatedRepresentation(xferSyntax, rep, dseq);
  if (result == EC_Normal) // COMPRESSED DATA
  {
    G4Exception("DicomVFileImage::ReadData()",
                "DFCT004",
                FatalException,
                "Compressed pixel data is not supported");

    if (BDSDicomFileMgr::verbose >= debugVerb)
    {
      G4cout << " DicomVFileImage::ReadData:  result == EC_Normal Reading compressed data " << std::endl;
    }
    DcmPixelItem *pixitem = nullptr;
    // Access first frame (skipping offset table)
    for (int ii = 1; ii < 2; ii++)
    {
      OFCondition cond = dseq->getItem(pixitem, ii);
      if (!cond.good())
      {
        break;
      }
      G4cout << ii << " PIX LENGTH " << pixitem->getLength() << G4endl;
    }
    if (pixitem == nullptr)
    {
      G4Exception("ReadData",
                  "dseq->getItem()",
                  FatalException,
                  "No DcmPixelItem in DcmPixelSequence");
    }
    Uint8 *pixData = nullptr;
    // Get the length of this pixel item
    // (i.e. fragment, i.e. most of the time, the lenght of the frame)
    Uint32 length = pixitem->getLength();
    if (length == 0)
    {
      G4Exception("ReadData",
                  "pixitem->getLength()",
                  FatalException,
                  "PixelData empty");
    }

    if (BDSDicomFileMgr::verbose >= debugVerb)
    {
      G4cout << " DicomVFileImage::ReadData:  number of pixels " << length << G4endl;
    }
    // Finally, get the compressed data for this pixel item
    result = pixitem->getUint8Array(pixData);
  }
  else
  { // UNCOMPRESSED DATA
    if (fBitAllocated == 8)
    { // Case 8 bits :
      Uint8 *pixData = nullptr;
      if (!(element->getUint8Array(pixData)).good())
      {
        G4Exception("ReadData",
                    "getUint8Array pixData, ",
                    FatalException,
                    ("PixelData not found: " + G4String(result.text())).c_str());
      }
      for (int ir = 0; ir < fNoVoxelY; ir++)
      {
        for (int ic = 0; ic < fNoVoxelX; ic++)
        {
          fHounsfieldV.push_back(pixData[ic + ir * fNoVoxelX] * fRescaleSlope + fRescaleIntercept);
        }
      }
    }
    else if (fBitAllocated == 16)
    { // Case 16 bits :
      Uint16 *pixData = nullptr;
      if (!(element->getUint16Array(pixData)).good())
      {
        G4Exception("ReadData",
                    "getUint16Array pixData, ",
                    FatalException,
                    ("PixelData not found: " + G4String(result.text())).c_str());
      }
      for (int ir = 0; ir < fNoVoxelY; ir++)
      {
        for (int ic = 0; ic < fNoVoxelX; ic++)
        {
          fHounsfieldV.push_back((int)pixData[ic + ir * fNoVoxelX] * fRescaleSlope + fRescaleIntercept);
        }
      }
    }
    else if (fBitAllocated == 32)
    { // Case 32 bits :
      Uint32 *pixData = nullptr;
      if (!(element->getUint32Array(pixData)).good())
      {
        G4Exception("ReadData",
                    "getUint32Array pixData, ",
                    FatalException,
                    ("PixelData not found: " + G4String(result.text())).c_str());
      }
      for (int ir = 0; ir < fNoVoxelY; ir++)
      {
        for (int ic = 0; ic < fNoVoxelX; ic++)
        {
          fHounsfieldV.push_back((int)pixData[ic + ir * fNoVoxelX] * fRescaleSlope + fRescaleIntercept);
        }
      }
    }
  }
}

void BDSDicomFileCT::operator+=(const BDSDicomFileCT &rhs)
{
  *this = *this + rhs;
}

BDSDicomFileCT BDSDicomFileCT::operator+(const BDSDicomFileCT &rhs)
{
  //----- Check that both slices has the same dimensions
  if (fNoVoxelX != rhs.GetNoVoxelX() || fNoVoxelY != rhs.GetNoVoxelY())
  {
    G4cerr << "DicomVFileImage error adding two slice headers: !!! Different number of voxels: "
           << "  X= " << fNoVoxelX << " =? " << rhs.GetNoVoxelX()
           << "  Y=  " << fNoVoxelY << " =? " << rhs.GetNoVoxelY()
           << "  Z=  " << fNoVoxelZ << " =? " << rhs.GetNoVoxelZ()
           << G4endl;
    G4Exception("DicomVFileImage::DicomVFileImage",
                "", FatalErrorInArgument, "");
  }
  //----- Check that both slices has the same extensions
  if (fMinX != rhs.GetMinX() || fMaxX != rhs.GetMaxX() || fMinY != rhs.GetMinY() || fMaxY != rhs.GetMaxY())
  {
    G4cerr << "DicomVFileImage error adding two slice headers: !!! Different extensions: "
           << "  Xmin= " << fMinX << " =? " << rhs.GetMinX()
           << "  Xmax= " << fMaxX << " =? " << rhs.GetMaxX()
           << "  Ymin= " << fMinY << " =? " << rhs.GetMinY()
           << "  Ymax= " << fMaxY << " =? " << rhs.GetMaxY()
           << G4endl;
    G4Exception("DicomVFileImage::operator+", "",
                FatalErrorInArgument, "");
  }

  //----- Check that both slices has the same orientations
  if (fOrientationRows != rhs.GetOrientationRows() ||
      fOrientationColumns != rhs.GetOrientationColumns())
  {
    G4cerr << "DicomVFileImage error adding two slice headers: !!! Slices have different orientations "
           << "  Orientation Rows = " << fOrientationRows << " & " << rhs.GetOrientationRows()
           << "  Orientation Columns " << fOrientationColumns << " & "
           << rhs.GetOrientationColumns() << G4endl;
    G4Exception("DicomVFileImage::operator+", "",
                FatalErrorInArgument, "");
  }

  //----- Check that the slices are contiguous in Z
  if (std::fabs(fMinZ - rhs.GetMaxZ()) > G4GeometryTolerance::GetInstance()->GetRadialTolerance() &&
      std::fabs(fMaxZ - rhs.GetMinZ()) > G4GeometryTolerance::GetInstance()->GetRadialTolerance())
  {
    G4cerr << "DicomVFileImage error adding two slice headers: !!! Slices are not contiguous in Z "
           << "  Zmin= " << fMinZ << " & " << rhs.GetMinZ()
           << "  Zmax= " << fMaxZ << " & " << rhs.GetMaxZ()
           << G4endl;
    G4Exception("DicomVFileImage::operator+", "",
                JustWarning, "");
  }

  //----- Build slice header copying first one
  BDSDicomFileCT temp(*this);

  //----- Add data from second slice header
  temp.SetMinZ(std::min(fMinZ, rhs.GetMinZ()));
  temp.SetMaxZ(std::max(fMaxZ, rhs.GetMaxZ()));
  temp.SetNoVoxelZ(fNoVoxelZ + rhs.GetNoVoxelZ());

  return temp;
}

void BDSDicomFileCT::DumpHeaderToTextFile(std::ofstream &fout)
{
  if (BDSDicomFileMgr::verbose >= warningVerb)
  {
    G4cout << fLocation << " DumpHeaderToTextFile " << fFileName << " " << fHounsfieldV.size() << G4endl;
  }

  G4String fName = fFileName.substr(0, fFileName.length() - 3) + "g4dcm";
  std::ofstream out(fName.c_str());

  if (BDSDicomFileMgr::verbose >= warningVerb)
  {
    G4cout << "### DicomVFileImage::Dumping Z Slice header to Text file " << G4endl;
  }

  G4int fCompress = theFileMgr->GetCompression();
  fout << fNoVoxelX / fCompress << " " << fNoVoxelY / fCompress << " " << fNoVoxelZ << std::endl;
  fout << fMinX << " " << fMaxX << std::endl;
  fout << fMinY << " " << fMaxY << std::endl;
  fout << fMinZ << " " << fMaxZ << std::endl;
}

void BDSDicomFileCT::Print(std::ostream &out)
{
  G4int fCompress = theFileMgr->GetCompression();
  out << "@@ CT Slice " << fLocation << G4endl;

  out << "@ NoVoxels " << fNoVoxelX / fCompress << " " << fNoVoxelY / fCompress << " "
      << fNoVoxelZ << G4endl;
  out << "@ DIM X: " << fMinX << " " << fMaxX
      << " Y: " << fMinY << " " << fMaxY
      << " Z: " << fMinZ << " " << fMaxZ << G4endl;
}

void BDSDicomFileCT::BuildMaterials()
{
  G4int fCompress = theFileMgr->GetCompression();
  if (fNoVoxelX % fCompress != 0 || fNoVoxelY % fCompress != 0)
  {
    G4Exception("DicompFileMgr.:BuildMaterials",
                "DFC004",
                FatalException,
                ("Compression factor = " + std::to_string(fCompress) + " has to be a divisor of Number of voxels X = " + std::to_string(fNoVoxelX) + " and Y " + std::to_string(fNoVoxelY)).c_str());
  }

  //  if( DicomVerb(debugVerb) ) G4cout << " BuildMaterials " << fFileName << G4endl;
  G4double meanHV = 0.;
  for (int ir = 0; ir < fNoVoxelY; ir += fCompress)
  {
    for (int ic = 0; ic < fNoVoxelX; ic += fCompress)
    {
      meanHV = 0.;
      int isumrMax = std::min(ir + fCompress, fNoVoxelY);
      int isumcMax = std::min(ic + fCompress, fNoVoxelX);
      for (int isumr = ir; isumr < isumrMax; isumr++)
      {
        for (int isumc = ic; isumc < isumcMax; isumc++)
        {
          meanHV += fHounsfieldV[isumc + isumr * fNoVoxelX];
          // G4cout << isumr << " " << isumc << " GET mean " << meanHV << G4endl;
        }
      }
      meanHV /= (isumrMax - ir) * (isumcMax - ic);
      G4double meanDens = theFileMgr->Hounsfield2density(meanHV);

      fDensities.push_back(meanDens);
      size_t mateID;
      if (theFileMgr->IsMaterialsDensity())
      {
        // If keyword "MATE_DENS" was used in data.dat
        mateID = theFileMgr->GetMaterialIndexByDensity(meanDens);
      }
      else
      {
        // If keyword "MATE" was used in data.dat
        mateID = theFileMgr->GetMaterialIndex(meanHV);
        //if (ir < 20) std::cout << "(" << ic << ", " << ir << ") " << "Material ID = " << mateID << G4endl;
      }
      fMateIDs.push_back(mateID);
    }
  }
}

void BDSDicomFileCT::DumpMateIDsToTextFile(std::ofstream &fout)
{
  G4int fCompress = theFileMgr->GetCompression();
  if (BDSDicomFileMgr::verbose >= warningVerb)
  {
    G4cout << fLocation << " DumpMateIDsToTextFile " << fFileName << " " << fMateIDs.size() << G4endl;
  }
  for (int ir = 0; ir < fNoVoxelY / fCompress; ir++)
  {
    for (int ic = 0; ic < fNoVoxelX / fCompress; ic++)
    {
      fout << fMateIDs[ic + ir * fNoVoxelX / fCompress];
      if (ic != fNoVoxelX / fCompress - 1)
      {
        fout << " ";
      }
    }
    fout << G4endl;
  }
}

void BDSDicomFileCT::DumpDensitiesToTextFile(std::ofstream &fout)
{
  G4int fCompress = theFileMgr->GetCompression();
  if (BDSDicomFileMgr::verbose >= warningVerb)
  {
    G4cout << fLocation << " DumpDensitiesToTextFile " << fFileName << " " << fDensities.size() << G4endl;
  }

  G4int copyNo = 0;
  for (int ir = 0; ir < fNoVoxelY / fCompress; ir++)
  {
    for (int ic = 0; ic < fNoVoxelX / fCompress; ic++)
    {
      fout << fDensities[ic + ir * fNoVoxelX / fCompress];
      if (ic != fNoVoxelX / fCompress - 1)
      {
        fout << " ";
      }
      if (copyNo % 8 == 7)
      {
        fout << G4endl;
      }
      copyNo++;
    }
    if (copyNo % 8 != 0)
    {
      fout << G4endl;
    }
  }
}

