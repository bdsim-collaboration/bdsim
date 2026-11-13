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
#ifdef USE_LCDD

#ifndef BDSGEOMETRYLCDD_H
#define BDSGEOMETRYLCDD_H

#include "BDSColours.hh"
#include "BDSGeometry.hh"
#include "BDSMaterials.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4PVPlacement.hh"
#include "G4QuadrangularFacet.hh"
#include "G4SubtractionSolid.hh"
#include "G4TessellatedSolid.hh"
#include "G4Trd.hh"
#include "G4TriangularFacet.hh"
#include "G4Tubs.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"

#include "BDSMagFieldMesh.hh"
#include "BDSSamplerSD.hh"

#include <fstream>
#include <map>
#include <vector>

// Headers required for XML parsing
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

/**
 * @brief LCDD geometry format loader.
 * 
 * @author J. C. Carter
 **/

struct POS_REF{
  G4String name;
  G4ThreeVector value;
};

struct ROT_REF{
  G4String name;
  G4ThreeVector value;
};

struct CONST_REF{
  G4String name;
  G4double value;
};

struct VIS_REF{
  G4String name;
  G4VisAttributes* value;
};

class BDSGeometryLCDD: public BDSGeometry
{
public:
  BDSGeometryLCDD(G4String LCDDfile);
  virtual ~BDSGeometryLCDD();

  inline G4String GetFieldVolName(){return itsFieldVolName;}

  void parseDoc();
  void parseLCDD(xmlNodePtr cur);
  void parseHEADER(xmlNodePtr cur);
  void parseDISPLAY(xmlNodePtr cur);
  void parseVIS(xmlNodePtr cur);
  void parseDEFINE(xmlNodePtr cur);
  void parseMATERIALS(xmlNodePtr cur);
  void parseSOLID(xmlNodePtr cur);
  void parseSTRUCTURE(xmlNodePtr cur);
  void parseVOLUME(xmlNodePtr cur);
  void parsePHYSVOL(xmlNodePtr cur, G4String volume_name);
  void parseFIELDS(xmlNodePtr cur);



  G4RotationMatrix* RotateComponent(G4ThreeVector rotvalues);

  virtual void Construct(G4LogicalVolume *marker);
  //  std::vector<G4VPhysicalVolume*> itsMultiplePhysicalVolumes;
  std::vector<G4LogicalVolume*> VOL_LIST;
  G4String parseStrChar(xmlChar* value);
  G4double parseDblChar(xmlChar* value);
  G4bool   parseBoolChar(xmlChar* value);

  G4bool   stripwhitespace(G4String& str);

  // used for parsing strings for math calculations
  G4bool EvaluateExpression(const char*, G4double& result);
  G4bool EvaluateTerm(const char* term, G4int termLength, G4double& result);
  G4bool VerifyExpression(const char*);
  G4bool VerifyNumber(const char*);
  G4bool StrToFloat(const char* str, G4int start, G4int end, G4double& f);

private:
  /// assignment and copy constructor not implemented nor used
  BDSGeometryLCDD& operator=(const BDSGeometryLCDD&);
  BDSGeometryLCDD(BDSGeometryLCDD&);
#ifndef NOUSERLIMITS
  G4UserLimits* itsUserLimits;
#endif
  G4String itsFieldVolName;
  // Fetching of imported objects
  G4VisAttributes*  GetVisByName(G4String name);
  G4VSolid*         GetSolidByName(G4String name);
  G4LogicalVolume*  GetLogVolByName(G4String name);
  G4ThreeVector     GetPosition(G4String name);
  G4ThreeVector     GetPosition(xmlNodePtr cur, G4double lunit=0.0);
  G4RotationMatrix* GetRotation(G4String name);
  G4RotationMatrix* GetRotation(xmlNodePtr cur, G4double aunit=0.0);

  //Reference to world logical volume
  G4String itsWorldRef;
  
  //Solid Builders

  void BuildBox(xmlNodePtr cur);
  void BuildTrd(xmlNodePtr cur);
  void BuildTube(xmlNodePtr cur);
  void BuildPolycone(xmlNodePtr cur);
  void BuildPolyhedra(xmlNodePtr cur);
  void BuildSubtraction(xmlNodePtr cur);
  void BuildTessellated(xmlNodePtr cur);

  G4String itsLCDDfile;
  G4LogicalVolume* itsMarkerVol;
  std::vector <struct CONST_REF> CONST_LIST;
  std::vector <struct POS_REF> POS_LIST;
  std::vector <struct ROT_REF> ROT_LIST;
  std::vector <struct VIS_REF> VIS_LIST;
  std::vector <G4VSolid*> SOLID_LIST;
  std::vector <G4LogicalVolume*> LOGVOL_LIST;

  G4double visRed, visGreen, visBlue;

  G4bool fieldIsUniform;
};

inline G4ThreeVector BDSGeometryLCDD::GetPosition(xmlNodePtr cur, G4double lunit)
{
  G4String name, type;
  G4double unit;
  G4ThreeVector pos;

  name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));	   
  unit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"unit"));	   
  type = parseStrChar(xmlGetProp(cur,(const xmlChar*)"type"));	   
  pos  = G4ThreeVector(parseDblChar(xmlGetProp(cur,(const xmlChar*)"x")),
		       parseDblChar(xmlGetProp(cur,(const xmlChar*)"y")),
		       parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")));
  if(unit!=0.0 && lunit==0) pos*=unit;
  else if(lunit!=0.0) pos*=lunit;
  POS_REF apos;
  apos.name = name;
  apos.value = pos;
  POS_LIST.push_back(apos);
#ifdef BDSDEBUG
  G4cout << "pos: " <<name << " " << pos << G4endl;
#endif
  return pos;
}
inline G4ThreeVector BDSGeometryLCDD::GetPosition(G4String name)
{
  G4int ID = -1;
  G4int i;
  for(i=0; i<(G4int)POS_LIST.size();i++)
    {
      if(POS_LIST[i].name==name)
	{
	  ID=i;
	  break;
	}
    }
  if(ID==-1) // couldn't find it.
    {
      G4cout << "Couldn't find position: " << name<<G4endl;
      G4Exception("Quitting in BDSGeometryLCDD", "-1", FatalException, "");
    } 
#ifdef BDSDEBUG
  G4cout << "pos: " <<name << " " << POS_LIST[ID].value << G4endl;
#endif
  return POS_LIST[ID].value;
}
inline G4RotationMatrix* BDSGeometryLCDD::GetRotation(xmlNodePtr cur, G4double aunit)
{
  G4String name, type;
  G4double unit;
  G4ThreeVector rotvect;
  
  name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));	   
  unit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"unit"));	   
  type = parseStrChar(xmlGetProp(cur,(const xmlChar*)"type"));	   
  rotvect = G4ThreeVector(parseDblChar(xmlGetProp(cur,(const xmlChar*)"x")),
			  parseDblChar(xmlGetProp(cur,(const xmlChar*)"y")),
			  parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")));
  if(unit!=0.0 && aunit==0) rotvect*=unit;
  else if(aunit!=0.0) rotvect*=aunit;
  ROT_REF arot;
  arot.name = name;
  arot.value = rotvect;
  ROT_LIST.push_back(arot);
#ifdef BDSDEBUG
  G4cout << "rot: " << name << " "<<rotvect << G4endl;
#endif
  return RotateComponent(rotvect);
}
inline G4RotationMatrix* BDSGeometryLCDD::GetRotation(G4String name)
{
  G4int ID = -1;
  G4int i;
  for(i=0; i<(G4int)ROT_LIST.size();i++)
    {
      if(ROT_LIST[i].name==name)
	{
	  ID=i;
	  break;
	}
    }
  if(ID==-1) // couldn't find it.
    {
      G4cout << "Couldn't find rotation: " << name<<G4endl;
      G4Exception("Quitting in BDSGeometryLCDD", "-1", FatalException, "");
    } 
#ifdef BDSDEBUG
  G4cout << "rot: " << name << " "<<ROT_LIST[ID].value << G4endl;
#endif
  return RotateComponent(ROT_LIST[ID].value);
}
inline G4VisAttributes* BDSGeometryLCDD::GetVisByName(G4String name)
{
  G4int ID = -1;
  G4int i;
  for(i=0; i<(G4int)VIS_LIST.size(); i++)
    {
      if(VIS_LIST[i].name==name)
	{
	  ID=i;
	  break;
	}
    }
  if(ID==-1) // couldn't find it.
    {
      G4cout << "Couldn't find visref: " << name<<G4endl;
      G4cout << "Using default Vis settings" << G4endl;
      return new G4VisAttributes(*BDSColours::Instance()->GetColour("warning"));
    } 
  
  return VIS_LIST[ID].value;
}
inline G4VSolid* BDSGeometryLCDD::GetSolidByName(G4String name)
{
  G4int ID = -1;
  G4int i;
  for(i=0; i<(G4int)SOLID_LIST.size(); i++)
    {
      if(SOLID_LIST[i]->GetName()==name)
	{
	  ID=i;
	  break;
	}
    }
  if(ID==-1) // couldn't find it.
    {
      G4cout << "Couldn't find solid: " << name<<G4endl;
      G4Exception("Quitting in BDSGeometryLCDD", "-1", FatalException, "");
    } 
  return SOLID_LIST[ID];
}

inline G4LogicalVolume* BDSGeometryLCDD::GetLogVolByName(G4String name)
{
  G4int ID = -1;
  G4int i;
  for(i=0; i<(G4int)LOGVOL_LIST.size(); i++)
    {
      if(LOGVOL_LIST[i]->GetName()==name)
	{
	  ID=i;
	  break;
	}
    }
  if(ID==-1) // couldn't find it.
    {
      G4cout << "Couldn't find logical volume: " << name<<G4endl;
      G4Exception("Quitting in BDSGeometryLCDD", "-1", FatalException, "");
    }
  return LOGVOL_LIST[ID];
}

inline G4String BDSGeometryLCDD::parseStrChar(xmlChar* value)
{
  if(value==nullptr) return "";
  G4String val = G4String((char*)value);
#ifdef BDSDEBUG
  G4cout << "BDSGeometryLCDD:parseStrChar> value = " << value << G4endl;
  G4cout << "strVAL: "<< val << G4endl;
#endif
  return val;
}

inline G4bool BDSGeometryLCDD::parseBoolChar(xmlChar* value)
{
  if(value==nullptr) return false;
#ifdef BDSDEBUG
  G4cout << "BDSGeometryLCDD:parseBoolChar> value = " << value << G4endl;
#endif
  G4String val = G4String((char*)value);
  
  if(val=="true") return true;
  if(val=="false") return false;

  return false;
}

inline G4double BDSGeometryLCDD::parseDblChar(xmlChar* value)
{
  if(value==nullptr) return 0.0;
  G4String val = G4String((char*)value);
  stripwhitespace(val);
#ifdef BDSDEBUG
  G4cout << "BDSGeometryLCDD:parseDblChar> value = " << value << G4endl;
  G4cout << "dblVAL: "<< val << G4endl;
#endif
  // now check for defined constants
  for(unsigned int i=0; i<CONST_LIST.size(); i++)
    {
      if(val.contains(CONST_LIST[i].name))
	{
	  //check that this is the entire constant, and not part of a larger name
	  // e.g "zpos" and "zpos_world"
	  G4int endpos = val.index(CONST_LIST[i].name)+(CONST_LIST[i].name).length();
	  G4int startpos = val.index(CONST_LIST[i].name);
	  G4bool replaceStr = false;
	  if(val==CONST_LIST[i].name) replaceStr = true;
	  
	  //check after string for math operators or end of string
	  else if((val.data()[endpos] == '+' ||
		   val.data()[endpos] == '-' ||
		   val.data()[endpos] == '*' ||
		   val.data()[endpos] == '/' ||
		   endpos>=(int)val.length())     &&
		  (val.data()[startpos-1] == '+' ||
		   val.data()[startpos-1] == '-' ||
		   val.data()[startpos-1] == '*' ||
		   val.data()[startpos-1] == '/' ||
		   startpos-1<=0))
	    replaceStr = true;
 
	  // replace all instances of constant with value

	  if(replaceStr)
	    {
	      char constval[40];
	      sprintf(constval,"%f",CONST_LIST[i].value);
	      val.replace(val.index((CONST_LIST[i].name)), (CONST_LIST[i].name).length(), constval);
#ifdef BDSDEBUG
	      G4cout << "BDSGeometryLCDD::ParseDbl Replacing " << CONST_LIST[i].name << " with " << constval << G4endl;
#endif
	      i=0;
	    }
	}
    }
  G4double dbl_val = 0.0;

  if(VerifyExpression(val)) EvaluateExpression(val.data(),dbl_val);
  else
    {
#ifdef BDSDEBUG
      G4cout << "BDSGeometryLCDD::ParseDbl Unable to evaluate expression: " << value << G4endl;
#endif
      G4Exception("Check spellings and that constants are declared properly", "-1", FatalException, "");
    }
    

#ifdef BDSDEBUG
      G4cout << "BDSGeometryLCDD::ParseDbl returning value: " << dbl_val << G4endl;
#endif
  return dbl_val;
}

inline G4bool BDSGeometryLCDD::EvaluateExpression(const char* expr, G4double& result)
{

  G4int length = strlen(expr);
  G4int end = length-1;
  double term;
  result = 0.0;
  G4int i;
  for(i = length-1; i>=0; i--)
    {
      if(expr[i]=='+')
	{
	  if(!EvaluateTerm(expr + i + 1, end-i, term))
	    return false;
	  result += term;
	  end = i-1;
	}
      else if (expr[i] == '-')
	{
	  if(!EvaluateTerm(expr + i + 1, end-i, term))
	    return false;
	  result -= term;
	  end = i-1;	  
	}
    }
  
  if (expr[0] !='+' && expr[0]!='-')
    {
      if(!EvaluateTerm(expr, end+1, term))
	return false;
      result += term;
      end = i-1;	  
    }

  return true;

}

inline G4bool BDSGeometryLCDD::EvaluateTerm(const char* term, G4int termLength, G4double& result)
{
  G4int end = termLength-1;
  result = 1.0;
  G4double number;
  G4int i;
  for (i = end ; i>=0; i--)
    {
      if(term[i] == '*')
	{
	  if(!StrToFloat(term+i+1, 0, end-i, number))
	    return false;
	  result *= number;
	  end = i-1;
	}
      else if(term[i] == '/')
	{
	  if(!StrToFloat(term+i+1, 0, end-i, number))
	    return false;
	  result /= number;
	  end = i-1;
	}
    }
  if (term[0] != '*' && term[0] != '/')
    {    
      if(!StrToFloat(term, 0, end+1, number))
	return false;
      result *= number;
    }
  return true;
}

inline G4bool BDSGeometryLCDD::VerifyExpression(const char* expr)
{
  G4int length = strlen(expr);
  if(length==0) return false;
  G4int i;
  for (i=0; i<length; i++)
    {
      if(expr[i] >='0' && expr[i] <= '9')
	continue;
      else
	if (expr[i] == '+' || expr[i] =='-' || expr[i] =='*' ||
	    expr[i] == '/' || expr[i] =='.' || expr[i] =='e' ||
	    expr[i] == 'E' || expr[i] =='^')
	  continue;
	else return false;
    }
  return true;
}

inline G4bool BDSGeometryLCDD::VerifyNumber(const char* expr)
{
  G4int length = strlen(expr);
  if(length==0) return false;
  G4int i;
  for (i=0; i<length; i++)
    {
      if(expr[i] >='0' && expr[i] <= '9')
	continue;
      else
	if (expr[i] =='.') continue;
	else return false;
    }
  return true;
}

inline G4bool BDSGeometryLCDD::StrToFloat(const char* str, G4int start, G4int end, G4double& f)
{
  if(!str) return false;
  
  G4double factor=0.1;
  G4int i,j;
  for(i=start; i<end; i++)
    {

      if(str[i]=='.' && (str[i+1]=='e' || str[i]=='E' || str[i] =='^'))
	{
	  for(j= i+2; j< end; j++)
	    if(str[j] <'0' || str[j] >'9') return false;
	  break;
	}
      else if(str[i]=='.')
	{
	  for(j= i+1; j< end; j++)
	    if(str[j] <'0' || str[j] >'9') return false;
	  break;
	}
      else if(str[i] =='^')
	{
	  for(j= i+1; j< end; j++)
	    {
	      if(str[j] == '.') continue;
	      if(str[j] <'0' || str[j] >'9') return false;
	    }
	  break;
	}
      else if(str[i]=='e'  || str[i]=='E')
	{
	  for(j= i+1; j< end; j++)
	    if(str[j] <'0' || str[j] >'9') return false;
	  break;
	}
      else if(str[i] <'0' || str[i]>'9') return false;
      factor *=10;
    }
  
  f=0.0;
  for(i=start; i<end; i++)
    {
      if(str[i]=='.') continue;

      else if(str[i]=='e'  || str[i]=='E')
	{		
	  G4double f1 = 0.0;
	  G4double f2 = 0.0;
	  StrToFloat(str,0,i,f1);
	  StrToFloat(str,i+1,end,f2);
	  f = f1*pow(10,f2);
	}
      else if(str[i]=='^')
	{
	  G4double f1 = 0.0;
	  G4double f2 = 0.0;
	  StrToFloat(str,0,i,f1);
	  StrToFloat(str,i+1,end,f2);
	  f = pow(f1,f2);
	}

      else
	{
	  f+= (G4double)(str[i] - '0')*factor;
	  factor *= 0.1;
	}
    }
  return true;

}

inline G4bool BDSGeometryLCDD::stripwhitespace(G4String& str)
{
  //return false if nothing done
  if(str.length()==0) return false;

  G4String newstr;

  str_size i;
  for(i=0;i<str.length();i++)
    {
      if(str[i] != ' ')
	newstr+=str[i];
    }

  str = newstr;
  return true;
}

inline void BDSGeometryLCDD::BuildBox(xmlNodePtr cur)
{
  G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  G4double x = parseDblChar(xmlGetProp(cur,(const xmlChar*)"x")); 
  G4double y = parseDblChar(xmlGetProp(cur,(const xmlChar*)"y")); 
  G4double z = parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")); 
  
  G4double lunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"lunit"));
  G4double aunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"aunit"));
  
  if(aunit!=0) {}
  if(lunit!=0) 
    {
      x*=lunit;
      y*=lunit;
      z*=lunit;
    }
  
  SOLID_LIST.push_back(new G4Box(name,
				 x/2.0,
				 y/2.0,
				 z/2.0)
		       );
}

inline void BDSGeometryLCDD::BuildTrd(xmlNodePtr cur)
{
  G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  G4double x1 = parseDblChar(xmlGetProp(cur,(const xmlChar*)"x1")); 
  G4double y1 = parseDblChar(xmlGetProp(cur,(const xmlChar*)"y1")); 
  G4double x2 = parseDblChar(xmlGetProp(cur,(const xmlChar*)"x2")); 
  G4double y2 = parseDblChar(xmlGetProp(cur,(const xmlChar*)"y2")); 
  G4double z = parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")); 
  
  G4double lunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"lunit"));
  G4double aunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"launit"));
  
  if(aunit!=0) {}
  if(lunit!=0) 
    {
      x1*=lunit;
      x2*=lunit;
      y1*=lunit;
      y2*=lunit;
      z*=lunit;
    }
  
  SOLID_LIST.push_back(new G4Trd(name,
				 x1/2.0,
				 x2/2.0,
				 y1/2.0,
				 y2/2.0,
				 z/2.0)
		       );
}

inline void BDSGeometryLCDD::BuildTube(xmlNodePtr cur)
{
  G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  G4double rmin = parseDblChar(xmlGetProp(cur,(const xmlChar*)"rmin"));
  G4double rmax = parseDblChar(xmlGetProp(cur,(const xmlChar*)"rmax"));
  G4double length = parseDblChar(xmlGetProp(cur,(const xmlChar*)"z"));
  G4double dphi = parseDblChar(xmlGetProp(cur,(const xmlChar*)"deltaphi"));
  
  G4double lunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"lunit"));
  G4double aunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"launit"));
  
  if(aunit!=0) dphi*=aunit;
  if(lunit!=0) 
    {
      rmin*=lunit;
      rmax*=lunit;
      length*=lunit;
    }
  
  SOLID_LIST.push_back(new G4Tubs(name,
				  rmin,
				  rmax,
				  length/2.0,
				  0.0,
				  dphi)
		       );
}
inline void BDSGeometryLCDD::BuildPolycone(xmlNodePtr cur)
{
  xmlNodePtr tempcur = cur->xmlChildrenNode;
  
  G4int numZPlanes = 0;
  while(tempcur!=nullptr)
    {
      if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"zplane")))
	numZPlanes++;
      tempcur = tempcur->next;
    }
  
  G4double* zPlanes = nullptr;
  G4double* rInner = nullptr;
  G4double* rOuter = nullptr;
  zPlanes = new G4double[numZPlanes];
  rInner = new G4double[numZPlanes];
  rOuter = new G4double[numZPlanes];
  
  tempcur = cur->xmlChildrenNode;
  G4int i=0;
  while(tempcur!=nullptr)
    {
      if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"zplane")))
	{
	  zPlanes[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"z"));
	  rInner[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"rmin"));
	  rOuter[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"rmax"));
	  i++;
	}
      tempcur = tempcur->next;
    }
  
  SOLID_LIST.push_back(new G4Polycone((parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"))),
				      parseDblChar(xmlGetProp(cur,(const xmlChar*)"startphi")),
				      parseDblChar(xmlGetProp(cur,(const xmlChar*)"deltaphi")),
				      numZPlanes,
				      zPlanes,
				      rInner,
				      rOuter)
		       );
  
  delete [] rInner;
  rInner = nullptr;
  delete [] rOuter;
  rOuter = nullptr;
  delete [] zPlanes;
  zPlanes = nullptr;
}

inline void BDSGeometryLCDD::BuildPolyhedra(xmlNodePtr cur)
{
  xmlNodePtr tempcur = cur->xmlChildrenNode;
  
  G4int numZPlanes = 0;
  while(tempcur!=nullptr)
    {
      if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"zplane")))
	numZPlanes++;
      tempcur = tempcur->next;
    }
  
  G4double* zPlanes = nullptr;
  G4double* rInner = nullptr;
  G4double* rOuter = nullptr;
  zPlanes = new G4double[numZPlanes];
  rInner = new G4double[numZPlanes];
  rOuter = new G4double[numZPlanes];
  
  tempcur = cur->xmlChildrenNode;
  G4int i=0;
  while(tempcur!=nullptr)
    {
      if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"zplane")))
	{
	  zPlanes[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"z"));
	  rInner[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"rmin"));
	  rOuter[i] = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"rmax"));
	  i++;
	}
      tempcur = tempcur->next;
    }
  
  SOLID_LIST.push_back(new G4Polyhedra((parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"))),
				       parseDblChar(xmlGetProp(cur,(const xmlChar*)"startphi")),
				       parseDblChar(xmlGetProp(cur,(const xmlChar*)"deltaphi")),
				       (G4int)parseDblChar(xmlGetProp(cur,(const xmlChar*)"numsides")),
				       numZPlanes,
				       zPlanes,
				       rInner,
				       rOuter)
		       );
  
  delete [] rInner;
  rInner = nullptr;
  delete [] rOuter;
  rOuter = nullptr;
  delete [] zPlanes;
  zPlanes = nullptr;
}




inline void BDSGeometryLCDD::BuildSubtraction(xmlNodePtr cur)
{
  
  G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  G4double lunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"lunit"));
  G4double aunit = parseDblChar(xmlGetProp(cur,(const xmlChar*)"launit"));
  G4String firstname, secondname;
  
  G4ThreeVector PlacementPoint = G4ThreeVector(0.,0.,0.);

  G4RotationMatrix* componentRotation = nullptr;

  G4ThreeVector FirstPlacementPoint = G4ThreeVector(0.,0.,0.);

  xmlNodePtr tempcur = cur->xmlChildrenNode;
  
  while(tempcur!=nullptr)
    {
      if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"first")))
	firstname = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref"));
      
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"second")))
	secondname = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref"));
      
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"position")))
	PlacementPoint = GetPosition(tempcur,lunit);
      
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"positionref")))
	PlacementPoint = GetPosition(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref")));
      
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"rotation")))
	componentRotation = GetRotation(tempcur,aunit);

      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"rotationref")))
	componentRotation = GetRotation(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref")));
      
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"firstposition")))
	G4cout << "BDSGeometryLCDD::BuildSubtraction: " << name<< " firstposition not supported" << G4endl;

      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"firstpositionref")))
	G4cout << "BDSGeometryLCDD::BuildSubtraction: " << name<< " firstpositionref not supported" << G4endl;
	
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"firstrotation")))
	G4cout << "BDSGeometryLCDD::BuildSubtraction: " << name<< " firstrotation not supported" << G4endl;
	
      else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"firstrotationref")))
	G4cout << "BDSGeometryLCDD::BuildSubtraction: " << name<< " firstrotationref not supported" << G4endl;
	      
      tempcur = tempcur->next;
    }
  
  
  if(componentRotation==0 && PlacementPoint.x()==0 && PlacementPoint.y()==0 && PlacementPoint.z()==0)
    {
      SOLID_LIST.push_back(new G4SubtractionSolid(name,
						  GetSolidByName(firstname),
						  GetSolidByName(secondname))
			   );
    }
  else
    {
      G4Transform3D transform(*componentRotation,PlacementPoint);
      SOLID_LIST.push_back(new G4SubtractionSolid(name,
						  GetSolidByName(firstname),
						  GetSolidByName(secondname),
						  transform)
			   );
      
    }
    
}

inline void BDSGeometryLCDD::BuildTessellated(xmlNodePtr cur){
  G4TessellatedSolid* tessellatedSolid = new G4TessellatedSolid(parseStrChar(xmlGetProp(cur,(const xmlChar*)"name")));

  xmlNodePtr tempcur = cur->xmlChildrenNode;

  G4int numTriFacets = 0;
  G4int numQuadFacets = 0;
  G4String sType; 
  G4FacetVertexType iType=ABSOLUTE;
  G4ThreeVector vertex[4];

  while(tempcur!=nullptr) {
    
    if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"triangular")) || (!xmlStrcmp(tempcur->name, (const xmlChar *)"quadrangular"))){
      sType = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"type"));
      if(!sType.compare("ABSOLUTE")){
	iType=ABSOLUTE;
      } else if (!sType.compare("RELATIVE")){
	iType=RELATIVE;
      } else {
	G4cerr << "BDSGeometryLCDD::BuildTessellateSolid -> error, unknown type " << sType << ". Valid options are ABSOLUTE or RELATIVE." << G4endl;   
      }
      vertex[0]=GetPosition(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"vertex1")));
      vertex[1]=GetPosition(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"vertex2")));
      vertex[2]=GetPosition(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"vertex3")));
    
      if(!xmlStrcmp(tempcur->name, (const xmlChar *)"triangular")){
#ifdef BDSDEBUG
	G4cout << "BDSGeometryLCDD adding triangular facet, vertices: "<< G4endl;
	G4cout << "vertex1: " << vertex[0].x() << " " << vertex[0].y() << " " << vertex[0].z() << G4endl;
	G4cout << "vertex2: " << vertex[1].x() << " " << vertex[1].y() << " " << vertex[1].z() << G4endl;
	G4cout << "vertex3: " << vertex[2].x() << " " << vertex[2].y() << " " << vertex[2].z() << G4endl;
#endif

	tessellatedSolid->AddFacet((G4VFacet*)(new G4TriangularFacet(vertex[0], vertex[1], vertex[2], iType)));	
	numTriFacets++;
      } else {
	vertex[3]=GetPosition(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"vertex4")));
#ifdef BDSDEBUG
	G4cout << "BDSGeometryLCDD adding quadrangular facet, vertices: "<< G4endl;
	G4cout << "vertex1: " << vertex[0].x() << " " << vertex[0].y() << " " << vertex[0].z() << G4endl;
	G4cout << "vertex2: " << vertex[1].x() << " " << vertex[1].y() << " " << vertex[1].z() << G4endl;
	G4cout << "vertex3: " << vertex[2].x() << " " << vertex[2].y() << " " << vertex[2].z() << G4endl;
	G4cout << "vertex4: " << vertex[3].x() << " " << vertex[3].y() << " " << vertex[3].z() << G4endl;
#endif
	tessellatedSolid->AddFacet((G4VFacet*)(new G4QuadrangularFacet(vertex[0], vertex[1], vertex[2], vertex[3], iType)));	
	numQuadFacets++;
      }
    }
    tempcur = tempcur->next;
  }
  G4cout << "BDSGeometryLCDD closing the solid... "<< G4endl;
  tessellatedSolid->SetSolidClosed(true);
  
  SOLID_LIST.push_back(tessellatedSolid);
}

#endif

#endif //from use_xml
