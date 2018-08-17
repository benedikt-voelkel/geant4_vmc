#ifndef CE_STACK_H
#define CE_STACK_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  CEMCStack.h
/// \brief Definition of the CEMCStack class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include "CEParticle.h"

#include <TVirtualMCStack.h>

#include <stack>

/// \ingroup E01
/// \brief Implementation of the TVirtualMCStack interface
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

class CEMCStack : public TVirtualMCStack
{
  public:
    CEMCStack(Int_t size);
    CEMCStack();
    virtual ~CEMCStack();

    // methods
    virtual void  PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
  	              Double_t px, Double_t py, Double_t pz, Double_t e,
  		            Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		              Double_t polx, Double_t poly, Double_t polz,
		              TMCProcess mech, Int_t& ntr, Double_t weight,
		              Int_t is) ;
    virtual TParticle* PopNextTrack(Int_t& itrack);
    virtual TParticle* PopPrimaryForTracking(Int_t i);

    // set methods
    virtual void  SetCurrentTrack(Int_t itrack);

    // get methods
    virtual Int_t  GetNtrackToDo() const;
    virtual Int_t  GetNtrack() const;
    virtual Int_t  GetNprimary() const;
    virtual TParticle* GetCurrentTrack() const;
    virtual Int_t  GetCurrentTrackNumber() const;
    virtual Int_t  GetCurrentParentTrackNumber() const;


  private:
    // methods
    CEParticle* GetParticle(Int_t id) const;

    // data members
    std::stack<CEParticle*>    fStack;        //!< The stack of particles (transient)
    TObjArray*                 fParticles;    ///< The array of particle (persistent)
    Int_t                      fCurrentTrack; ///< The current track number
    Int_t                      fNPrimary;     ///< The number of primaries

    ClassDef(CEMCStack,1) // CEMCStack
};

#endif //CE_STACK_H
