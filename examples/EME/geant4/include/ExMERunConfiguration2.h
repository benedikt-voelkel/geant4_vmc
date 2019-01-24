#ifndef EXME_RUN_CONFIGURATION2_H
#define EXME_RUN_CONFIGURATION2_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERunConfiguration2.h
/// \brief Definition of the ExMERunConfiguration2 class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4RunConfiguration.h"

/// \ingroup EME
/// \brief User Geant4 VMC run configuration
///
/// This class overrides setting of the default Geant4 VMC physics list
/// with the LHEP_BERT hadronics physics list
///
/// \author I. Hrivnacova; IPN, Orsay

class ExMERunConfiguration2 : public TG4RunConfiguration
{
  public:
    ExMERunConfiguration2(const TString& userGeometry,
                          const TString& specialProcess = "stepLimiter" );
    virtual ~ExMERunConfiguration2();

    // methods
    virtual G4VUserPhysicsList*  CreatePhysicsList();

    //virtual G4VUserDetectorConstruction*   CreateDetectorConstruction();
    //virtual G4VUserPrimaryGeneratorAction* CreatePrimaryGenerator();
};

#endif //EXME_RUN_CONFIGURATION2_H
