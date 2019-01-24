#ifndef EXME_POST_DET_CONSTRUCTION_H
#define EXME_POST_DET_CONSTRUCTION_H 1

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2015 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMEPostDetConstruction.h
/// \brief Definition of the ExMEPostDetConstruction class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4VUserPostDetConstruction.h"
#include "globals.hh"

/// Post detector construction class which integrates
/// user defined magnetic field equation of motion and integrator

class ExMEPostDetConstruction : public TG4VUserPostDetConstruction
{
  public:
    ExMEPostDetConstruction();
    virtual ~ExMEPostDetConstruction();

  public:
    virtual void Construct();
};

#endif //EXME_POST_DET_CONSTRUCTION_H
