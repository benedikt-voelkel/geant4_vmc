#ifndef ME_SINGLE_STACK_H
#define ME_SINGLE_STACK_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  MEMCSingleStack.h
/// \brief Definition of the MEMCSingleStack class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \author B. Volkel; University Heidelberg

#include <TVirtualMCStack.h>

#include <stack>

/// \ingroup ME
/// \brief Implementation of the TVirtualMCStack interface
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg

class TParticle;
class MEParticle;

class MEMCSingleStack : public TVirtualMCStack
{
  public:
    MEMCSingleStack(Int_t size);
    MEMCSingleStack();
    virtual ~MEMCSingleStack();

    // methods
    void PrintStack() const;
    void Reset();
    virtual void PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
  	                       Double_t px, Double_t py, Double_t pz, Double_t e,
            		           Double_t vx, Double_t vy, Double_t vz, Double_t tof,
          		             Double_t polx, Double_t poly, Double_t polz,
          		             TMCProcess mech, Int_t& ntr, Double_t weight,
          		             Int_t is) override;
    virtual TParticle* PopNextTrack(Int_t& itrack) override;
    virtual TParticle* PopPrimaryForTracking(Int_t i) override;

    // set methods
    virtual void  SetCurrentTrack(Int_t itrack) override;

    // get methods
    virtual Int_t  GetNtrack() const override;
    virtual Int_t  GetNprimary() const override;
    virtual TParticle* GetCurrentTrack() const override;
    virtual Int_t  GetCurrentTrackNumber() const override;
    virtual Int_t  GetCurrentParentTrackNumber() const override;

  private:
    // methods
    MEParticle* GetParticle(Int_t id) const;

    // data members
    std::stack<MEParticle*>    fStack;        //!< The stack of particles (transient)
    TObjArray*                 fParticles;    ///< The array of particle (persistent)
    Int_t                      fCurrentTrack; ///< The current track number
    Int_t                      fNPrimary;     ///< The number of primaries

    ClassDefOverride(MEMCSingleStack,1) // MEMCSingleStack
};

#endif // ME_SINGLE_STACK_H
