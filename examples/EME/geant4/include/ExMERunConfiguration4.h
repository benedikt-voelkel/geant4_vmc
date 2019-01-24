#ifndef EXME_RUN_CONFIGURATION4_H
#define EXME_RUN_CONFIGURATION4_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2015 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERunConfiguration4.h
/// \brief Definition of the ExMERunConfiguration4 class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4RunConfiguration.h"

/// \ingroup EME
/// \brief User Geant4 VMC run configuration
///
/// This class demonstrates inclusion of a user defined detector construction class
/// with a user defined magnetic field equation of motion and/or its integrator.
/// \author I. Hrivnacova; IPN, Orsay

class ExMERunConfiguration4 : public TG4RunConfiguration
{
  public:
    ExMERunConfiguration4(const TString& userGeometry,
                         const TString& physicsList = "emStandard",
                         const TString& specialProcess = "stepLimiter",
                         Bool_t specialStacking = false,
                         Bool_t mtApplication = true);
    virtual ~ExMERunConfiguration4();

    // methods
    virtual TG4VUserPostDetConstruction*  CreateUserPostDetConstruction();
};

#endif //EXME_RUN_CONFIGURATION4_H
