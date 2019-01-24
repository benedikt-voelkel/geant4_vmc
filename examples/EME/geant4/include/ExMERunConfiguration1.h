#ifndef EXME_RUN_CONFIGURATION1_H
#define EXME_RUN_CONFIGURATION1_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERunConfiguration1.h
/// \brief Definition of the ExMERunConfiguration1 class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4RunConfiguration.h"

/// \ingroup EME
/// \brief User Geant4 VMC run configuration
///
/// This class overrides setting of the default Geant4 VMC geometry
/// construction with Geant4 detector construction
///
/// \author I. Hrivnacova; IPN, Orsay

class ExMERunConfiguration1 : public TG4RunConfiguration
{
  public:
    ExMERunConfiguration1(const TString& physicsList = "FTFP_BERT",
                          const TString& specialProcess = "stepLimiter");
    virtual ~ExMERunConfiguration1();

    // methods
    virtual G4VUserDetectorConstruction*  CreateDetectorConstruction();
};

#endif //EXME_RUN_CONFIGURATION1_H
