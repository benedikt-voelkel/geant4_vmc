//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file exampleMELinkDef.h
/// \brief The CINT link definitions for example ME classes

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class  MEMCSingleApplication+;
#pragma link C++ class  MEMCMultiApplication+;
#pragma link C++ class  MEMCSingleStack+;
#pragma link C++ class  MEMCMultiStack+;
#pragma link C++ class  MEParticle+;

#endif
