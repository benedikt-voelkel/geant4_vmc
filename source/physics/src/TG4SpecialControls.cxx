// $Id: TG4SpecialControls.cxx,v 1.6 2007/03/22 09:03:44 brun Exp $ //

//------------------------------------------------
// The Geant4 Virtual Monte Carlo package
// Copyright (C) 2007, Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: vmc@pcroot.cern.ch
//-------------------------------------------------

// Category: physics
// Class TG4VSpecialControls
// -------------------------
// See the class description in the header file.

#include "TG4SpecialControls.h"
#include "TG4GeometryServices.h"
#include "TG4Limits.h"

#include <G4StepStatus.hh>
#include <G4ProcessManager.hh>
#include <G4ProcessVector.hh>

//_____________________________________________________________________________
TG4SpecialControls::TG4SpecialControls(const G4String& processName)
  : G4VProcess(processName, fUserDefined),
    TG4Verbose("specialControls"),
    fSwitchControls(kUnswitch),
    fSwitchedProcesses(),
    fSwitchedControls(0),
    fLastTrackID(0) {
//    
   verboseLevel = VerboseLevel();
}

//_____________________________________________________________________________
TG4SpecialControls::~TG4SpecialControls() {
//
}

//
// private methods   
//

//_____________________________________________________________________________
void TG4SpecialControls::Reset()
{
/// Reset the buffers to the initial state.
                            
  fSwitchControls = kUnswitch;

  // clear buffers
  fSwitchedProcesses.clear();
  fSwitchedControls.clear();
}

//
// public methods   
//
          
//_____________________________________________________________________________
G4double TG4SpecialControls::PostStepGetPhysicalInteractionLength(
                           const G4Track& track, G4double /*previousStepSize*/,
                           G4ForceCondition* condition)
{
/// Return the Step-size (actual length) which is allowed 
// by this process.

  *condition = NotForced;

  if (track.GetTrackID() != fLastTrackID) {
    // new track
    Reset();
    fLastTrackID = track.GetTrackID();
  }  

  G4double proposedStep = DBL_MAX;
  //G4double minStep = (1.0e-9)*m;
  G4double minStep = 0.;
    // must be greater than DBL_MIN - so that particle can get out of
    // the boundary 
    // proposedStep = 0.; causes navigator to fall into panic 
  
  G4StepStatus status     
    = track.GetStep()->GetPreStepPoint()->GetStepStatus();

  // get limits
#ifdef MCDEBUG
  TG4Limits* limits 
     = TG4GeometryServices::Instance()
         ->GetLimits(track.GetVolume()->GetLogicalVolume()->GetUserLimits()); 
#else  
  TG4Limits* limits 
    = (TG4Limits*) track.GetVolume()->GetLogicalVolume()->GetUserLimits();
#endif    

  if (!limits) return proposedStep;

  if (fSwitchControls != kUnswitch) {
    if (status == fGeomBoundary) {
      if  (limits->IsControl()) {
        // particle is exiting a logical volume with special controls
        // and entering another logical volume with special controls 
        proposedStep = minStep;
        fSwitchControls = kReswitch;
        if (VerboseLevel() > 1) { 
          G4cout << "kReswitch" << G4endl;
        }  
      }
      else {
        // particle is exiting a logical volume with special controls
        // and entering a logical volume without special controls 
        proposedStep = minStep;
        fSwitchControls = kUnswitch;
        if (VerboseLevel() > 1) { 
          G4cout << "kUnswitch" << G4endl;
        }  
      }
    }
  }
  else if (limits->IsControl()) {
       // particle is entering a logical volume with special controls
       // that have not yet been set
       proposedStep = minStep;
       fSwitchControls = kSwitch;
       if (VerboseLevel() > 1) { 
         G4cout << "kSwitch" << G4endl;
       }         
  }  
  return proposedStep;
}

//_____________________________________________________________________________
G4VParticleChange* TG4SpecialControls::PostStepDoIt(
                      const G4Track& track, const G4Step& /*step*/)
{
/// Change processes activation of the current track
/// according to the current user limits.

  G4ProcessManager* processManager
    = track.GetDefinition()->GetProcessManager();
  G4ProcessVector* processVector = processManager->GetProcessList();

  if ((fSwitchControls==kUnswitch) || (fSwitchControls==kReswitch)) {
  
    // set processes activation back
    for (G4int i=0; i<fSwitchedProcesses.length(); i++) {
      if (VerboseLevel() > 1) {
        G4cout << "Reset process activation back in " 
                 << track.GetVolume()->GetName() 
               << G4endl;
      }
      processManager
        ->SetProcessActivation(fSwitchedProcesses[i],fSwitchedControls[i]);
    }
    fSwitchedProcesses.clear();
    fSwitchedControls.clear();
  }

  if ((fSwitchControls==kSwitch) ||  (fSwitchControls==kReswitch)) {

    // set TG4Limits processes controls
    TG4Limits* limits 
    = (TG4Limits*) track.GetVolume()->GetLogicalVolume()->GetUserLimits();

    for (G4int i=0; i<processVector->length(); i++) {

      TG4G3ControlValue control = limits->GetControl((*processVector)[i]);
      G4bool activation = processManager->GetProcessActivation(i);

      if (   control != kUnsetControlValue && 
           ! TG4Globals::Compare(activation, control) ) {

        // store the current processes controls
        if (VerboseLevel() > 1) {
          G4cout << "Something goes to fSwitchedProcesses" << G4endl;
        }  
        fSwitchedProcesses.insert((*processVector)[i]);
        fSwitchedControls.push_back(activation);

        // set new process activation
        if (control == kInActivate) {
          if (VerboseLevel() > 1) {
            G4cout << "Set process inactivation for " 
                   << (*processVector)[i]->GetProcessName() << " in " 
                       << track.GetVolume()->GetName() 
                   << G4endl;
          }
          processManager->SetProcessActivation(i,false);
        }  
        else {
          // ((control == kActivate) || (control == kActivate2)) 
          if (VerboseLevel() > 1) {
            G4cout << "Set process activation for " 
                   << (*processVector)[i]->GetProcessName() << " in " 
                   << track.GetVolume()->GetName() 
                   << G4endl;
          }
          processManager->SetProcessActivation(i,true);
        }
      }         
    }
  }
    
  // processManager->DumpInfo();              
  aParticleChange.Initialize(track);
  aParticleChange.ProposeTrackStatus(fAlive);
  return &aParticleChange;
}

