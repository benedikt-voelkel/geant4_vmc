//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  exampleEMELinkDef.h
/// \brief The CINT link definitions for example EME classes

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class  ExMEMCApplication+;
#pragma link C++ class  ExMEMCStack+;
#pragma link C++ class  ExMEDetectorConstruction+;
#pragma link C++ class  ExMECalorHit+;
#pragma link C++ class  ExMECalorimeterSD+;
#pragma link C++ class  ExMEPrimaryGenerator+;
#pragma link C++ class  std::stack<TParticle*,deque<TParticle*> >+;

#endif
