#ifndef EXME_REGION_CONSTRUCTION_H
#define EXME_REGION_CONSTRUCTION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMERegionConstruction.h
/// \brief Definition of the ExMERegionConstruction class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "TG4VUserRegionConstruction.h"

/// \ingroup EME
/// \brief Special class for definition of regions
///
/// \author I. Hrivnacova; IPN, Orsay

class ExMERegionConstruction : public TG4VUserRegionConstruction
{
  public:
    ExMERegionConstruction();
    virtual ~ExMERegionConstruction();

    // methods
    virtual void Construct();
};

#endif //EXME_REGION_CONSTRUCTION_H
