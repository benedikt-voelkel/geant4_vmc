#ifndef EXME_RUN_CONFIGURATION3_H
#define EXME_RUN_CONFIGURATION3_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERunConfiguration3.h
/// \brief Definition of the ExMERunConfiguration3 class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4RunConfiguration.h"

/// \ingroup EME
/// \brief User Geant4 VMC run configuration
///
/// This class demonstrates how to add to Geant4 VMC geometry construction
/// user defined regions
///
/// \author I. Hrivnacova; IPN, Orsay

class ExMERunConfiguration3 : public TG4RunConfiguration
{
  public:
    ExMERunConfiguration3(const TString& userGeometry,
                          const TString& physicsList = "FTFP_BERT",
                          const TString& specialProcess = "stepLimiter");
    virtual ~ExMERunConfiguration3();

    // methods
    virtual TG4VUserRegionConstruction*  CreateUserRegionConstruction();
};

#endif //EXME_RUN_CONFIGURATION3_H
