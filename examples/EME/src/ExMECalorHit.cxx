//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file ExMECalorHit.cxx
/// \brief Implementation of the ExMECalorHit class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo \n
/// Id: ExN03CalorHit.cc,v 1.5 2002/01/09 17:24:12 ranjard Exp \n
/// GEANT4 tag Name: geant4-05-00
///
/// \date 06/03/2002
/// \author I. Hrivnacova; IPN, Orsay

#include <Riostream.h>

#include "ExMECalorHit.h"

using namespace std;

/// \cond CLASSIMP
ClassImp(ExMECalorHit)
/// \endcond

//_____________________________________________________________________________
ExMECalorHit::ExMECalorHit()
  : TObject(),
    fEdepAbs(0.),
    fTrackLengthAbs(0.),
    fEdepGap(0.),
    fTrackLengthGap(0.)
{
/// Default constructor
}

//_____________________________________________________________________________
ExMECalorHit::~ExMECalorHit() 
{
/// Destructor
}

//_____________________________________________________________________________
void ExMECalorHit::Print(Option_t* /*option*/) const
{
/// Print hit info

  cout << "In absorber: " << endl
       << "   energy deposit (keV): " << fEdepAbs * 1.0e06 << endl
       << "   track length (cm): " << fTrackLengthAbs  << endl
       << "In gap: " << endl
       << "   energy deposit (keV): " << fEdepGap * 1.0e06 << endl
       << "   track length (cm): " << fTrackLengthGap  << endl;
}

//_____________________________________________________________________________
void ExMECalorHit::Reset()
{
/// Reset all accounted values.

  fEdepAbs = 0.;
  fTrackLengthAbs = 0.;
  fEdepGap = 0.;
  fTrackLengthGap = 0.;
}
