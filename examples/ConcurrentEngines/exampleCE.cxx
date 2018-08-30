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
#include "TError.h"

#include "CEMCSingleApplication.h"
#include "CEMCConcurrentApplication.h"

#include "TG4RunConfiguration.h"
#include "TGeant4.h"

#include "TGeant3TGeo.h"

#include "TThread.h"
#include "TInterpreter.h"

//------------------------------------------------------------------------------
// Print a help message
//------------------------------------------------------------------------------
void printHelp()
{
  std::cout << "You can either run TGeant3 or TGeant4 individually (1) or " \
               "both concurrently (2)\n";
  std::cout << "Case (1)\n"
            << "Specify either \"TGeant3\" or \"TGeant4\" as first argument " \
               "to simulate the entire geometry with the specified engine.\n";
  std::cout << "Case (2)\n"
            << "Specify either \"TGeant3\" or \"TGeant4\" as first argument " \
               "in which case the everything is simulated using the specified "\
               "engine except for the tracker tube which will be simulated by "\
               "the other engine.\nThe second argument must be \"concurrent\" "\
               "in that case." << std::endl;
}


//------------------------------------------------------------------------------
// Run TGeant3TGeo and TGeant4 concurrently
//------------------------------------------------------------------------------
Int_t runConcurrent(const char* firstEngineName, int argc, char** argv)
{

  // Only accept TGeant3 or TGeant4 as engine names, no default/fallback
  if(strcmp(firstEngineName, "TGeant3") != 0 &&
     strcmp(firstEngineName, "TGeant4") != 0) {
    Error("runSingle", "Engine name %s unknown. Please choose between \
                        \"TGeant\"3 and \"TGeant4\".", firstEngineName);
    return 1;
  }
  Info("runConcurrent", "Instantiate MC application for concurrent engine run");
  CEMCConcurrentApplication* appl
    =  new CEMCConcurrentApplication("ConcurrentApplication", "");

  Info("runConcurrent", "Construct the detector geometry");
  appl->ConstructUserGeometry();

  // Use this later for some further demonstration
  TVirtualMC* mc = nullptr;

  Info("runConcurrent", "Setup Geant4 VMC");
  // Run configuration for TGeant4 with the geometry construction mode
  TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot");

  // TGeant4
  TGeant4* geant4 = new TGeant4("TGeant4", "", runConfiguration, argc, argv);

  // Customise Geant4 setting
  // (verbose level, global range cut, ..)
  // geant4->ProcessGeantMacro("g4config.in");
  geant4->ProcessGeantCommand("/mcVerbose/all 1");

  // Demonstrate how the TMCManager can be used to get the current engine
  mc = TMCManager::GetMC();
  Info("runConcurrent", "VMC %s was set up", mc->GetName());


  Info("runConcurrent", "Setup Geant3 VMC");

  TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
  geant3->SetHADR(0);

  // Demonstrate how the TMCManager can be used to get the current engine which
  // has now changed to TGeant3TGeo
  mc = TMCManager::GetMC();
  std::cout << mc->GetName() << std::endl;
  Info("runConcurrent", "VMC %s was set up", mc->GetName());

  // Now prepare to make decisions how the engines will be used concurrently
  //
  // Everything will be simulated using TGeant3TGeo except for the tracker tube
  // which will be simulated using TGeant4. This either requires the knowledge of
  // the volume name or its ID. Since the current volume ID during simulation can
  // be extracted from the current engine, first get the ID of the tracker tube
  // from the TGeoManager. That means we have to know the name of the volume.
  Int_t volIdChange = gGeoManager->GetUID("TRTU");

  // In the following tow lambda functions are passed to the TMCStackManager to
  // decide how primary particles are pushed to the stack and when to change the
  // engine.
  //
  // This function is used to decide to which engine a track (aka TParticle),
  // which has not been transported yet, will be forwarded.
  TVirtualMC* firstEngine;
  TVirtualMC* secondEngine;
  if(strcmp(firstEngineName, "TGeant3") == 0) {
    firstEngine = geant3;
    secondEngine = geant4;
  } else {
    firstEngine = geant4;
    secondEngine = geant3;
  }

  TMCStackManager::Instance()->RegisterSpecifyEngineForTrack(
   [firstEngine, secondEngine, volIdChange]
   (TParticle* track, TVirtualMC*& targetMC)->Bool_t
   {
     Int_t volId;
     // Default is GEANT3
     targetMC = firstEngine;
     TGeoNode* node = gGeoManager->FindNode(track->Vx(), track->Vy(), track->Vz());
     if(node) {
       volId = node->GetVolume()->GetNumber();
       if(volId == volIdChange) {
         targetMC = secondEngine;
       }
     }
     return kTRUE;
   }
  );
  // This function is invoked in each step to decide whether a track should be
  // moved to another engine.
  TMCStackManager::Instance()->RegisterSuggestTrackForMoving(
   [firstEngine, secondEngine, volIdChange]
   (TVirtualMC* currentMC, TVirtualMC*& targetMC)->Bool_t
   {
     Int_t copyNo;
     Int_t volId = currentMC->CurrentVolID(copyNo);
     if(!currentMC->IsTrackEntering()) {
       return kFALSE;
     }
     if(volId == volIdChange) {
       targetMC = secondEngine;
       return kTRUE;
     }
     targetMC = firstEngine;
     return kTRUE;
   }
  );

  // After all preparation steps are done, the application can be run just like
  // the following. Note, that CEMCApplication::Run(Int_t nofEvents) is not
  // inherited from TVirtualMCConcurrentApplication.
  appl->Run(1);
  // Print some status information.
  appl->PrintStatus();
  // The geometry is exported to a ROOT file. The tracks are saved there as well
  // and to have everything in one canvas just attach the created file
  // "CE_geometry.root" to a ROOT session and run
  // 1) TGeoManager::Import("CE_geometry.root")
  // 2) gGeoManager->GetTopVolume()->Draw()
  // 3) gGeoManager->DrawTracks()
  // Note, that CEMCApplication::ExportGeometry() is not inherited from
  // TVirtualMCConcurrentApplication.
  appl->ExportGeometry();

  return 0;

}

//------------------------------------------------------------------------------
// Run either TGeant3TGeo or TGeant4
//------------------------------------------------------------------------------
Int_t runSingle(const char* engineName, int argc, char** argv)
{
  // Only accept TGeant3 or TGeant4 as engine names, no default/fallback
  if(strcmp(engineName, "TGeant3") != 0 && strcmp(engineName, "TGeant4") != 0) {
    Error("runSingle", "Engine name %s unknown. Please choose between \
                        \"TGeant\"3 and \"TGeant4\".", engineName);
    return 1;
  }

  Info("runSingle", "Instantiate MC application for single engine run");
  CEMCSingleApplication* appl
    =  new CEMCSingleApplication("SingleApplication", "");

  Info("runSingle", "Construct the detector geometry");
  appl->ConstructUserGeometry();

  // Use this later for some further demonstration
  TVirtualMC* mc = nullptr;

  if(strcmp(engineName, "TGeant4") == 0) {
    Info("runSingle", "Setup Geant4 VMC");
    // Run configuration for TGeant4 with the geometry construction mode
    TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot");
    // TGeant4
    TGeant4* geant4 = new TGeant4("TGeant4", "", runConfiguration, argc, argv);
    // Customise Geant4 setting
    // (verbose level, global range cut, ..)
    // geant4->ProcessGeantMacro("g4config.in");
    geant4->ProcessGeantCommand("/mcVerbose/all 1");
  } else {
    Info("runSingle", "Setup Geant3 VMC");
    TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
    geant3->SetHADR(0);
  }

  // Demonstrate how the TMCManager can be used to get the current engine which
  // is either TGeant3TGeo or TGeant4.
  mc = TMCManager::GetMC();
  std::cout << mc->GetName() << std::endl;
  Info("runSingle", "VMC %s was set up", mc->GetName());

  // After all preparation steps are done, the application can be run just like
  // the following. Note, that CEMCApplication::Run(Int_t nofEvents) is not
  // inherited from TVirtualMCConcurrentApplication.
  appl->Run(1);
  // Print some status information.
  appl->PrintStatus();
  // The geometry is exported to a ROOT file. The tracks are saved there as well
  // and to have everything in one canvas just attach the created file
  // "CE_geometry.root" to a ROOT session and run
  // 1) TGeoManager::Import("CE_geometry.root")
  // 2) gGeoManager->GetTopVolume()->Draw()
  // 3) gGeoManager->DrawTracks()
  // Note, that CEMCApplication::ExportGeometry() is not inherited from
  // TVirtualMCConcurrentApplication.
  appl->ExportGeometry();

  return 0;
}


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

//------------------------------------------------------------------------------
  // Name of the engine can be chosen. It has to be either TGeant3, TGeant4. If
  // it is left blank, the concurrent of TGeant3 and TGeant4 will be initiated.
  const char* engineToBeUsed;

  // No user argument
  if(argc < 2) {
    printHelp();
    return 0;
  }

  // So there is at least one user argument
  // Check whether help is requested
  for(int i = 0; i < argc; i++) {
    if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      printHelp();
      return 0;
    }
  }
  // 2 arguments given by the user, assume concurrent
  if(argc > 2) {
    if(strcmp(argv[2], "concurrent") != 0) {
      printHelp();
      return 1;
    }
    return runConcurrent(argv[1], argc, argv);
  }
  // No concurrent assume first argument to be the engine name
  return runSingle(argv[1], argc, argv);
//------------------------------------------------------------------------------

}
