#ifndef ME_MULTI_STACK_H
#define ME_MULTI_STACK_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  MEMCMultiStack.h
/// \brief Definition of the MEMCMultiStack class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \author B. Volkel; University Heidelberg

#include <TVirtualMCMultiStack.h>

/// \ingroup ME
/// \brief Implementation of the TVirtualMCMultiStack interface
///
/// \date 10/12/2018
/// \author B. Volkel; Univerity Heidelberg


class TParticle;
class MEParticle;

class MEMCMultiStack : public TVirtualMCMultiStack
{
  public:
    MEMCMultiStack(Int_t size);
    MEMCMultiStack();
    virtual ~MEMCMultiStack();

    // methods
    void Reset();
    void PrintStack() const;
    virtual TParticle* PushUserTrack(Int_t parent, Int_t pdg,
                                     Double_t px,Double_t py, Double_t pz,
                                     Double_t e, Double_t vx, Double_t vy,
                                     Double_t vz, Double_t tof, Double_t polx,
                                     Double_t poly, Double_t polz,
                                     Int_t geoStateIndex,
                                     ETrackTransportStatus transportStatus,
                                     TMCProcess mech, Int_t& ntr,
                                     Double_t weight, Int_t is) override;

  private:
    // methods
    MEParticle* GetParticle(Int_t id) const;

    // data members
    TObjArray*                 fParticles;    ///< The array of particle (persistent)
    Int_t                      fNPrimary;     ///< The number of primaries

    ClassDefOverride(MEMCMultiStack,1) // MEMCMultiStack
};

#endif // ME_MULTI_STACK_H
