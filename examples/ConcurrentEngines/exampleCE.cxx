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
#include <cstdlib>
#include <chrono>

#include "TTrack.h"
#include "TMCStackManager.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TError.h"

#include "CEMCSingleApplication.h"
#include "CEMCMultiApplication.h"

#include "TG4RunConfiguration.h"
#include "TGeant4.h"

#include "TGeant3TGeo.h"

#include "TThread.h"
#include "TInterpreter.h"

#include "TStopwatch.h"

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
            << "Specify either \"TGeant3\" or \"TGeant4\" as first and second " \
               "argument, respectively. If the same engine is chosen twice, " \
               "an artificial concurrent run is done.\n" \
               "In any case, everything is simulated using the first "\
               "engine except for the tracker tube which will be simulated by "\
               "the other engine." << std::endl;
}


//------------------------------------------------------------------------------
// Run TGeant3TGeo and TGeant4 concurrently
//------------------------------------------------------------------------------
Int_t runMulti(Int_t nEvents, const char* firstEngineName,
                    const char* secondEngineName,
                    int argc, char** argv)
{
  Info("runMulti", "Run concurrent mode");
  Bool_t simulateG3 = kFALSE;
  Bool_t simulateG4 = kFALSE;
  // Only accept TGeant3 or TGeant4 as engine names, no default/fallback
  if((strcmp(firstEngineName, "TGeant3") != 0 &&
     strcmp(firstEngineName, "TGeant4") != 0) ||
     (strcmp(secondEngineName, "TGeant3") != 0 &&
     strcmp(secondEngineName, "TGeant4") != 0)) {
    Error("runMulti", "Engine name unknown. Please choose between \
                        \"TGeant\"3 and \"TGeant4\".");
    return 1;
  }
  if(strcmp(firstEngineName, "TGeant4") == 0 ||
     strcmp(secondEngineName, "TGeant4") == 0) {
    simulateG4 = kTRUE;
  }
  if(strcmp(firstEngineName, "TGeant3") == 0 ||
     strcmp(secondEngineName, "TGeant3") == 0) {

    simulateG3 = kTRUE;
  }
  Info("runMulti", "Instantiate MC application for concurrent engine run");
  CEMCMultiApplication* appl
    =  new CEMCMultiApplication("MultiApplication", "");

  Info("runMulti", "Construct the detector geometry");
  appl->ConstructUserGeometry();

  //
  // Declare some variables used later.
  //

  TG4RunConfiguration* runConfiguration;
  TGeant4* geant4;
  TGeant3* geant3;
  TVirtualMC* firstEngine;
  TVirtualMC* secondEngine;

  if(simulateG4) {

    Info("runMulti", "Setup Geant4 VMC");
    // Run configuration for TGeant4 with the geometry construction mode
    runConfiguration = new TG4RunConfiguration("geomRoot");

    // TGeant4
    geant4 = new TGeant4("TGeant4", "", runConfiguration, argc, argv);

    // Customise Geant4 setting
    // (verbose level, global range cut, ..)
    // geant4->ProcessGeantMacro("g4config.in");
    geant4->ProcessGeantCommand("/mcVerbose/all 0");
    Info("runMulti", "VMC %s was set up", geant4->GetName());
    if(strcmp(firstEngineName, "TGeant4") == 0) {
      firstEngine = geant4;
    }
    if(strcmp(secondEngineName, "TGeant4") == 0) {
      secondEngine = geant4;
    }
  }

  if(simulateG3) {

    Info("runMulti", "Setup Geant3 VMC");

    TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
    geant3->SetHADR(0);

    Info("runMulti", "VMC %s was set up", geant3->GetName());
    if(strcmp(firstEngineName, "TGeant3") == 0) {
      firstEngine = geant3;
    }
    if(strcmp(secondEngineName, "TGeant3") == 0) {
      secondEngine = geant3;
    }
  }

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

  if(strcmp(firstEngineName, secondEngineName) == 0) {
    TMCStackManager::Instance()->RegisterSpecifyEngineForTrack(
     [firstEngine]
     (TTrack* track, TVirtualMC*& targetMC)
     {
       targetMC = firstEngine;
     }
    );
  } else {
    TMCStackManager::Instance()->RegisterSpecifyEngineForTrack(
     [firstEngine, secondEngine, volIdChange]
     (TTrack* track, TVirtualMC*& targetMC)
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
     }
    );
  }
  // Need to keep track of former volume and track id to properly handle the
  // artificial "change between the same" engine.
  Int_t formerVolId = -1;
  Int_t formerTrackId = -1;
  Bool_t volChanged = kFALSE;
  // This function is invoked in each step to decide whether a track should be
  // moved to another engine.
  if(strcmp(firstEngineName, secondEngineName) == 0) {
    TMCStackManager::Instance()->RegisterSuggestTrackForMoving(
     [volChanged, volIdChange, formerVolId, formerTrackId, firstEngine]
     (TVirtualMC* currentMC, TVirtualMC*& targetMC) mutable
     {
       //targetMC = currentMC;
       //return;
       //Info("RegisterSuggestTrackForMoving", "volume change same engine, former vol id: %i, former track id: %i", formerVolId, formerTrackId);
       Int_t volId = gGeoManager->GetCurrentVolume()->GetNumber();
       Int_t trackId = TMCStackManager::Instance()->GetCurrentTrackNumber();
       // Same volume id or new track, don't change engine
       if(volId == formerVolId || trackId != formerTrackId) {
         formerTrackId = trackId;
         formerVolId = volId;
         volChanged = kFALSE;
         return;
       }
       // Volume id has changed so check if the current is the one where engine
       // should be changed. Also it has to be the same track as before because
       // that means that this track crossed a volume boundary.
       if(volId == volIdChange && trackId == formerTrackId && !volChanged) {
         targetMC = firstEngine;
         formerVolId = volId;
         volChanged = kTRUE;
         return;
       }
       // Volume id has changed and the former one was
       if(formerVolId == volIdChange && trackId == formerTrackId && !volChanged) {
         volChanged = kTRUE;
         targetMC = firstEngine;
         formerVolId = volId;
         return;
       }
       formerVolId = volId;
       formerTrackId = trackId;
       volChanged = kFALSE;
     }
    );
  } else {
    TMCStackManager::Instance()->RegisterSuggestTrackForMoving(
     [firstEngine, secondEngine, volIdChange]
     (TVirtualMC* currentMC, TVirtualMC*& targetMC)
     {
       //Info("RegisterSuggestTrackForMoving", "volume change different engine");
       Int_t volId = gGeoManager->GetCurrentVolume()->GetNumber();
       if(volId == volIdChange) {
         if(currentMC == secondEngine) {
           return;
         } else {
           targetMC = secondEngine;
           return;
         }
       }
       targetMC = firstEngine;
     }
    );
  }
  // After all preparation steps are done, the application can be run just like
  // the following. Note, that CEMCApplication::Run(Int_t nofEvents) is not
  // inherited from TVirtualMCMultiApplication.
  TStopwatch timer;
  auto start = std::chrono::high_resolution_clock::now();
  appl->Run(nEvents);
  auto finish = std::chrono::high_resolution_clock::now();
  auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
  //timer.Stop();
  //timer.Print();
  std::cout << "Time elapsed: " << microseconds.count() << " x 10^-6 s" << std::endl;
  // Print some status information.
  appl->PrintStatus();
  TMCStackManager::Instance()->Print();
  // The geometry is exported to a ROOT file. The tracks are saved there as well
  // and to have everything in one canvas just attach the created file
  // "CE_geometry.root" to a ROOT session and run
  // 1) TGeoManager::Import("CE_geometry.root")
  // 2) gGeoManager->GetTopVolume()->Draw()
  // 3) gGeoManager->DrawTracks()
  // Note, that CEMCApplication::ExportGeometry() is not inherited from
  // TVirtualMCMultiApplication.
  //appl->ExportGeometry();

  return 0;

}

//------------------------------------------------------------------------------
// Run either TGeant3TGeo or TGeant4
//------------------------------------------------------------------------------
Int_t runSingle(Int_t nEvents, const char* engineName, int argc, char** argv)
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


  if(strcmp(engineName, "TGeant4") == 0) {
    Info("runSingle", "Setup Geant4 VMC");
    // Run configuration for TGeant4 with the geometry construction mode
    TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot");
    // TGeant4
    TGeant4* geant4 = new TGeant4("TGeant4", "", runConfiguration, argc, argv);
    // Customise Geant4 setting
    // (verbose level, global range cut, ..)
    // geant4->ProcessGeantMacro("g4config.in");
    geant4->ProcessGeantCommand("/mcVerbose/all 0");
  } else {
    Info("runSingle", "Setup Geant3 VMC");
    TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
    geant3->SetHADR(0);
  }

  Info("runSingle", "VMC %s was set up", appl->GetMC()->GetName());

  // After all preparation steps are done, the application can be run just like
  // the following. Note, that CEMCApplication::Run(Int_t nofEvents) is not
  // inherited from TVirtualMCMultiApplication.
  //TStopwatch timer;
  auto start = std::chrono::high_resolution_clock::now();
  appl->Run(nEvents);
  auto finish = std::chrono::high_resolution_clock::now();
  auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start);
  //timer.Stop();
  //timer.Print();
  std::cout << "Time elapsed: " << microseconds.count() << " x 10^-6 s" << std::endl;
  // Print some status information.
  appl->PrintStatus();
  TMCStackManager::Instance()->Print();
  // The geometry is exported to a ROOT file. The tracks are saved there as well
  // and to have everything in one canvas just attach the created file
  // "CE_geometry.root" to a ROOT session and run
  // 1) TGeoManager::Import("CE_geometry.root")
  // 2) gGeoManager->GetTopVolume()->Draw()
  // 3) gGeoManager->DrawTracks()
  // Note, that CEMCApplication::ExportGeometry() is not inherited from
  // TVirtualMCMultiApplication.

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

  // User arguments
  // 1: number of events
  // 2: first engines
  // 3: second engine (optional)
  // No user argument
  if(argc < 3 || argc > 4) {
    printHelp();
    return 1;
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
  if(argc == 4) {
    return runMulti(atoi(argv[1]), argv[2], argv[3], argc, argv);
  }
  // Just one user argument
  return runSingle(atoi(argv[1]), argv[2], argc, argv);
//------------------------------------------------------------------------------

}
