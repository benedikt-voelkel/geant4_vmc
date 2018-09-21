//------------------------------------------------
// The Geant4 Virtual Monte Carlo package
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file TG4SensitiveDetector.cxx
/// \brief Implementation of the TG4SensitiveDetector class
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4SensitiveDetector.h"
#include "TG4GeometryServices.h"
#include "TG4StepManager.h"

#include <TVirtualMCApplication.h>

G4ThreadLocal G4int TG4SensitiveDetector::fgSDCounter = 0;

//_____________________________________________________________________________
TG4SensitiveDetector::TG4SensitiveDetector(G4String sdName, G4int mediumID)
  : G4VSensitiveDetector(sdName),
    fStepManager(TG4StepManager::Instance()),
    fMCApplication(TVirtualMCApplication::Instance()),
    fID(++fgSDCounter),
    fMediumID(mediumID)
{
/// Standard constructor with the specified \em name
}

//_____________________________________________________________________________
TG4SensitiveDetector::~TG4SensitiveDetector()
{
/// Destructor
}

//
// public methods
//

//_____________________________________________________________________________
void TG4SensitiveDetector::UserProcessHits(const G4Track* /*track*/,
                                           const G4Step* /*step*/)
{
/// Call VMC application stepping function.

  fMCApplication->Stepping();
}

//_____________________________________________________________________________
G4bool TG4SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
/// Call user defined sensitive detector.

  // let user sensitive detector process normal step
  fStepManager->SetStep(step, kNormalStep);
  fMCApplication->Stepping();

  return true;
}

//_____________________________________________________________________________
G4bool TG4SensitiveDetector::ProcessHitsOnBoundary(G4Step* step)
{
/// Call user defined sensitive detector
/// when crossing a geometrical boundary.

  // let user sensitive detector process boundary step
  // \note This is only done now if the track has not been killed before
  if(step->GetTrack()->GetTrackStatus() != fStopAndKill) {
    fStepManager->SetStep(step, kBoundary);
    fMCApplication->Stepping();
  }

  return true;
}
