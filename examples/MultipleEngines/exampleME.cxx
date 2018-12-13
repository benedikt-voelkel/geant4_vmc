//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file exampleME.cxx
/// \brief The Geant4 VMC example E01 application executable
///
/// The Geant4 VMC application executable
/// with explicitely instantiated TGeant3 or TGeant4 and linked
/// with all libraries.
///
/// \date 10/12/2018B. Volkel; University Heidelberg

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <string>

#include "TParticle.h"
#include "TGeoNode.h"
#include "TGeoManager.h"
#include "TError.h"

#include "MEMCSingleApplication.h"
#include "MEMCMultiApplication.h"

#include "TG4RunConfiguration.h"
#include "TGeant4.h"

#include "TGeant3TGeo.h"

#include "TThread.h"
#include "TInterpreter.h"

#include "TStopwatch.h"


namespace defaults
{
  const std::string firstEngine = "TGeant3";
  const std::string secondEngine = "TGeant4";
  Int_t nEvents = 2;
}

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
// Check for valid engine names
//------------------------------------------------------------------------------
Bool_t checkEngine(const std::string& engine)
{
  if(engine.compare(defaults::firstEngine) != 0 &&
     engine.compare(defaults::secondEngine) != 0) {
       return kFALSE;
  }
  return kTRUE;
}

//------------------------------------------------------------------------------
// Parse arguments and extract number of events and engine names
// Fall back to defaults if possible
//------------------------------------------------------------------------------
Bool_t parseArgs(Int_t argc, char** argv, Int_t& nEvents,
                 std::string& firstEngine, std::string& secondEngine)
{
  // User arguments
  // 1: number of events
  // 2: first engine
  // 3: second engine (optional)

  if(argc > 4) {
    std::cerr << "Invalid number of arguments\n";
    printHelp();
    return kFALSE;
  }

  // User passed everything
  if(argc == 4) {
    if(!checkEngine(argv[2]) || !checkEngine(argv[3])) {
      std::cerr << "Invalid engine found\n";
      printHelp();
      return kFALSE;
    }
    nEvents = atoi(argv[1]);
    firstEngine = argv[2];
    secondEngine = argv[3];
  // User passed number of events and one engine
  } else if(argc == 3) {
    if(!checkEngine(argv[2])) {
      std::cerr << "Invalid engine found\n";
      printHelp();
      return kFALSE;
    }
    nEvents = atoi(argv[1]);
    firstEngine = argv[2];
    secondEngine = "";
  // User passed only number of events
  } else if(argc == 2) {
    nEvents = atoi(argv[1]);
    firstEngine = defaults::firstEngine;
    secondEngine = defaults::secondEngine;
  } else {
  // Nothing passed, inly defaults
    nEvents = defaults::nEvents;
    firstEngine = defaults::firstEngine;
    secondEngine = defaults::secondEngine;
  }
  return kTRUE;
}

//------------------------------------------------------------------------------
// Apply cuts and en-/disable certain processes for given TVirtualMC
//------------------------------------------------------------------------------
void applyCuts(TVirtualMC* mc)
{

  std::cout << "Apply processes and cuts..." << std::endl;
  // Process name flags and values
  const std::vector<const char*> procNames = {"PAIR", "COMP", "PHOT", "PFIS",
                                              "DRAY", "ANNI", "BREM", "HADR",
                                              "MUNU", "DCAY", "LOSS", "MULS",
                                              "CKOV"};
  const std::vector<Int_t> procVals = {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

  // Cut name flags and values
  const std::vector<const char*> cutNames = {"CUTGAM", "CUTELE", "CUTNEU",
                                             "CUTHAD", "CUTMUO", "BCUTE",
                                             "BCUTM", "DCUTE", "DCUTM",
                                             "PPCUTM", "TOFMAX"};
  const Double_t cutValCommon = 1.0E-3;
  const Double_t tofmax = 1.0E10;
  const std::vector<Double_t> cutVals = {cutValCommon, cutValCommon,
                                         cutValCommon, cutValCommon,
                                         cutValCommon, cutValCommon,
                                         cutValCommon, cutValCommon,
                                         cutValCommon, cutValCommon,
                                         tofmax};

  // Set/apply processes
  for(Int_t i = 0; i < procNames.size(); i++) {
    mc->SetProcess(procNames[i], procVals[i]);
  }
  // Set/apply cuts
  for(Int_t i = 0; i < cutNames.size(); i++) {
    mc->SetCut(cutNames[i], cutVals[i]);
  }
}


//------------------------------------------------------------------------------
// Setup TGeant3
//------------------------------------------------------------------------------
TVirtualMC* setupGeant3()
{
  TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
  geant3->SetHADR(0);
  return geant3;
}

//------------------------------------------------------------------------------
// Setup TGeant4
//------------------------------------------------------------------------------
TVirtualMC* setupGeant4(int argc, char** argv)
{
  TG4RunConfiguration* runConfiguration = new TG4RunConfiguration("geomRoot", "QGSP_FTFP_BERT+optical", "stepLimiter+specialCuts", true);

  // TGeant4
  TGeant4* geant4 = new TGeant4("TGeant4", "", runConfiguration, argc, argv);

  // Customise Geant4 setting
  geant4->ProcessGeantCommand("/mcVerbose/all 0");
  geant4->ProcessGeantCommand("/mcTracking/skipNeutrino true");

  return geant4;
}

//------------------------------------------------------------------------------
// Run TGeant3TGeo and TGeant4 together
//------------------------------------------------------------------------------
Int_t runMulti(Int_t nEvents, const char* firstEngineName,
               const char* secondEngineName, int argc, char** argv)
{
  Info("runMulti", "Run mutli mode");

  MEMCMultiApplication* appl
    =  new MEMCMultiApplication("MultiApplication", "");

  appl->ConstructGlobalGeometry();

  // Use these pointers when deciding whether a track should be moved.
  TVirtualMC* firstEngine;
  TVirtualMC* secondEngine;

  if(strcmp(firstEngineName, "TGeant3") == 0) {
    firstEngine = setupGeant3();
  } else {
    firstEngine = setupGeant4(argc, argv);
  }
  if(strcmp(firstEngineName, secondEngineName) == 0) {
    secondEngine = firstEngine;
  } else if(strcmp(secondEngineName, "TGeant3") == 0) {
    secondEngine = setupGeant3();
  } else {
    secondEngine = setupGeant4(argc, argv);
  }
  Info("runMulti", "Setup engines.");
  std::cout << "first engine: " << firstEngine->GetName() << "\n"
            << "second engine: " << secondEngine->GetName() << std::endl;

  // NOTE We need to know the volume name here while the geometry is actually
  // constructed within the application MEMCMultiApplication. This can be done
  // within the application as well.
  // It just shows that it is possible from anywhere.
  Int_t volIdChange = gGeoManager->GetUID("TRTU");

  // This lambda specifies the initial stack
  appl->RegisterSpecifyEngineForTrack(
   [firstEngine, secondEngine, volIdChange]
   (const TParticle* track, TVirtualMC*& targetMC)
   {
     Int_t volId;
     // Default is GEANT3
     targetMC = firstEngine;
     // That is an expensive operation at this point but if the user wants to
     // make sure the particle ends up on the right stack it needs to be done.
     TGeoNode* node = gGeoManager->FindNode(track->Vx(), track->Vy(), track->Vz());
     if(node) {
       volId = node->GetVolume()->GetNumber();
       if(volId == volIdChange) {
         targetMC = secondEngine;
       }
       return;
     }
     // Fall back to firstEngine if node was not found.
     targetMC = firstEngine;
   }
  );
  // This function is used to decide whether a track should be moved from one
  // engine (currentMC) to another one (targetMC)
  appl->RegisterSuggestTrackForMoving(
   [firstEngine, secondEngine, volIdChange]
   (const TVirtualMC* currentMC, TVirtualMC*& targetMC)
   {
     //Info("RegisterSuggestTrackForMoving", "volume change different engine");
     Int_t volId = gGeoManager->GetCurrentVolume()->GetNumber();
     if(volId == volIdChange) {
       // No need to check whether currentMC is the same as secondEngine. It is
       // taken care of internally.
       targetMC = secondEngine;
       return;
     }
     // No need to check whether currentMC is the same as firstEngine. It is
     // taken care of internally.
     targetMC = firstEngine;
   }
  );

  // Export the geometry
  appl->ExportGeometry("geometry_multi.root");

  // Initialise the transport including a user lambda which is applied to each
  // registered TVirtualMC. Here, a few cut/process values are applied.
  // NOTE InitTransport and RunTransport are methods of
  // TVirtualMCMultiApplication.
  appl->InitTransport(applyCuts);
  appl->RunTransport(nEvents);
  // Print some status information.
  appl->PrintStatus();

  return 0;
}

//------------------------------------------------------------------------------
// Run either TGeant3TGeo or TGeant4
//------------------------------------------------------------------------------
Int_t runSingle(Int_t nEvents, const char* engineName, int argc, char** argv)
{
  Info("runSingle", "Run single mode");
  MEMCSingleApplication* appl
    =  new MEMCSingleApplication("SingleApplication", "");

  if(strcmp(engineName, "TGeant4") == 0) {
    Info("runSingle", "Setup Geant4 VMC");
    setupGeant4(argc, argv);
  } else {
    Info("runSingle", "Setup Geant3 VMC");
    setupGeant3();
  }

  // NOTE That is not a method of TVirtualMCApplication but of derived class
  // MEMCSingleApplication
  appl->InitTransport(applyCuts);

  // Export the geometry
  // NOTE Geometry export has to be done here since the geometry is built when
  // TVirtualMC is initialised calling
  // TVirtualMCApplication::ConstructGeometry()
  appl->ExportGeometry("geometry_single.root");

  // NOTE That is not a method of TVirtualMCApplication but of derived class
  // MEMCSingleApplication
  appl->RunTransport(nEvents);

  // Print some status information.
  appl->PrintStatus();

  return 0;
}

//------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{

  std::string firstEngine, secondEngine;
  Int_t nEvents;

  parseArgs(argc, argv, nEvents, firstEngine, secondEngine);

  if(secondEngine.compare("") != 0) {
    return runMulti(nEvents, firstEngine.c_str(), secondEngine.c_str(), argc, argv);
  }
  return runSingle(nEvents, firstEngine.c_str(), argc, argv);
}
