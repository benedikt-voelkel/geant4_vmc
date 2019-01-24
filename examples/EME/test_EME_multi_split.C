//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \ingroup Tests
/// \file test_EME_multi.C
/// \brief Example EME Test macro for mixed engine run
///
/// Running Example03

//#include "set_vis.C"

void test_EME_multi_split(const TString& configMacro1, const TString& configMacro2, Int_t level=0)
{
/// Macro function for testing example EME
/// \param configMacro  configuration macro loaded in initialization
///
/// Run 5 events with 20 primaries and print the calorimeter hits.

  // Create application if it does not yet exist
  Bool_t needDelete = kFALSE;
  if ( ! TVirtualMCApplication::Instance() ) {
    new ExMEMCApplication("ExampleME", "The exampleME MC application", kTRUE, kTRUE);
    needDelete = kTRUE;
  }

  // MC application
  ExMEMCApplication* appl
    = (ExMEMCApplication*)TVirtualMCApplication::Instance();
  appl->SetVerboseLevel(level);
  appl->GetPrimaryGenerator()->SetNofPrimaries(20);
  appl->SetPrintModulo(1);

  appl->InitMC({configMacro1.Data(), configMacro2.Data()});

  // visualization setting
  // set_vis();

  appl->RunMC(5);

  if ( needDelete ) delete appl;
}
