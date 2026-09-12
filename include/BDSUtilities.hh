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
#ifndef BDSUTILITIES_H
#define BDSUTILITIES_H

#include "globals.hh"   // geant4 globals / types
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include <limits>
#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class BDSExtent;

class G4UserLimits;

namespace CLHEP {
  class HepRotation;
}
typedef CLHEP::HepRotation G4RotationMatrix;

/**
 * @brief Various utility functions that have no specific place - 
 * for example string manipulations, and checking
 *
 * Functions are part of the BDS namespace 
 * @author Laurie Nevay
 */

namespace BDS
{
  /// Logical not for isalpha UnaryPredicate as needed for string manipulations
  struct non_alpha
  {
    G4bool operator()(char c);
  };

  /// Utility function to simplify lots of syntax changes for pedantic g4 changes.
  G4bool StrContains(const G4String& str, const G4String& test);

  /// Utility function to simplify lots of syntax changes for pedantic g4 changes.
  G4int StrCompare(const G4String& str, const G4String&, G4String::caseCompare mode=G4String::ignoreCase);

  /// Utility function to simplify lots of syntax changes for pedantic g4 changes.
  G4String LowerCase(const G4String& str);

  /// Because Geant4 is removing this we need to maintain it to have backwards compatibility,
  /// sadly polluting BDSIM.
  enum class StringStripType
  {
    leading,
    trailing,
    both
  };
  /// Utility function to simplify lots of syntax changes for pedantic g4 changes.
  G4String StrStrip(const G4String& str,
                    char ch,
                    StringStripType stripType = StringStripType::both);

  /// Remove white space and special characters in the name
  G4String PrepareSafeName(G4String name);

  /// Calculate the +- 1 orientation multiplier for absolute angles
  /// seems trivial, but used in a lot of places so put in one place
  G4int CalculateOrientation(G4double angle);

  /// Calculate input and output normal vector
  std::pair<G4ThreeVector,G4ThreeVector> CalculateFaces(G4double angleInIn,
							G4double angleOutIn);

  /// Ensure a variable passed by reference is capped within lowerLimit and upperLimit.
  /// If it's outside these it is defaulted to those limit values.
  void EnsureInLimits(G4double& value, G4double lowerLimit, G4double upperLimit);

  /// Checks if filename exists
  G4bool FileExists(const G4String& filename);

  /// Check if directory exists.
  G4bool DirectoryExists(const G4String& path);

  /// Get the current dir the program was executed from.
  std::string GetCurrentDir();
  
  /// Returns path from which BDSIM is executed
  /// supports linux/unix and mac OS
  std::string GetBDSIMExecPath();

  /// Get the full absolute directory path where a file can be found.
  /// Option excludeNameFromPath: if true will return only the path without
  /// the filename appended to exclude the filename from path.
  /// Option useCWDForPrefix: if the path is relative, then we would normally
  /// prepend the absolute path of the main input file so it's relative to that
  /// but with an absolute path. However, we may want to do this relative to the
  /// executable directory of the program. If this option is on, it'll be CWD.
  G4String GetFullPath(G4String filename, bool excludeNameFromPath=false, bool useCWDForPrefix=false);

  /// Split a full file path into the path and file components. The path
  /// ends with '/'.
  void SplitPathAndFileName(const G4String& filePath,
			    G4String& path,
			    G4String& filename);

  /// Split a filename.ext into filename and extension. Extension includes '.'.
  void SplitFileAndExtension(const G4String& fileName,
                             G4String&       file,
                             G4String&       extension);

  /// Try to catch abort signals. This is not guaranteed to work.
  /// Main goal is to close output stream / files.
  void HandleAborts(int signal_number);
  
  /// Return true if value is consistent with 0. This is done by checking the
  /// difference between the value and 0 is within 'tolerance', which is by default
  /// the minimum difference from zero, epsilon (~1e-15). If a very small but finite
  /// number is a possibility, a different tolerance should be used.
  G4bool IsFinite(G4double value,
		  G4double tolerance = std::numeric_limits<double>::epsilon());

  /// Apply IsFinite to each component of a 3-vector.
  G4bool IsFinite(const G4ThreeVector& variable,
		  G4double tolerance = std::numeric_limits<double>::epsilon());

  /// Test whether a number is non-zero - ie abs(number) > minimum number
  G4bool NonZero(G4double value);

  /// Test if a number is above a certain tolerance for tracking strength
  /// purposes. Currently at 1e-50 as potential can be small and squared. Uses
  /// IsFinite() with 1e-50 as tolerance.
  G4bool IsFiniteStrength(G4double variable);

  /// Check if character array is an integer, and returns the integer by reference
  G4bool IsInteger(const char* s, int& convertedInteger);
  
  /// Check if character array is an integer, and returns the double by reference
  G4bool IsNumber(const char* s, double& convertedNumber);

  template <typename T>
  G4int Sign(T val)
  {return G4int((T(0) < val) - (val < T(0)));}

  inline G4String BoolToString(G4bool in)
  {return in ? "true" : "false";}
  
  /// Print out details of a rotation matrix - the matrix itself, unit vectors.
  /// Optional keyname to identify in output stream
  void PrintRotationMatrix(G4RotationMatrix* rm, G4String keyName = "unknown");

  /// Check if the geant4 environmental variables necessary for a run are set
  G4bool Geant4EnvironmentIsSet();

  /// Check if the optional G4PARTICLEHPDATA exists in the environment.
  /// Will print out error and exit if no variable found. Physics list argument
  /// used for feedback purposes.
  void CheckHighPrecisionDataExists(const G4String& physicsListName);

  /// Check if the optional Low Energy Neutron Data set exists via the environmental
  /// variable G4LENDDATA. Physics list argument used for feedback purposes.
  void CheckLowEnergyNeutronDataExists(const G4String& phhysicsListName);
    
  ///@{ Get parameter value from the specification ('spec') string
  G4double GetParameterValueDouble(G4String spec, G4String name);
  G4String GetParameterValueString(G4String spec, G4String name);
  G4int    GetParameterValueInt(G4String spec, G4String name);
  ///@}

  /// Split a string on whitespace and return a vector of these 'words'.
  std::vector<G4String> SplitOnWhiteSpace(const G4String& input);
  
  /// Rotate a two vector in polar coordinates by an angle.
  G4TwoVector Rotate(const G4TwoVector& vec, const G4double& angle);

  /// Check if two planes will overlap - as defined by their UNIT normal
  /// vectors at 0,0,0 and the z separation of the two. Incoming is assumed to have
  /// a positive z component and outgoing a -ve z component in this convention.
  G4bool WillIntersect(const G4ThreeVector& incomingNormal,
		       const G4ThreeVector& outgoingNormal,
		       const G4double&      zSeparation,
		       const BDSExtent&     incomingExtent,
		       const BDSExtent&     outgoingExtent);

  /// Checking for "cutted crossed Z planes" for a cylinder with angled faces. This
  /// utility only works in one dimension unlike the other one with the same name and
  /// vector arguments.
  G4bool WillIntersect(const G4double& angleIn,
		       const G4double& angleOut,
		       const G4double& horizontalWidth,
		       const G4double& length);

  /// Given an x and y coordinate, calculate what the z coordinate must be to
  /// lie on the plane defined by the supplied normal vector - assumes plane
  /// intercepts 0,0,0. Used in WillIntersect.
  G4double GetZOfPointOnPlane(const G4ThreeVector& normal, G4double x, G4double y);

  /// Rotate a face normal vector for a given component to that of the reference
  /// trajectory on that face.  This takes the component full angle and assumes the
  /// component changes the angle continuously between its faces and therefore half
  /// of this angle is used for the rotation.
  G4ThreeVector RotateToReferenceFrame(G4ThreeVector faceNormal, G4double fullAngle);

  /// Split a format and file path string around the ":" character. This format
  /// is used for geometry and field maps
  std::pair<G4String, G4String> SplitOnColon(const G4String& formatAndPath);

  /// Create a user limits instance based on a default with a new step length limit
  /// of the length parameter. Check the max step length in the defaultUL and use
  /// the shorter of the two. Note the G4UserLimits instance is not const although
  /// it could be as the accessors in that class are not const. Fraction is the fraction
  /// of the length that max step will be because if it's exactly the length of an object,
  /// it can prove problematic to resolve the outgoing surface - better to give some margin.
  G4UserLimits* CreateUserLimits(G4UserLimits*  defaultUL,
				 G4double length,
				 G4double fraction = 1.6);

  /// Get the current memory usage.
  G4double GetMemoryUsage();

  /// Take one long string and split on space and then on colon. "key1:value1 key2:value2" etc.
  std::map<G4String, G4String> GetUserParametersMap(const G4String& userParameters,
                                                    char delimiter = ':');

  /// Generic function to get an item from a map with a default value and not throw an exception
  /// from unsafe access. Saves writing the searching code everywhere. Based on:
  /// https://stackoverflow.com/questions/2333728/stdmap-default-value
  template <typename K, typename V>
  V MapGetWithDefault(const std::map <K,V>& m, const K& key, const V& defaultValue)
  {
    typename std::map<K,V>::const_iterator it = m.find(key);
    return it == m.end() ? defaultValue : it->second;
  }
  template <typename K, typename V>
  V MapGetWithDefault(const std::unordered_map <K,V>& m, const K& key, const V& defaultValue)
  {
    typename std::unordered_map<K,V>::const_iterator it = m.find(key);
    return it == m.end() ? defaultValue : it->second;
  }

  /// Calculate the event number to stop at given a start and number to do. Saturates
  /// at the maximum number of an int.
  G4int VerboseEventStop(G4int verboseEventStart,
			 G4int verboseEventContinueFor);

  /// Logic of whether this event should be verbose or not. Code here so it's not duplicated.
  G4bool VerboseThisEvent(G4int eventIndex,
			  G4int eventStart,
			  G4int eventStop);

  /// Calculate the rigidity for a total momentum and charge.
  G4double Rigidity(G4double momentumMagnitude,
		    G4double charge);

  /// Return true if a string "expression" starts with "prefix".
  inline G4bool StartsWith(const std::string& expression,
			   const std::string& prefix) {return expression.size() >= prefix.size() &&
      expression.rfind(prefix, 0) == 0;}

  /// Return true if a string "expression" ends with "suffix".
  inline G4bool EndsWith(const std::string& expression,
			 const std::string& suffix) {return expression.size() >= suffix.size() &&
      expression.compare(expression.size() - suffix.size(), suffix.size(), suffix) == 0;}

  /// Calculate safe length of an angled volume so it fills the length of its container.
  G4double CalculateSafeAngledVolumeLength(G4double angleIn,
                                           G4double angleOut,
                                           G4double length,
                                           G4double containerWidth,
                                           G4double containerHeight=0);

  /// Overloaded method to process G4ThreeVectors instead of angles.
  G4double CalculateSafeAngledVolumeLength(G4ThreeVector inputfaceIn,
                                           G4ThreeVector outputfaceIn,
                                           G4double length,
                                           G4double containerWidth,
                                           G4double containerHeight=0);

  /// Calculate the arc length from the chord length for a given angle.
  G4double ArcLengthFromChordLength(G4double chordLength, G4double angle);
}

#endif
