#ifndef TG4_EM_PHYSICS_LIST_H
#define TG4_EM_PHYSICS_LIST_H

// $Id$

//------------------------------------------------
// The Geant4 Virtual Monte Carlo package
// Copyright (C) 2007, 2008 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: vmc@pcroot.cern.ch
//-------------------------------------------------

/// \file TG4EmPhysicsList.h
/// \brief Definition of the TG4EmPhysicsList class 
///
/// \author I. Hrivnacova; IPN Orsay

#include "TG4Verbose.h"

#include <G4VModularPhysicsList.hh>
#include <globals.hh>

/// \ingroup physics_list
/// \brief The standard EM physics list 
///
/// The EM physics list is implemented as modular physics list
/// with registered G4EmStandardPhysics, G4EmExtraPhysics and G4DecayPhysics 
/// builders,
///
/// The EM extra physics processes, not activated by default,
/// can be switched on using the following Geant4 commands:
/// - /physics_engine/tailor/SyncRadiation on
/// - /physics_engine/tailor/GammaNuclear on
/// - /physics_engine/tailor/MuonNuclear on
///
/// \author I. Hrivnacova; IPN Orsay

class TG4EmPhysicsList: public G4VModularPhysicsList,
                        public TG4Verbose
{
  public:
    TG4EmPhysicsList(const G4String& selection = "emStandard");
    virtual ~TG4EmPhysicsList();
  
    // static methods
    static G4String AvailableSelections();
    static G4bool   IsAvailableSelection(const G4String& selection);
 
    // methods
    virtual void  ConstructProcess();

                  /// No cuts are set here
    virtual void  SetCuts() {}
    
    virtual G4int VerboseLevel() const;
    virtual void  VerboseLevel(G4int level);

    // set methods
    void SetRangeCut(G4double value);
    
  protected:
    // static data members
    static const G4double  fgkDefaultCutValue; ///< default cut value

  private:
    /// Not implemented
    TG4EmPhysicsList(const TG4EmPhysicsList& right);
    /// Not implemented
    TG4EmPhysicsList& operator=(const TG4EmPhysicsList& right);

    // methods
    void Configure(const G4String& /*selection*/);
};

#endif //TG4_MODULAR_PHYSICS_LIST_H

