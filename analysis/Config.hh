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
#ifndef CONFIG_H
#define CONFIG_H
#include "BinSpecification.hh"

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"
#include "SpectraParticles.hh"

#include <map>
#include <set>
#include <string>
#include <vector>

class HistogramDef;
class HistogramDefSet;

/**
 * @brief Configuration and configuration parser class.
 * 
 * @author Stewart Boogert
 */

class Config
{
  /// Private members first as required in accessors.
private:
  /// Optional version of option keys in file.
  std::map<std::string, std::string> alternateKeys;

  /// @{ Storage of options.
  std::map<std::string, bool>        optionsBool;
  std::map<std::string, std::string> optionsString;
  std::map<std::string, double>      optionsNumber;
  /// @}

  /// Storage of histogram options. This owns the HistogramDef objects. Key is tree name,
  /// value is vector of all histogram definitions for that tree.
  std::map<std::string, std::vector<HistogramDef*> > histoDefs;

  /// Copy of definition used to identify only 'simple' histogram definitions. Doesn't own.
  std::map<std::string, std::vector<HistogramDef*> > histoDefsSimple;

  /// Copy of definition used to identify only 'per entry' histogram definitions. Doesn't own.
  std::map<std::string, std::vector<HistogramDef*> > histoDefsPerEntry;

  /// Sets of histogram definitions per particle. Only for event branch.
  std::vector<HistogramDefSet*> eventHistoDefSetsSimple;
  std::vector<HistogramDefSet*> eventHistoDefSetsPerEntry;
  
  /// List of branches in event tree to produce ParticleSet objects on. (per event and simple).
  std::vector<std::string> eventParticleSetBranches;
  std::vector<std::string> eventParticleSetSimpleBranches;
  
public:
  virtual ~Config();

  /// Singleton accessor
  static Config* Instance(const std::string& fileName = "",
			  const std::string& inputFilePath = "",
			  const std::string& outputFileName = "",
			  const std::string& defaultOutputFileSuffix = "_ana");

  void ParseInputFile();

  /// @{ General accessor for option.
  inline std::string GetOptionString(const std::string& key) const {return optionsString.at(key);}
  inline bool        GetOptionBool(const std::string& key)   const {return optionsBool.at(key);}
  inline double      GetOptionNumber(const std::string& key) const {return optionsNumber.at(key);}
  /// @}

  /// Access all histogram definitions.
  inline const std::vector<HistogramDef*>& HistogramDefinitions(const std::string& treeName) const
  {return histoDefs.at(treeName);}

  /// Access all simple histogram definitions - throws exception if out of range.
  inline const std::vector<HistogramDef*>& HistogramDefinitionsSimple(const std::string& treeName) const
  {return histoDefsSimple.at(treeName);}

  /// Access all per entry histogram definitions - throws exception if out of range.
  inline const std::vector<HistogramDef*>& HistogramDefinitionsPerEntry(const std::string& treeName) const
  {return histoDefsPerEntry.at(treeName);}

  inline const std::vector<HistogramDefSet*>& EventHistogramSetDefinitionsSimple() const
  {return eventHistoDefSetsSimple;}

  inline const std::vector<HistogramDefSet*>& EventHistogramSetDefinitionsPerEntry() const
  {return eventHistoDefSetsPerEntry;}
  
  inline const std::vector<std::string>& EventParticleSetNamesSimple()   const {return eventParticleSetSimpleBranches;}
  inline const std::vector<std::string>& EventParticleSetNamesPerEntry() const {return eventParticleSetBranches;}

  /// Access all branches that are required for activation. This does not specialise on the
  /// leaf inside the branch and if one variable is required, the whole branch will be activated
  /// as there isn't much difference.  This can of course be revised in future.
  const RBDS::VectorString& BranchesToBeActivated(const std::string& treeName) const
  {return branches.at(treeName);}

  /// Access the map of all branches to be activated per tree.
  inline const RBDS::BranchMap& BranchesToBeActivated() const {return branches;}

  /// Set a branch to be activated if not already.
  void SetBranchToBeActivated(const std::string& treeName, const std::string& branchName);

  /// @{ Accessor.
  inline std::string InputFilePath() const             {return optionsString.at("inputfilepath");}
  inline std::string OutputFileName() const            {return optionsString.at("outputfilename");}
  inline std::string CalculateOpticalFunctionsFileName() const {return optionsString.at("opticslfilename");}
  inline bool   AllBranchesToBeActivated() const  {return optionsBool.at("allbranchesactivated");}
  inline bool   Debug() const                     {return optionsBool.at("debug");}
  inline bool   CalculateOpticalFunctions() const {return optionsBool.at("calculateoptics");}
  inline bool   EmittanceOnTheFly() const         {return optionsBool.at("emittanceonthefly");}
  inline bool   ProcessSamplers() const           {return optionsBool.at("processsamplers");}
  inline bool   PrintOut() const                  {return optionsBool.at("printout");}
  inline double PrintModuloFraction() const       {return optionsNumber.at("printmodulofraction");}
  /// @}
  /// @{ Whether per entry loading is needed. Alternative is only TTree->Draw().
  inline bool   PerEntryBeam()   const {return optionsBool.at("perentrybeam");}
  inline bool   PerEntryEvent()  const {return optionsBool.at("perentryevent");}
  inline bool   PerEntryRun()    const {return optionsBool.at("perentryrun");}
  inline bool   PerEntryOption() const {return optionsBool.at("perentryoption");}
  inline bool   PerEntryModel()  const {return optionsBool.at("perentrymodel");}
  /// @}

  /// Print out the per event and simple histogram set definitions as these
  /// are (assumed to be) spectra definitions that people might want to see expanded.
  void PrintHistogramSetDefinitions() const;

  /// Update the "energy" variable of any spectra histograms to "totalEnergy" for
  /// cylindrical and spherical samplers only as they have different variable names.
  /// This can only be done after loading data and knowing which samplers are of these types.
  /// Also, provide both sets of names so we can categorise each as plane, cylindrical or
  /// spherical sampler. This is done on sets of histograms which is uniquely for spectra.
  void FixCylindricalAndSphericalSamplerVariablesInSets(const std::set<std::string>& allCNames,
                                                        const std::set<std::string>& allSNames);
  
 protected:
  /// Private constructor for singleton pattern.
  Config() = delete;
  /// Constructor used when merging only.
  Config(const std::string& inputFilePathIn,
	 const std::string& outputFileNameIn,
         const std::string& defaultOutputFileSuffix = "_ana");
  /// Desired constructor, also private for singleton pattern.
  Config(const std::string& fileNameIn,
	 const std::string& inputFilePathIn,
	 const std::string& outputFileNameIn,
	 const std::string& defaultOutputFileSuffix);

  /// Set defaults in member maps for all options so that the keys can
  /// always be accessed.
  void InitialiseOptions(const std::string& analysisFile);

  /// Parse a line beginning with histogram. Uses other functions if appropriately defined.
  void ParseHistogramLine(const std::string& line);

  /// Parse a spectra definition line.
  void ParseSpectraLine(const std::string& line);

  /// Parse a particle set line.
  void ParseParticleSetLine(const std::string& line);

  /// Parse everything after the histogram declaration and check all parameters.
  void ParseHistogram(const std::string& line, const int nDim);

  /// Check whether a histogram definition word contains the world 'simple' and
  /// if so, it's not a per-entry histogram.
  void ParsePerEntry(const std::string& name, bool& perEntry) const;

  /// Return true if 'input' contains 'word' - CI = case insensitive.
  bool ContainsWordCI(const std::string& input,
		      const std::string& word) const;
  
  /// Parse whether each dimension is log or linear.
  void ParseLog(const std::string& definition,
		bool& xLog,
		bool& yLog,
		bool& zLog) const;

  /// Update the vector of required branches for a particular tree to be
  /// activated for analysis. Note this is not required for simple histograms
  /// that will be used with TTree->Draw(). Only per-entry histograms require
  /// loading the data.
  void UpdateRequiredBranches(const HistogramDef* def);

  /// Update the vector of required branches for a particular tree to be
  /// activated for analysis based on a single string definition such as Primary.x.
  void UpdateRequiredBranches(const std::string& treeName,
			      const std::string& var);

  /// Check if the supplied tree name is one of the static member vector of
  /// allowed tree names.
  bool InvalidTreeName(const std::string& treeName) const;

  /// Check whether the tree name ends in a '.' or not and fix it (simple mistake.
  /// Then apply InvalidTreeName and throw std::string error if it's a problem.
  void CheckValidTreeName(std::string& treeName) const;

  /// Parse the bin substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBins(const std::string& bins,
		 int nDim,
		 BinSpecification& xBinning,
		 BinSpecification& yBinning,
		 BinSpecification& zBinning) const;

  /// Parse binning substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBinning(const std::string& binning,
		    int nDim,
                    BinSpecification& xBinning,
                    BinSpecification& yBinning,
                    BinSpecification& zBinning,
                    bool xLog,
                    bool yLog,
                    bool zLog) const;
  /// Return a vector of strings by splitting on whitespace.
  std::vector<std::string> SplitOnWhiteSpace(const std::string& line) const;
    
  /// Parser a list of particle PDG IDs into a set.
  std::set<ParticleSpec> ParseParticles(const std::string& word) const;
    
  /// Parse a settings line in input file and appropriate update member map.
  void ParseSetting(const std::string& line);

  /// Return a lower case copy of a string.
  std::string LowerCase(const std::string& st) const;
  
  /// Register a histogram name for future checking against. If it already exists, the
  /// function returns true, else false for successful registration.
  bool RegisterHistogramName(const std::string& newHistName);

  static Config* instance;

  /// Vector of permitted tree names.
  static std::vector<std::string> treeNames;

  /// Index of which line in the file we're on while parsing - for feedback.
  int lineCounter = 0;

  /// Cache of which branches need to be activated for this analysis.
  RBDS::BranchMap branches;

  /// Cache of all spectra names declared to permit unique naming of histograms
  /// when there's more than one spectra per branch used.
  std::map<std::string, int> spectraNames;
  
  std::set<std::string> histogramNames;

  ClassDef(Config,1);
};

#endif
