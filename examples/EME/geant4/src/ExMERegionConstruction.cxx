//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERegionConstruction.cxx
/// \brief Implementation of the ExMERegionConstruction class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo \n
///
/// \author I. Hrivnacova; IPN, Orsay

#include "ExMERegionConstruction.h"

#include "TG4GeometryServices.h"

#include "G4LogicalVolume.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4SystemOfUnits.hh"

//_____________________________________________________________________________
ExMERegionConstruction::ExMERegionConstruction()
  : TG4VUserRegionConstruction()
{
/// Standard constructor
}

//_____________________________________________________________________________
ExMERegionConstruction::~ExMERegionConstruction()
{
/// Destructor
}

//
// protected methods
//

//_____________________________________________________________________________
void  ExMERegionConstruction::Construct()
{
/// Definition of regions

  G4LogicalVolume* logicalVolume
    = TG4GeometryServices::Instance()->FindLogicalVolume("LAYE");
  if ( ! logicalVolume )  {
    G4cerr << "Logical volume LAYE not found" << G4endl;
    return;
  }


  G4Region* testRegion = new G4Region("Test_region");
  testRegion->AddRootLogicalVolume(logicalVolume);
  testRegion->SetProductionCuts(new G4ProductionCuts());

  std::vector<double> cuts;
  cuts.push_back(1.0*cm);cuts.push_back(1.0*cm);cuts.push_back(1.0*cm);
  testRegion->GetProductionCuts()->SetProductionCuts(cuts);
}
