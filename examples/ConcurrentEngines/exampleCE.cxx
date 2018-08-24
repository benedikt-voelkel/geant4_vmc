//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file exampleE01.cxx
/// \brief The Geant4 VMC example E01 application executable
///
/// The Geant4 VMC application executable
/// with explicitely instantiated TGeant3 or TGeant4 and linked
/// with all libraries.
///
/// \date 26/02/2014
/// \author I. Hrivnacova; IPN, Orsay

#include <iostream>
//#include <functional>

#include "TMCManager.h"
#include "TParticle.h"
#include "TMCStackManager.h"
#include "TGeoNode.h"
#include "TGeoManager.h"

#include "CEMCApplication.h"

#include "TG4RunConfiguration.h"
#include "TGeant4.h"

#include "TGeant3TGeo.h"

#include "TThread.h"
#include "TInterpreter.h"

/// Application main program
int main(int argc, char** argv)
{
  // Initialize Root threading.
  // (Multi-threading is triggered automatically if Geant4 was built
  //  in MT mode.)
#ifdef G4MULTITHREADED
   TThread::Initialize();
   gInterpreter->SetProcessLineLock(false);
#endif

  // Create MC application
  CEMCApplication* appl
    =  new CEMCApplication("ExampleCE",
                             "The exampleE01 MC application");

   // External geometry construction
   std::cout << "[INFO] Construct the user Geometry" << std::endl;
   appl->ConstructUserGeometry();

   TVirtualMC* mc = nullptr;


  std::cout << "[INFO] Setup Geant4 VMC" << std::endl;

  // RunConfiguration for Geant4
  TG4RunConfiguration* runConfiguration
    = new TG4RunConfiguration("geomRoot");

  // TGeant4
  TGeant4* geant4
    = new TGeant4("TGeant4", "The Geant4 Monte Carlo", runConfiguration,
                  argc, argv);

  // Customise Geant4 setting
  // (verbose level, global range cut, ..)
  // geant4->ProcessGeantMacro("g4config.in");
  geant4->ProcessGeantCommand("/mcVerbose/all 1");
  mc = TMCManager::GetMC();
  std::cout << "[INFO] VMC" << mc->GetName() << " was set up" << std::endl;


  std::cout << "[INFO] Setup Geant3 VMC" << std::endl;

  TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
  geant3->SetHADR(0);

  mc = TMCManager::GetMC();
  std::cout << mc->GetName() << std::endl;
  std::cout << "[INFO] VMC " << mc->GetName() << " was set up" << std::endl;

  // The ID of the volume to be processed with GEANT4, namely the tracker tube
  // This either requires the knowledge of the volume name or its ID. Here it is
  // demonstrated by using the name and extract the ID using the TGeoManager
  Int_t volIdG4 = gGeoManager->GetUID("TRTU");

  TMCStackManager::Instance()->RegisterSuggestTrackForMoving(
    [geant3, geant4, volIdG4](TVirtualMC* currentMC, TVirtualMC*& targetMC)->Bool_t
    {
      Int_t copyNo;
      Int_t volId = currentMC->CurrentVolID(copyNo);
      if(!currentMC->IsTrackEntering()) {
        return kFALSE;
      }
      if(volId == volIdG4) {
        targetMC = geant4;
        return kTRUE;
      }
      targetMC = geant3;
      return kTRUE;
    }
  );

  TMCStackManager::Instance()->RegisterSpecifyEngineForTrack(
    [geant3, geant4, volIdG4](TParticle* track, TVirtualMC*& targetMC)->Bool_t
    {
      Int_t volId;
      // Default is GEANT3
      targetMC = geant3;
      TGeoNode* node = gGeoManager->FindNode(track->Vx(), track->Vy(), track->Vz());
      if(node) {
        volId = node->GetVolume()->GetNumber();
        if(volId == volIdG4) {
          targetMC = geant4;
        }
      }
      return kTRUE;
    }
  );

  // Initialization
  appl->Run(1);
  appl->PrintStatus();
  // Export Geometry
  appl->ExportGeometry();

  delete appl;
}
