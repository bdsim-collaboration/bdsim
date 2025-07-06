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
#include "BDSExecOptions.hh"

#include <cstdlib>
#include <getopt.h>
#include <iomanip>
#include <string>

#include "globals.hh" // geant4 types / globals

#include "BDSDebug.hh"
#include "BDSColourFromMaterial.hh"
#include "BDSColours.hh"
#include "BDSMaterials.hh"
#include "BDSOutputLoader.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"
#include "parser/getEnv.h"
#include "parser/options.h"

BDSExecOptions::BDSExecOptions(int argc, char **argv):
  options(GMAD::Options()),
  beam(GMAD::Beam()),
  ignoreSIGINT(false)
{
  if (argc == 1)
    {// require at least 1 executable option (--file) - else print help message
      Usage();
      exit(0);
    }
  Parse(argc, argv);

  /// after parsing the absolute path can be reconstructed
  std::string bp = GetPath(options.inputFileName);
  options.set_value("bdsimPath", bp);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "BDSIMPATH set to: " << options.bdsimPath << G4endl;
#endif
  
  if (options.recreate)
    {
      G4cout << __METHOD_NAME__ << "Recreation mode. Loading options from recreate file:\n\""
             << options.recreateFileName << "\"\n" << G4endl;
      bool runBatch = options.batch;
      BDSOutputLoader loader(options.recreateFileName);
      GMAD::Options recreateOptions = loader.Options();
      GMAD::Beam    recreateBeam    = loader.Beam();
      // Give precedence to exec options - only ones that have been set.
      recreateOptions.Amalgamate(options, true);
      recreateBeam.Amalgamate(beam, true, options.startFromEvent);
      options       = recreateOptions; // Now replace member.
      options.batch = runBatch;        // override batch flag to allow control
      beam          = recreateBeam;
    }
}

void BDSExecOptions::Parse(int argc, char **argv)
{
  static struct option LongOptions[] = {{ "help" , 0, 0, 0 },
                                        { "reference", 0, 0, 0},
                                        { "citation", 0, 0, 0},
                                        { "version", 0, 0, 0 },
                                        { "versionGit", 0, 0, 0 },
                                        { "verbose", 0, 0, 0 },
                                        { "verbose_step", 0, 0, 0 },         // old
                                        { "verboseSteppingBDSIM", 0, 0, 0 }, // new
                                        { "verbose_event", 0, 0, 0 },        // old
                                        { "verboseEventBDSIM", 0, 0, 0 },    // new
                                        { "verbose_event_num", 1, 0, 0 },    // old
                                        { "verboseEventNumber", 1, 0, 0 },   // new
                                        { "verboseEventStart", 1, 0, 0 },
                                        { "verboseEventContinueFor",  1, 0, 0 },
                                        { "verbose_G4run", 1, 0, 0 },        // old
                                        { "verboseRunLevel", 1, 0, 0},       // new
                                        { "verbose_G4event", 1, 0, 0 },      // old
                                        { "verboseEventLevel", 1, 0, 0},     // new
                                        { "verbose_G4tracking", 1, 0, 0 },   // old
                                        { "verboseTrackingLevel", 1, 0, 0 }, // new
                                        { "verbose_G4stepping", 1, 0, 0 },
                                        { "verboseSteppingLevel", 1, 0, 0 },
                                        { "verboseSteppingEventStart", 1, 0, 0 },
                                        { "verboseSteppingEventContinueFor", 1, 0, 0 },
                                        { "verboseSteppingPrimaryOnly", 0, 0, 0 },
                                        { "file", 1, 0, 0 },
                                        { "distrFile", 1, 0, 0 },
                                        { "distrFileNLinesSkip", 1, 0, 0 },
                                        { "distrFileLoopNTimes", 1, 0, 0 },
                                        { "E0", 1, 0, 0},
                                        { "P0", 1, 0, 0},
                                        { "Ek0", 1, 0, 0},
                                        { "vis_debug", 0, 0, 0 },
                                        { "vis_mac", 1, 0, 0 },
                                        { "geant4Macro", 1, 0, 0 }, // old
                                        { "geant4MacroFileName", 1, 0, 0 },
                                        { "geant4PhysicsMacroFileName", 1, 0, 0 },
                                        { "output", 1, 0, 0 },
                                        { "outfile", 1, 0, 0 },
                                        { "batch", 0, 0, 0 },
                                        { "colours",   0, 0, 0 },
                                        { "materials", 0, 0, 0 },
                                        { "circular", 0, 0, 0 },
                                        { "seed",           1, 0, 0},
                                        { "recreate",       1, 0, 0},
                                        { "startFromEvent", 1, 0, 0},
                                        { "writeSeedState", 0, 0, 0},
                                        { "seedState",      1, 0, 0},
                                        { "seedStateFileName", 1, 0, 0},
                                        { "survey", 1, 0, 0 },
                                        { "ngenerate", 1, 0, 0 },
                                        { "nGenerate", 1, 0, 0 },
                                        { "nturns",    1, 0, 0 },
                                        { "nTurns",    1, 0, 0 },
                                        { "printFractionEvents", 1, 0, 0},
                                        { "printFractionTurns", 1, 0, 0},
                                        { "printPhysicsProcesses", 0, 0, 0},
                                        { "exportGeometryTo", 1, 0, 0 },
                                        { "generatePrimariesOnly", 0, 0, 0 },
                                        { "ignoresigint", 0, 0, 0},
                                        { 0, 0, 0, 0 }};
  
  int OptionIndex  = 0;
  int OptionNumber = 0;
  const char* optionName;
  // number numberConversionOK check
  bool numberConversionOK = true;
  for (;;)
    {
      OptionIndex = 0;
      
      // see e.g. http://linux.die.net/man/3/getopt
      int c = getopt_long(argc, argv, "Vv",
                          LongOptions, &OptionIndex );
      
      if (c == -1) // end of options list
        {break;}
      
      switch (c)
        {
        case '?': // unrecognised option
          {
            G4cout << "invalid option for command " << argv[0] << G4endl << G4endl << G4endl;
            Usage();
            exit(1);
            break;
          }
        case 0:
          {
            OptionNumber++;
            optionName = LongOptions[OptionIndex].name;
            if ( !strcmp(optionName , "help") )
              {
                Usage();
                exit(0);
              }
            else if ( !strcmp(optionName, "reference") || !strcmp(optionName, "citation") )
              {
                PrintCitation();
                exit(0);
              }
            else if ( !strcmp(optionName, "version") )
              {
                G4cout << "@BDSIM_VERSION@" << G4endl;
                exit(0);
              }
            else if ( !strcmp(optionName, "versionGit") )
              {
                G4cout << "@BDSIM_VERSION@ based on git commit SHA1: @VERSION_SHA1@" << G4endl;
                exit(0);
              }
            else if ( !strcmp(optionName , "batch") )
              {options.set_value("batch",true);}
            else if ( !strcmp(optionName , "verbose") )
              {options.set_value("verbose", true);}
            else if ( !strcmp(optionName , "verbose_step") || !strcmp(optionName , "verboseSteppingBDSIM"))
              {// we shouldn't have verbose steps without (minimally) verbose events
                options.set_value("verboseSteppingBDSIM", true); 
                options.set_value("verboseEventBDSIM",    true);
              }
            else if ( !strcmp(optionName , "verbose_event") || !strcmp(optionName, "verboseEventBDSIM") )
              {options.set_value("verboseEventBDSIM", true);}
            else if ( !strcmp(optionName , "verbose_event_num") || !strcmp(optionName , "verboseEventNumber"))
              {
                int result = -1;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseEventStart", result);
                options.set_value("verboseEventContinueFor", 1);
              }
            else if ( !strcmp(optionName , "verboseEventStart") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseEventStart", result);
              }
            else if ( !strcmp(optionName , "verboseEventContinueFor") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseEventContinueFor", result);
              }
            else if ( !strcmp(optionName , "verbose_G4run") || !strcmp(optionName , "verboseRunLevel") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseRunLevel", result);
              }
            else if ( !strcmp(optionName , "verbose_G4event") || !strcmp(optionName , "verboseEventLevel") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseEventLevel", result);
              }
            else if ( !strcmp(optionName , "verbose_G4tracking") || !strcmp(optionName , "verboseTrackingLevel") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseTrackingLevel", result);
              }
            else if ( !strcmp(optionName , "verbose_G4stepping") || !strcmp(optionName , "verboseSteppingLevel"))
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseSteppingLevel", result);
              }
            else if ( !strcmp(optionName , "verboseSteppingEventStart") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseSteppingEventStart", result);
              }
            else if ( !strcmp(optionName , "verboseSteppingEventContinueFor") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("verboseSteppingEventContinueFor", result);
              }
            else if ( !strcmp(optionName , "verboseSteppingPrimaryOnly") )
              {options.set_value("verboseSteppingPrimaryOnly", true);}
            else if ( !strcmp(optionName , "output") )
              {options.set_value("outputFormat", std::string(optarg));}
            else if ( !strcmp(optionName , "outfile") )
              {options.set_value("outputFileName", std::string(optarg));}
            else if ( !strcmp(optionName , "survey") )
              {
                options.set_value("surveyFileName", std::string(optarg));
                options.set_value("survey",         true);
              }
            else if ( !strcmp(optionName , "file") )
              {options.set_value("inputFileName", std::string(optarg));}
            else if ( !strcmp(optionName, "distrFile") )
              {// build absolute path
                beam.set_value("distrFile", std::string(optarg));
                beam.set_value("distrFileFromExecOptions", true);
              }
            else if ( !strcmp(optionName, "distrFileNLinesSkip") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                beam.set_value("nlinesSkip", result);
              }
            else if ( !strcmp(optionName, "distrFileLoopNTimes") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                beam.set_value("distrFileLoopNTimes", result);
                beam.set_value("distrFileLoop", true);
              }
            else if (!strcmp(optionName, "E0") )
              {
                double result = 1;
                numberConversionOK = BDS::IsNumber(optarg, result);
                beam.set_value("E0", result);
              }
            else if (!strcmp(optionName, "P0") )
              {
                double result = 1;
                numberConversionOK = BDS::IsNumber(optarg, result);
                beam.set_value("P0", result);
              }
            else if (!strcmp(optionName, "Ek0") )
              {
                double result = 1;
                numberConversionOK = BDS::IsNumber(optarg, result);
                beam.set_value("Ek0", result);
              }
            else if ( !strcmp(optionName, "vis_debug") )
              {options.set_value("visDebug", true);}
            else if ( !strcmp(optionName, "vis_mac") )
              {options.set_value("visMacroFileName",  std::string(optarg));}
            else if ( !strcmp(optionName, "geant4Macro") || !strcmp(optionName, "genat4MacroFileName") )
              {options.set_value("geant4MacroFileName", std::string(optarg));}
            else if ( !strcmp(optionName, "geant4PhysicsMacroFileName") )
              {
                options.set_value("geant4PhysicsMacroFileName", std::string(optarg));
                options.set_value("geant4PhysicsMacroFileNameFromExecOptions", true);
              }
            else if ( !strcmp(optionName, "colours") )
              {
                BDSColourFromMaterial::Instance(); // force construction of extra colours
                BDSColours::Instance()->Print();
                exit(0);// return after printing colour list
              }
            else if ( !strcmp(optionName, "materials") )
              {
                BDSMaterials::Instance()->ListMaterials();
                exit(0);// return after printing material list
              }
            else if ( !strcmp(optionName, "circular") )
              {options.set_value("circular", true);}
            else if ( !strcmp(optionName, "seed")  )
              {
                int result = -1;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("seed", result);
              }
            else if ( !strcmp(optionName, "recreate") )
              {
                options.set_value("recreate", true);
                options.set_value("recreateFileName", std::string(optarg));
              }
            else if ( !strcmp(optionName, "startFromEvent") )
              {
                int result = 0;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("startFromEvent", result);
              }
            else if ( !strcmp(optionName, "writeSeedState") )
              {options.set_value("writeSeedState", true);}
            else if ( !strcmp(optionName, "seedState")  || !strcmp(optionName, "seedStateFileName"))
              {
                options.set_value("useASCIISeedState", true);
                options.set_value("seedStateFileName", std::string(optarg));
              }
            else if ( !strcmp(optionName, "ngenerate") || !strcmp(optionName, "nGenerate"))
              {
                int result = 1;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("ngenerate", result);
                beam.set_value("distrFileMatchLength", false); // ngenerate overrides.
              }
            else if ( !strcmp(optionName, "nturns") || !strcmp(optionName, "nTurns"))
              {
                int result = 1;
                numberConversionOK = BDS::IsInteger(optarg, result);
                options.set_value("nturns", result);
              }
            else if ( !strcmp(optionName, "printFractionEvents") )
              {
                double result = 1;
                numberConversionOK = BDS::IsNumber(optarg, result);
                options.set_value("printFractionEvents", result);
              }
            else if ( !strcmp(optionName, "printFractionTurns") )
              {
                double result = 1;
                numberConversionOK = BDS::IsNumber(optarg, result);
                options.set_value("printFractionTurns", result);
              }
            else if ( !strcmp(optionName, "printPhysicsProcesses") )
              {options.set_value("printPhysicsProcesses", true);}
            else if ( !strcmp(optionName, "generatePrimariesOnly") )
              {options.set_value("generatePrimariesOnly", true);}
            else if ( !strcmp(optionName, "ignoresigint") )
              {ignoreSIGINT = true;}
            else if ( !strcmp(optionName, "exportGeometryTo") )
              {
                std::string fn = optarg;
                // Expand to an absolute path relative to the executable as this is an executable option.
                // Later on, it will be processed again, but since it's an absolute path it won't change.
                G4String fnAbs = BDS::GetFullPath(fn, false, true);
                if (fn.substr(fn.find_last_of('.') + 1) == "gdml")
                  {
                    options.set_value("exportType",     std::string("gdml"));
                    options.set_value("exportFileName", (std::string)fnAbs);
                  }
                else
                  {
                    G4cerr << __METHOD_NAME__ << "Unknown geometry format \""
                           << fn.substr(fn.find_last_of('.') + 1) << "\"\n"
                           << "Please specify a valid filename extension - options are: \"gdml\"" << G4endl;
                    exit(1);
                  }
                options.set_value("exportGeometry", true);
              }
            // remember if you extend this to do it also in the usage print out
            
            if (!numberConversionOK)
              {// conversion from character string to a number went wrong -> exit
                G4cerr << __METHOD_NAME__ << "Conversion to number (or integer) went wrong for \""
                       << optionName << "\" with value: \"" << optarg << "\"" << G4endl;
                exit(1);
              }
            
            break;
          }
        default:
          {
            G4cout << "WARNING unknown returned character code " <<  c << G4endl;
            break;
          }
        }
    }
  // there should be no remaining options
  if (OptionNumber < argc - 1)
    {
      G4cout << __METHOD_NAME__ << "there are remaining unknown options: " << G4endl;
      for (int i=1; i<argc; i++)
        {
          // options with '-' are ignored by getopt_long, other unknown options are covered
          if (strncmp(argv[i], "-", 1))
            {G4cout << "\"" << argv[i] << "\"" << G4endl;}
        }

      G4cout << "Please check your command line arguments" << G4endl;
      exit(1);
    }
  }

void BDSExecOptions::Usage() const
{
  G4cout<<"Usage: bdsim [options]" << G4endl;
  G4cout<<"Note options are case sensitive." << G4endl;
  G4cout<<"bdsim --help : display this message" << G4endl;
  G4cout<<"Options (alphabetically):" << G4endl;
  G4cout<<"--file=<filename>            : specify the input file "                           << G4endl
        <<"--batch                      : batch mode - no graphics"                          << G4endl
        <<"--reference or --citation    : print the citation information then quit"          << G4endl
        <<"--circular                   : assume circular machine - turn control"            << G4endl
        <<"--colours                    : list available colours included in bdsim"          << G4endl
        <<"                               by default"                                        << G4endl
        <<"--distrFile=<filename>       : file to use for bunch distribution"                << G4endl
        <<"--distrFileNLinesSkip=N      : skip N lines in the file (userfile)"               << G4endl
        <<"--distrFileLoopNTimes=N      : repeat the distribution file N times"              << G4endl
        <<"--exportGeometryTo=<filename.extension> : export the fully constructed geometry"  << G4endl
        <<"                               to a file. Only gdml extension supported."         << G4endl
        <<"--E0=N                       : set E0 for the bunch for this run (GeV only)"      << G4endl
        <<"--Ek0=N                      : set Ek0 for the bunch for this run (GeV only)"     << G4endl
        <<"--geant4MacroFileName=<filename>        : macro run after visualisation"          << G4endl
        <<"--geant4PhysicsMacroFileName=<filename> : physics macro file name"                << G4endl
        <<"--generatePrimariesOnly      : generate N primary particle coordinates"           << G4endl
        <<"                               without simulation then quit"                      << G4endl
        <<"--materials                  : list materials included in bdsim by default"       << G4endl
        <<"--ngenerate=N                : the number of primary events to simulate:"         << G4endl
        <<"                               overrides ngenerate option in the input gmad file" << G4endl
        <<"--nturns=N                   : the number of turns to simulate:"                  << G4endl
        <<"                               overrides nturns option in the input gmad file"    << G4endl
        <<"--output=<fmt>               : output format (rootevent|none), default rootevent" << G4endl
        <<"--outfile=<file>             : output file name. Will be appended with _N"        << G4endl
        <<"                               where N = 0, 1, 2, 3... etc."                      << G4endl
        <<"--printFractionEvents=N      : fraction of events to print out (default 0.1)"     << G4endl
        <<"                               -1 is all, range [0-1]"                            << G4endl
        <<"--printFractionTurns=N       : fraction of turns to print out (default 0.2)"      << G4endl
        <<"                               -1 is all, range [0-1]"                            << G4endl
        <<"--printPhysicsProcesses      : print out every particle registered and all "      << G4endl
        <<"                               their processes - depends on physics list in input"<< G4endl
        <<"--P0=N                       : set P0 for the bunch for this run (GeV only)"      << G4endl
        <<"--recreate=<file>            : the rootevent file to recreate events from"        << G4endl
        <<"--seed=N                     : the seed to use for the random number generator"   << G4endl
        <<"--seedStateFileName=<file>   : use this ASCII file seed state to run an event"    << G4endl
        <<"--startFromEvent=N           : event offset to start from when recreating events" << G4endl
        <<"--survey=<file>              : print survey info to <file>"                       << G4endl
        <<"--verbose                    : display general parameters before run"             << G4endl
        <<"--verboseRunLevel=N          : set Geant4 verbosity at run level [0:5]"           << G4endl
        <<"--verboseEventLevel=N        : set Geant4 event manager verbosity level"          << G4endl
        <<"--verboseEventStart=N        : display tracking information from event number N"  << G4endl
        <<"--verboseEventContinueFor=N  : number of events to continue verbose event info for" << G4endl
        <<"--verboseTrackingLevel=N     : set Geant4 verbosity about new tracks."            << G4endl
        <<"--verboseSteppingLevel=N     : set Geant4 Stepping manager verbosity level [0:5]" << G4endl
        <<"--verboseSteppingEventStart=N       : event to start verbose stepping."           << G4endl
        <<"--verboseSteppingEventContinueFor=N : number of events to continue verbose stepping" << G4endl
        <<"                                      information for"                            << G4endl
        <<"--verboseSteppingPrimaryOnly : only print out verbose stepping for primary particle" << G4endl
        <<"--verboseEventBDSIM          : display BDSIM information for every event "        << G4endl
        <<"--verboseSteppingBDSIM       : display BDSIM tracking information after each step"<< G4endl
        <<"--version                    : display version number"                            << G4endl
        <<"--versionGit                 : display version number including git commit SHA1"  << G4endl
        <<"--vis_debug                  : display all volumes in visualiser"                 << G4endl
        <<"--vis_mac=<file>             : file with the visualisation macro script, default" << G4endl
        <<"                               provided by BDSIM openGL (OGLSQt))"                << G4endl
        <<"--writeSeedState             : write an ASCII file seed state for each event"     << G4endl;
}

void BDSExecOptions::PrintCopyright() const
{
  // Print header & program information
  G4cout<<"BDSIM : version @BDSIM_VERSION@"<<G4endl;
  G4cout<<"        (C) 2001-@CURRENT_YEAR@ Royal Holloway University London - GPLv3"  << G4endl;
  G4cout<<G4endl;
  G4cout<<"        Reference: Computer Physics Communications, 107200 (2020)" << G4endl;
  G4cout<<"                   https://doi.org/10.1016/j.cpc.2020.107200"      << G4endl;
  G4cout<<"                   https://arxiv.org/abs/1808.10745"               << G4endl;
  G4cout<<"        Website:   https://bdsim-collaboration.github.io/web/"<<G4endl;
  G4cout<<G4endl;
}

void BDSExecOptions::PrintCitation() const
{
  G4cout << "BDSIM: An accelerator tracking code with particle-matter interactions." << G4endl;
  G4cout << "L.J. Nevay et al., Computer Physics Communications, 1070200 (2020)" << G4endl;
  G4cout << "https://doi.org/10.1016/j.cpc.2020.107200" << G4endl << G4endl;
  G4cout << R"(@article{NEVAY2020107200,)" << G4endl;
  G4cout << R"(title = {BDSIM: An accelerator tracking code with particle–matter interactions},)" << G4endl;
  G4cout << R"(journal = {Comput. Phys. Commun.},)" << G4endl;
  G4cout << R"(volume = {252},)" << G4endl;
  G4cout << R"(pages = {107200},)" << G4endl;
  G4cout << R"(year = {2020},)" << G4endl;
  G4cout << R"(issn = {0010-4655},)" << G4endl;
  G4cout << R"(doi = {https://doi.org/10.1016/j.cpc.2020.107200},)" << G4endl;
  G4cout << R"(url = {https://www.sciencedirect.com/science/article/pii/S0010465520300400},)" << G4endl;
  G4cout << R"(author = {L.J. Nevay and S.T. Boogert and J. Snuverink and A. Abramov and L.C. Deacon and H. Garcia-Morales and H. Lefebvre and S.M. Gibson and R. Kwee-Hinzmann and W. Shields and S.D. Walker},)" << G4endl;
  G4cout << R"(})" << G4endl;
}

void BDSExecOptions::Print() const
{
  for (const auto& key : options.KeysOfSetValues())
    {G4cout << "Executable option> " << std::setw(27) << std::left << key << ": " << std::setw(15) << std::left << options.get_value_string(key) << G4endl;}
  for (const auto& key : beam.KeysOfSetValues())
    {G4cout << "Executable option> " << std::setw(27) << std::left << key << ": " << std::setw(15) << std::left << beam.get_value_string(key)    << G4endl;}
}

G4String BDSExecOptions::GetPath(G4String fileName)
{
  // Set fullPath to mirror what is done in parser.l (i.e. if no environment
  // variable set, assume base filename path is that of the gmad file).
  G4String fullPath = getEnv("BDSIMPATH");
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "filename = " << fileName << G4endl;
#endif
  if (fullPath.empty())
    {
      G4String inputFilepath = "";
      // get the path part of the supplied path to the main input file
      G4String::size_type found = fileName.rfind("/"); // find the last '/'
      if (found != G4String::npos)
        {inputFilepath = fileName.substr(0,found);} // the path is the bit before that
      // else remains empty string
      // need to know whether it's an absolute or relative path
      if ((fileName.substr(0,1)) == "/")
        {// the main file has an absolute path
          fullPath = inputFilepath;
        }
      else
        {
          G4String curDir = BDS::GetCurrentDir();
          fullPath = curDir + "/" + inputFilepath;
        }
    }
  
  if (fullPath.back() != '/') // ensure ends in '/'
    {fullPath += "/";} // only add if needed
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "fullpath = " << fullPath << G4endl;
#endif
  return fullPath;
}
