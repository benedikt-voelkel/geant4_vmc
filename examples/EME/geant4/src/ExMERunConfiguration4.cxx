//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2015 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERunConfiguration4.cxx
/// \brief Implementation of the ExMERunConfiguration4 class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo \n
///
/// \author I. Hrivnacova; IPN, Orsay

#include "ExMERunConfiguration4.h"
#include "ExMEPostDetConstruction.h"


//_____________________________________________________________________________
ExMERunConfiguration4::ExMERunConfiguration4(const TString& userGeometry,
                                             const TString& physicsList,
                                             const TString& specialProcess,
                                             Bool_t specialStacking,
                                             Bool_t mtApplication)
  : TG4RunConfiguration(userGeometry, physicsList, specialProcess,
  	                    specialStacking, mtApplication)
{
/// Standard constructor
}

//_____________________________________________________________________________
ExMERunConfiguration4::~ExMERunConfiguration4()
{
/// Destructor
}

//
// protected methods
//

//_____________________________________________________________________________
TG4VUserPostDetConstruction*  ExMERunConfiguration4::CreateUserPostDetConstruction()
{
/// User defined detector construction

  return new ExMEPostDetConstruction();
}
