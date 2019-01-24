//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2015 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMEPostDetConstruction.cxx
/// \brief Implementation of the ExMEPostDetConstruction class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo \n
///
/// \author I. Hrivnacova; IPN, Orsay

#include "ExMEPostDetConstruction.h"

#include "TG4GeometryManager.h"

#include "G4Mag_UsualEqRhs.hh"
#include "G4NystromRK4.hh"

//_____________________________________________________________________________
ExMEPostDetConstruction::ExMEPostDetConstruction()
 : TG4VUserPostDetConstruction()
{}

//_____________________________________________________________________________
ExMEPostDetConstruction::~ExMEPostDetConstruction()
{
}

//_____________________________________________________________________________
void ExMEPostDetConstruction::Construct()
{
  // Create "user" equation of motion;
  // G4Mag_UsualEqRhs eqution and G4NystromRK4 stepper should be replaced with users
  // own objects
  G4cout << "Create user defined equation of motion and stepper" << G4endl;
  G4Mag_EqRhs* equation = new G4Mag_UsualEqRhs(0);
  G4MagIntegratorStepper* stepper = new G4NystromRK4(equation);

  TG4GeometryManager::Instance()->SetUserEquationOfMotion(equation);
  TG4GeometryManager::Instance()->SetUserStepper(stepper);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
