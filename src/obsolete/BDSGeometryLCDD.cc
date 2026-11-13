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
#include "BDSGlobalConstants.hh" 
#include "BDSGeometryLCDD.hh"
#include "BDSGeometryType.hh"
#include "BDSMaterials.hh"
#include "BDSSamplerSD.hh"
#include "BDSFieldMagDetectorSolenoid.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4TessellatedSolid.hh"

#include <cstdlib>
#include <cstring>
#include <list>


BDSGeometryLCDD::BDSGeometryLCDD(G4String LCDDfile):
  itsMarkerVol(nullptr),
  fieldIsUniform(false)
{
#ifndef NOUSERLIMITS
  itsUserLimits = new G4UserLimits();
  itsUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  if(BDSGlobalConstants::Instance()->ThresholdCutCharged()>0){
    itsUserLimits->SetUserMinEkine(BDSGlobalConstants::Instance()->ThresholdCutCharged());
  }
#endif

  itsFieldVolName="";
  itsLCDDfile = LCDDfile;
  visRed = visGreen = visBlue = 0.0;

  // define some standard constants
  CONST_REF aconst;
  aconst.name="pi";
  aconst.value=CLHEP::pi;
  CONST_LIST.push_back(aconst);

  aconst.name="TWOPI";
  aconst.value=CLHEP::twopi;
  CONST_LIST.push_back(aconst);

  
  aconst.name="HALFPI";
  aconst.value=CLHEP::halfpi;
  CONST_LIST.push_back(aconst);

  // define units using mm and radians =1 and tesla as =0.001
  aconst.name="radian";
  aconst.value=CLHEP::radian;
  CONST_LIST.push_back(aconst);

  aconst.name="degree";
  aconst.value=CLHEP::degree;
  CONST_LIST.push_back(aconst);
  
  aconst.name="mm";
  aconst.value=CLHEP::mm;
  CONST_LIST.push_back(aconst);
  
  aconst.name="cm";
  aconst.value=CLHEP::cm;
  CONST_LIST.push_back(aconst);
  
  aconst.name="m";
  aconst.value=CLHEP::m;
  CONST_LIST.push_back(aconst);

  aconst.name="cm2";
  aconst.value=CLHEP::cm2;
  CONST_LIST.push_back(aconst);

  aconst.name="cm3";
  aconst.value=CLHEP::cm3;
  CONST_LIST.push_back(aconst);

  aconst.name="tesla";
  aconst.value=CLHEP::tesla;
  CONST_LIST.push_back(aconst);

  aconst.name="g";
  aconst.value=CLHEP::g;
  CONST_LIST.push_back(aconst);

  aconst.name="kg";
  aconst.value=CLHEP::kg;
  CONST_LIST.push_back(aconst);

  aconst.name="mol";
  aconst.value=CLHEP::mole;
  CONST_LIST.push_back(aconst);

  aconst.name="mole";
  aconst.value=CLHEP::mole;
  CONST_LIST.push_back(aconst);
#ifdef BDSDEBUG
  G4cout << "BDSGeometryLCDD CONST_LIST defined units: " <<  G4endl;
  for(unsigned int i=0; i<CONST_LIST.size(); i++){
    G4cout << CONST_LIST[i].name << " " << CONST_LIST[i].value << G4endl;
  }
#endif
}

BDSGeometryLCDD::~BDSGeometryLCDD()
{
  delete itsUserLimits;
}

void BDSGeometryLCDD::Construct(G4LogicalVolume *marker)
{
  itsMarkerVol = marker;
  parseDoc();

}

void BDSGeometryLCDD::parseDoc()
{
  const char* docname = itsLCDDfile.c_str();
  xmlDocPtr doc;
  xmlNodePtr cur;

  doc = xmlParseFile(docname);

  if (doc == nullptr )
  {
  G4Exception("Document not parsed successfully", "-1", FatalException, "");
  }

  cur = xmlDocGetRootElement(doc); //GO the first node

  if (cur == nullptr)
  {
   xmlFreeDoc(doc);
   G4Exception("empty document", "-1", FatalException, "");
  }

//test the xml format, must be start by lcdd
   if (xmlStrcmp(cur->name, (const xmlChar *) "lcdd"))
   {
   xmlFreeDoc(doc);
   G4Exception("XML document of the wrong type, root node != lcdd\nCheck your XML file\n", "-1", FatalException, "");
   }

   cur = cur->xmlChildrenNode;
   
   while (cur != nullptr)
     {
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"header")))
	 {
	   parseHEADER(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"iddict")))
	 {
	   G4cout << "Warning: from BDSGeometryLCDD.cc::parseDoc() - LCDD iddict not currently implemented in BDSIM" << G4endl;
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"sensitive_detectors")))
	 {
	   G4cout << "Warning: from BDSGeometryLCDD.cc::parseDoc() - LCDD sensitive_detectors not currently implemented in BDSIM" << G4endl;
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"limits")))
	 {
	   G4cout << "Warning: from BDSGeometryLCDD.cc::parseDoc() - LCDD limits not currently implemented in BDSIM" << G4endl;
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"regions")))
	 {
	   G4cout << "Warning: from BDSGeometryLCDD.cc::parseDoc() - LCDD limits not currently implemented in BDSIM" << G4endl;
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"display")))
	 {
	   parseDISPLAY(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"gdml")))
	 {
	   parseLCDD(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"fields")))
	 {
	   G4cout << "Importing LCDD fields" << G4endl;
	   parseFIELDS(cur);
	 }
       cur = cur->next;
     }
   if(!field){
#ifdef BDSDEBUG
     G4cout << "BDSGeometryLCDD.cc> No magnetic fields defined. Making default (zero) BDSMagField" << G4endl;
#endif
     //field = new BDSMagFieldMesh();
   }

   xmlFreeDoc(doc);
   return;
}

void BDSGeometryLCDD::parseHEADER(xmlNodePtr cur)
{
  G4cout << "================================" << G4endl;
  G4cout << "          LCDD header           " << G4endl;
  G4cout << "--------------------------------" << G4endl;
  

   xmlNodePtr tempcur = cur->xmlChildrenNode;
   while (tempcur != nullptr)
     {
       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"detector")))
	 {
	   G4cout << "Detector: " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name")) << G4endl;
	 }
       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"generator")))
	 {
	   G4cout << "Generated by: " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name")) 
		  << ", version " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"version")) 
		  << ", file " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"file"))
 		  << ", checksum " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"checksum")) 
		  << G4endl;
	 }
       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"author")))
	 {
	   G4cout << "Author: " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name")) << G4endl;
	 }
       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"comment")))
	 {
	   G4cout << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name")) << G4endl;
	 }
       tempcur = tempcur->next;
     }
   return;
}

void BDSGeometryLCDD::parseDISPLAY(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;

  while (cur != nullptr)
     {
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"vis")))
	 {
	   parseVIS(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"VisType")))
	 {
	   G4Exception("LCDD VisType not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"DisplayType")))
	 {
	   G4Exception("LCDD DisplayType not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       cur = cur->next;
     }
  return;
}

void BDSGeometryLCDD::parseVIS(xmlNodePtr cur)
{
  G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  G4bool visible = parseBoolChar(xmlGetProp(cur,(const xmlChar*)"visible"));
  G4bool show_daughters = parseBoolChar(xmlGetProp(cur,(const xmlChar*)"show_daughters"));
  G4String line_style = parseStrChar(xmlGetProp(cur,(const xmlChar*)"line_style"));
  G4String drawing_style = parseStrChar(xmlGetProp(cur,(const xmlChar*)"drawing_style"));

  G4double R = 1.0, G = 1.0, B = 1.0, alpha = 1.0;

  cur = cur->xmlChildrenNode;
  
  while(cur!=nullptr)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"color")))
	{
	  R = parseDblChar(xmlGetProp(cur,(const xmlChar*)"R"));
	  G = parseDblChar(xmlGetProp(cur,(const xmlChar*)"G"));
	  B = parseDblChar(xmlGetProp(cur,(const xmlChar*)"B"));
	  alpha = parseDblChar(xmlGetProp(cur,(const xmlChar*)"alpha"));

	}
      else if ((!xmlStrcmp(cur->name, (const xmlChar *)"color")))
	{
	  
	}
      cur = cur->next;
    }

  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(R,G,B,alpha));

  if(drawing_style=="solid" )VisAtt->SetForceSolid(true);
  else if(drawing_style=="wireframe") VisAtt->SetForceWireframe(true);

  if(line_style=="unbroken" )VisAtt->SetLineStyle(G4VisAttributes::unbroken);
  else if(line_style=="dashed") VisAtt->SetLineStyle(G4VisAttributes::dashed);
  else if(line_style=="dotted") VisAtt->SetLineStyle(G4VisAttributes::dotted);

  VisAtt->SetVisibility(visible);
  if(show_daughters) VisAtt->SetDaughtersInvisible(false);
  else VisAtt->SetDaughtersInvisible(true);

  VIS_REF avis;
  avis.name = name;
  avis.value = VisAtt;
  VIS_LIST.push_back(avis);
  return;
}

void BDSGeometryLCDD::parseLCDD(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;

  while (cur != nullptr)
     {
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"define")))
	 {
	   G4cout << "Importing LCDD Definitions" << G4endl;
	   parseDEFINE(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"materials")))
	 {
	   G4cout << "Importing LCDD Materials" << G4endl;
	   parseMATERIALS(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"solids")))
	 {
	   G4cout << "Importing LCDD Solids" << G4endl;
	   parseSOLID(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"structure")))
	 {
	   G4cout << "Importing LCDD Structures" << G4endl;
	   parseSTRUCTURE(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"setup")))
	 {
	   G4cout << "Importing LCDD Setup" << G4endl;
	   G4String name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
	   G4String version = parseStrChar(xmlGetProp(cur,(const xmlChar*)"version"));
	   xmlNodePtr tempcur = cur->xmlChildrenNode;
	 
	   while(tempcur!=nullptr)
	     {
	       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"world")))
		 itsWorldRef = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref"));
	       tempcur = tempcur->next;
	     }

	   	   G4LogicalVolume* topvol = GetLogVolByName(itsWorldRef);
	   	   new G4PVPlacement(nullptr,
	   	     G4ThreeVector(0.,0.,0.),
				     topvol,
				     topvol->GetName()+"_PhysiComp",
				     itsMarkerVol,
				     false,
				     0, BDSGlobalConstants::Instance()->CheckOverlaps());
#ifndef NOUSERLIMITS
	   	   topvol->SetUserLimits(itsUserLimits);
#endif
	 }
       cur = cur->next;
     }
  return;
}

void BDSGeometryLCDD::parseFIELDS(xmlNodePtr cur)
{
  xmlNodePtr tempcur = cur->xmlChildrenNode;
  tempcur=tempcur->next;
  while (tempcur != nullptr){
    if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"solenoid"))){
      if(fieldIsUniform==true){
	G4Exception("BDSGeometryLCDD::parseFIELDS> making solenoid field but already built dipole field...", "-1", FatalException, "");
      } 
      G4String name = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name"));
      //G4double lunit = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"lunit"));
      //G4double funit = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"funit"));
      //G4double outer_radius = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"outer_radius")) * lunit;
      //G4double inner_radius = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"inner_radius")) * lunit;
      //G4double inner_field = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"inner_field")) * funit;
      //G4double outer_field = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"outer_field")) * funit;
      //G4double zmax = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"zmax")) * lunit;
      //G4double zmin = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"zmin")) * lunit;

      //Make the magnetic field
      //field = new BDSFieldMagDetectorSolenoid(inner_field, outer_field, inner_radius, outer_radius, zmin, zmax);
    }else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"bdsimdipole"))){
      G4String name = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"name"));
      itsFieldVolName = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"volume"));
      G4double funit = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"funit"));
      G4double field = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"field")) * funit;
      const G4ThreeVector fieldVec(0,field,0);
      //field=new BDSUniformMagField(fieldVec); TBC
      G4cout << "REIMPLEMENT UNIFORM FIELD" << G4endl; exit(1);
      fieldIsUniform=true;
    }else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"text"))){
    }  else {
      G4cout << tempcur->name << G4endl;
      G4Exception("BDSGeometryLCDD.cc: parsing <fields - types other than solenoid and bdsimdipole are not currently implemented in BDSIM", "-1", FatalException, "");      
    }
    tempcur=tempcur->next;
  }
}

void BDSGeometryLCDD::parseSTRUCTURE(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;

  while (cur != nullptr)
     {
       
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"volume")))
	 parseVOLUME(cur);
       cur = cur->next;
     }
  
  return;
}


void BDSGeometryLCDD::parseDEFINE(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;

  while (cur != nullptr)
     {
       
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"rotation")))
	 {	
	   ROT_REF arot;
	   arot.name=(parseStrChar(xmlGetProp(cur,(const xmlChar*)"name")));
	   arot.value=G4ThreeVector(parseDblChar(xmlGetProp(cur,(const xmlChar*)"x")),
				    parseDblChar(xmlGetProp(cur,(const xmlChar*)"y")),
				    parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")));

	   ROT_LIST.push_back(arot);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"position")))
	 {
	   POS_REF apos;
	   apos.name=(parseStrChar(xmlGetProp(cur,(const xmlChar*)"name")));
	   apos.value=G4ThreeVector(parseDblChar(xmlGetProp(cur,(const xmlChar*)"x")),
				    parseDblChar(xmlGetProp(cur,(const xmlChar*)"y")),
				    parseDblChar(xmlGetProp(cur,(const xmlChar*)"z")));

	   POS_LIST.push_back(apos);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"constant")))
	 {
	   CONST_REF aconst;
	   aconst.name=(parseStrChar(xmlGetProp(cur,(const xmlChar*)"name")));
	   aconst.value=parseDblChar(xmlGetProp(cur,(const xmlChar*)"value"));
	   CONST_LIST.push_back(aconst);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"expression")))
	 {
	   G4Exception("LCDD expression not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"quantity")))
	 {
	   G4Exception("LCDD quantity not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       cur = cur->next;
     }
  
  return;
}

void BDSGeometryLCDD::parseMATERIALS(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;
  while (cur != nullptr)
    {
      if ((!xmlStrcmp(cur->name, (const xmlChar *)"define")))
	{
	  G4Exception("LCDD define not currently implemented in BDSIM", "-1", FatalException, "");
	  //Not coded yet
	}
      else if ((!xmlStrcmp(cur->name, (const xmlChar *)"isotope")))
	{
	  G4Exception("LCDD isotop not currently implemented in BDSIM", "-1", FatalException, "");
	  //Not coded yet
	}
      else if ((!xmlStrcmp(cur->name, (const xmlChar *)"element")))
	{
	  xmlNodePtr tempcur = cur->xmlChildrenNode;
	  tempcur = tempcur->next;
	  
	  G4String name, formula, type;
	  G4double Z, value, unit;

	  formula = parseStrChar(xmlGetProp(cur,(const xmlChar*)"formula"));	   
	   
	  if(!BDSMaterials::Instance()->CheckElement(formula)){
	    if(!xmlStrcmp(tempcur->name, (const xmlChar *)"atom")){
	      type = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"type"));	 
	      if (strcmp("A",type)){
		G4Exception("BDSGeometryLCDD.cc: parsing <element - types other than A are not currently implemented in BDSIM", "-1", FatalException, "");  
	      }
	      unit = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"unit"));	 
	      name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));	   
	      Z = parseDblChar(xmlGetProp(cur,(const xmlChar*)"Z"));	   
	       
	      value = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"value"));	   
	      BDSMaterials::Instance()->AddElement(name, formula, Z, value*unit/(CLHEP::g/CLHEP::mole)); 
	    } else {
	       G4Exception("BDSGeometryLCDD.cc: not an atom, not currently implemented in BDSIM", "-1", FatalException, "");  
	    }
	  } else {
	    G4cout << "Warning: BDSGeometryLCDD.cc: element " << formula << " already defined in BDSMaterials.cc" << G4endl;
	  }
	}
      else if ((!xmlStrcmp(cur->name, (const xmlChar *)"material")))
	 {
	   xmlNodePtr tempcur = cur->xmlChildrenNode;
	   
	   G4String name, type;
	   G4double value(0), unit(1);
	   
	   name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));	   
	   if(BDSMaterials::Instance()->CheckMaterial(name)){
	     G4cout << "Warning: BDSGeometryLCDD.cc: material " << name << " already defined in BDSMaterials.cc" << G4endl;
	   } else {

	     G4int numFractions = 0;
	     G4int numComposites = 0;
	     G4bool composite=false;
	     G4bool fraction=false;
	     
	     tempcur = cur->xmlChildrenNode;
	     while(tempcur!=nullptr){

	       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"fraction"))){
		 numFractions++;
		 fraction=true;
	       }
	       else if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"composite"))){
		 numComposites++;
		 composite=true;
	       }
	       
	       if ((!xmlStrcmp(tempcur->name, (const xmlChar *)"D"))){
		 value = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"value"));
		 unit = parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"unit"));
		 type = parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"type"));
#ifdef BDSDEBUG
		 G4cout << "BDSGeometryLCDD:ParseMaterials value = " << value << ", unit = " << unit << ", type = " << type << G4endl; 
#endif
		 if (!strcmp("",type)){
		   G4cout << "Warning - BDSGeometryLCDD.cc: parsing <material - type not defined, assuming type density." << G4endl;
		 } else if (strcmp("density",type)){
		   G4Exception("BDSGeometryLCDD.cc: parsing <material - types other than density are not currently implemented in BDSIM", "-1", FatalException, "");  
		 }
	       }
	       tempcur = tempcur->next;
	     }

	     if(fraction==composite){
	       G4Exception("BDSGeometry LCDD: Ill defined material fractions.", "-1", FatalException, "");
	     }
	     
	     std::list<G4String> components;
	     std::list<G4int> weights;
	     std::list<G4double> fractions;

	     tempcur = cur->xmlChildrenNode;
#ifdef BDSDEBUG
	     G4cout << "BDSGeometryLCDD::parseMATERIALS - making list of fractions/composites" << G4endl;
#endif
	     while(tempcur!=nullptr){
#ifdef BDSDEBUG
	       G4cout << "BDSGeometryLCDD::parseMATERIALS - name = " << tempcur->name << G4endl;
#endif	    
	       if (!xmlStrcmp(tempcur->name, (const xmlChar *)"fraction")){
#ifdef BDSDEBUG
		 G4cout << "BDSGeometryLCDD::parseMATERIALS - component = " << parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref")) << G4endl;
#endif
		 components.push_back(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref")));
#ifdef BDSDEBUG
		 G4cout << components.back() << G4endl;
		 G4cout << "BDSGeometryLCDD::parseMATERIALS - fraction = " << parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"n")) << G4endl;
#endif
		 fractions.push_back(parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"n")));
	       } else if (!xmlStrcmp(tempcur->name, (const xmlChar *)"composite")){
		 components.push_back(parseStrChar(xmlGetProp(tempcur,(const xmlChar*)"ref")));
		 weights.push_back((G4int)parseDblChar(xmlGetProp(tempcur,(const xmlChar*)"n")));
	       }
	       tempcur = tempcur->next;
	     }

#ifdef BDSDEBUG
	     std::list<G4String>::iterator stIter;
	     for(stIter = components.begin();
	     	 stIter!= components.end();
	     	 stIter++){
	       G4cout << "String element: " << *stIter << G4endl;
	     }
#endif

	     if (weights.size()>0){
#ifdef BDSDEBUG
	       G4cout << "Size of weights: " << weights.size() << G4endl;
#endif
	       BDSMaterials::Instance()->AddMaterial(name,
						     value*unit/(CLHEP::g/CLHEP::cm3),
						     kStateSolid,
						     300,
						     1,
						     components,
						     weights);
	     } else if(fractions.size()>0){
#ifdef BDSDEBUG
	       G4cout << "Size of fractions: " << fractions.size() << G4endl;
#endif
	       BDSMaterials::Instance()->AddMaterial(name,
						     value*unit/(CLHEP::g/CLHEP::cm3),
						     kStateSolid,
						     300,
						     1,
						     components,
						     fractions);
	     } else G4Exception("BDSGeometry LCDD: Ill defined material fractions - list of fractions and weights empty.", "-1", FatalException, "");

	   }
	 }
      cur = cur->next;
    }
  return;
}


void BDSGeometryLCDD::parseSOLID(xmlNodePtr cur)
{
  cur = cur->xmlChildrenNode;

  while (cur != nullptr)
     {
       
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"tube")))
	 BuildTube(cur);

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"box")))
	 BuildBox(cur);

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"cone")))
	 {
	   G4Exception("LCDD cone not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"eltube")))
	 {
	   G4Exception("LCDD eltube not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"hype")))
	 {
	   G4Exception("LCDD hype not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"intersection")))
	 {
	   G4Exception("LCDD intersection not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"orb")))
	 {
	   G4Exception("LCDD orb not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"sphere")))
	 {
	   G4Exception("LCDD sphere not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"trap")))
	 {
	   G4Exception("LCDD trap not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"subtraction")))
	 BuildSubtraction(cur);

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"torus")))
	 {
	   G4Exception("LCDD torus not currently implemented in BDSIM", "-1", FatalException, "");
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"union")))
	 {
	   G4Exception("LCDD union not currently implemented in BDSIM", "-1", FatalException, "");
	 }

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"polycone")))
	 BuildPolycone(cur);

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"polyhedra")))
	 {
	   BuildPolyhedra(cur);
	 }

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"trd")))
	 {
	   BuildTrd(cur);
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"tessellated")))
	 {
	   BuildTessellated(cur);
	 }
       cur = cur->next;
     }
  
  return;
}

void BDSGeometryLCDD::parseVOLUME(xmlNodePtr cur)
{
  G4String volume_name = parseStrChar(xmlGetProp(cur,(const xmlChar*)"name"));
  cur = cur->xmlChildrenNode;

  G4String materialref;
  G4String solidref;
  G4String visref;
  xmlNodePtr origcur = cur;
  while (cur != nullptr)
     {
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"materialref")))
	 {
	   materialref = parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref"));
#ifdef BDSDEBUG
           G4cout << "BDSGeometryLCDD> materialref = " << materialref << G4endl; 
#endif
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"solidref")))
	 {
	   solidref = parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref"));  
#ifdef BDSDEBUG
           G4cout << "BDSGeometryLCDD> solidref = " << solidref << G4endl; 
#endif
	 }
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"visref")))
	 {
	   visref = parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref")); 
#ifdef BDSDEBUG
           G4cout << "BDSGeometryLCDD> visref = " << visref << G4endl; 
#endif
	 }
       
       cur = cur->next;
     }

  if(materialref!="" && solidref!="")
    {
      G4LogicalVolume* alogvol = new G4LogicalVolume(GetSolidByName(solidref),
						     BDSMaterials::Instance()->GetMaterial(materialref),
						     volume_name);
#ifndef NOUSERLIMITS
      alogvol->SetUserLimits(itsUserLimits);
#endif
      
      sensitiveComponents.push_back(alogvol);
      LOGVOL_LIST.push_back(alogvol);

      if(visref==""){ //Default vis settings
      G4VisAttributes* VisAtt = 
       new G4VisAttributes(*BDSColours::Instance()->GetColour("default"));
      VisAtt->SetForceWireframe(true);
      alogvol->SetVisAttributes(VisAtt);
      }
      else{
        alogvol->SetVisAttributes(GetVisByName(visref));
      }
      
      while(origcur!=nullptr)
	{
	  if ((!xmlStrcmp(origcur->name, (const xmlChar *)"physvol")))
	    {
	      parsePHYSVOL(origcur, volume_name);
	    }
	  origcur = origcur->next;
	}
    }
  
  else
    {
      G4cout << "Can't build" << volume_name << " : " << solidref << G4endl;
      G4Exception("Can't build volume","-1", FatalException, "");
    }
  

  
  return;
}

void BDSGeometryLCDD::parsePHYSVOL(xmlNodePtr cur, G4String volume_name)
{
  cur = cur->xmlChildrenNode;

  G4String volumeref;

  G4RotationMatrix* componentRotation = nullptr;
  
  G4ThreeVector PlacementPoint;


  while (cur != nullptr)
     {
       if ((!xmlStrcmp(cur->name, (const xmlChar *)"volumeref")))
	 volumeref = parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref"));

       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"positionref")))
	 PlacementPoint = GetPosition(parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref")));
       
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"position")))
	 PlacementPoint = GetPosition(cur);
       
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"rotationref")))
	 componentRotation = GetRotation(parseStrChar(xmlGetProp(cur,(const xmlChar*)"ref")));
       
       else if ((!xmlStrcmp(cur->name, (const xmlChar *)"rotation")))
	 componentRotation = GetRotation(cur);

       cur = cur->next;
     }

  G4LogicalVolume* currentVol = GetLogVolByName(volumeref);
  G4LogicalVolume* parentVol = GetLogVolByName(volume_name);

  new G4PVPlacement(componentRotation,
		    PlacementPoint,
		    currentVol,
		    currentVol->GetName()+"_PhysiComp",
		    parentVol,
		    false,
		    0, BDSGlobalConstants::Instance()->CheckOverlaps());
  

  return;
}

 G4RotationMatrix* BDSGeometryLCDD::RotateComponent(G4ThreeVector rotvalues)
{
  G4RotationMatrix *rotateComponent = new G4RotationMatrix;
  
  //Assuming order is rotate x, then y, then z;

  rotateComponent->rotateX(rotvalues.x());
  rotateComponent->rotateY(rotvalues.y());
  rotateComponent->rotateZ(rotvalues.z());
  
  
  return rotateComponent;
}

#endif
