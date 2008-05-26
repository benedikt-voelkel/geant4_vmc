// $Id$

//------------------------------------------------
// The Geant4 Virtual Monte Carlo package
// Copyright (C) 2007, 2008 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: vmc@pcroot.cern.ch
//-------------------------------------------------

/// \file TG4LVStructure.cxx
/// \brief Implementation of the TG4LVStructure class 
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4LVStructure.h"
#include "TG4Globals.h"

#ifdef G4VIS_USE
#include "TG4ColourStore.h"

#include <G4Colour.hh>
#include <G4VisAttributes.hh>
#endif //G4VIS_USE
#include <G4LogicalVolume.hh>
#include <G4BooleanSolid.hh>
#include <set>

//_____________________________________________________________________________
TG4LVStructure::TG4LVStructure(G4String path)
  : fStructures(),
    fLogicalVolumes(),
    fPathName(path),
    fDirName(path),
    fVerboseLevel(0)
{
/// Standard constructor

  G4int i = fDirName.length();
  if (i > 1) {
    fDirName.remove(i-1);
    G4int isl = fDirName.last('/');
    fDirName.remove(0,isl+1);
    fDirName += "/";
  }
}

//_____________________________________________________________________________
TG4LVStructure::TG4LVStructure(const TG4LVStructure& right)
  : fStructures(),
    fLogicalVolumes(),
    fPathName(),
    fDirName(),
    fVerboseLevel(0)
{
/// Copy constructor

  // copy stuff
  *this = right;
}

//_____________________________________________________________________________
TG4LVStructure::~TG4LVStructure() 
{
/// Destructor

  ClearAndDestroy(&fStructures);
  fLogicalVolumes.resize(0);
}

//
// operators
//

//_____________________________________________________________________________
TG4LVStructure& TG4LVStructure::operator=(const TG4LVStructure &right)
{
/// Asignment operator

  // check assignement to self
  if (this == &right) return *this;

  // copy vector of structures
  ClearAndDestroy(&fStructures);
  G4int i;
  for (i=0; i<G4int(right.fStructures.size()); i++) {
    // new full structure tree has to be created
    TG4LVStructure* rhsStructure = right.fStructures[i];
    fStructures.push_back(new TG4LVStructure(*rhsStructure)); 
  }  
  
  // copy vector of logical volumes
  fLogicalVolumes.resize(0);
  for (i=0; i<G4int(right.fLogicalVolumes.size()); i++) {
    G4LogicalVolume* rhsLV = right.fLogicalVolumes[i];
    fLogicalVolumes.push_back(rhsLV); 
  }  
  
  fPathName = right.fPathName;
  fDirName = right.fPathName;
  fVerboseLevel = right.fVerboseLevel;

  return *this;
}

//_____________________________________________________________________________
G4int TG4LVStructure::operator==(const TG4LVStructure &right) const
{
/// Equal operator

  // check == to self
  if (this == &right) return true;

  return false;
}

//
// private methods
//

//_____________________________________________________________________________
TG4LVStructure* TG4LVStructure::FindSubDirectory(const G4String& subDir) const
{
/// Find the subdirectory.

  for( G4int i=0; i<G4int(fStructures.size()); i++ ) {
    if (subDir == fStructures[i]->fDirName) return fStructures[i];
  } 
  return 0;
}

//_____________________________________________________________________________
G4String TG4LVStructure::ExtractDirName(const G4String& name) const
{
/// Extracts the directory name from the path.

  G4String subDir = name;
  G4int i = name.first('/');
  if (i != G4int(std::string::npos)) subDir.remove(i+1);
  return subDir;
}

//_____________________________________________________________________________
void TG4LVStructure::ClearAndDestroy(LVStructuresVector* structures) 
{
/// Clears the structures vectors and deletes all contained 
/// elements.
/// (According to geant4.3.2/source/global/STLInterface/g4rw/tpordvec.icc.)

  std::set<TG4LVStructure*,std::greater<TG4LVStructure*> > tmp;
  for (size_t sz=0; sz<structures->size(); sz++)
    {
      TG4LVStructure* current;
      current=(*structures)[sz];
      if (current)
         tmp.insert(current);
    }
    
  std::set<TG4LVStructure*, std::greater<TG4LVStructure*> >::iterator it;
  for (it=tmp.begin(); it!=tmp.end(); it++)
    {
      delete *it;
    }
  // std_pvector::erase(std_pvector::begin(), std_pvector::end());
  structures->resize(0);
}

//
// public methods
//

//_____________________________________________________________________________
void TG4LVStructure::AddNewVolume(G4LogicalVolume* lv, 
                                  const G4String& treeStructure)
{
/// Add new logical volume to the structure.

  G4String remainingPath = treeStructure;
  remainingPath.remove(0, fPathName.length());  
  if (!remainingPath.isNull()) { 
    // The lv should be kept in subdirectory.
    // First, check if the subdirectoy exists.
    G4String subDir = ExtractDirName( remainingPath );
    TG4LVStructure* targetLVS = FindSubDirectory(subDir);
    if (targetLVS == 0) { 
      // Subdirectory not found. Create a new directory.
      subDir.prepend(fPathName);
      targetLVS = new TG4LVStructure(subDir);
      fStructures.push_back( targetLVS );
    }
    targetLVS->AddNewVolume(lv, treeStructure);
  }
  else { 
    // the logical volumes should be kept in this directory.
    G4LogicalVolume* targetLV = GetVolume(lv->GetName());
    if (targetLV != 0) {
      // G4cout << lv->GetName() << " has been already stored in "
      //        << fPathName << G4endl;
    }
    else {
      fLogicalVolumes.push_back(lv);
    }
  }
}

//_____________________________________________________________________________
G4LogicalVolume* TG4LVStructure::GetVolume(const G4String& lvName) const
{
/// Return logical volume of lvName if present in the structure,
/// returns 0 otherwise.

  for (G4int i=0; i<G4int(fLogicalVolumes.size()); i++) {
    G4LogicalVolume* targetLV = fLogicalVolumes[i];
    if (lvName == targetLV->GetName()) return targetLV;
  }
  return 0;
}

//_____________________________________________________________________________
G4LogicalVolume* TG4LVStructure::FindVolume(const G4String& name) const
{
/// Find logical volume of given name in all structure tree.

  G4String path = name;
  path.remove(0, fPathName.length());
  if (path.first('/') != G4int(std::string::npos)) { 
    // SD exists in sub-directory
    G4String subDir = ExtractDirName(path);
    TG4LVStructure* targetLVS = FindSubDirectory(subDir);
    if (targetLVS == 0) {  
      // The subdirectory is not found
      TG4Globals:: Warning(
        "TG4LVStructure", "FindVolume",
        TString(subDir) + " is not found in " + TString(fPathName));
      return 0;
    }
    else { 
      return targetLVS->FindVolume(name); 
    }
  }
  else { 
    // LV must exist in this directory
    G4LogicalVolume* targetLV = GetVolume(path);
    if (targetLV == 0) {  
      // The fLogicalVolumes is not found.
      TG4Globals:: Warning(
        "TG4LVStructure", "FindVolume",
        TString(path) + " is not found in " + TString(fPathName));
    }
    return targetLV;
  }
}

//_____________________________________________________________________________
void TG4LVStructure::ListTree() const
{
/// Print LV tree structure.

  for (G4int i=0; i<G4int(fLogicalVolumes.size()); i++) {
    G4LogicalVolume* lv = fLogicalVolumes[i];
    G4cout << fPathName << lv->GetName() << G4endl;
  }
  for (G4int j=0; j<G4int(fStructures.size()); j++) { 
    fStructures[j]->ListTree(); 
  }
}
        
//_____________________________________________________________________________
void TG4LVStructure::ListTreeLong() const
{
/// Print LV tree structure with number of
/// daughters (physical volume), indicates Boolean solid.

  for (G4int i=0; i<G4int(fLogicalVolumes.size()); i++) {
    G4LogicalVolume* lv = fLogicalVolumes[i];

    G4cout << fPathName << lv->GetName() << " (" << lv->GetNoDaughters();
            
    if (dynamic_cast<G4BooleanSolid*>(lv->GetSolid()))
      G4cout << ", B";

    G4cout << ")" << G4endl;
  }
  for (G4int j=0; j<G4int(fStructures.size()); j++) { 
    fStructures[j]->ListTreeLong(); 
  }
}
        
//_____________________________________________________________________________
void TG4LVStructure::SetVerboseLevel(G4int verbose) 
{
/// Set verbose level.

  fVerboseLevel = verbose;  
  for (G4int i=0; i<G4int(fStructures.size()); i++) { 
    fStructures[i]->SetVerboseLevel(verbose); 
  }
}

#ifdef G4VIS_USE
//_____________________________________________________________________________
void TG4LVStructure::SetTreeVisibility(G4bool visibility)       
{
/// Set visibility to all logical volumes in the structure 
/// tree.

  for (G4int i=0; i<G4int(fLogicalVolumes.size()); i++) {
    G4LogicalVolume* lv = fLogicalVolumes[i];

    const G4VisAttributes* kpVisAttributes = lv->GetVisAttributes();
    G4VisAttributes* newVisAttributes; 
    if (kpVisAttributes) {
      G4Colour colour   = kpVisAttributes->GetColour();
      newVisAttributes = new G4VisAttributes(colour); 
    }
    else
      newVisAttributes = new G4VisAttributes();
    delete kpVisAttributes;

    newVisAttributes->SetVisibility(visibility); 

    lv->SetVisAttributes(newVisAttributes);
  }
  for (G4int j=0; j<G4int(fStructures.size()); j++) { 
    fStructures[j]->SetTreeVisibility(visibility); 
  }
}

//_____________________________________________________________________________
void TG4LVStructure::SetTreeColour(const G4String& colName)
{
/// Set colour specified  by name to all logical volumes
/// in the structure tree.

  for (G4int i=0; i<G4int(fLogicalVolumes.size()); i++) {
    G4LogicalVolume* lv = fLogicalVolumes[i];

    const G4VisAttributes* kpVisAttributes = lv->GetVisAttributes ();
    G4VisAttributes* newVisAttributes; 
    if (kpVisAttributes) {
      G4bool oldVisibility = kpVisAttributes->IsVisible();
      newVisAttributes = new G4VisAttributes(oldVisibility); 
    }
    else
      newVisAttributes = new G4VisAttributes();
    delete kpVisAttributes;

    TG4ColourStore* pColours = TG4ColourStore::Instance();
    G4Colour colour = pColours->GetColour(colName);
    newVisAttributes->SetColour(colour);

    lv->SetVisAttributes(newVisAttributes);
  }
  for (G4int j=0; j<G4int(fStructures.size()); j++) { 
    fStructures[j]->SetTreeColour(colName); 
  }
}
#endif //G4VIS_USE            


