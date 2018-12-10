#ifndef ME_PARTICLE_H
#define ME_PARTICLE_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  MEParticle.h
/// \brief Definition of the MEParticle class
///
/// Geant4 ExampleN01 adapted to Virtual Monte Carlo
///
/// \author B. Volkel; University Heidelberg

#include <TObject.h>

class TParticle;

/// \ingroup ME
/// \brief Extended TParticle with pointers to mother and daughter
/// particles
///
/// \date 10/12/2018
/// \author B. Volkel; University Heidelberg

class MEParticle : public TObject
{
  public:
    MEParticle(Int_t id, TParticle* particle);
    MEParticle(Int_t id, TParticle* particle, MEParticle* mother);
    MEParticle();
    virtual ~MEParticle();

    // methods
    void SetMother(MEParticle* particle);
    void AddDaughter(MEParticle* particle);

    // get methods
    Int_t         GetID() const;
    TParticle*    GetParticle() const;
    MEParticle*   GetMother() const;
    Int_t         GetNofDaughters() const;
    MEParticle*   GetDaughter(Int_t i) const;

  private:
    // data members
    Int_t         fID;        ///< The particle Id
    TParticle*    fParticle;  ///< The particle definition
    MEParticle*   fMother;    ///< The particle mother
    TObjArray*    fDaughters; ///< The particle daughters

    ClassDef(MEParticle,1) // Extended TParticle
};

#endif // ME_PARTICLE_H
