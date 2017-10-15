#include "beam.h"

#include <algorithm>
#include <iostream>

using namespace GMAD;

Beam::Beam():
  BeamBase()
{
  PublishMembers();
}

Beam::Beam(const GMAD::BeamBase& options):
  BeamBase(options)
{
  PublishMembers();
}

double Beam::get_value(std::string property_name) const
{
  double value;
  try
    {value = get<double>(this,property_name);}
  catch (std::runtime_error)
    {
      try
	{value = (double)get<int>(this,property_name);}	// try int and convert
      catch (std::runtime_error)
	{
	  try
	    {value = (double)get<long>(this,property_name);} // try long and convert
	  catch (std::runtime_error)
	    {
	      std::cerr << "beam.cc> Error: unknown property \"" << property_name
			<< "\" (only works on numerical properties)" << std::endl;
	      exit(1);
	    }
	}
    }
  return value;
}

void Beam::Amalgamate(const Beam& beamIn, bool override, int startFromEvent)
{
  if (override)
    {
      for (auto const key : beamIn.setKeys)
	{
	  try
	    {
          set(this, &beamIn, key);
          setKeys.push_back(key);
        }
	  catch (std::runtime_error)
	    {
	      std::cerr << "Error: Amalgate unknown beam option \"" << key << "\"" << std::endl;
	      exit(1);
	    }
	}
      // if we're recreating from a file, still load external file but
      // advance to the event number
      nlinesIgnore += startFromEvent;
    }
  else
    {// don't override - ie give preference to ones set in this instance
      for (auto const key : beamIn.setKeys)
	{
	  auto const& ok = setKeys; // shortcut
	  auto result = std::find(ok.begin(), ok.end(), key);
	  if (result == ok.end())
	    {//it wasn't found so ok to copy
	      try
		{
		  set(this, &beamIn, key);
		  setKeys.push_back(key);
		}
	      catch (std::runtime_error)
		{
		  std::cerr << "Error: Amalgate unknown beam option \"" << key << "\""
			    << std::endl;
		  exit(1);
		}
	    }
	}
    }
}

bool Beam::HasBeenSet(std::string name) const
{
  auto result = std::find(setKeys.begin(), setKeys.end(), name);
  if (result == setKeys.end())
    {return false;}
  else
    {return true;}
}

void Beam::PublishMembers()
{
  publish("particle",             &Beam::particleName);
  publish("energy",               &Beam::beamEnergy);
  publish("distrType",            &Beam::distrType);
  publish("xDistrType",           &Beam::xDistrType);
  publish("yDistrType",           &Beam::yDistrType);
  publish("zDistrType",           &Beam::zDistrType);
  publish("distrFile",            &Beam::distrFile);
  publish("distrFileFormat",      &Beam::distrFileFormat);
  publish("matchDistrFileLength", &Beam::matchDistrFileLength);
  publish("nlinesIgnore",         &Beam::nlinesIgnore);
  
  publish("X0",    &Beam::X0);
  publish("Y0",    &Beam::Y0);
  publish("Z0",    &Beam::Z0);
  publish("S0",    &Beam::S0);
  publish("Xp0",   &Beam::Xp0);
  publish("Yp0",   &Beam::Yp0);
  publish("Zp0",   &Beam::Zp0);
  publish("T0",    &Beam::T0);
  publish("E0",    &Beam::E0);
  publish("sigmaT",&Beam::sigmaT);
  publish("sigmaE",&Beam::sigmaE);
  
  publish("betx",  &Beam::betx);
  publish("bety",  &Beam::bety);
  publish("alfx",  &Beam::alfx);
  publish("alfy",  &Beam::alfy);
  publish("emitx", &Beam::emitx);
  publish("emity", &Beam::emity);
  publish("dispx", &Beam::dispx);
  publish("dispy", &Beam::dispy);
  publish("dispxp",&Beam::dispxp);
  publish("dispyp",&Beam::dispyp);
  
  // options for beam distrType="gauss"
  publish("sigmaX", &Beam::sigmaX);
  publish("sigmaXp",&Beam::sigmaXp);
  publish("sigmaY", &Beam::sigmaY);
  publish("sigmaYp",&Beam::sigmaYp);

  // options for beam distrType="square" or distrType="circle"
  publish("envelopeX", &Beam::envelopeX);
  publish("envelopeXp",&Beam::envelopeXp);
  publish("envelopeY", &Beam::envelopeY);
  publish("envelopeYp",&Beam::envelopeYp);
  publish("envelopeT", &Beam::envelopeT);
  publish("envelopeE", &Beam::envelopeE);
  publish("envelopeR", &Beam::envelopeR);
  publish("envelopeRp",&Beam::envelopeRp);

  // options for beam distrType="gaussmatrix"
  publish("sigma11",&Beam::sigma11);
  publish("sigma12",&Beam::sigma12);
  publish("sigma13",&Beam::sigma13);
  publish("sigma14",&Beam::sigma14);
  publish("sigma15",&Beam::sigma15);
  publish("sigma16",&Beam::sigma16);
  publish("sigma22",&Beam::sigma22);
  publish("sigma23",&Beam::sigma23);
  publish("sigma24",&Beam::sigma24);
  publish("sigma25",&Beam::sigma25);
  publish("sigma26",&Beam::sigma26);
  publish("sigma33",&Beam::sigma33);
  publish("sigma34",&Beam::sigma34);
  publish("sigma35",&Beam::sigma35);
  publish("sigma36",&Beam::sigma36);
  publish("sigma44",&Beam::sigma44);
  publish("sigma45",&Beam::sigma45);
  publish("sigma46",&Beam::sigma46);
  publish("sigma55",&Beam::sigma55);
  publish("sigma56",&Beam::sigma56);
  publish("sigma66",&Beam::sigma66);

  // options for beam distrType="eshell"
  publish("shellX",      &Beam::shellX);
  publish("shellXp",     &Beam::shellXp);
  publish("shellY",      &Beam::shellY);
  publish("shellYp",     &Beam::shellYp);
  publish("shellXWidth", &Beam::shellXWidth);
  publish("shellXpWidth",&Beam::shellXpWidth);
  publish("shellYWidth", &Beam::shellYWidth);
  publish("shellYpWidth",&Beam::shellYpWidth);

  // options for beam distrType="ring"
  publish("Rmin",&Beam::Rmin);
  publish("Rmax",&Beam::Rmax);

  // options for beam distrType="halo"
  publish("haloNSigmaXInner",      &Beam::haloNSigmaXInner);
  publish("haloNSigmaXOuter",      &Beam::haloNSigmaXOuter);
  publish("haloNSigmaYInner",      &Beam::haloNSigmaYInner);
  publish("haloNSigmaYOuter",      &Beam::haloNSigmaYOuter);
  publish("haloXCutInner",         &Beam::haloXCutInner);
  publish("haloYCutInner",         &Beam::haloYCutInner);
  publish("haloPSWeightParameter", &Beam::haloPSWeightParameter);
  publish("haloPSWeightFunction",  &Beam::haloPSWeightFunction);
}