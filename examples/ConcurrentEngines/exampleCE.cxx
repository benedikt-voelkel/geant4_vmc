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
   appl->SetTransportMediaProperties();



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
  TVirtualMC* mc = TVirtualMCApplication::GetMC();
  std::cout << "[INFO] VMC" << mc->GetName() << " was set up" << std::endl;


  std::cout << "[INFO] Setup Geant3 VMC" << std::endl;

  TGeant3* geant3 = new TGeant3TGeo("C++ Interface to Geant3");
  geant3->SetHADR(0);

  mc = TVirtualMCApplication::GetMC();
  std::cout << mc->GetName() << std::endl;
  std::cout << "[INFO] VMC" << mc->GetName() << " was set up" << std::endl;


  appl->InitMCs();
  appl->RunMCs(1);

  delete appl;
}
